#include <iostream>
#include "ast.h"
#include "ast_routines.h"
#include "parser.h"
#include "environment.h"
#include "interpreter.h"

#include <iterator>

using yal::parser::parsed_t;
using yal::ast::expression;
using yal::environment;

int main(int argc, char* argv[]) {

    using std::begin;
    using std::end;

    environment env = yal::make_global_environment();
    std::string line;

    while (true) {
        std::cout << ">>> " << std::flush;
        if (!std::getline(std::cin, line)) {
            return 0;
        }

        try {
            parsed_t &&expr = yal::parser::parse_full(begin(line), end(line));
            if (!expr) { return 0; }
            expression &&result = yal::execute(*expr, env);
            std::cout << result << std::endl;
        }
        catch (std::exception const& err) {
            std::cerr << err.what() << std::endl;
        }
    }
    return 0;
}