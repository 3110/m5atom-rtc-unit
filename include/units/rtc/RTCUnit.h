#ifndef RTC_UNIT_H_
#define RTC_UNIT_H_

#include <Wire.h>
#include <time.h>

#include "common.h"

struct RTCTime {
    RTCTime(uint8_t hour = 0, uint8_t minute = 0, uint8_t second = 0)
        : hour(hour), minute(minute), second(second) {
    }
    RTCTime(const struct tm& info)
        : hour(info.tm_hour), minute(info.tm_min), second(info.tm_sec) {
    }
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct RTCDate {
    RTCDate(uint16_t year = 0, uint8_t month = 0, uint8_t date = 0,
            uint8_t weekday = 0)
        : year(year), month(month), date(date), weekday(weekday) {
    }
    RTCDate(const struct tm& info)
        : year(info.tm_year + 1900),
          month(info.tm_mon + 1),
          date(info.tm_mday),
          weekday(info.tm_wday) {
    }
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t weekday;
};

class RTCUnit {
public:
    static constexpr uint8_t DEFAULT_DEVICE_ADDRESS = 0x51;

    RTCUnit(uint8_t deviceAddress = DEFAULT_DEVICE_ADDRESS);
    virtual ~RTCUnit(void);
    virtual bool begin(TwoWire* wire = &Wire);
    virtual bool update(const struct tm& info);
    virtual bool adjust(const struct tm& info);
    virtual bool setDate(const RTCDate& date);
    virtual bool setTime(const RTCTime& time);
    virtual bool setDateTime(const RTCDate& date, const RTCTime& time);
    virtual bool getDate(RTCDate& date);
    virtual bool getTime(RTCTime& time);
    virtual bool getDateTime(RTCDate& date, RTCTime& time);

protected:
    virtual bool writeReg(uint8_t reg, uint8_t data);
    virtual bool readReg(uint8_t reg, uint8_t& data);
    virtual uint8_t toBCD(uint8_t dec) const;
    virtual uint8_t toDecimal(uint8_t bcd) const;

private:
    uint8_t DEVICE_ADDRESS;
    TwoWire* wire;
};

#endif