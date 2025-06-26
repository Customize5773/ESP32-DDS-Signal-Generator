#ifndef WEB_INTERFACE_H
#define WEB_INTERFACE_H

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ad9833_controller.h"

class WebInterface {
public:
    WebInterface(AD9833Controller& dds);
    void begin();
    
private:
    AsyncWebServer server_;
    AD9833Controller& dds_;
    
    static const char WEB_INTERFACE_HTML[];
};

#endif
