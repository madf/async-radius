#ifndef ERROR_H
#define ERROR_H

#include <boost/system/error_code.hpp>

namespace RadProto
{
    class ErrorCategory: public boost::system::error_category
    {
        public:
            const char* name() const noexcept;
            std::string message(int ev) const noexcept;
    };

    enum class Error
    {
        success = 0,

        numberOfBytesIsLessThan20,
        requestLengthIsShort,
        eapMessageAttributeError
    };

    boost::system::error_code make_error_code(Error);
}

namespace boost
{
    namespace system
    {
        template <>
        struct is_error_code_enum<::RadProto::Error> : std::true_type
        {
        };
    }
}

#endif
