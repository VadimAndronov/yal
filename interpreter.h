#ifndef CW_YAL_INTERPRETER_H
#define CW_YAL_INTERPRETER_H

#include "ast.h"
#include "environment.h"

namespace yal {
    using ast::expression;
    expression execute(expression expr, environment &env);

}
#endif //CW_YAL_INTERPRETER_H
