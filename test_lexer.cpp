#include "lexer.h"
lexer lex(
	"("_T |
	")"_T |
	"+"_T |
	"-"_T |
	"*"_T |
	"/"_T |
	"number"_T >> "\\d+"
);
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
