#pragma once			// lexer.h
#include "lexer_initializer.h"
#include <iostream>

template <typename CharT>
class basic_lexer
{
	lexer_initializer rules;
	using iterator = const CharT*;
	using string_type = std::basic_string<CharT>;
	string_type str;
	iterator iter;
public:
	struct token;
	using value_type = token;
	using position_type = std::pair<unsigned, unsigned>;
	struct token
	{
		long long id;
		string_type value;
	};
public:
	basic_lexer(const lexer_initializer& list):
		rules(list), iter(&str[0]) {}
	virtual ~basic_lexer() = default;
public:
	void reset() { str = ""; iter = &str[0]; }
	bool empty() const { return !*iter; }
	basic_lexer& operator << (string_type&& src)
	{
		auto diff = iter - &str[0];
		str += std::forward<string_type>(src) + "\n";
		iter = &str[0] + diff;
		return *this;
	}
	void operator <= (string_type&& src)
	{
		str = std::forward<string_type>(src);
		iter = &str[0];
	}
	value_type next()
	{
		static string_type spaces = " \t\r\n";
		if (*iter)
		{
			std::match_results<const CharT*> result;
			for (const auto& rule: rules)
			{
				if (std::regex_search(iter, result, rule.mode, std::regex_constants::match_continuous))
				{
					while (iter != result.suffix().first)
							++iter;
					while (*iter && spaces.find_first_of(*iter) != string_type::npos)
							++iter;
					string_type res = result[0];
					std::cout << res << std::endl;
					return {str_hash_64(res.c_str()), res};
				}
			}
			iter = &str[str.length()];
		}
	}
};

using lexer = basic_lexer<char>;
