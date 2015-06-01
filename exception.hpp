#ifndef CW_YAL_EXCEPTION_HPP
#define CW_YAL_EXCEPTION_HPP

#include <stdexcept>
#include "ast.h"

namespace yal {
    struct error : std::logic_error {
        using std::logic_error::logic_error;
    };

    struct name_error : error {
        name_error(ast::symbol const& sym) : error{"Name not found: " + sym.repr} {}
    };

    struct invalid_node : error {
        invalid_node(ast::expression const& val)
                : error{"Unexpected value " + to_string(val) + " at this place"} {}

        invalid_node(ast::expression const& val, std::string const& expected)
                : error{"Invalid value " + to_string(val) + ", expected " + expected} {}
    };

    struct value_error : error {
        value_error(std::string const& str) : error{str} {}
    };
}

#endif //CW_YAL_EXCEPTION_HPP
