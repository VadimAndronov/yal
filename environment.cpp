#include "environment.h"
#include "parser.h"
#include "exception.hpp"
#include "interpreter.h"

namespace yal {

    template<typename Iter>
    struct make_env_visitor : boost::static_visitor<environment> {
        environment &env;
        Iter begin;
        Iter end;

        make_env_visitor(environment &env, Iter begin, Iter end)
                : env(env), begin(begin), end(end) { }

        environment operator()(ast::symbol const &arglist) const {
            auto argpack = std::vector<ast::expression>{ast::list(begin, end)};
            return environment{env, {arglist}, argpack.begin(), argpack.end()};
        }

        environment operator()(std::vector<ast::symbol> const &formals) const {
            return environment{env, formals, begin, end};
        }
    };


    environment make_environment(
            environment & env,
            ast::call::iterator begin,
            ast::call::iterator end,
            ast::formals_t formals) {
        return boost::apply_visitor(make_env_visitor<ast::call::iterator>{env, begin, end}, formals);
    }


    environment *get_parent(environment const &aConst) {
        return aConst.parent;
    }

    void environment::register_symbol(ast::symbol const &sym, ast::expression val) {
        frame.emplace(sym, std::forward<ast::expression>(val));
    }


    environment::environment(environment &parent, std::vector<ast::symbol> formals, ast::call::iterator a,
                             ast::call::iterator b) : parent(&parent) {
        std::size_t argsize = formals.size();
        std::size_t paramsize = static_cast<decltype(formals.size())>(std::distance(a, b));
        if (argsize != paramsize)
            throw value_error{"Expected " + std::to_string(argsize) +
                              " arguments, got " + std::to_string(paramsize)};

        for (auto&& sym : formals)
            register_symbol(sym, *a++);
    }

    ast::expression &environment::operator[](ast::symbol const &sym) {
        for (auto current = this; current != nullptr; current = current->parent) {
            auto &&it = current->frame.find(sym);
            if (it != current->frame.end())
                return it->second;
        }
    }

    template <typename R, template<typename> class Op>
    struct binary_operation : boost::static_visitor<R> {
        template <typename T>
        auto operator ()(ast::literal<T> const& a, ast::literal<T> const& b) const -> R {
            return Op<T>{}(ast::as_raw(a), ast::as_raw(b));
        }

        template <typename T, typename U>
        auto operator ()(T const&, U const&) const -> R {
            throw value_error{"Mismatching or invalid operand types"};
        }
    };

    template <typename T>
    struct not_equal_to : std::binary_negate<std::equal_to<T>> {
        not_equal_to() : std::binary_negate<std::equal_to<T>>{std::equal_to<T>{}} {}
    };

    auto make_global_environment() -> environment {
        auto env = environment{};

        using namespace ast;

#   define REGISTER_UNARY_OPERATOR(name, op, type) \
    env.register_symbol(symbol{name}, \
        call{env, "args", [] (environment& env) { \
            auto&& args = ast::as_list(env["args"]); \
            return std::accumulate( \
                std::next(begin(args)), end(args), \
                *begin(args), [] (expression const& a, expression const& b) { \
                    return as_literal(as_raw<type>(a) op as_raw<type>(b)); \
                }); \
        }} \
    )

        REGISTER_UNARY_OPERATOR("+", +, double);
        REGISTER_UNARY_OPERATOR("-", -, double);
        REGISTER_UNARY_OPERATOR("*", *, double);
        REGISTER_UNARY_OPERATOR("/", /, double);
        REGISTER_UNARY_OPERATOR("and", &&, bool);
        REGISTER_UNARY_OPERATOR("or", ||, bool);

#   undef REGISTER_UNARY_OPERATOR

#   define REGISTER_BINARY_OPERATOR(name, op, ret) \
    env.register_symbol(symbol{name}, \
        call{env, {"a", "b"}, [] (environment& env) { \
            auto&& a = env["a"]; \
            auto&& b = env["b"]; \
            return as_literal(boost::apply_visitor(binary_operation<ret, op>{}, a, b)); \
        }} \
    )

        REGISTER_BINARY_OPERATOR("==", std::equal_to, bool);
        REGISTER_BINARY_OPERATOR("!=", not_equal_to, bool);
        REGISTER_BINARY_OPERATOR("<", std::less, bool);
        REGISTER_BINARY_OPERATOR(">", std::greater, bool);
        REGISTER_BINARY_OPERATOR("<=", std::less_equal, bool);
        REGISTER_BINARY_OPERATOR(">=", std::greater_equal, bool);

#   undef REGISTER_BINARY_OPERATOR

        env.register_symbol(symbol{"not"},
                            ast::call{env, std::vector<symbol>{"a"}, [](environment &env) {
                                return as_literal(not as_raw<bool>(env["a"]));
                            }}
        );

        env.register_symbol(symbol{"empty?"},
                            call{env, std::vector<symbol>{"a"}, [](environment &env) {
                                auto &&a = as_list(env["a"]);
                                return as_literal(empty(a));
                            }}
        );

        env.register_symbol(symbol{"length"},
                            call{env, std::vector<symbol>{"a"}, [](environment &env) {
                                auto &&a = as_list(env["a"]);
                                return as_literal<double>(length(a));
                            }}
        );

        env.register_symbol(symbol{"quote"},
                            macro{env, std::vector<symbol>{"expr"},
                                  [](environment &env) { return env["expr"]; }}
        );

        env.register_symbol(symbol{"lambda"},
                            macro{env, {"args", "expr"}, [](environment &env) {
                                auto &&args = as_list(env["args"]);
                                auto formals = std::vector<symbol>(length(args));
                                std::transform(begin(args), end(args), begin(formals), as_symbol);
                                auto expr = env["expr"];
                                return call{env, formals, [expr](environment &frame) {
                                    return execute(expr, frame);
                                }};
                            }}
        );

        env.register_symbol(symbol{"define"},
                            macro{env, {"name", "expr"}, [](environment &env) {
                                auto &&name = as_symbol(env["name"]);
                                get_parent(env)->register_symbol(name, execute(env["expr"], *get_parent(env)));
                                return execute(nil, env);
                            }}
        );

        env.register_symbol(symbol{"if"},
                            macro{env, {"cond", "conseq", "alt"}, [](environment &env) {
                                auto &&cond = execute(env["cond"], *get_parent(env));
                                return execute(is_true(cond) ? env["conseq"] : env["alt"], *get_parent(env));
                            }}
        );

        env.register_symbol(symbol{"set!"},
                            macro{env, {"name", "expr"}, [](environment &env) {
                                auto &&name = as_symbol(env["name"]);
                                (*get_parent(env))[name] = execute(env["expr"], *get_parent(env));
                                return execute(nil, env);
                            }}
        );

        env.register_symbol(symbol{"begin"},
                            macro{env, "args", [](environment &env) {
                                auto &&result = expression{};
                                for (auto &&expr : as_list(env["args"]))
                                    result = execute(expr, *get_parent(env));
                                return result;
                            }}
        );

        return env;
    }
}