#include <exception>
#include <string>
#include <stdexcept>

using namespace std;

class Exception: public runtime_error
{
public:
    /** 
     * @brief Constructor (C strings).
     * 
     * @param message C-style string error message
     */
    explicit Exception(const char* message)
        : msg_(message), runtime_error(message) {}

    /** 
     * @brief Constructor (C strings).
     * 
     * @param message C-style string error message
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit Exception(const char* message, const char *file, unsigned int line)
        : msg_(message), file_(file), line_(line), runtime_error(string(file)+":"+to_string(line)+":"+message) {}

    /** 
     * @brief Constructor (C strings).
     * 
     * @param message C-style string error message
     * @param int exception code
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit Exception(const char* message, int code, const char *file, unsigned int line)
        : msg_(message), code_(code), file_(file), line_(line), runtime_error(string(file)+":"+to_string(line)+":"+message) {}

    /** 
     * @brief Constructor (C++ STL strings).
     * 
     * @param message The error message
     */
    explicit Exception(const string& message)
        : msg_(message), runtime_error(message) {}

    /** 
     * @brief Constructor (C++ STL strings).
     * 
     * @param message The error message
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit Exception(const string& message, const char *file, unsigned int line)
        : msg_(message), file_(file), line_(line), runtime_error(string(file)+":"+to_string(line)+":"+message) {}

    /** 
     * @brief Constructor (C++ STL strings).
     * 
     * @param message The error message
     * @param int exception code
     * @param file from __FILE__ macro
     * @param line from __LINE__ macro
     */
    explicit Exception(const string& message, int code, const char *file, unsigned int line)
        : msg_(message), code_(code), file_(file), line_(line), runtime_error(string(file)+":"+to_string(line)+":"+message) {}

    /** 
     * @brief Destructor. Virtual to allow for subclassing.
     */
    virtual ~Exception() noexcept {}

    /** 
     *  @brief Returns a pointer to the (constant) error description.
     * 
     *  @return A pointer to a const char*
     */
    virtual const char* what() const noexcept {
       return msg_.c_str();
    }

    /**
     * @brief Returns the user defined exception code.
     * 
     * @return int 
     */
    virtual int code() const noexcept {
        return code_;
    }

    /**
     * @brief Returns a pointer to the (constant) filename
     * where the exception was thrown.
     * 
     * @return const char* 
     */
    virtual const char* file() const noexcept {
        return file_.c_str();
    }

    /**
     * @brief Returns the line number where the exception was thrown.
     * 
     * @return unsigned int 
     */
    virtual unsigned int line() const noexcept {
        return line_;
    }

protected:
    /** Error message. */
    string msg_;
    /** File. */
   string file_;
    /** Line. */
   unsigned int line_;
    /** Code. */
   int code_;
};

#define Throw(arg) throw Exception(arg, __FILE__, __LINE__);
#define Throw(arg, code) throw Exception(arg, code, __FILE__, __LINE__);
