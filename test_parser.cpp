#include "parser.h"
#include <iostream>
#include <cstdio>
#define Bin2(oper) make_reflect<ast_type>([](ast_type& ast){\
	return ast[0].gen() oper ast[1].gen();\
})
#define Bin1l(oper) make_reflect<ast_type>([](ast_type& ast){\
	return oper ast[0].gen();\
})
#define PassOn() make_reflect<ast_type>([](ast_type& ast){\
	return ast[0].gen();\
})
#define FetchTerm(x) make_reflect<ast_type>([](ast_type& ast){\
	return ast.term(x);\
})
using rtype = int;
using ast_type = ast<rtype>;
using namespace std;
reflected_lexer<ast_type> lex(
	"+"_t, "-"_t, "*"_t, "/"_t, "~"_t, "%"_t, "<<"_t, ">>"_t, "&"_t, "^"_t, "|"_t, "("_t, ")"_t,
	"dec"_t = "[1-9]\\d*"_r
		>> lexer_reflect<ast_type>([](const string& src){
			rtype r; sscanf(src.c_str(), "%d", &r); return r;
		}),
	"oct"_t = "0[0-7]*"_r
		>> lexer_reflect<ast_type>([](const string& src){
			rtype r; sscanf(src.c_str(), "%o", &r); return r;
		}),
	"hex"_t = "0[xX][0-9a-fA-F]+"_r
		>> lexer_reflect<ast_type>([](const string& src){
			rtype r; sscanf(src.c_str(), "%x", &r); return r;
		})
);
parser<ast_type> my_parser(lex,
	"start"_p = "expr"_p
		>> make_reflect<ast_type>([](ast_type& ast){
			auto r = ast[0].gen();
			cout << r << endl; return r;
		}),
	"expr"_p = 
		"expr"_p + "|"_t + "expr1"_p	>> Bin2(|)
		|"expr1"_p						>> PassOn(),
	"expr1"_p = 
		"expr1"_p + "^"_t + "expr2"_p	>> Bin2(^)
		|"expr2"_p						>> PassOn(),
	"expr2"_p = 
		"expr2"_p + "&"_t + "expr3"_p	>> Bin2(&)
		|"expr3"_p						>> PassOn(),
	"expr3"_p = 
		"expr3"_p + "<<"_t + "expr4"_p	>> Bin2(<<)
		|"expr3"_p + ">>"_t + "expr4"_p	>> Bin2(>>)
		|"expr4"_p						>> PassOn(),
	"expr4"_p = 
		"expr4"_p + "+"_t + "expr5"_p	>> Bin2(+)
		|"expr4"_p + "-"_t + "expr5"_p	>> Bin2(-)
		|"expr5"_p						>> PassOn(),
	"expr5"_p = 
		"expr5"_p + "*"_t + "expr6"_p	>> Bin2(*)
		|"expr5"_p + "/"_t + "expr6"_p	>> Bin2(/)
		|"expr5"_p + "%"_t + "expr6"_p	>> Bin2(%)
		|"expr6"_p						>> PassOn(),
	"expr6"_p = 
		"-"_t + "expr6"_p				>> Bin1l(-)
		|"~"_t + "expr6"_p				>> Bin1l(~)
		|"elem"_p						>> PassOn(),
	"elem"_p = 
		"integer"_p						>> PassOn()
		|"("_t + "expr"_p + ")"_t		>> PassOn(),
	"integer"_p =
		"dec"_t							>> FetchTerm(0)
		|"oct"_t						>> FetchTerm(0)
		|"hex"_t						>> FetchTerm(0)
);
int main()
{
	while (1)
	{
		cout << ">>> ";
		string buffer;
		getline(cin, buffer);
		my_parser.parse(buffer.c_str());
	}
}
