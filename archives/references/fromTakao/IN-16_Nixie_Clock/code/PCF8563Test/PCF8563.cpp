#include "PCF8563.h"

DateTime::DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t dayOfTheWeek){
    _y = year;
    _mon = month;
    _day = day;
    _h = hour;
    _min = minute;
    _sec = second;
    _dayw = dayOfTheWeek;
}

PCF8563::PCF8563(){
}

void PCF8563::begin(TwoWire *wire){
    _wire = wire;
    Serial.println("PCF8563 begin");
    // _wire->begin(pin_sda, pin_scl);
    delay(50); // wati until stabilize

    //Initialization
    writeRegister(ADDR_PCF8563, 0x0D, 0x82);
    writeRegister(ADDR_PCF8563, CONTROL_STATUS_1, 0x00);
    writeRegister(ADDR_PCF8563, CONTROL_STATUS_2, 0x00);

    uint8_t vl = readRegister(ADDR_PCF8563, REG_VL_SEC) & 0x80;
    if(vl){
        DateTime d = DateTime(2022, 10, 11, 11, 34, 40, 1);
        setTime(d);
        Serial.println("RTC: Blown-out Detected.");
    }

    // Serial.println("PCF8563 Initialization finished.");
}

void PCF8563::writeByte(uint8_t addr, uint8_t d){
    _wire->beginTransmission(addr);
    _wire->write(d);
    // _wire->endTransmission();
    uint8_t ret = _wire->endTransmission();
    // if (ret)
    // {
    //     Serial.print("Write failed. Code = ");
    //     Serial.println(ret);
    // }

}

void PCF8563::writeBytes(uint8_t reg, uint8_t *d, uint8_t num){
    _wire->beginTransmission(ADDR_PCF8563);
    _wire->write(reg);
    for (uint8_t i = 0; i < num; i++)
    {
        _wire->write(d[i]);
    }
    // _wire->endTransmission();
    uint8_t ret = _wire->endTransmission();
    // if (ret)
    // {
    //     Serial.print("writeBytes() failed. Code = ");
    //     Serial.println(ret);
    // }
}

void PCF8563::readBytes(uint8_t reg, uint8_t *buf, uint8_t num){
    _wire->beginTransmission(ADDR_PCF8563);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom((uint8_t)ADDR_PCF8563, (byte)num);
    for (uint8_t i = 0; i < num; i++)
    {
        if(!_wire->available()) break;
        buf[i] = _wire->read();
    }

    while (_wire->available())
    {
        _wire->read();
    }
}

void PCF8563::writeRegister(uint8_t addr, uint8_t reg, uint8_t d){
  _wire->beginTransmission(ADDR_PCF8563);
  _wire->write(reg);
  _wire->write(d);
    uint8_t ret = _wire->endTransmission();
    // if (ret)
    // {
    //     Serial.print("writeRegister failed. Code = ");
    //     Serial.println(ret);
    // }
}

uint8_t PCF8563::readRegister(uint8_t addr, uint8_t reg){
    writeByte(addr, reg);
    _wire->requestFrom(addr, (byte)1);
    return _wire->read();
}

void PCF8563::setTime(uint8_t h, uint8_t m, uint8_t s){
    writeByte(ADDR_PCF8563, REG_VL_SEC);
    uint8_t buf[3];
    buf[0] = (s / 10) << 4 | (s % 10);
    buf[1] = (m / 10) << 4 | (m % 10);
    buf[2] = (h / 10) << 4 | (h % 10);
}

void PCF8563::setTime(DateTime d){

    writeByte(ADDR_PCF8563, REG_VL_SEC);
    uint8_t buf[7];
    buf[0] = (d.second() / 10) << 4 | (d.second() % 10);
    buf[1] = (d.minute() / 10) << 4 | (d.minute() % 10);
    buf[2] = (d.hour() / 10) << 4 | (d.hour() % 10);
    buf[3] = (d.day() / 10) << 4 | (d.day() % 10);
    buf[4] = d.dayOfTheWeek();
    buf[5] = (d.month() / 10) << 4 | (d.month() % 10);
    buf[6] = (d.year() % 100 / 10) << 4 | (d.year() % 10);

    writeBytes(REG_VL_SEC, buf, 7);
}

int PCF8563::getTime(DateTime *date){
    writeByte(ADDR_PCF8563, REG_VL_SEC);

    uint8_t r[7];
    readBytes(REG_VL_SEC, r, 7);

    uint16_t year = (r[6] >> 4) * 10 + (r[6] & 0x0F);
    uint8_t month = ((r[5] & 0x1F) >> 4) * 10 + (r[5] & 0x0F);
    uint8_t week = r[4] & 0x07;
    uint8_t day = ((r[3] & 0x3F) >> 4) * 10 + (r[3] & 0x0F);
    uint8_t hour = ((r[2] & 0x3F) >> 4) * 10 + (r[2] & 0x0F);
    uint8_t min = ((r[1] & 0x7F) >> 4) * 10 + (r[1] & 0x0F);
    uint8_t sec = ((r[0] & 0x7F) >> 4) * 10 + (r[0] & 0x0F);

    *date = DateTime(year, month, day, hour, min, sec, week);

  while (_wire->available())
  {
    //flush rx buffer
    _wire->read();
  }

//   *date = DateTime(year, month, day, hour, minute, second, week);
  return 0;
}

