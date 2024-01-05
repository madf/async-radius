#include "error.h"
#include <string>

using Exception = RadProto::Exception;
Exception::Exception(const boost::system::error_code& errorCode)
    : std::runtime_error(errorCode.message()),
      m_errorCode(errorCode)
{
}

using ErrorCategory = RadProto::ErrorCategory;
const char* ErrorCategory::name() const noexcept
{
    return "library";
}

using Error = RadProto::Error;
std::string ErrorCategory::message(int ev) const noexcept
{
    switch (static_cast<Error>(ev))
    {
        case Error::success:
            return "No error";
        case Error::numberOfBytesIsLessThan20:
            return "Request length is less than 20 bytes";
        case Error::requestLengthIsShort:
            return "Request length is less than specified in the request";
        case Error::eapMessageAttributeError:
            return "The EAP-Message attribute is present, but the Message-Authenticator attribute is missing";
        default:
            return "(Unrecognized error)";
    }
}

const boost::system::error_category& theCategory()
{
    static const ErrorCategory instance;
    return instance;
}

boost::system::error_code RadProto::make_error_code(Error e)
{
    return boost::system::error_code(static_cast<int>(e), theCategory());
}
