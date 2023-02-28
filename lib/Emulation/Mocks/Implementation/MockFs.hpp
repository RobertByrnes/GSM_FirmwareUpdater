#if not defined(MOCK_FS_H)
#define MOCK_FS_H

#include <Arduino.h>
#include <Emulator.h>
#include <memory>

namespace fs {

#define FILE_READ       "r"
#define FILE_WRITE      "w"
#define FILE_APPEND     "a"

class File;

class FileImpl;
typedef std::shared_ptr<FileImpl> FileImplPtr;
class FSImpl;
typedef std::shared_ptr<FSImpl> FSImplPtr;

enum SeekMode {
    SeekSet = 0,
    SeekCur = 1,
    SeekEnd = 2
};

class File : public Emulator, public Stream
{
public:
    File(FileImplPtr p = FileImplPtr()) : _p(p) {
        _timeout = 0;
    }

    size_t write(uint8_t) override { return this->mock<size_t>("write"); }
    size_t write(const uint8_t *buf, size_t size) override { return this->mock<size_t>("write"); }
    int available() override { return this->mock<int>("available"); }
    int read() override { return this->mock<int>("read"); }
    int peek() override { return this->mock<int>("peek"); }
    void flush() override {}
    size_t read(uint8_t* buf, size_t size) { return this->mock<size_t>("read"); }
    size_t readBytes(char *buffer, size_t length)
    {
        return read((uint8_t*)buffer, length);
    }

    bool seek(uint32_t pos, SeekMode mode) { return this->mock<bool>("seek"); };
    bool seek(uint32_t pos)
    {
        return seek(pos, SeekSet);
    }
    size_t position() { return this->mock<size_t>("position"); }
    size_t size() { return this->mock<size_t>("size"); }
    bool setBufferSize(size_t size) { return this->mock<bool>("setBufferSize"); }
    void close() {}
    operator bool() const;
    time_t getLastWrite() { return this->mock<time_t>("getLastWrite"); }
    const char* path() { return this->mock<const char*>("path"); }
    const char* name() { return this->mock<const char*>("path"); }

    boolean isDirectory(void) { return this->mock<boolean>("isDirectory"); }
    boolean seekDir(long position) { return this->mock<boolean>("seekDir"); }
    File openNextFile(const char* mode = FILE_READ) { return this->mock<File>("openNextFile"); }
    String getNextFileName(void) { return this->mock<String>("getNextFileName"); }
    void rewindDirectory(void) {}

protected:
    FileImplPtr _p;
};

class FS : virtual public Emulator
{
public:
    FS(FSImplPtr impl) : _impl(impl) {}

    File open(const char* path, const char* mode = FILE_READ, const bool create = false) { return this->mock<File>("open"); }
    File open(const String& path, const char* mode = FILE_READ, const bool create = false) { return this->mock<File>("open"); }

    bool exists(const char* path) { return this->mock<bool>("exists"); }
    bool exists(const String& path) { return this->mock<bool>("exists"); }

    bool remove(const char* path) { return this->mock<bool>("remove"); }
    bool remove(const String& path) { return this->mock<bool>("remove"); }

    bool rename(const char* pathFrom, const char* pathTo) { return this->mock<bool>("rename"); }
    bool rename(const String& pathFrom, const String& pathTo) { return this->mock<bool>("rename"); }

    bool mkdir(const char *path) { return this->mock<bool>("mkdir"); }
    bool mkdir(const String &path) { return this->mock<bool>("mkdir"); }

    bool rmdir(const char *path) { return this->mock<bool>("rmdir"); }
    bool rmdir(const String &path) { return this->mock<bool>("rmdir"); }


protected:
    FSImplPtr _impl;
};

} // namespace fs

#ifndef FS_NO_GLOBALS
using fs::FS;
using fs::File;
using fs::SeekMode;
using fs::SeekSet;
using fs::SeekCur;
using fs::SeekEnd;
#endif //FS_NO_GLOBALS

#endif
