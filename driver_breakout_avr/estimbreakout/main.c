/*
 * estimbreakout.c
 *
 * Created: 6/4/2019 1:07:39 PM
 * Author : raster
 */ 

#include <avr/io.h>

#include "addr_table.h"

#define F_CPU		20000000L // CPUclock speed 20MHz
#define F_CLKPER	6 // MCLKCTRLB.PDIV Prescaler Division
#define T_RISE		300 // T_rise electrical characteristic max ns
#define F_BAUD		400000L // I2C clock speed 400 kHz

#define DAC_ADDR	0xc1 // DAC default address

#define PULSE_DELAY 0x78

uint8_t addr;
uint16_t voltages[2] = {0x0000, 0x0000};

uint8_t readAddr() {
	ADC0.CTRLA = ADC_ENABLE_bm;
	ADC0.CTRLB = ADC_SAMPNUM1_bm | ADC_SAMPNUM2_bm;
	ADC0.CTRLC = ADC_SAMPCAP_bm | ADC_REFSEL0_bm | ADC_PRESC_gm;
	ADC0.CTRLD = ADC_INITDLY0_bm | ADC_INITDLY2_bm;
	ADC0.COMMAND = ADC_STCONV_bm;
	
	while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) {}
	uint16_t voltage = ADC0.RESH | ADC0.RESL;
	
	uint8_t index = sizeof(addr_ref)/sizeof(addr_ref[0]);
	
	while (index--) {
		if (addr_ref[index][0] >= voltage && addr_ref[index][1] <= voltage) {
			return index;
		}
	}
	return 0x00;
}

void initPORT() {
	PORTA.DIRSET |= 0x60;
	PORTB.DIRSET |= 0x0c;
}

void initTWI() {
	PORTB.DIRSET |= 0x03;
	
	uint32_t baud = ((( F_CPU / F_CLKPER )/F_BAUD) - (((( F_CPU / F_CLKPER )*T_RISE)/1000)/1000)/1000 - 10)/2;
	TWI0.MBAUD = (uint8_t)baud;
	TWI0.MCTRLA = TWI_QCEN_bm | TWI_ENABLE_bm;
	
	TWI0.MADDR = DAC_ADDR; // DAC default address
	
	while (!(TWI0.MSTATUS & TWI_WIF_bm)) {}
}

void initSPI() {
	PORTA.DIRSET |= 0x04;
	
	SPI0.CTRLA = SPI_MASTER_bm | SPI_ENABLE_bm;
}

#define P_RW_bm		0x80
#define P_CMD_bm	0x60
#define P_ADDR_bm	0x1f

void delayPulse() {
	uint8_t delay = PULSE_DELAY;
	while(delay--) {}
}

void CMD_IO(uint8_t rw) {
	SPI0.DATA = addr;
	
	waitSPI();
	
	uint8_t io_data = SPI0.DATA;
	
	if(rw) {
		if(io_data & 0x01) {
			PORTA.OUTSET = ( PORTA.OUT & 0x9f ) | 0x20;
		}
		if(io_data & 0x02) {
			PORTB.OUTSET = ( PORTB.OUT & 0xf3 ) | 0x04;
		}
		delayPulse();
		
		if(io_data & 0x01) {
			PORTA.OUTSET = ( PORTA.OUT & 0x9f ) | 0x40;
		}
		if(io_data & 0x02) {
			PORTB.OUTSET = ( PORTB.OUT & 0xf3 ) | 0x08;
		}
		delayPulse();
		
		if(io_data & 0x01) {
			PORTA.OUTSET = PORTA.OUT & 0x9f;
		}
		if(io_data & 0x02) {
			PORTB.OUTSET = PORTB.OUT & 0xf3;
		}
	}
}

void CMD_VOLTAGE(uint8_t rw, uint8_t channel) {
	
	SPI0.DATA = (uint8_t)(voltages[channel] >> 8);
	
	waitSPI();
	
	uint16_t volt = SPI0.DATA << 8;
	
	SPI0.DATA = (uint8_t)(voltages[channel] & 0xff);
	
	waitSPI();
		
	volt |= SPI0.DATA;
	
	if(rw) {
		voltages[channel] = volt;
		writeVoltage(channel, voltages[channel]);
	}
}

void waitSPI() {
	while(!(SPI0.INTFLAGS & SPI_RXCIE_bm)){}
}

void waitTWI() {
	while (!(TWI0.MSTATUS & TWI_WIF_bm)) {}
}

void ignoreSPI(uint8_t count) {
	while(count--) {
		waitSPI();
	}
}

void writeVoltage(uint8_t dac, uint16_t voltage) {
	TWI0.MDATA = (dac << 2) | 0x00; // Memory address/Read/Write Command
	
	waitTWI();
	
	TWI0.MDATA = (uint8_t)(voltage >> 8); // MSB
	
	waitTWI();
			
	TWI0.MDATA = (uint8_t)(voltage & 0xff); // LSB
	
	waitTWI();
}

int main(void)
{
	initPORT();
	addr = readAddr();
	initTWI();
	initSPI();
	
    while (1) 
    {
		waitSPI();
		
		uint8_t spi_data = SPI0.DATA;
		
		if ((spi_data & P_ADDR_bm) == addr) {
			switch ((spi_data & P_CMD_bm) >> 5) {
				case 0x01:
					CMD_IO((spi_data & P_RW_bm) >> 7);
					break;
				case 0x02:
					CMD_VOLTAGE((spi_data & P_RW_bm) >> 7, 0x00);
					break;
				case 0x03:
					CMD_VOLTAGE((spi_data & P_RW_bm) >> 7, 0x01);
					break;
			}
		} else {
			switch ((spi_data & P_CMD_bm) >> 5) {
				case 0x01:
					ignoreSPI(1);
					break;
				case 0x02:
					ignoreSPI(2);
					break;
				case 0x03:
					ignoreSPI(2);
					break;
			}
		}
    }
}

