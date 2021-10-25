#ifndef NTP_CONTROLLER_H_
#define NTP_CONTROLLER_H_

#include "wifi/WiFiController.h"

class NTPController : public WiFiController {
public:
    static constexpr size_t MAX_NTP_SERVER_LEN = 32;
    static constexpr long DEFAULT_GMT_OFFSET_SEC = 9 * 60 * 60;  // JST-9
    static constexpr int DEFAULT_DAYLIGHT_OFFSET_SEC = 0;
    static constexpr unsigned long DEFAULT_UPDATE_INTERVAL_MS =
        60 * 60 * 1000;  // 1 hour
    static constexpr size_t DEFAULT_DATETIME_FORMAT_LEN = 24;

    NTPController(const char* nvsNamespace,
                  unsigned long updateIntervalMs = DEFAULT_UPDATE_INTERVAL_MS,
                  long gmtOffsetSec = DEFAULT_GMT_OFFSET_SEC,
                  int daylightOffsetSec = DEFAULT_DAYLIGHT_OFFSET_SEC);
    virtual ~NTPController(void);

    virtual bool begin(const char* nvsConfigPath,
                       void (*connectingCallback)(uint8_t retries));
    virtual bool update(struct tm& info, bool force = false);

private:
    const unsigned long UPDATE_INTERVAL_MS;
    const long GMT_OFFSET_SEC;
    const int DAYLIGHT_OFFSET_SEC;
    char server[MAX_NTP_SERVER_LEN + 1];
    unsigned long prevUpdatedMs;
};

#endif