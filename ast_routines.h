#ifndef CW_YAL_AST_ROUTINES_H
#define CW_YAL_AST_ROUTINES_H

#include "ast.h"


namespace yal {
    namespace ast {

        using values_t = std::vector<expression>;

        ///////////////////////////////////////////////////////////////////////////
        //  Inline routines
        ///////////////////////////////////////////////////////////////////////////

        inline bool operator==(symbol const &lhs, symbol const &rhs) {
            return lhs.repr == rhs.repr;
        }

        inline bool operator!=(symbol const &lhs, symbol const &rhs) {
            return lhs.repr != rhs.repr;
        }

        inline expression const & head(list const &list) {
            return list.values[0];
        }

        inline list::const_range tail(list const &list) {
            return { begin(list.values) + 1, end(list.values) };
        }


        inline bool empty(list const &list) {
            return list.values.empty();
        }

        inline std::size_t length(list const &list) {
            return list.values.size();
        }

        inline values_t const& values(list const &list) {
            return list.values;
        }

        inline auto begin(list const &list) -> decltype(begin(list.values)) {
            return begin(list.values);
        }

        inline auto end(list const &list) -> decltype(end(list.values)) {
            return end(list.values);
        }

        ///////////////////////////////////////////////////////////////////////////
        // Declarations
        ///////////////////////////////////////////////////////////////////////////

        template <typename T>
        literal<T> as_literal(expression const& value);

        template <typename T>
        literal<T> as_literal(T const& value);

        template <typename T>
        T as_raw(literal<T> const& value);

        template <typename T>
        T as_raw(expression const& value);

        symbol as_symbol(expression const& value);
        list as_list(expression const& value);
        bool is_true(expression const& value);
        std::string to_string(expression const& val);

        template <typename T>
        std::ostream& operator <<(std::ostream& out, literal<T> const& lit) {
            return out << lit.value;
        }

        std::ostream& operator <<(std::ostream& out, symbol const& sym);
        std::ostream& operator <<(std::ostream& out, macro const& macro);
        std::ostream& operator <<(std::ostream& out, call const& call);

        std::ostream& operator <<(std::ostream& out, list const& list);

        extern const list nil;

    } //ast
}

namespace std {
    template <>
    struct hash<::yal::ast::symbol> {
        size_t operator ()(::yal::ast::symbol const& value) const {
            return std::hash<decltype(value.repr)>{}(value.repr);
        }
    };
} // namespace std


#endif //CW_YAL_AST_ROUTINES_H
