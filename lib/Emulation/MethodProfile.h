// TODO move to package
#if not defined(INVOKABLE_H)
#define INVOKABLE_H

#include <any>
#include <vector>

typedef std::pair<int, std::any> RetVal; // times to return this value / value to return

typedef struct {
    const char * methodName;
    // void (*method)(); // TODO consider adding a pointer or rename type to MethodProfile
    RetVal retVal; // nTimes - default = 1/ retVal
    std::vector<RetVal> then = {};
    int invoked;
} MethodProfile;

#endif
