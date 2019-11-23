/* To build and execute this program:
	$ sudo apt install i2c-tools       (Not required)
	$ sudo apt install libi2c-dev
	$ g++ -o read_temp lsm9ds0_temperature.cpp -li2c
	$ ./read_temp 
*/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

extern "C" {
    #include <linux/i2c.h>
    #include <linux/i2c-dev.h>
    #include <i2c/smbus.h>
}

using namespace std;

#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW

#define OUT_TEMP_L_XM		0x05
#define WHO_AM_I_XM			0x0F
#define INT_CTRL_REG_M		0x12

#define CTRL_REG0_XM		0x1F
#define CTRL_REG1_XM		0x20
#define CTRL_REG2_XM		0x21
#define CTRL_REG3_XM		0x22
#define CTRL_REG4_XM		0x23
#define CTRL_REG5_XM		0x24
#define CTRL_REG6_XM		0x25
#define CTRL_REG7_XM		0x26

#define MAX_BUS 64

short temperature;
	
unsigned char xmAddress;
// I2CBus keeps track of the bus
int I2CBus;

//File to write/read from
int file;

void initI2C();
unsigned char I2CreadByte(unsigned char address, unsigned char subAddress);
unsigned char xmReadByte(unsigned char subAddress);
void readTemp();
unsigned short begin(int bus, unsigned char xmAddr);
void xmReadBytes(unsigned char subAddress, unsigned char * dest, unsigned char count);
void I2CreadBytes(unsigned char address, unsigned char subAddress, unsigned char * dest, unsigned char count);
void initAccel();
void initTemp();
void I2CwriteByte(unsigned char address, unsigned char subAddress, unsigned char data);
void xmWriteByte(unsigned char subAddress, unsigned char data);
	
int main() {
	int status = begin(1, LSM9DS0_XM);    /* 1 for /dev/i2c-1, 2 for /dev/i2c-2 */
	cout << "Status: " << status << endl;
	while (1) {
		readTemp();
		cout << "T: " << (21.0 + (float)temperature/8.) << endl;
		sleep(1);
	}
}

unsigned short begin(int bus, unsigned char xmAddr)
{
	I2CBus = bus;  		
	xmAddress = xmAddr;
	initI2C();					// Initialize I2C
	unsigned char xmTest = xmReadByte(WHO_AM_I_XM);	// Read the accel/mag WHO_AM_I register
	initTemp();
	return xmTest;
}

void initI2C()
{
	//Open I2C file as bidirectional
	char namebuf[MAX_BUS];
	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
	if ((file=open(namebuf, O_RDWR)) < 0) {
		cout << "Failed to open sensor on " << namebuf << " I2C Bus" << endl;
	}
}

void initTemp()
{	
	/* CTRL_REG5_XM enables temp sensor, sets mag resolution and data rate
	Bits (7-0): TEMP_EN M_RES1 M_RES0 M_ODR2 M_ODR1 M_ODR0 LIR2 LIR1
	TEMP_EN - Enable temperature sensor (0=disabled, 1=enabled)	 */
	xmWriteByte(CTRL_REG5_XM, 0x80); // enable temperature sensor
}

void readTemp()
{
	unsigned char temp[2]; // We'll read two chars from the temperature sensor into temp	
	xmReadBytes(OUT_TEMP_L_XM, temp, 2); // Read 2 chars, beginning at OUT_TEMP_L_M
	temperature = (((short) temp[1] << 12) | temp[0] << 4 ) >> 4; // Temperature is a 12-bit signed integer
}

unsigned char I2CreadByte(unsigned char address, unsigned char subAddress)
{
	if (ioctl(file, I2C_SLAVE, address) < 0) {
		cout << "I2C_SLAVE address " << address << " failed..." << endl;
	}
	return i2c_smbus_read_byte_data(file, subAddress);
}

unsigned char xmReadByte(unsigned char subAddress)
{
	// Whether we're using I2C or SPI, read a char using the
	// accelerometer-specific I2C address or SPI CS pin.
	return I2CreadByte(xmAddress, subAddress);
}

void I2CreadBytes(unsigned char address, unsigned char subAddress, unsigned char * dest, unsigned char count)
{  
	if (ioctl(file, I2C_SLAVE, address) < 0) {
		cout << "I2C_SLAVE address " << address << " failed..." << endl;
	}
	unsigned char sAddr = subAddress | 0x80; // or with 0x80 to indicate multi-read
	int result = i2c_smbus_read_i2c_block_data(file, sAddr, count, dest);
	if (result != count) {
		printf("Failed to read block from I2C");
	}
}

void xmReadBytes(unsigned char subAddress, unsigned char * dest, unsigned char count)
{
	// Whether we're using I2C or SPI, read multiple chars using the
	// accelerometer-specific I2C address or SPI CS pin.
	I2CreadBytes(xmAddress, subAddress, dest, count);
}

void xmWriteByte(unsigned char subAddress, unsigned char data)
{
	// Whether we're using I2C or SPI, write a char using the
	// accelerometer-specific I2C address or SPI CS pin.
	return I2CwriteByte(xmAddress, subAddress, data);
}

void I2CwriteByte(unsigned char address, unsigned char subAddress, unsigned char data)
{
	if (ioctl(file, I2C_SLAVE, address) < 0) {
		cout << "I2C_SLAVE address " << address << " failed..." << endl;
	}
	int result = i2c_smbus_write_byte_data(file, subAddress, data);
	if (result == -1) {
		printf("Failed to write byte to " + subAddress);
	}
}




