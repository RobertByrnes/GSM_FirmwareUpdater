// TODO move to package
#if not defined(INVOKABLE_H)
#define INVOKABLE_H

#include <any>
#include <vector>

typedef std::pair<int, std::any> RetVal; // times to return this value, default 1 / value to return

typedef struct {
    const char * methodName;
    RetVal retVal;
    std::vector<RetVal> then = {};
    int invoked = 0;
} MethodProfile;

#endif
