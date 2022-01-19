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

#include "ds18b20.h"

void OneWireDeviceDS18B20::sendSetCommand(OneWireDeviceCmd cmd, uint8_t data) {
    switch (cmd) {
        case SET_TEMPERATURE_RESOLUTION:
            setResolution(data);
            break;
    }
}

float OneWireDeviceDS18B20::sendGetCommand(OneWireDeviceCmd cmd) {

    switch (cmd) {
        case GET_TEMPERATURE:
            return getTemperature();
    }

    return 0;
}

void OneWireDeviceDS18B20::setResolution(uint8_t res) {
    scratchPad[4] = res;
    writeScratchPad();
}

void OneWireDeviceDS18B20::readScratchPad() {
    owBus->reset();
    owBus->select(address);
    owBus->writeByte(0xBE); // Read EEPROM

    // TODO => collect all comments &  use simple loop
    // byte 0: temperature LSB
    // byte 1: temperature MSB
    // byte 2: high alarm temp
    // byte 3: low alarm temp
    // byte 4: DS18S20: store for crc
    //         DS18B20 & DS1822: configuration register
    // byte 5: internal use & crc
    // byte 6: DS18S20: COUNT_REMAIN
    //         DS18B20 & DS1822: store for crc
    // byte 7: DS18S20: COUNT_PER_C
    //         DS18B20 & DS1822: store for crc
    // byte 8: SCRATCHPAD_CRC
    //
    for (uint8_t i = 0; i <= 8; i++)
        scratchPad[i] = owBus->readByte();

    owBus->reset();
}

void OneWireDeviceDS18B20::writeScratchPad() {
    owBus->reset();
    owBus->select(address);

    owBus->writeByte(0x4E); // WRITESCRATCH
    owBus->writeByte(scratchPad[2]); // high alarm temp
    owBus->writeByte(scratchPad[3]); // low alarm temp    
    owBus->writeByte(scratchPad[4]); // configuration

    owBus->reset();

    // save the newly written values to eeprom
    owBus->writeByte(0x48); // COPYSCRATCH to eeprom
    owBus->reset();
}

float OneWireDeviceDS18B20::getTemperature() {
    __disable_irq();  
    readScratchPad();
    __enable_irq();
    
//    printf("LSB=%d, MSB=%d\r\n", scratchPad[0], scratchPad[1]);
    int16_t rawTemperature = (((int16_t)scratchPad[1]) << 8) | scratchPad[0];
//    printf("rawTemp=%d\r\n", rawTemperature);
//    printf("resolution=%d\r\n", scratchPad[4]);
    
    if ((rawTemperature>>8) & 0x80) { // Check if temperature is negative.
        rawTemperature = (rawTemperature ^ 0xFFFF) + 1;
        rawTemperature *= -1;
    }    
    
    switch (scratchPad[4])
    {
        case OWTEMP_12_BIT:
            return (float)rawTemperature * 0.0625;

        case OWTEMP_11_BIT:
            return (float)(rawTemperature >> 1) * 0.125;

        case OWTEMP_10_BIT:
            return (float)(rawTemperature >> 2) * 0.25;

        case OWTEMP_9_BIT:
            return (float)(rawTemperature >> 3) * 0.5;
    }
    
    return 0;
}