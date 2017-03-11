#pragma once
#include "parser_initializer.h"

template <class AstTy>
class parser
{
	using param_list = initializer<parser_init_element<AstTy>>;
	param_list params;
public:
	template <class... Args, typename = typename
			std::enable_if<
				tmp_and<
					std::is_same<
						Args, parser_init_element<AstTy>
					>::value...
				>::value
			>::type
		>
		parser(const Args&... args):
			params(args...) {}
};