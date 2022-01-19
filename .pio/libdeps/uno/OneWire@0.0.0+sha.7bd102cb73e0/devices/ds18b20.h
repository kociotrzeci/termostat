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

#ifndef DS18B20_H
#define DS18B20_H

#include "OneWireDeviceTemperature.h"

// Device resolution
#define OWTEMP_9_BIT  0x1F //  9 bit
#define OWTEMP_10_BIT 0x3F // 10 bit
#define OWTEMP_11_BIT 0x5F // 11 bit
#define OWTEMP_12_BIT 0x7F // 12 bit

class OneWireDeviceDS18B20 : public OneWireDeviceTemperature {
public:
    OneWireDeviceDS18B20(OneWire* ow, DeviceAddress address) : OneWireDeviceTemperature(ow, address) {};
    virtual void sendSetCommand(OneWireDeviceCmd cmd, uint8_t data);
    virtual float sendGetCommand(OneWireDeviceCmd cmd);
    
private:
    typedef uint8_t ScratchPad[9];
    ScratchPad scratchPad;
    void setResolution(uint8_t res);
    void readScratchPad();
    void writeScratchPad();
    float getTemperature(void);
};

#endif