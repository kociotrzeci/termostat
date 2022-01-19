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

#include "OneWire.h"

uint8_t const OneWire::dscrc_table[] = {
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
    190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
    219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
    87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

OneWire::OneWire(PinName pin) : DIO(pin)
{
    DIO.output();
    DIO = 0;
    DIO.input();

    reset();
    findDevAddresses();
}


uint8_t OneWire::reset(void)
{
    uint8_t presence;
    DIO.output();
    DIO = 0;
    wait_us(480);
    DIO.input();
    wait_us(70);
    presence = DIO;
    wait_us(410);
    return(presence);
}

uint8_t OneWire::readBit(void)
{
    uint8_t retval;
    wait_us(1);
    DIO.output();
    DIO = 0;
    wait_us(2);
    DIO.input();
    wait_us(10);
    retval=DIO;
    wait_us(48);
    return(retval);
}

uint8_t OneWire::readByte(void)
{
    uint8_t i, value=0;
    for (i=0; i<8; i++) {
        if (readBit())
            value |= 0x01<<i;
    }
    return value;
}

void OneWire::writeBit(uint8_t bitval)
{
    wait_us(1);
    DIO.output();
    DIO = 0;
    wait_us(10);
    if (bitval)
        DIO = 1;
    wait_us(50);
    DIO.input();
}

void OneWire::writeByte(uint8_t val)
{
    uint8_t i, temp;
    for (i=0; i<8; i++) {
        temp = val>>i;
        temp &= 0x01;
        writeBit(temp);
    }
}

uint8_t OneWire::calcCRC( uint8_t x)
{
    return dscrc_table[dowcrc^x];
}

void OneWire::skip()
{
    writeByte(0xcc);
}

void OneWire::select(uint8_t* devAddr)
{
    writeByte(0x55);
    for (uint8_t i=0; i<8; i++) {
        writeByte(devAddr[i]);
    }
}

void OneWire::sendGlobalCommand(uint8_t cmd)
{
    reset();
    skip();
    writeByte(cmd);
}

void OneWire::resetSearch(void)
{
    lastDiscrep = 0;
    doneFlag = FALSE;
}

uint8_t OneWire::search(uint8_t* devAddr)
{
    uint8_t m = 1; // ROM Bit index
    uint8_t n = 0; // ROM Byte index
    uint8_t k = 1; // bit mask
    uint8_t x = 0;
    uint8_t discrepMarker = 0; // discrepancy marker
    uint8_t g; // Output bit
    uint8_t nxt; // return value
    uint8_t flag;
    nxt = FALSE; // set the next flag to false
    dowcrc = 0; // reset the dowcrc
    DeviceAddress ROM;

    flag = reset(); // reset the 1-Wire
    if (flag||doneFlag) { // no parts -> return false
        lastDiscrep = 0; // reset the search
        return FALSE;
    }

    writeByte(0xF0); // send SearchROM command
    do { // for all eight bytes
        x = 0;
        if (readBit()) x = 2;
        wait_us(120);
        if (readBit()) x |= 1; // and its complement
        if (x ==3) // there are no devices on the 1-Wire
            break;
        else {
            if (x>0) // all devices coupled have 0 or 1
                g = x>>1; // bit write value for search
            else {
                // if this discrepancy is before the last discrepancy on a previous Next then pick the same as last time
                if (m<lastDiscrep)
                    g = ((ROM[n]&k)>0);
                else // if equal to last pick 1
                    g = (m==lastDiscrep); // if not then pick 0
                // if 0 was picked then record position with mask k
                if (g==0) discrepMarker = m;
            }
            if (g==1) // isolate bit in ROM[n] with mask k
                ROM[n] |= k;
            else
                ROM[n] &= ~k;
            writeBit(g); // ROM search write
            m++; // increment bit counter m
            k = k<<1; // and shift the bit mask k
            if (k==0) { // if the mask is 0 then go to new ROM // byte n and reset mask
                dowcrc = calcCRC(ROM[n]); // accumulate the CRC
                n++;
                k++;
            }
        }
    } while (n<8); //loop until through all ROM bytes 0-7
    if (m<65||dowcrc) // if search was unsuccessful then
        lastDiscrep=0; // reset the last discrepancy to 0
    else { // search was successful, so set lastDiscrep, lastOne, nxt
        lastDiscrep = discrepMarker;
        doneFlag = (lastDiscrep==0);
        nxt = TRUE; // indicates search is not complete yet, more parts remain
    }

    for (uint8_t i = 0; i < 8; i++)
        devAddr[i] = ROM[i];

    return nxt;
}

void OneWire::findDevAddresses(void)
{
    foundDevNum = 0;
    resetSearch();

    DeviceAddress devAddr;
    while (search(devAddr)) {
        foundDevNum++;
    }
    delete devAddr;

    wait_us(250);
    resetSearch();

    foundDevAdresses = (DeviceAddresses*) malloc(foundDevNum * sizeof(int));
    for (uint8_t i = 0; i < foundDevNum; i++) {
        foundDevAdresses[i] = (DeviceAddress*) malloc(sizeof(DeviceAddress));
        search((*foundDevAdresses)[i]);
    }

    printf("OneWire Bus: Found %d devices\r\n", getFoundDevNum() );
}

DeviceAddresses* OneWire::getFoundDevAddresses()
{
    return foundDevAdresses;
}

uint8_t OneWire::getFoundDevNum()
{
    return foundDevNum;
}