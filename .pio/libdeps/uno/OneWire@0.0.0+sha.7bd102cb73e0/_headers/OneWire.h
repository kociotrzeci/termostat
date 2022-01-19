/* Copyright (c) 2012, Ivan Kravets <me@ikravets.com>, www.ikravets.com. MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "mbed.h"

#define FALSE 0
#define TRUE 1

#define ONEWIRE_DS18B20_FAMILY 0x28

enum OneWireDeviceCmd {
    SET_TEMPERATURE_RESOLUTION = 0,
    GET_TEMPERATURE
};

typedef uint8_t DeviceAddress[8];
typedef DeviceAddress*  DeviceAddresses;

class OneWire {
public:
    OneWire(PinName pin);

    uint8_t reset(void);
    uint8_t readByte(void);
    void writeByte(uint8_t val);
    void select(uint8_t* devAddr);
    void resetSearch(void);

    uint8_t search(uint8_t* devAddr);
    void sendGlobalCommand(uint8_t);
    DeviceAddresses* getFoundDevAddresses();
    uint8_t getFoundDevNum();

private:
    DigitalInOut DIO;
    static const uint8_t dscrc_table[];

    DeviceAddresses* foundDevAdresses;
    uint8_t foundDevNum;

    uint8_t lastDiscrep;
    uint8_t doneFlag;
    uint8_t dowcrc;

    uint8_t readBit(void);
    void writeBit(uint8_t bitval);

    void skip();
    uint8_t calcCRC(uint8_t x);
    void findDevAddresses(void);
};

#include "OneWireDevice.h"

#endif