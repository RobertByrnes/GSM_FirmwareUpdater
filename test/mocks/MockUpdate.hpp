#if not defined(MOCK_UPDATE_H)
#define MOCK_UPDATE_H

#ifndef ESP8266UPDATER_H
#define ESP8266UPDATER_H

#include <Arduino.h>
// #include <MD5Builder.h>
#include <functional>
// #include "esp_partition.h"
#include <Emulator.h>

#define UPDATE_ERROR_OK                 (0)
#define UPDATE_ERROR_WRITE              (1)
#define UPDATE_ERROR_ERASE              (2)
#define UPDATE_ERROR_READ               (3)
#define UPDATE_ERROR_SPACE              (4)
#define UPDATE_ERROR_SIZE               (5)
#define UPDATE_ERROR_STREAM             (6)
#define UPDATE_ERROR_MD5                (7)
#define UPDATE_ERROR_MAGIC_BYTE         (8)
#define UPDATE_ERROR_ACTIVATE           (9)
#define UPDATE_ERROR_NO_PARTITION       (10)
#define UPDATE_ERROR_BAD_ARGUMENT       (11)
#define UPDATE_ERROR_ABORT              (12)

#define UPDATE_SIZE_UNKNOWN 0xFFFFFFFF

#define U_FLASH   0
#define U_SPIFFS  100
#define U_AUTH    200

#define ENCRYPTED_BLOCK_SIZE 16

#define SPI_SECTORS_PER_BLOCK   16      // usually large erase block is 32k/64k
#define SPI_FLASH_BLOCK_SIZE    (SPI_SECTORS_PER_BLOCK*SPI_FLASH_SEC_SIZE)

class MockUpdate : public Emulator {
public:
	MockUpdate() {}
	~MockUpdate() {}

    typedef std::function<void(size_t, size_t)> THandlerFunction_Progress;

    /*
      This callback will be called when Update is receiving data
    */
    MockUpdate& onProgress(THandlerFunction_Progress fn) { return this->mock<MockUpdate&>("onProgress"); }

    /*
      Call this to check the space needed for the update
      Will return false if there is not enough space
    */
    bool begin(size_t size=UPDATE_SIZE_UNKNOWN, int command = U_FLASH, int ledPin = -1, uint8_t ledOn = LOW, const char *label = NULL) { return this->mock<bool>("begin"); }

    /*
      Writes a buffer to the flash and increments the address
      Returns the amount written
    */
    size_t write(uint8_t *data, size_t len) { return this->mock<size_t>("write"); }

    /*
      Writes the remaining bytes from the Stream to the flash
      Uses readBytes() and sets UPDATE_ERROR_STREAM on timeout
      Returns the bytes written
      Should be equal to the remaining bytes when called
      Usable for slow streams like Serial
    */
    size_t writeStream(Stream &data) { return this->mock<size_t>("writeStream"); }

    /*
      If all bytes are written
      this call will write the config to eboot
      and return true
      If there is already an update running but is not finished and !evenIfRemaining
      or there is an error
      this will clear everything and return false
      the last error is available through getError()
      evenIfRemaining is helpfull when you update without knowing the final size first
    */
    bool end(bool evenIfRemaining = false) { return this->mock<bool>("end"); }

    /*
      Aborts the running update
    */
    void abort() {}

    /*
      Prints the last error to an output stream
    */
    void printError(Print &out) {}

    const char * errorString() { return this->mock<const char *>("errorString"); }

    /*
      sets the expected MD5 for the firmware (hexString)
    */
    bool setMD5(const char * expected_md5) { return this->mock<bool>("setMD5"); }

    /*
      returns the MD5 String of the successfully ended firmware
    */
    // String md5String(void){ return _md5.toString(); }
	// TODO create this mock

    /*
      populated the result with the md5 bytes of the successfully ended firmware
    */
    // void md5(uint8_t * result){ return _md5.getBytes(result); }
	// TODO create this mock

    //Helpers
    uint8_t getError(){ return _error; }
    void clearError(){ _error = UPDATE_ERROR_OK; }
    bool hasError(){ return _error != UPDATE_ERROR_OK; }
    bool isRunning(){ return _size > 0; }
    bool isFinished(){ return _progress == _size; }
    size_t size(){ return _size; }
    size_t progress(){ return _progress; }
    size_t remaining(){ return _size - _progress; }

    /*
      Template to write from objects that expose
      available() and read(uint8_t*, size_t) methods
      faster than the writeStream method
      writes only what is available
    */
    template<typename T>
    size_t write(T &data) { return this->mock<size_t>("write"); }

    /*
      check if there is a firmware on the other OTA partition that you can bootinto
    */
    bool canRollBack() { return this->mock<bool>("canRollBack"); }
    /*
      set the other OTA partition as bootable (reboot to enable)
    */
    bool rollBack() { return this->mock<bool>("rollBack"); }

  private:
    void _reset() {}
    void _abort(uint8_t err) {}
    bool _writeBuffer() { return this->mock<bool>("_writeBuffer"); }
    bool _verifyHeader(uint8_t data) { return this->mock<bool>("_verifyHeader"); }
    bool _verifyEnd() { return this->mock<bool>("_verifyEnd"); }
	// TODO create the following mocks
    // bool _enablePartition(const esp_partition_t* partition) { return this->mock<bool>("_enablePartition"); } 
    // bool _chkDataInBlock(const uint8_t *data, size_t len) const { return this->mock<bool>("_chkDataInBlock"); }    // check if block contains any data or is empty


    uint8_t _error;
    uint8_t *_buffer;
    uint8_t *_skipBuffer;
    size_t _bufferLen;
    size_t _size;
    THandlerFunction_Progress _progress_callback;
    uint32_t _progress;
    uint32_t _paroffset;
    uint32_t _command;
    // const esp_partition_t* _partition;

    String _target_md5;
    // MD5Builder _md5;

    int _ledPin;
    uint8_t _ledOn;
};

extern MockUpdate Update;

#endif
#endif
