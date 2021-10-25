#include <M5Atom.h>

#include "common.h"
#include "ntp/NTPController.h"
#include "units/rtc/RTCUnit.h"

const CRGB CRGB_STARTED(0xff, 0xff, 0xff);
const CRGB CRGB_CONNECTING(0x0, 0xff, 0xff);
const CRGB CRGB_CONNECTED(0x00, 0xff, 0x00);
const CRGB CRGB_DISCONNECTED(0xff, 0x00, 0x00);

const bool ENABLE_SERIAL = true;
const bool ENABLE_I2C = true;
const bool ENABLE_DISPLAY = true;

const int SDA_PIN = 26;
const int SCL_PIN = 32;

const char* NVS_NAMESPACE = "RTCUnit";
const char* NVS_CONFIG_PATH = "/settings.json";

NTPController ntpController(NVS_NAMESPACE);
RTCUnit rtcUnit;
struct tm info;

#if defined(ENABLE_SERIAL_MONITOR)
inline void showDateTime(const char* prefix, const RTCDate& date,
                         const RTCTime& time) {
    static const char DAY_OF_WEEK[][4] = {"Sun", "Mon", "Tue", "Wed",
                                          "Thr", "Fri", "Sat"};
    SERIAL_PRINTF_LN("%s: %04d/%02d/%02d(%s) %02d:%02d:%02d", prefix, date.year,
                     date.month, date.date, DAY_OF_WEEK[date.weekday],
                     time.hour, time.minute, time.second);
}
#endif

void connectingCallback(uint8_t retries) {
    M5.dis.fillpix(retries % 2 == 0 ? CRGB::Black : CRGB_CONNECTING);
}

void setup(void) {
    M5.begin(ENABLE_SERIAL, ENABLE_I2C, ENABLE_DISPLAY);
    Wire1.begin(SDA_PIN, SCL_PIN);
    M5.dis.fillpix(CRGB_CONNECTING);

    if (!rtcUnit.begin(&Wire1)) {
        SERIAL_PRINTLN("Failed to initialize RTC Unit");
        while (true) {
            delay(100);
        }
    }
#if defined(ENABLE_SERIAL_MONITOR)
    RTCDate d;
    RTCTime t;
    rtcUnit.getDateTime(d, t);
    showDateTime("Initial RTC DateTime", d, t);
#endif

    if (ntpController.begin(NVS_CONFIG_PATH, connectingCallback)) {
        M5.dis.fillpix(CRGB_CONNECTED);
    } else {
        M5.dis.fillpix(CRGB_DISCONNECTED);
    }
}

void loop(void) {
    M5.update();
    if (ntpController.update(info)) {
        rtcUnit.update(info);
#if defined(ENABLE_SERIAL_MONITOR)
        RTCDate sysDate(info);
        RTCTime sysTime(info);
        showDateTime("NTP", sysDate, sysTime);
#endif
    }
#if defined(ENABLE_SERIAL_MONITOR)
    RTCDate rtcDate;
    RTCTime rtcTime;
    rtcUnit.getDateTime(rtcDate, rtcTime);
    showDateTime("RTC", rtcDate, rtcTime);
#endif
    delay(60 * 1000);
}