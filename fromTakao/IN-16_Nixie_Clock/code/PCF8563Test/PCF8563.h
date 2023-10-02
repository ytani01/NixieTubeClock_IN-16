#include <Wire.h>
#include <Arduino.h>

#define ADDR_PCF8563            0x51
#define REG_VL_SEC              0x02
#define REG_MIN                 0x03
#define REG_HOUR                0x04
#define REG_WEEK                0x06
#define REG_DAY                 0x05
#define REG_CENTURY_MONTH       0x07
#define REG_YEAR                0x08

#define CONTROL_STATUS_1 0x00
#define CONTROL_STATUS_2 0x01

#define UPDATE_INTERVAL_MIN 500

class DateTime{
    public:
        DateTime(){};
        DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayOfTheWeek);
        uint16_t year() const {return _y;}
        uint8_t month() const {return _mon;}
        uint8_t day() const {return _day;}
        uint8_t hour() const {return _h;}
        uint8_t minute() const {return _min;}
        uint8_t second() const {return _sec;}
        uint8_t dayOfTheWeek() const {return _dayw;};


    private:
        uint16_t _y;
        uint8_t _mon, _day, _h, _min, _sec, _dayw;
};

class PCF8563
{
private:
    /* data */

public:
    PCF8563(/* args */);
    void begin(TwoWire *wire);
    int getTime(DateTime *date);
    // DateTime getTime(void);
    void setTime(uint8_t h, uint8_t m, uint8_t s);
    void setTime(DateTime d);
    // void setDateTime(uint8_t h, uint8_t m, uint8_t s);
    void writeByte(uint8_t addr, uint8_t d);
    void writeBytes(uint8_t reg, uint8_t *d, uint8_t num);
    void writeRegister(uint8_t addr, uint8_t reg, uint8_t d);
    uint8_t readRegister(uint8_t addr, uint8_t reg);
    void readBytes(uint8_t reg, uint8_t *buf, uint8_t num);

    uint16_t year;
    uint8_t month, week, day, second, minute, hour;
    unsigned long lastUpdate;
    TwoWire *_wire;
};
