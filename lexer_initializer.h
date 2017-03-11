#pragma once			// lexer_initializer.h
#include <cstring>
#include <string>
#include <cstddef>
#include <regex>
#include "initializer.h"

inline constexpr long long str_hash_64(const char *str)
{
	long long seed = 131;
	long long hash = 0;
	while (*str)
		hash = hash * seed + (*str++);
	return hash & ((unsigned long long)(-1LL) >> 1);
}

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

struct lexer_init_element: element
{
	std::regex mode;
	long long value;
	lexer_init_element(const lexer_element& lex):
		element(lex), mode(std::regex([](const char* p){
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
		element(lex), mode(std::regex(*rule.src_str ?
				std::string(rule.src_str) + (rule.whole_word ? "\\b" : "") : "$^",
			std::regex::nosubs | std::regex::optimize)) {}
};
inline lexer_init_element operator >> (const lexer_element& lex, const lexer_rule& rule)
{
	return lexer_init_element(lex, rule);
}

using lexer_initializer = initializer<lexer_init_element>;
