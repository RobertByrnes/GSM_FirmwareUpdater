#if not defined(MOCK_SPIFFS_H)
#define MOCK_SPIFFS_H

#include <Arduino.h>
#include <Emulator.h>
#include "MockFS.hpp"

namespace fs {

class SPIFFSFS : virtual public Emulator, public FS {
public:
    SPIFFSFS(FSImplPtr impl): FS(impl) {}
    virtual ~SPIFFSFS() {}

    bool begin(
        bool formatOnFail=false,
        const char * basePath="/spiffs",
        uint8_t maxOpenFiles=10,
        const char * partitionLabel=NULL
    ) {
        return this->mock<bool>("begin");
    }
    bool format() { return this->mock<bool>("format"); }
    size_t totalBytes() { return this->mock<size_t>("totalBytes"); }
    size_t usedBytes() { return this->mock<size_t>("usedBytes"); }
    void end() {}

private:
    char * partitionLabel_;
};

}

fs::FSImplPtr impl;
extern fs::SPIFFSFS SPIFFS(impl);

#endif
