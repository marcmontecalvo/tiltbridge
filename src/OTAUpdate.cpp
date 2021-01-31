#include "OTAUpdate.h"

#ifndef DISABLE_OTA_UPDATES

WiFiClient uClient;

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName)
{
    return header.substring(strlen(headerName.c_str()));
}

// OTA Logic
void execOTA()
{
    int contentLength = 0;
    bool isValidContentType = false;
#ifdef LCD_TFT
    String bin = "/firmware/tiltbridge/firmware_tft.bin"; // bin file name with a slash in front.
//#elif LCD_SSD1306
//    String bin = "/firmware/tiltbridge/firmware_ssd1306.bin"; // bin file name with a slash in front.
#else
    // This shouldn't ever happen - but if it does, die. We don't want to flash random firmware.
    return;
#endif

    // Connect to server
    // uClient.connect(host, port)
    if (uClient.connect("www.tiltbridge.com", WEBPORT)) {
        // Connection Succeed - fetch the bin
        uClient.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                     "Host: www.tiltbridge.com\r\n" +
                     "Cache-Control: no-cache\r\n" +
                     "Connection: close\r\n\r\n");

        unsigned long timeout = millis();
        while (uClient.available() == 0) {
            if (millis() - timeout > 5000) {
                uClient.stop();
                return;
            }
        }
        // Once the response is available, check it

        while (uClient.available()) {
            String line = uClient.readStringUntil('\n'); // read line till /n
            line.trim();                                // remove space, to check if the line is end of headers

            // if the the line is empty,
            // this is end of headers
            // break the while and feed the
            // remaining `uClient` to the
            // Update.writeStream();
            if (!line.length()) {
                //headers ended
                break; // and get the OTA started
            }

            // Check if the HTTP Response is 200
            // else break and Exit Update
            if (line.startsWith("HTTP/1.1")) {
                if (line.indexOf("200") < 0)
                {
                    break;
                }
            }

            // extract headers here
            // Start with content length
            if (line.startsWith("Content-Length: ")) {
                contentLength = atoi((getHeaderValue(line, "Content-Length: ")).c_str());
            }

            // Next, the content type
            if (line.startsWith("Content-Type: ")) {
                //                String contentType = getHeaderValue(line, "Content-Type: ");
                if (getHeaderValue(line, "Content-Type: ") == "application/octet-stream") {
                    isValidContentType = true;
                }
            }
        }
    } else {
        // Connection failed
    }

    // check contentLength and content type
    if (contentLength && isValidContentType) {
        // Check if there is enough to OTA Update
        bool canBegin = Update.begin(contentLength);

        // If yes, begin
        if (canBegin) {
            __attribute__((unused)) size_t written = Update.writeStream(uClient);

            //            if (written == contentLength) {
            //                //Succeeded
            //            } else {
            //                // Failed
            //            }

            if (Update.end()) {
                // OTA update finished
                if (Update.isFinished()) {
                    // Succeeded - restart
                    ESP.restart();
                } else {
                    // Failed
                }
            } else {
                // Error occured
            }
        } else {
            // not enough space to begin OTA - check the partition table
            uClient.flush();
        }
    } else {
        //        Serial.println("There was no content in the response");
        uClient.flush();
    }
}

#endif
