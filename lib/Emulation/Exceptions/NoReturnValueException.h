// TODO move to package
#if not defined(NO_RETURN_VALUE_EXCEPTION_H)
#define NO_RETURN_VALUE_EXCEPTION_H

#include "Exception.h"

class NoReturnValueException : public Exception {
public:
    /** 
     * @brief Constructor (C strings).
     * 
     * @param message C-style string error message
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit NoReturnValueException(const char* message, const char *file, unsigned int line)
        : Exception(string(file) + ":" + to_string(line) + ":" + message)
        {
            this->msg_ = message;
            this->file_ = file;
            this->line_ = line;
        }

    /** 
     * @brief Constructor (C strings).
     * 
     * @param message C-style string error message
     * @param int exception code
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit NoReturnValueException(const char* message, int code, const char *file, unsigned int line)
        : Exception(string(file) + ":" + to_string(line) + ":" + message)
        {
            this->msg_ = message;
            this->code_ = code;
            this->file_ = file;
            this->line_ = line;
        }

    /** 
     * @brief Destructor. Virtual to allow for subclassing.
     */
    virtual ~NoReturnValueException() noexcept {}
};

#define NoReturnValueException(arg) throw NoReturnValueException(arg, __FILE__, __LINE__);
#define CodedNoReturnValueException(arg, code) throw NoReturnValueException(arg, code, __FILE__, __LINE__);

#endif
