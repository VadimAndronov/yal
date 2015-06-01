#include <iterator>

#include "parser.h"
#include "grammar.hpp"


namespace yal {
    namespace parser {

        template<typename Iterator>
        parsed_t parse(Iterator &begin, Iterator end) {
            static yal::grammar<Iterator> grammar{};
            yal::ast::expression result{};
            bool success = qi::phrase_parse(begin, end, grammar, qi::ascii::space, result);
            return {success, result};
        }



        //explicit instantiation
        template parsed_t parse<multi_pass_iterator>(multi_pass_iterator&, multi_pass_iterator);
        template parsed_t parse<std::string::iterator>(std::string::iterator&, std::string::iterator);


    } // parser
}
