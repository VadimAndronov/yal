#include "interpreter.h"
#include "exception.hpp"


namespace yal {

    namespace visitors {
        using namespace ast;

        struct eval_call_visitor : boost::static_visitor<expression> {
            environment &env;
            list::const_range args;

            eval_call_visitor(environment &env, list::const_range args) :
                    env(env), args(args) { }

            expression operator()(call const &call) const {

                auto evaluated_args = std::vector<expression>(std::distance(begin(args), end(args)));

                std::transform(begin(args), end(args), begin(evaluated_args),
                               [this](expression const &v) { return execute(v, env); });

                return call(env, begin(evaluated_args), end(evaluated_args));
            }

            expression operator()(macro const &macro) const {
                return macro(env, begin(args), end(args));
            }

            template<typename T>
            expression operator()(T const &val) const {
                throw invalid_node{val, "call"};
            }
        };

        struct eval_visitor : boost::static_visitor<expression> {
            environment &env;

            eval_visitor(environment &env) : env(env) { }

            auto operator()(symbol const &sym) const -> expression {
                return env[sym];
            }

            template<typename T>
            auto operator()(literal <T> const &lit) const -> expression {
                return lit;
            }

            auto operator()(list const &cons) const -> expression {
                if (empty(cons))
                    throw invalid_node{cons, "non-empty list"};
                auto &&call = execute(head(cons), env);
                return boost::apply_visitor(eval_call_visitor{env, tail(cons)}, call);
            }

            template<typename T>
            auto operator()(T const &val) const -> expression {
                // Everything else is invalid.
                throw invalid_node{val};
            }
        };
    }

    expression execute(expression expr, environment &env) {
        return boost::apply_visitor(visitors::eval_visitor{env}, expr);;
    }
    
    


}
