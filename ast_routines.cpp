#include "ast_routines.h"
#include "exception.hpp"

#include <sstream>

namespace yal {
    namespace ast {

        ///////////////////////////////////////////////////////////////////////////
        // Explicit instantiation
        ///////////////////////////////////////////////////////////////////////////

        template struct callable<call_trait::call>;
        template struct callable<call_trait::macro>;

        template struct literal<bool>;
        template struct literal<double>;
        template struct literal<std::string>;

        ///////////////////////////////////////////////////////////////////////////
        // Definition
        ///////////////////////////////////////////////////////////////////////////

        symbol as_symbol(expression const& value) {
            return boost::get<symbol>(value);
        }

        template <typename T>
        literal<T> as_literal(expression const& value) try {
            return boost::get<literal<T>>(value);
        }
        catch (boost::bad_get const& err) {
            throw value_error{"Invalid argument type"};
        }

        template literal<bool> as_literal<bool>(expression const&);
        template literal<double> as_literal<double>(expression const&);
        template literal<std::string> as_literal<std::string>(expression const&);

        template <typename T>
        literal<T>  as_literal(T const& value) {
            return {value};
        }

        template literal<bool> as_literal<bool>(bool const&);
        template literal<double> as_literal<double>(double const&);
        template literal<std::string> as_literal<std::string>(std::string const&);

        template <typename T>
        T as_raw(literal<T> const& value) {
            return value.value;
        }

        template bool as_raw<bool>(literal<bool> const&);
        template double as_raw<double>(literal<double> const&);
        template std::string as_raw<std::string>(literal<std::string> const&);

        template <typename T>
        T as_raw(expression const& value) {
            return as_raw(as_literal<T>(value));
        }

        template bool as_raw<bool>(expression const&);
        template double as_raw<double>(expression const&);
        template std::string as_raw<std::string>(expression const&);

        list as_list(expression const& value) {
            return boost::get<list>(value);
        }

        std::string to_string(expression const& val) {
            std::ostringstream ostr;
            ostr << val;
            return ostr.str();
        }

        std::ostream& operator <<(std::ostream& out, symbol const& sym) {
            return out << sym.repr;
        }

        template <>
        std::ostream& operator << <bool>(std::ostream& out, literal<bool> const& lit) {
            return out << (as_raw(lit) ? "#t" : "#f");
        }

        std::ostream& operator <<(std::ostream& out, macro const&)  {
            return out << "#macro";
        }

        std::ostream& operator <<(std::ostream& out, call const&) {
            return out << "#call";
        }

        std::ostream& operator <<(std::ostream& out, list const& list) {
            out << '(';
            auto first = true;
            for (auto&& value : values(list)) {
                if (first) first = false;
                else out << ' ';
                out << value;
            }
            return out << ')';
        }

        list const nil{symbol{"quote"}, list{}};

        struct is_true_visitor : boost::static_visitor<bool> {
            bool operator ()(literal<bool> const& lit) const {
                return as_raw(lit);
            }

            bool operator ()(list const& list) const {
                return not empty(list);
            }

            template <typename T>
            bool operator ()(T const&) const {
                return false;
            }
        };
        bool is_true(expression const &value) {
            return boost::apply_visitor(is_true_visitor{}, value);
        }



    }
}