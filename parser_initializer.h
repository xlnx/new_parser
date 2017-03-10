#pragma once			// parser_initializer.h
#include "lexer_initializer.h"
#include <functional>
#include <memory>

template <typename RetTy>
struct ast;

template <typename AstTy, typename = typename
	std::enable_if<std::is_class<AstTy>::value>::type>
struct parser_ast_data;

template <template <typename> class AstTy, typename RetTy>
struct parser_ast_data<AstTy<RetTy>>
{
	using ast_type = AstTy<RetTy>;
	typename ast_type::on_exec_reflection on_exec;
	typename ast_type::on_parse_reflection on_parse;
	constexpr parser_ast_data(
			const std::function<RetTy(ast_type&)>& rexec =
				std::function<RetTy(ast_type&)>(
					[](ast_type&)->RetTy{}
				), 
			const std::function<void(ast_type&)>& rparse =
				std::function<void(ast_type&)>(
					[](ast_type&){}
				)):
		on_exec([rexec](ast<RetTy>& n)->RetTy{
				rexec(static_cast<ast_type&>(n));
			}),
		on_parse([rparse](ast<RetTy>& n)->RetTy{
				rparse(static_cast<ast_type&>(n));
			}) {}
};
template <typename T>
inline constexpr parser_ast_data<T> make_reflect(
	const std::function<typename T::value_type(T&)>& rexec =
			std::function<typename T::value_type(T&)>(
				[](T&)->typename T::value_type{}
			),
	const std::function<void(T&)>& rparse =
			std::function<void(T&)>(
				[](T&){}
			)
		)
	{ return parser_ast_data<T>(rexec, rparse); }

template <typename RetTy>
struct ast
{
	using value_type = RetTy;
	using on_exec_reflection = std::function<RetTy(ast<RetTy>&)>;
	using on_parse_reflection = std::function<void(ast<RetTy>&)>;
private:
	std::vector<std::unique_ptr<ast<RetTy>>> sub_ast;
	on_exec_reflection ref_exec;
	on_parse_reflection ref_parse;
public:
	template <template <typename> class AstTy>
	ast(const parser_ast_data<AstTy<RetTy>>& ast_data):
		ref_exec(ast_data.on_exec), ref_parse(ast_data.on_parse) {}
	ast<RetTy>& operator [] (unsigned i)
		{ return *sub_ast[i]; }
	RetTy gen(ast<RetTy>&) { return ref_exec(*this); }
};


template <typename AstTy>
struct parser_element: element
{
	element_list params;
	parser_ast_data<AstTy> ast_data;
	parser_element(const element& elem):
		element(~element_count++),
		params({elem}) {}
	parser_element(
			const element_list& rules,
			const parser_ast_data<AstTy>& data =
				parser_ast_data<AstTy>()):
		element(~element_count++),
		params(rules),
		ast_data(data) {}
private:
	static unsigned element_count;
};
template <typename AstTy>
unsigned parser_element<AstTy>::element_count = 0;

template <typename T, typename AstTy, typename = typename
		std::enable_if<
			std::is_constructible<
				element_list, const T&
			>::value
		>::type
	>
inline parser_element<AstTy> operator >> (const T& list, const parser_ast_data<AstTy>& f)
{
	return parser_element<AstTy>(element_list(list), f);
}
