#ifndef CW_YAL_AST_H
#define CW_YAL_AST_H

#include <string>

#include <boost/variant.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include "dummy.hpp"

namespace yal {
    namespace ast {

        ///////////////////////////////////////////////////////////////////////////
        // Forward declaration
        ///////////////////////////////////////////////////////////////////////////

        enum class call_trait {
            macro, call
        };

        template<call_trait C>
        struct callable;

        struct list;

        ///////////////////////////////////////////////////////////////////////////
        // AST nodes
        ///////////////////////////////////////////////////////////////////////////

        using macro = callable<call_trait::macro>;
        using call = callable<call_trait::call>;


        struct symbol {
            std::string repr;

            symbol() = default;

            symbol(std::string repr) : repr(std::move(repr)) { }

            symbol(char const *repr) : repr(repr) { }
        };

        template<typename T>
        struct literal {
            T value;

            literal() = default;

            literal(T const &value) : value{value} { }
        };


        using expression = boost::variant<
                symbol,
                literal<bool>,
                literal<double>,
                literal<std::string>,
                boost::recursive_wrapper<macro>,
                boost::recursive_wrapper<call>,
                boost::recursive_wrapper<list>
        >;

        struct list {
            using range = boost::sub_range<std::vector<expression>>;
            using const_range = boost::sub_range<std::vector<expression> const>;
            std::vector<expression> values;

            list() = default;

            list(std::initializer_list<expression> values) : values(values) { }

            list(const_range::iterator begin, const_range::iterator end)
                    : values(begin, end) { }
        };

        using formals_t = boost::variant<std::vector<symbol>, symbol>;


        template<call_trait C>
        struct callable {
            using iterator = list::const_range::iterator;
            using function_type = std::function<expression(environment &)>;

            callable(environment &parent, std::vector<symbol> const &formals, function_type lambda);
            callable(environment &parent, symbol const &arglist, function_type lambda);

            environment *parent;
            formals_t formals;
            function_type lambda;

            expression operator()(environment &env, iterator begin, iterator end) const;
        };



    } //ast
}

BOOST_FUSION_ADAPT_STRUCT(yal::ast::symbol, (std::string, repr))
BOOST_FUSION_ADAPT_STRUCT(yal::ast::literal<bool>, (bool, value))
BOOST_FUSION_ADAPT_STRUCT(yal::ast::literal<double>, (double, value))
BOOST_FUSION_ADAPT_STRUCT(yal::ast::literal<std::string>, (std::string, value))
BOOST_FUSION_ADAPT_STRUCT(yal::ast::list, (std::vector<yal::ast::expression>, values))


#endif //CW_YAL_AST_H
