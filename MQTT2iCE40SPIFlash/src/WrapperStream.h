/******************************************************************************
Copyright 2017 Gnarly Grey LLC
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include <Stream.h>
#include <SPI.h>


class WrapperStream: public Stream {

  public:
    WrapperStream();
    void begin(int size);
    
    virtual int available() {};
    virtual int read() {};
    virtual int peek() {};

    void flush();
    size_t write(uint8_t c);
};

// WinBond flash commands
#define WB_WRITE_ENABLE       0x06
#define WB_WRITE_DISABLE      0x04
#define WB_CHIP_ERASE         0xc7
#define WB_READ_STATUS_REG_1  0x05
#define WB_READ_DATA          0x03
#define WB_PAGE_PROGRAM       0x02
#define WB_BLOCK_ERASE        0x20
#define WB_JEDEC_ID           0x9f


#define SS  5  // A1 for IU board

void get_jedec_id(void);
void chip_erase(void);
void read_page(unsigned int page_number);
void write_byte(uint32_t page, uint8_t offset, uint8_t databyte);
void print_page_bytes(uint8_t *page_buffer);
void _get_jedec_id(uint8_t *b1, uint8_t *b2, uint8_t *b3);
void _chip_erase(void);
void _read_page(uint32_t page_number, uint8_t *page_buffer);
void _write_page(uint32_t page_number, uint8_t *page_buffer);
void write_bytex(uint32_t page, uint8_t offset, char databyte, bool mask);
void _read_pagex(uint32_t page_number, char *page_buffer);
void _write_pagex(uint32_t page_number, char *page_buffer);
void _erase_block(uint32_t page_number);
void not_busyx(void);
