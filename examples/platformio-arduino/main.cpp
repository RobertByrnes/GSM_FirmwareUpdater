#define ESP_UART Serial
#define MODEM_UART Serial1

#include "SSLClient.h"
#include <modem.hpp>
#include <https.hpp>
#include <gsm_firmwareupdater.h>

const char apn[] = "giffgaff.com"; 
const char gprsUser[] = "gg"; 
const char gprsPass[] = "p";
const char hostname[] = "www.example.com";
const char versionFileUrl[] = "/bin/firmware.txt";
const char binaryFileUrl[] = "/bin/firmware.bin";
const char simPin[] = "";
const char updateFilePath[] = "/update.bin";
std::string currentFirmwareVersion = "1.0.1";
int port = 443;

TinyGsm modemDriver(MODEM_UART);
TinyGsmClient gsmTransportLayer(modemDriver);
SSLClient secureLayer(&gsmTransportLayer);
HttpClient httpClient = HttpClient(secureLayer, hostname, port);
Modem<TinyGsm> modemClass;
GSM_FirmwareUpdater updateHandler(currentFirmwareVersion);
HTTPS<TinyGsm, HttpClient> https;

// CA Certificate for lets encrypt (valid until Jun  4 11:04:38 2035 GMT)
const char root_ca[] PROGMEM =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIFFjCCAv6gAwIBAgIRAJErCErPDBinU/bWLiWnX1owDQYJKoZIhvcNAQELBQAw\n"
    "TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
    "cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMjAwOTA0MDAwMDAw\n"
    "WhcNMjUwOTE1MTYwMDAwWjAyMQswCQYDVQQGEwJVUzEWMBQGA1UEChMNTGV0J3Mg\n"
    "RW5jcnlwdDELMAkGA1UEAxMCUjMwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK\n"
    "AoIBAQC7AhUozPaglNMPEuyNVZLD+ILxmaZ6QoinXSaqtSu5xUyxr45r+XXIo9cP\n"
    "R5QUVTVXjJ6oojkZ9YI8QqlObvU7wy7bjcCwXPNZOOftz2nwWgsbvsCUJCWH+jdx\n"
    "sxPnHKzhm+/b5DtFUkWWqcFTzjTIUu61ru2P3mBw4qVUq7ZtDpelQDRrK9O8Zutm\n"
    "NHz6a4uPVymZ+DAXXbpyb/uBxa3Shlg9F8fnCbvxK/eG3MHacV3URuPMrSXBiLxg\n"
    "Z3Vms/EY96Jc5lP/Ooi2R6X/ExjqmAl3P51T+c8B5fWmcBcUr2Ok/5mzk53cU6cG\n"
    "/kiFHaFpriV1uxPMUgP17VGhi9sVAgMBAAGjggEIMIIBBDAOBgNVHQ8BAf8EBAMC\n"
    "AYYwHQYDVR0lBBYwFAYIKwYBBQUHAwIGCCsGAQUFBwMBMBIGA1UdEwEB/wQIMAYB\n"
    "Af8CAQAwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYfr52LFMLGMB8GA1UdIwQYMBaA\n"
    "FHm0WeZ7tuXkAXOACIjIGlj26ZtuMDIGCCsGAQUFBwEBBCYwJDAiBggrBgEFBQcw\n"
    "AoYWaHR0cDovL3gxLmkubGVuY3Iub3JnLzAnBgNVHR8EIDAeMBygGqAYhhZodHRw\n"
    "Oi8veDEuYy5sZW5jci5vcmcvMCIGA1UdIAQbMBkwCAYGZ4EMAQIBMA0GCysGAQQB\n"
    "gt8TAQEBMA0GCSqGSIb3DQEBCwUAA4ICAQCFyk5HPqP3hUSFvNVneLKYY611TR6W\n"
    "PTNlclQtgaDqw+34IL9fzLdwALduO/ZelN7kIJ+m74uyA+eitRY8kc607TkC53wl\n"
    "ikfmZW4/RvTZ8M6UK+5UzhK8jCdLuMGYL6KvzXGRSgi3yLgjewQtCPkIVz6D2QQz\n"
    "CkcheAmCJ8MqyJu5zlzyZMjAvnnAT45tRAxekrsu94sQ4egdRCnbWSDtY7kh+BIm\n"
    "lJNXoB1lBMEKIq4QDUOXoRgffuDghje1WrG9ML+Hbisq/yFOGwXD9RiX8F6sw6W4\n"
    "avAuvDszue5L3sz85K+EC4Y/wFVDNvZo4TYXao6Z0f+lQKc0t8DQYzk1OXVu8rp2\n"
    "yJMC6alLbBfODALZvYH7n7do1AZls4I9d1P4jnkDrQoxB3UqQ9hVl3LEKQ73xF1O\n"
    "yK5GhDDX8oVfGKF5u+decIsH4YaTw7mP3GFxJSqv3+0lUFJoi5Lc5da149p90Ids\n"
    "hCExroL1+7mryIkXPeFM5TgO9r0rvZaBFOvV2z0gp35Z0+L4WPlbuEjN/lxPFin+\n"
    "HlUjr8gRsI3qfJOQFy/9rKIJR0Y/8Omwt/8oTWgy1mdeHmmjk7j1nYsvC9JSQ6Zv\n"
    "MldlTTKB3zhThV1+XWYp6rjd5JW1zbVWEkLNxE7GJThEUG3szgBVGP7pSWTUTsqX\n"
    "nLRbwHOoq7hHwg==\n"
    "-----END CERTIFICATE-----\n";

    
void setup()
{
    ESP_UART.begin(115200);
    delay(100);

    if (!modemClass.setupPMU(Wire)) { // Start board power management
        log_w("Setting board power management error");
    }

    MODEM_UART.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); // Set SIM module baud rate and UART pins
    secureLayer.setCACert(root_ca);
    modemClass.setupModem();
    log_i("Firmware: v%s", currentFirmwareVersion.c_str());
}

void loop()
{
    log_i("Initialising modem");
    if (!modemDriver.init()) {
        log_w("fail. Restarting modem");
        modemClass.setupModem();
        if (!modemDriver.restart()) {
            log_e("failed. Even after restart");
            return;
        }
    }
    log_i("Modem OK");

    modemClass.logModemInformation(modemDriver);
 
    if (strlen(simPin) && modemDriver.getSimStatus() != 3) { // Unlock SIM card with a PIN if needed
        modemDriver.simUnlock(simPin);
    }

    modemClass.connect(modemDriver, apn, gprsUser, gprsPass, MODEM_LED_PIN);
    modemClass.logConnectionInformation(modemDriver);
    
    try {
        std::string response = https.get(modemDriver, httpClient, versionFileUrl);
        httpClient.stop();

        if (updateHandler.checkUpdateAvailable(response)) {
            if (https.download<fs::SPIFFSFS>(modemDriver, httpClient, SPIFFS, binaryFileUrl, updateFilePath)) {
                log_i("Firmware on disk, attempting update");
                updateHandler.updateFromFS(updateFilePath);
            }
        }

        modemDriver.gprsDisconnect();
        log_i("GPRS disconnected");
        digitalWrite(MODEM_LED_PIN, LOW);
    } catch (...) { // If for example a 404 response was returned
        log_e("Errored retrying...");
    }

    delay(5000);
}
