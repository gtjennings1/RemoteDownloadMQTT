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
#include "WrapperStream.h"
#include <HardwareSerial.h>
#include <SPI.h>

int image_size = 0;
int count=0;
int page_count = 0;
char page_data[256];
char hexval_prev;
int block_prev=1;

WrapperStream::WrapperStream() { }

void WrapperStream::begin(int size) {
  image_size = size;
  count = 0;
  page_count=0;
  get_jedec_id();
}

void WrapperStream::flush() {
  Serial.flush();
}

size_t WrapperStream::write(uint8_t c) {
  char hexval = (char) c;
  if (count<=image_size) {
      if((hexval=='0') || (hexval=='1') || (hexval=='2') || (hexval=='3') || (hexval=='4') || (hexval=='5') || (hexval=='6') || (hexval=='7') || (hexval=='8') ||  (hexval=='9') || (hexval=='a') || (hexval=='b') || (hexval=='c') || (hexval=='d') || (hexval=='e') || (hexval=='f'))
      {
        //hexval=0x0;
        //if (count<8*32)
        //Serial.print((char) c,HEX);
        if     (hexval=='0') hexval=0x0;
        else if(hexval=='1') hexval=0x1;
        else if(hexval=='2') hexval=0x2;
        else if(hexval=='3') hexval=0x3;
        else if(hexval=='4') hexval=0x4;
        else if(hexval=='5') hexval=0x5;
        else if(hexval=='6') hexval=0x6;
        else if(hexval=='7') hexval=0x7;
        else if(hexval=='8') hexval=0x8;
        else if(hexval=='9') hexval=0x9;
        else if(hexval=='a') hexval=0xA;
        else if(hexval=='b') hexval=0xB;
        else if(hexval=='c') hexval=0xC;
        else if(hexval=='d') hexval=0xD;
        else if(hexval=='e') hexval=0xE;
        else if(hexval=='f') hexval=0xF;
        //if (count<8*32)
        //Serial.print((char) hexval,HEX);  
        //if (count==31)
        //Serial.print("\n");
                bool mask = page_count%2;
        //if(mask)
        //  write_bytex((page_count/2)/256, (page_count/2)%256, hexval+ (hexval_prev<<4), page_count%2);
        int offset = (page_count/2)%256;
        int page   = (page_count/2)/256;
        int block  = (page_count/2)/4096;

        if(block!=block_prev)
        {
          _erase_block(page);
		  //Serial.println(page);
        }
        if(mask)
          page_data[offset] = ( hexval   ) + (hexval_prev<<4) ;
        if((offset==255 && mask == 1))
		{
          _write_pagex(page, page_data);
			//Serial.println(count);
			//Serial.println(image_size);
		}
        if(count>=(image_size-11))//gj
        {
		  //Serial.println("last page!!!");
          //Serial.print("\n");
          //Serial.println(image_size);
		  //Serial.println(count);
          for(int j=0;j<255-offset;j++)
              page_data[offset+j+1] = 0xff; 
          _write_pagex(page, page_data);
		  //for(int j=0;j<20;j++)
		  //   Serial.print((char) page_data[j],HEX);
		  // Serial.print("\n");
        }
        hexval_prev = hexval;
        block_prev=block;
        page_count++;
		
	  //if (count>315920)
	  //Serial.println(count);
      }

      count++;

 }
  
      
return 1;
}

////////////need to move to own file!!!//////////////////////
void get_jedec_id(void) {
  Serial.println("command: get_jedec_id");
  uint8_t b1, b2, b3;
  _get_jedec_id(&b1, &b2, &b3);
  char buf[128];
  sprintf(buf, "Manufacturer ID: %02xh\nMemory Type: %02xh\nCapacity: %02xh",
    b1, b2, b3);
  Serial.println(buf);
  Serial.println("Ready");
} 
void chip_erase(void) {
  Serial.println("command: chip_erase");
  _chip_erase();
  Serial.println("Ready");
}
void read_page(unsigned int page_number) {
  char buf[80];
  sprintf(buf, "command: read_page(%04xh)", page_number);
  Serial.println(buf);
  uint8_t page_buffer[256];
  _read_page(page_number, page_buffer);
  print_page_bytes(page_buffer);
  Serial.println("Ready");
}
void write_byte(uint32_t page, uint8_t offset, uint8_t databyte) {
  char buf[80];
  sprintf(buf, "command: write_byte(%04xh, %04xh, %02xh)", page, offset, databyte);
  Serial.println(buf);
  uint8_t page_data[256];
  _read_page(page, page_data);
  page_data[offset] = databyte;
  _write_page(page, page_data);
  Serial.println("Ready");
}
void print_page_bytes(uint8_t *page_buffer) {
  char buf[10];
  for (int i = 0; i < 16; ++i) {
    for (int j = 0; j < 16; ++j) {
      sprintf(buf, "%02x", page_buffer[i * 16 + j]);
      Serial.print(buf);
    }
    Serial.println();
  }
}
/*
================================================================================
Low-Level Device Routines
The functions below perform the lowest-level interactions with the flash device.
They match the timing diagrams of the datahsheet. They are called by wrapper 
functions which provide a little more feedback to the user. I made them stand-
alone functions so they can be re-used. Each function corresponds to a flash
instruction opcode.
================================================================================
*/

/*
 * See the timing diagram in section 9.2.35 of the
 * data sheet, "Read JEDEC ID (9Fh)".
 */
void _get_jedec_id(uint8_t *b1, uint8_t *b2, uint8_t *b3) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_JEDEC_ID);
  *b1 = SPI.transfer(0); // manufacturer id
  *b2 = SPI.transfer(0); // memory type
  *b3 = SPI.transfer(0); // capacity
  digitalWrite(SS, HIGH);
  not_busyx();
}  
/*
 * See the timing diagram in section 9.2.26 of the
 * data sheet, "Chip Erase (C7h / 06h)". (Note:
 * either opcode works.)
 */
void _chip_erase(void) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_CHIP_ERASE);
  digitalWrite(SS, HIGH);
  /* See notes on rev 2 
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_DISABLE);
  digitalWrite(SS, HIGH);
  */
  not_busyx();
}
/*
 * See the timing diagram in section 9.2.10 of the
 * data sheet, "Read Data (03h)".
 */
void _read_page(uint32_t page_number, uint8_t *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_DATA);
  // Construct the 24-bit address from the 16-bit page
  // number and 0x00, since we will read 256 bytes (one
  // page).
  SPI.transfer((page_number >> 8) & 0xFF);
  SPI.transfer((page_number >> 0) & 0xFF);
  SPI.transfer(0);
  for (int i = 0; i < 256; ++i) {
    page_buffer[i] = SPI.transfer(0);
  }
  digitalWrite(SS, HIGH);
  not_busyx();
}
/*
 * See the timing diagram in section 9.2.21 of the
 * data sheet, "Page Program (02h)".
 */
void _write_page(uint32_t page_number, uint8_t *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_PAGE_PROGRAM);
  SPI.transfer((page_number >>  8) & 0xFF);
  SPI.transfer((page_number >>  0) & 0xFF);
  SPI.transfer(0);
  for (int i = 0; i < 256; ++i) {
    SPI.transfer(page_buffer[i]);
  }
  digitalWrite(SS, HIGH);
  /* See notes on rev 2
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_DISABLE);
  digitalWrite(SS, HIGH);
  */
  not_busyx();
}
void write_bytex(uint32_t page, uint8_t offset, char databyte, bool mask) {
  char buf[80];
  sprintf(buf, "command: write_byte(%04xh, %04xh, %02xh)", page, offset, databyte);
  //Serial.println(buf);
  char page_data[256];
  _read_pagex(page, page_data);
  //if(mask)
  //  page_data[offset] = (databyte   ) + (page_data[offset]<<4) ;
  //else
  //  page_data[offset] = (databyte<<4) + (page_data[offset]>>4) ;
  page_data[offset] = (databyte   );
  //Serial.println(page_data[offset]);  
  _write_pagex(page, page_data);
  //Serial.println("Ready");
}


/*
 * See the timing diagram in section 9.2.10 of the
 * data sheet, "Read Data (03h)".
 */
void _read_pagex(uint32_t page_number, char *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_DATA);
  // Construct the 24-bit addreSS from the 16-bit page
  // number and 0x00, since we will read 256 bytes (one
  // page).
  SPI.transfer((page_number >> 8) & 0xFF);
  SPI.transfer((page_number >> 0) & 0xFF);
  SPI.transfer(0);
  for (int i = 0; i < 256; ++i) {
    page_buffer[i] = SPI.transfer(0);
  }
  digitalWrite(SS, HIGH);
  not_busyx();
}


/*
 * See the timing diagram in section 9.2.21 of the
 * data sheet, "Page Program (02h)".
 */
void _write_pagex(uint32_t page_number, char *page_buffer) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_PAGE_PROGRAM);
  SPI.transfer((page_number >>  8) & 0xFF);
  SPI.transfer((page_number >>  0) & 0xFF);
  SPI.transfer(0);
  for (int i = 0; i < 256; ++i) {
    SPI.transfer(page_buffer[i]);
    //Serial.println(page_buffer[i]);
  }
  digitalWrite(SS, HIGH);
  not_busyx();
}

void _erase_block(uint32_t page_number) {
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_WRITE_ENABLE);
  digitalWrite(SS, HIGH);
  digitalWrite(SS, LOW);  
  SPI.transfer(WB_BLOCK_ERASE);
  SPI.transfer((page_number >>  8) & 0xFF);
  SPI.transfer((page_number >>  0) & 0xFF);
  SPI.transfer(0);
  digitalWrite(SS, HIGH);
  not_busyx();
  
}

/* 
 * not_busy() polls the status bit of the device until it
 * completes the current operation. Most operations finish
 * in a few hundred microseconds or less, but chip erase 
 * may take 500+ms. Finish all operations with this poll.
 *
 * See section 9.2.8 of the datasheet
 */
void not_busyx(void) {
  digitalWrite(SS, HIGH);  
  digitalWrite(SS, LOW);
  SPI.transfer(WB_READ_STATUS_REG_1);       
  while (SPI.transfer(0) & 1) {}; 
  digitalWrite(SS, HIGH);  
}
