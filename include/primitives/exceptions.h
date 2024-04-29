#ifndef PRIMITIVES_EXCEPTIONS_H
#define PRIMITIVES_EXCEPTIONS_H
#include <exception>

class NotImplementedException
        : public std::exception {

public:
    // Construct with given error message:
    NotImplementedException(const char * error = "Functionality not yet implemented!")
    {
        errorMessage = error;
    }

    // Provided for compatibility with std::exception.
    const char * what() const noexcept
    {
        return errorMessage.c_str();
    }

private:

    std::string errorMessage;
};
#endif //PRIMITIVES_EXCEPTIONS_H
