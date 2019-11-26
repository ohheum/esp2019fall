#include <stdio.h> 
#include <fcntl.h> 
#include <sys/ioctl.h> 
#include <linux/i2c.h> 
#include <linux/i2c-dev.h> 
#include <unistd.h>

int writeRegister(int file, unsigned int registerAddress, unsigned char value){
   unsigned char buffer[2];
   buffer[0] = registerAddress;
   buffer[1] = value;
   if(write(file, buffer, 2)!=2){
      perror("I2C: Failed write to the device\n");
      return 1;
   }
   return 0;
}

int main(){
    int file;
    printf("Starting the DS3231 test application\n"); 
    if((file=open("/dev/i2c-1", O_RDWR)) < 0){
        perror("failed to open the bus\n");
        return 1; 
    }
    if(ioctl(file, I2C_SLAVE, 0x1D) < 0){
      perror("Failed to connect to the sensor\n");
      return 1;
    }

    char writeBuffer[1] = {0x0F}; 
    if(write(file, writeBuffer, 1)!=1){
        perror("Failed to reset the read address\n");
        return 1; 
    }

    char buf[1];
    if(read(file, buf, 1)!=1){
        perror("Failed to read in the buffer\n");
        return 1; 
    }
    printf("WHO_AM_I: %d\n", buf[0]);

    writeRegister(file, 0x24, 0x80);

    unsigned char temp[2];
    char tempReg[] = {0x05, 0x06}; 
    for(int i = 0; i < 10; i++)
    {
        if(write(file, tempReg, 1)!=1){
            perror("Failed to reset the read address\n");
            return 1; 
        }   
        if(read(file, temp, 1)!=1){
            perror("Failed to read in the buffer\n");
            return 1; 
        }

        if(write(file, (tempReg+1), 1)!=1){
            perror("Failed to reset the read address\n");
            return 1; 
        }   
 
        if(read(file, (temp+1), 1)!=1){
            perror("Failed to read in the buffer\n");
            return 1; 
        }

        short temperature = (((short) temp[1] << 12) | temp[0] << 4 ) >> 4;
        printf("T: %lf\n", (21.0 + (double)temperature/8.));
        sleep(1);
    }
    close(file);
    return 0;
}
