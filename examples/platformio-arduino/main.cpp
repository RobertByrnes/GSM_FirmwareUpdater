#define ESP_UART Serial
#define MODEM_UART Serial1

#include "SSLClient.h"
#include <modem.hpp>
#include <https.hpp>
#include <gsm_firmwareupdater.h>

const char apn[] = ""; 
const char gprs_user[] = ""; 
const char gprs_pass[] = "";
const char hostname[] = "www.example.com";
const char versionFileUrl[] = "/bin/firmware.txt";
const char binaryFileUrl[] = "/bin/firmware.bin";
const char simPin[] = "";
const char updateFilePath[] = "/update.bin";
std::string currentFirmwareVersion = "1.0.1";
int port = 443;

TinyGsm sim_modem(MODEM_UART);
TinyGsmClient gsm_transpor_layer(sim_modem);
SSLClient secure_presentation_layer(&gsm_transpor_layer);
HttpClient http_client = HttpClient(secure_presentation_layer, hostname, port);

GSM_FirmwareUpdater updateHandler(currentFirmwareVersion);

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

    if (!Modem::setupPMU()) { // Start board power management
        log_w("Setting board power management error");
    }

    MODEM_UART.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX); // Set SIM module baud rate and UART pins
    secure_presentation_layer.setCACert(root_ca);
    Modem::setupModem();
    log_i("Firmware: v%s", currentFirmwareVersion.c_str());
}

void loop()
{
    log_i("Initialising modem");
    if (!sim_modem.init()) {
        log_w("fail. Restarting modem");
        Modem::setupModem();
        if (!sim_modem.restart()) {
            log_e("failed. Even after restart");
            return;
        }
    }
    log_i("Modem OK");

    Modem::logModemInformation(sim_modem);
 
    if (strlen(simPin) && sim_modem.getSimStatus() != 3) { // Unlock SIM card with a PIN if needed
        sim_modem.simUnlock(simPin);
    }

    Modem::connect(sim_modem, apn, gprs_user, gprs_pass, MODEM_LED_PIN);
    Modem::logConnectionInformation(sim_modem);
    
    try {
        std::string response = HTTPS::get(sim_modem, http_client, versionFileUrl);
        http_client.stop();

        if (updateHandler.checkUpdateAvailable(response)) {
            if (HTTPS::download(sim_modem, http_client, binaryFileUrl, updateFilePath)) {
                log_i("Firmware on disk, attempting update");
                updateHandler.updateFromFS(updateFilePath);
            }
        }

        sim_modem.gprsDisconnect();
        log_i("GPRS disconnected");
        digitalWrite(MODEM_LED_PIN, LOW);
    } catch (uint8_t error) {
        log_e("GPRS disconnected during request, failed");
    }

    delay(15000);
}
