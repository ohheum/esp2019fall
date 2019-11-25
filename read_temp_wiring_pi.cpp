#include <iostream>
#include <errno.h>
#include <wiringPiI2C.h>
#include<unistd.h>

using namespace std;

int main()
{
   int fd = wiringPiI2CSetup(0x1D);
   if (fd < 0) {
      cout << "Falied to open I2C device. " << endl;
      return 1;
   }
   int who_am_i = wiringPiI2CReadReg8(fd, 0x0F);
   cout << "WHO_AM_I: " << who_am_i << endl;

   int result = wiringPiI2CWriteReg8(fd, 0x24, 0x80);
   if(result == -1)
   {
      cout << "Failed to enable temperature sensor." << endl;
      return 1;
   }

   for(int i = 0; i < 10; i++)
   {
      char temp[2];
      temp[0] = wiringPiI2CReadReg8(fd, 0x05);
      temp[1] = wiringPiI2CReadReg8(fd, 0x06);

      // int temp16 = wiringPiI2CReadReg16(fd, 0x05);
      // char *temp = (char *)(&temp16);

      int temperature = (((short) temp[1] << 12) | temp[0] << 4 ) >> 4;
      cout << "T: " << (21.0 + (float)temperature/8.) << endl;
      sleep(1);
   }
}
