#include "lexer.h"
#include <iostream>
lexer<> lex(
	"("_t,
	")"_t,
	"+"_t,
	"-"_t,
	"*"_t,
	"/"_t,
	"number"_t = "\\d+"_r
);
// init<init_elem> | elem
int main()
{
	lex <= "( 1 + 2 )";
	while (!lex.empty())
	{
		lex.next();
	}
	lex << "( 1 + 3 )" << "2 + 1";
	while (!lex.empty())
	{
		lex.next();
	}
}
