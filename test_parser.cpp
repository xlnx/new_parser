#include "parser_initializer.h"

template <typename RetTy>
struct ast_template: ast<RetTy>
{};
using ast_type = ast_template<int>;

parser_element<ast_type>
expr_R = 
	"("_T + ")"_T >> make_reflect<ast_type>(
		[](ast_type&)->typename ast_type::value_type{
			
		}
	),
if_R =
	"if"_T + "("_T + expr_R + ")"_T;

int main()
{

}
