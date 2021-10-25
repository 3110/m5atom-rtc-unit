#include "units/rtc/RTCUnit.h"

const uint8_t TIME_MASK[] = {
    0x7f,  // second
    0x7f,  // minute
    0x3f,  // hour
};

const uint8_t DATE_MASK[] = {
    0x3f,  // date
    0x07,  // weekday
    0x1f,  // month
    0xff,  // year
};

const uint8_t DATETIME_MASK[] = {
    TIME_MASK[0],  // second
    TIME_MASK[1],  // minute
    TIME_MASK[2],  // hour
    DATE_MASK[0],  // date
    DATE_MASK[1],  // weekday
    DATE_MASK[2],  // month
    DATE_MASK[3],  // year
};

RTCUnit::RTCUnit(uint8_t deviceAddress)
    : DEVICE_ADDRESS(deviceAddress), wire(nullptr) {
}

RTCUnit::~RTCUnit(void) {
    if (this->wire != nullptr) {
        this->wire = nullptr;
    }
}

bool RTCUnit::begin(TwoWire* wire) {
    if (wire == nullptr) {
        SERIAL_PRINTLN("wire is null");
        return false;
    }
    this->wire = wire;
    this->wire->begin();
    writeReg(0x00, 0x00);
    writeReg(0x01, 0x00);
    writeReg(0x0D, 0x00);
    return true;
}

bool RTCUnit::adjust(const struct tm& info) {
    RTCDate d(info);
    RTCTime t(info);
    return setDateTime(d, t);
}

bool RTCUnit::update(const struct tm& info) {
    return adjust(info);
}

bool RTCUnit::setDate(const RTCDate& date) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(0x05);
    this->wire->write(toBCD(date.date));
    this->wire->write(toBCD(date.weekday));
    if (date.year < 2000) {
        this->wire->write(toBCD(date.month) | 0x80);
    } else {
        this->wire->write(toBCD(date.month));
    }
    this->wire->write(toBCD(static_cast<uint8_t>(date.year % 100)));
    this->wire->endTransmission();
    return true;
}

bool RTCUnit::setTime(const RTCTime& time) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(0x02);
    this->wire->write(toBCD(time.second));
    this->wire->write(toBCD(time.minute));
    this->wire->write(toBCD(time.hour));
    this->wire->endTransmission();
    return true;
}

bool RTCUnit::setDateTime(const RTCDate& date, const RTCTime& time) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    if (!setDate(date)) {
        return false;
    }
    if (!setTime(time)) {
        return false;
    }
    return true;
}

bool RTCUnit::getDate(RTCDate& date) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    uint8_t buf[sizeof(DATE_MASK)] = {0};
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(0x05);
    this->wire->endTransmission(false);
    this->wire->requestFrom(this->DEVICE_ADDRESS, sizeof(buf));
    if (this->wire->available() != sizeof(buf)) {
        SERIAL_PRINTLN("requestFrom() failed");
        return false;
    }
    uint16_t yearOffset = buf[2] & 0x80 ? 1900 : 2000;
    for (size_t i = 0; i < sizeof(buf); ++i) {
        buf[i] = toBCD(this->wire->read() & DATE_MASK[i]);
    }
    date.date = buf[0];
    date.weekday = buf[1];
    date.month = buf[2];
    date.year = yearOffset + buf[3];
    return true;
}

bool RTCUnit::getTime(RTCTime& time) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    uint8_t buf[sizeof(TIME_MASK)] = {0};
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(0x02);
    this->wire->endTransmission(false);
    this->wire->requestFrom(this->DEVICE_ADDRESS, sizeof(buf));
    if (this->wire->available() != sizeof(buf)) {
        SERIAL_PRINTLN("requestFrom() failed");
        return false;
    }
    for (size_t i = 0; i < sizeof(buf); ++i) {
        buf[i] = toBCD(this->wire->read() & TIME_MASK[i]);
    }
    time.second = buf[0];
    time.minute = buf[1];
    time.hour = buf[2];
    return true;
}

bool RTCUnit::getDateTime(RTCDate& date, RTCTime& time) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    uint8_t buf[sizeof(DATETIME_MASK)] = {0};
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(0x02);
    this->wire->endTransmission(false);
    this->wire->requestFrom(this->DEVICE_ADDRESS, sizeof(buf));
    if (this->wire->available() != sizeof(buf)) {
        SERIAL_PRINTLN("requestFrom() failed");
        return false;
    }
    uint16_t yearOffset = buf[2] & 0x80 ? 1900 : 2000;
    for (size_t i = 0; i < sizeof(buf); ++i) {
        buf[i] = toDecimal(this->wire->read() & DATETIME_MASK[i]);
    }
    time.second = buf[0];
    time.minute = buf[1];
    time.hour = buf[2];
    date.date = buf[3];
    date.weekday = buf[4];
    date.month = buf[5];
    date.year = buf[6] + yearOffset;
    return true;
}

bool RTCUnit::writeReg(uint8_t reg, uint8_t data) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(reg);
    this->wire->write(data);
    this->wire->endTransmission();
    return true;
}

bool RTCUnit::readReg(uint8_t reg, uint8_t& data) {
    if (this->wire == nullptr) {
        SERIAL_PRINTLN("this->wire is null");
        return false;
    }
    this->wire->beginTransmission(this->DEVICE_ADDRESS);
    this->wire->write(reg);
    this->wire->endTransmission(false);
    this->wire->requestFrom(this->DEVICE_ADDRESS, static_cast<uint8_t>(1));
    data = this->wire->read();
    return true;
}

uint8_t RTCUnit::toBCD(uint8_t dec) const {
    return ((dec / 10) << 4) | (dec % 10);
}

uint8_t RTCUnit::toDecimal(uint8_t bcd) const {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}
