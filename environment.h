#ifndef CW_YAL_ENVIERONMENT_H
#define CW_YAL_ENVIERONMENT_H

#include "ast.h"
#include "ast_routines.h"
#include <unordered_map>

namespace yal {

    struct environment {
        environment(environment* parent = nullptr) : parent{parent} {}

        environment(environment&, std::vector<ast::symbol>, ast::call::iterator a, ast::call::iterator b);

        ast::expression& operator [](ast::symbol const& sym);

        void register_symbol(ast::symbol const &sym, ast::expression val);

        friend environment*get_parent(environment const &);

    private:
        std::unordered_map<ast::symbol, ast::expression> frame;
        environment* parent;
    };

    environment make_environment(
            environment &,
            ast::call::iterator,
            ast::call::iterator,
            ast::formals_t
    );

    environment make_global_environment();

}
#endif //CW_YAL_ENVIERONMENT_H
