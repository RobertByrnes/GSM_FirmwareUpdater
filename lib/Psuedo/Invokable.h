#if not defined(INVOKABLE_H)
#define INVOKABLE_H

typedef struct {
    const char * methodName;
    void (*method)();
    int invoked;
    int waits;
} Invokable;

#endif
