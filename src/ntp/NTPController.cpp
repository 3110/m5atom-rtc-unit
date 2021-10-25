#include "ntp/NTPController.h"

#include <time.h>

#include "common.h"

const char KEY_NTP_SERVER[] = "ntp_server";

NTPController::NTPController(const char* nvsNamespace,
                             unsigned long updateIntervalMs, long gmtOffsetSec,
                             int daylightOffsetSec)
    : WiFiController(nvsNamespace),
      UPDATE_INTERVAL_MS(updateIntervalMs),
      GMT_OFFSET_SEC(gmtOffsetSec),
      DAYLIGHT_OFFSET_SEC(daylightOffsetSec),
      server{0} {
}

NTPController::~NTPController(void) {
}

bool NTPController::begin(const char* nvsConfigPath,
                          void (*connectingCallback)(uint8_t retries)) {
    if (WiFiController::begin(nvsConfigPath, connectingCallback) == false) {
        return false;
    }
    if (!getPreference(KEY_NTP_SERVER, this->server, sizeof(this->server))) {
        SERIAL_PRINTF_LN("%s is not set", KEY_NTP_SERVER);
        return false;
    }
    SERIAL_PRINTF_LN("Configure NTP server %s", this->server);
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, this->server);
    return true;
}

bool NTPController::update(struct tm& info, bool force) {
    if (!WiFiController::update()) {
        return false;
    }
    unsigned long now = millis();
    if (force || this->prevUpdatedMs == 0 ||
        now - this->prevUpdatedMs > this->UPDATE_INTERVAL_MS) {
        this->prevUpdatedMs = now;
        if (!getLocalTime(&info)) {
            SERIAL_PRINTLN("Failed to get local time");
            return false;
        } else {
            SERIAL_PRINTF_LN("Synchronize with %s", this->server);
            return true;
        }
    }
    return false;
}
