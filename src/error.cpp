#include "error.h"
#include <string>

namespace lib
{
    const char* ErrorCategory::name() const noexcept
    {
        return "library";
    }

    std::string ErrorCategory::message(int ev) const noexcept
    {
        switch(static_cast<Error>(ev))
        {
            case Error::success:
                return "No error";
            case Error::numberBytesLess20:
                return "Request length less than 20 bytes";
            case Error::requestLengthShort:
                return "Request length less than specified in the request";
            case Error::eapMessageAttributeError:
                return "The EAP-Message attribute is present, but the Message-Authenticator attribute is missing";
            default:
                return "(Unrecognized error)";
        }
    }

    boost::system::error_category const& theCategory()
    {
        static const ErrorCategory instance;
        return instance;
    }

    boost::system::error_code make_error_code(Error e)
    {
        return boost::system::error_code(static_cast<int>(e), theCategory());
    }
}
