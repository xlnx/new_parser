#include "parser.h"
#include "lexer.h"
using ast_type = ast<int>;

reflected_lexer<ast_type> lex(
	"if"_T = "if"_W
		>> lexer_reflect<ast_type>(
			[](const std::string&)->int{}
		),
	"+"_T >> lexer_reflect<ast_type>(
			[](const std::string&)->int{}
		),
	"("_T,
	")"_T
);
parser<ast_type> my_parser(lex,
	"expr"_R =
		"("_T + "+"_T + ")"_T
			>> make_reflect<ast_type>(
				[](ast_type&)->typename ast_type::value_type{},
				[](ast_type&){}
			)
);
#include <iostream>
int main()
{
	my_parser.parse("( + )");
}
