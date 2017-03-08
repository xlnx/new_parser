#pragma once			// lexer_initializer.h
#include <cstring>
#include <string>
#include <cstddef>
#include <regex>
#include <vector>
#include <type_traits>

inline constexpr long long str_hash_64(const char *str)
{
	long long seed = 131;
	long long hash = 0;
	while (*str)
		hash = hash * seed + (*str++);
	return hash & ((unsigned long long)(-1LL) >> 1);
}

struct element
{
	long long value;
	constexpr element(long long v): value(v) {}
};

struct lexer_element: element
{
	const char* src_str;
	constexpr lexer_element(const char s[]):
		element(str_hash_64(s)), src_str(s) {}
};
inline constexpr lexer_element operator ""_T (const char s[], std::size_t)
{
	return lexer_element(s);
}

struct lexer_rule
{
	const char* src_str;
	bool whole_word;
	constexpr lexer_rule(const char s[], bool w = false):
		src_str(s), whole_word(w) {}
};
inline constexpr lexer_rule operator ""_W (const char s[], std::size_t)
{
	return lexer_rule(s, true);
}

struct lexer_init_element
{
	std::regex mode;
	long long value;
	lexer_init_element(const lexer_element& lex):
		value(lex.value), mode(std::regex([](const char* p){
			static const auto escape_lst = "*.?+-$^[](){}|\\/";
			std::string res;
			while (*p)
			{
				if (strchr(escape_lst, *p)) res += "\\";
				res += *p++;
			}
			return res;
		}(lex.src_str),
			std::regex::nosubs | std::regex::optimize)) {}
	lexer_init_element(const lexer_element& lex, const lexer_rule& rule):
		value(lex.value), mode(std::regex(*rule.src_str ?
				std::string(rule.src_str) + (rule.whole_word ? "\\b" : "") : "$^",
			std::regex::nosubs | std::regex::optimize)) {}
};
inline lexer_init_element operator >> (const lexer_element& lex, const lexer_rule& rule)
{
	return lexer_init_element(lex, rule);
}

template <typename T, typename = typename
	std::enable_if<std::is_class<T>::value>::type>
struct initializer: std::vector<T>
{
	using value_type = T;
	initializer(const T& elem):
		std::vector<T>({elem}) {}
	template <typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
		initializer(const initializer<U>& list)
		{ for (auto& elem: list) std::vector<T>::push_back(elem); }
};
template <typename T>
inline initializer<T> operator | (const T& e1, const T& e2)
{
	initializer<T> list(e1);
	list.push_back(e2);
	return list;
}
template <typename T>
inline const initializer<T>& operator | (const initializer<T>& list, const T& e)
{
	const_cast<initializer<T>&>(list).push_back(e);
	return list;
}
template <typename T, typename U, typename = typename
			std::enable_if<
				!std::is_same<T, U>::value && 
				std::is_constructible<T, const U&>::value
			>::type>
inline initializer<T> operator | (const initializer<U>& list, const T& e)
{
	auto l = initializer<T>(list);
	l.push_back(e);
	return l;
}

using lexer_initializer = initializer<lexer_init_element>;
