#ifndef CW_YAL_PARSER_H
#define CW_YAL_PARSER_H

#include <string>
#include "ast.h"

#include <boost/optional.hpp>
#include <boost/spirit/home/support/iterators/multi_pass.hpp>

namespace yal { namespace parser {

        using parsed_t = boost::optional<ast::expression>;

        template <typename Iterator>
        parsed_t parse(Iterator &begin, Iterator end);

        template <typename Iterator>
        parsed_t parse_full(Iterator begin, Iterator end) {
            parsed_t&& result = parse(begin, end);
            return {result and begin == end, *result};
        }

        using multi_pass_iterator = boost::spirit::multi_pass<std::istreambuf_iterator<char>>;
    }
}

#endif //CW_YAL_PARSER_H
