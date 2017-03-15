#include "parser.h"
#include "longint/longint.h"
#include <iostream>
#define Bin2(oper) make_reflect<ast_type>([](ast_type& ast){\
	return ast[0].gen() oper ast[1].gen();\
})
#define Bin2Int(oper) make_reflect<ast_type>([](ast_type& ast){\
	return ast[0].gen() oper ast[1].gen().as_int();\
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
using rtype = longint;
using ast_type = ast<rtype>;
using namespace std;
reflected_lexer<ast_type> lex(
	"+"_t, "-"_t, "*"_t, "/"_t, "~"_t, "%"_t, "<<"_t, ">>"_t, "&"_t, "^"_t, "|"_t, "("_t, ")"_t,
	"integer"_t = "(0[xX][0-9a-fA-F]+)|([1-9]\\d*)|(0[0-7]*)"_r
		>> lexer_reflect<ast_type>([](const string& src){return longint(src.c_str());})
);
parser<ast_type> my_parser(lex,
	"start"_p = "expr"_p
		>> make_reflect<ast_type>([](ast_type& ast)->rtype{
			auto x = ast[0].gen();
			cout << x << endl;
			return x;
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
		"expr3"_p + "<<"_t + "expr4"_p	>> Bin2Int(<<)
		|"expr3"_p + ">>"_t + "expr4"_p	>> Bin2Int(>>)
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
		"integer"_t						>> FetchTerm(0)
		|"("_t + "expr"_p + ")"_t		>> PassOn()
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
