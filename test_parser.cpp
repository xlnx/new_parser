#include "parser.h"

template <typename RetTy>
struct ast_template: ast<RetTy>
{};
using ast_type = ast_template<int>;

parser<ast_type> my(
	"expr"_R = 
		"("_T + ")"_T >> make_reflect<ast_type>(
		)
	,
	"if"_R =
		"if"_T + "("_T + "expr"_R + ")"_T >> make_reflect<ast_type>(
			[](ast_type&)->typename ast_type::value_type{

			}
		)
	,
	"shit"_R = "if"_T >> make_reflect<ast_type>()
);

int main()
{

}
