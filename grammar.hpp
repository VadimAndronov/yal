#ifndef CW_YAL_GRAMMAR_HPP
#define CW_YAL_GRAMMAR_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_match_attr.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/object/construct.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

#include "ast.h"

namespace yal {

    namespace qi = boost::spirit::qi;

    template<typename Iterator>
    struct grammar : qi::grammar<Iterator, ast::expression(), qi::ascii::space_type> {
        template<typename Signature>
        using rule_t = qi::rule<Iterator, Signature, qi::ascii::space_type>;

        grammar() : grammar::base_type(start) {
            using namespace qi;
            using namespace boost::phoenix;

            start %= sym | lit | lst;
            sym %= eps >> lexeme[+(char_("A-Za-z") | char_('-') | char_("+*/%~&|^!=<>?"))];
            lit %= boolean | number | string;
            boolean %=
                    qi::lit("#t")[_val = construct<ast::literal<bool>>(true)] |
                    qi::lit("#f")[_val = construct<ast::literal<bool>>(false)];
            number %= double_;
            string %= eps >> lexeme['"' >> *(char_ - '"') >> '"'];
            lst %= '(' >> *start >> ')';
        }

    private:
        rule_t<ast::expression()> start;
        rule_t<ast::symbol()> sym;
        rule_t<ast::expression()> lit;
        rule_t<ast::literal<bool>()> boolean;
        rule_t<ast::literal<double>()> number;
        rule_t<ast::literal<std::string>()> string;
        rule_t<ast::list()> lst;
    };
}


#endif //CW_YAL_GRAMMAR_HPP
