#pragma once
#include "parser_initializer.h"
#include "lexer.h"
#include <set>
#include <map>
#include <queue>
#include <vector>
#include <stack>

template <class AstTy, class CharTy = char>
class parser
{
	static const long long empty_elem;
	static const long long stack_bottom;
	using param_list = initializer<parser_init_element<AstTy>>;
	using ast_base = ast<typename AstTy::value_type>;
	enum action_type { a_error = 0, a_accept, a_move_in, a_reduce };
	struct action
	{
		action_type flag;
		const parser_rule<AstTy>* rule;
		constexpr action(action_type act = a_error, const parser_rule<AstTy>* r = nullptr):
			flag(act), rule(r) {}
	};
	reflected_lexer<AstTy, CharTy>& lex;
	param_list params;
	std::set<long long> signs;
	std::vector<std::map<long long, action>> ACTION;		// [state][sign]->action->rule_id
	std::vector<std::map<long long, std::size_t>> GOTO;		// [state][sign]->state
	std::map<const parser_rule<AstTy>*, long long> parent_of;
	std::map<const parser_rule<AstTy>*, std::size_t> index_of;
	std::map<long long, parser_init_element<AstTy>*> param_of;
public:
	template <class... Args, typename = typename
			std::enable_if<
				tmp_and<
					std::is_same<
						Args, parser_init_element<AstTy>
					>::value...
				>::value
			>::type
		>
		parser(reflected_lexer<AstTy, CharTy>& engine, const Args&... args):
			lex(engine), params(args...), signs(lex.signs)
		{
			element start = params.back();
			params = params | parser_init_element<AstTy>(
				""_p = start >> make_reflect<AstTy>(
					[](AstTy& ast){return ast[0].gen();}
				)	// tag: ~0LL
			);
			std::map<long long, std::set<long long>> FIRST, FOLLOW;
			for (auto& elem: lex.signs)
			{
				FIRST[elem].insert(elem);
			}
			bool add_sub = false;
			do {
				add_sub = false;
				for (auto& param: params)
				{
					for (auto& rule: param)
					{
						bool has_empty = false;
						for (auto& elem: rule)
						{
							has_empty = false;
							for (auto first_elem: FIRST[elem.value])
							{
								if (first_elem == empty_elem)
								{
									has_empty = true;
								}
								else if (!FIRST[param.value].count(first_elem))
								{
									FIRST[param.value].insert(first_elem);
									add_sub = true;
								}
							}
							if (!has_empty) break;
						}
						if (has_empty && !FIRST[param.value].count(empty_elem))
						{
							FIRST[param.value].insert(empty_elem);
							add_sub = true;
						}
					}
				}
			} while (add_sub);

			do {
				add_sub = false;
				for (auto& param: params)
				{
					for (auto& rule: param)
					{
						for (auto i = rule.size() - 1; i != 0; --i)
						{
							if (rule[i - 1].value < 0)	// parser element
							{
								if (rule[i].value < 0)	// parser element
								{
									for (auto first_elem: FIRST[rule[i].value])
									{
										if (first_elem != empty_elem &&
											!FOLLOW[rule[i - 1].value].count(first_elem))
										{
											FOLLOW[rule[i - 1].value].insert(first_elem);
											add_sub = true;
										}
									}
									if (FIRST[rule[i].value].count(empty_elem))
									{
										for (auto follow_elem: FOLLOW[param.value])
										{
											if (!FOLLOW[rule[i - 1].value].count(follow_elem))
											{
												FOLLOW[rule[i - 1].value].insert(follow_elem);
												add_sub = true;
											}
										}
									}
								}
								else if (!FOLLOW[rule[i - 1].value].count(rule[i].value))
								{
									FOLLOW[rule[i - 1].value].insert(rule[i].value);
									add_sub = true;
								}
							}
						}
						if (rule.back().value < 0)
						{
							for (auto follow_elem: FOLLOW[param.value])
							{
								if (!FOLLOW[rule.back().value].count(follow_elem))
								{
									FOLLOW[rule.back().value].insert(follow_elem);
									add_sub = true;
								}
							}
						}
					}
				}
			} while (add_sub);

			for (auto& param: params)
			{
				static std::size_t index = 0;
				for (auto& rule: param)
				{
					parent_of[&rule] = param.value;
					index_of[&rule] = index++;
				}
				FOLLOW[param.value].insert(stack_bottom);
				param_of[param.value] = &param;
				signs.insert(param.value);
			}
			signs.insert(stack_bottom);
			struct item
			{
				const parser_rule<AstTy>& rule;
				std::size_t position;
				bool operator < (const item& other) const
					{ return &rule < &other.rule || &rule == &other.rule && position < other.position; }
			};
			using closure = std::set<item>;
			std::vector<closure> closures;
			auto gen_closure = [&](closure I)
			{
				bool gen_sub = false;
				do {
					gen_sub = false;
					for (auto& term: closure(I))		// prevent iter loss
					{
						if (term.rule.size() != term.position)
						{
							if (auto param_ptr = param_of[term.rule[term.position].value])
							{
								for (auto& rule: *param_ptr)
								{
									if (!I.count(item{rule, 0}))
									{
										I.insert(item{rule, 0});
										gen_sub = true;
									}
								}
							}
						}
					}
				} while (gen_sub);
				closures.push_back(std::move(I));
				GOTO.push_back(std::map<long long, std::size_t>());
				ACTION.push_back(std::map<long long, action>());
			};
			gen_closure({item{params.back()[0], 0}});
			do {
				add_sub = false;
				for (std::size_t state = 0; state != closures.size(); ++state)
				{
					const parser_rule<AstTy>* empty_sign_ptr = nullptr;
					for (auto& term: closures[state])
					{
						if (term.rule.size() == 1 && term.rule[0].value == empty_elem)
						{
							empty_sign_ptr = &term.rule;
						}
						else if (term.rule.size() == term.position)
						{
							for (auto elem: FOLLOW[parent_of[&term.rule]])
							{
								ACTION[state][elem] = action(
									&term.rule == &params.back()[0] ? a_accept : a_reduce,
									&term.rule
								);
							}
						}
					}
					for (auto& sign: signs)
					{
						if (!GOTO[state][sign])
						{
							closure NEW;
							for (auto& term: closures[state])
							{
								if (term.position != term.rule.size() &&
									term.rule[term.position].value == sign)
								{
									NEW.insert(item{term.rule, term.position + 1});
								}
							}
							if (!NEW.empty())
							{
								bool is_sub = false;
								const parser_rule<AstTy>* rule_ptr = nullptr;
								std::size_t dest_state = 0;
								for (; dest_state != closures.size(); ++dest_state)
								{
									is_sub = true;
									for (auto& term: NEW)
									{
										if (!rule_ptr)
											rule_ptr = &term.rule;
										else if (index_of[&term.rule] < index_of[rule_ptr])
											rule_ptr = &term.rule;
										if (!closures[dest_state].count(term))
										{
											is_sub = false;
											break;
										}
									}
									if (is_sub) break;
								}
								if (is_sub)
								{
									GOTO[state][sign] = dest_state;
									ACTION[state][sign] = a_move_in;
								}
								else
								{
									GOTO[state][sign] = closures.size();
									gen_closure(NEW);
									switch (ACTION[state][sign].flag)
									{
									case a_accept: case a_move_in: throw std::bad_cast();
									case a_error: ACTION[state][sign] = a_move_in; break;
									default: if (index_of[rule_ptr] < index_of[ACTION[state][sign].rule])
										ACTION[state][sign] = a_move_in;
									}
								}
							}
						}
					}
					if (empty_sign_ptr)
					{
						for (auto& sign: signs)
						{
							if (ACTION[state][sign].flag == a_error)
								ACTION[state][sign] = action(
									a_reduce,
									empty_sign_ptr
								);
						}
					}
				}
			} while (add_sub);
		}
	virtual ~parser() = default;
public:
	void parse(const CharTy* buffer)
	{
		std::queue<typename reflected_lexer<AstTy, CharTy>::value_type> tokens;
		lex <= buffer;
		while (!lex.empty()) {
			tokens.push(lex.next());
		}
		std::stack<std::size_t> states;
		std::vector<std::shared_ptr<AstTy>> ast_stack;
		std::vector<typename reflected_lexer<AstTy, CharTy>::value_type> term_stack;
		states.push(0);

		auto merge = [&](const parser_rule<AstTy>* rule)
		{
			std::shared_ptr<ast_base> this_ast(std::make_shared<AstTy>(ast_base(rule->ast_data)));
			if (rule->size() > 1 || (*rule)[0].value != empty_elem)
			{
				std::size_t ast_size = 0, term_size = 0;
				for (auto& dummy: *rule)
				{
					if (dummy.value < 0) ast_size++;
						else term_size++;
				}
				for (auto itr = ast_stack.end() - ast_size; itr != ast_stack.end(); ++itr)
				{
					this_ast->sub_ast.push_back(*itr);
				}
				for (auto itr = term_stack.end() - term_size; itr != term_stack.end(); ++itr)
				{
					this_ast->sub_terms.push_back(lex.handlers[itr->id](itr->value));
				}
				for (auto& dummy: *rule)
				{
					states.pop(); if (dummy.value < 0) ast_stack.pop_back();
						else term_stack.pop_back();
				}
			}		// sub_rules
			rule->ast_data.on_parse(*this_ast);
			ast_stack.push_back(std::move(this_ast));
			states.push(GOTO[states.top()][parent_of[rule]]);
		};
		do {
			auto sign = tokens.empty() ? stack_bottom : tokens.front().id;
			switch (ACTION[states.top()][sign].flag)
			{
			case a_move_in:
				states.push(GOTO[states.top()][sign]);
				if (sign >= 0)
				{
					term_stack.push_back(tokens.front());
				}
				tokens.pop(); break;
			case a_accept:
				if (ast_stack.size() == 1 && tokens.empty() 
						&& term_stack.empty()) goto SUCCESS;
					throw std::bad_cast();
			case a_error:
				throw std::bad_cast();
			default:
				merge(ACTION[states.top()][sign].rule);
			}
		} while (1);
		SUCCESS:
		ast_stack.front()->gen();
	}
};

template <class AstTy, class CharTy>
const long long parser<AstTy, CharTy>::empty_elem = 0LL;
template <class AstTy, class CharTy>
const long long parser<AstTy, CharTy>::stack_bottom = 1LL;