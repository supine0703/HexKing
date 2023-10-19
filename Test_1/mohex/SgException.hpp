#ifndef SGEXCEPTION_HPP
#define SGEXCEPTION_HPP

#include <exception>
#include <string>

//----------------------------------------------------------------------------

/** Base class for exceptions. */
class SgException : public std::exception
{
public:
    /** Construct with no message. */
    SgException();

    /** Construct with message. */
    SgException(const std::string& message);

    /** Destructor. */
    virtual ~SgException() throw();

    /** Implementation of std::exception::what(). */
    const char* what() const throw();

private:
    std::string m_message;
};

//----------------------------------------------------------------------------

inline SgException::SgException()
{
}

inline SgException::SgException(const std::string& message)
    : m_message(message)
{
}

inline SgException::~SgException() throw()
{
}

inline const char* SgException::what() const throw()
{
    return m_message.c_str();
}

//----------------------------------------------------------------------------


#endif // SGEXCEPTION_HPP
