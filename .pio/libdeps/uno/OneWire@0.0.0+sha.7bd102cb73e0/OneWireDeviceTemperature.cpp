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

#include "OneWireDeviceTemperature.h"

void OneWireDeviceTemperature::startConversationForAll(OneWire* owBus, uint8_t resolution)
{
    DeviceAddresses* devAddresses = owBus->getFoundDevAddresses();
    for (uint8_t i = 0; i < owBus->getFoundDevNum(); i++) {
        OneWireDevice* owDevice = OneWireDeviceFactory::init(owBus, (*devAddresses)[i]);
        if (owDevice->getFamily() != ONEWIRE_DS18B20_FAMILY)
            continue;

        owDevice->sendSetCommand(SET_TEMPERATURE_RESOLUTION, resolution);
        delete owDevice;
    }
    //delete devAddresses;

//    owBus->sendGlobalCommand(0xcc); // Skip ROM command.
    owBus->sendGlobalCommand(0x44);
//    wait_us(750);

    switch (resolution) {
        case OWTEMP_9_BIT:
            wait_us(94);
            break;
        case OWTEMP_10_BIT:
            wait_us(188);
            break;
        case OWTEMP_11_BIT:
            wait_us(375);
            break;
        case OWTEMP_12_BIT:
        default:
            wait_us(750);
            break;
    }
};