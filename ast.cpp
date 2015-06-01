#include "ast.h"
#include "environment.h"

namespace yal {
    namespace ast {

        template<call_trait C>
        callable<C>::callable(
                environment &parent,
                std::vector<symbol> const &formals,
                typename callable<C>::function_type lambda)
                : parent{&parent}, formals{formals}, lambda{lambda} { }

        template<call_trait C>
        callable<C>::callable(
                environment &parent,
                symbol const &arglist,
                typename callable<C>::function_type lambda)
                : parent{&parent}, formals{arglist}, lambda{lambda} { }


        template<call_trait C>
        expression callable<C>::operator()(
                environment &env,
                iterator begin,
                iterator end) const {
            auto &&frame = make_environment(env, begin, end, formals);
            return lambda(frame);
        }

        template struct callable<call_trait::macro>;
        template struct callable<call_trait::call>;

    }
}