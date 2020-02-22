/*
 * addr_table.h
 * 
 * A reference table from ADC input to an address code
 * 
 * 5 DIP switch address (32 addresses) 5 Vref
 * Base resistor to GND 1K
 * Resistor Set bit 0 = 1K, bit 1 = 2k, bit 2 = 4k, bit 3 = 8k, bit 4 = 16k
 *
 * Created: 6/4/2019 1:08:21 PM
 *  Author: raster
 */ 


#ifndef ADDR_TABLE_H_
#define ADDR_TABLE_H_

static const uint16_t addr_ref[63][2] = {
	{ 0x0000, 0x0013 }, // 0x00
	{ 0x014c, 0x0156 }, // 0x01
	{ 0x00d8, 0x00ea }, // 0x02
	{ 0x0192, 0x0198 }, // 0x03
	{ 0x0079, 0x0093 }, // 0x04
	{ 0x0173, 0x017b }, // 0x05
	{ 0x011b, 0x0127 }, // 0x06
	{ 0x01ab, 0x01b0 }, // 0x07
	{ 0x003a, 0x005a }, // 0x08
	{ 0x0161, 0x0169 }, // 0x09
	{ 0x00fc, 0x010b }, // 0x0a
	{ 0x019f, 0x01a4 }, // 0x0b
	{ 0x00ad, 0x00c2 }, // 0x0c
	{ 0x0183, 0x018a }, // 0x0d
	{ 0x0135, 0x0140 }, // 0x0e
	{ 0x01b6, 0x01ba }, // 0x0f
	{ 0x0014, 0x0039 }, // 0x10
	{ 0x0157, 0x0160 }, // 0x11
	{ 0x00eb, 0x00fb }, // 0x12
	{ 0x0199, 0x019e }, // 0x13
	{ 0x0094, 0x00ac }, // 0x14
	{ 0x017c, 0x0182 }, // 0x15
	{ 0x0128, 0x0134 }, // 0x16
	{ 0x01b1, 0x01b5 }, // 0x17
	{ 0x005b, 0x0078 }, // 0x18
	{ 0x016a, 0x0172 }, // 0x19
	{ 0x010c, 0x011a }, // 0x1a
	{ 0x01a5, 0x01aa }, // 0x1b
	{ 0x00c3, 0x00d7 }, // 0x1c
	{ 0x018b, 0x0191 }, // 0x1d
	{ 0x0141, 0x014b }, // 0x1e
	{ 0x01bb, 0x03ff }  // 0x1f
};



#endif /* ADDR_TABLE_H_ */