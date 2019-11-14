// main.c

#include <wiringPi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdint.h> 

#define MAX_TIME 100 
#define DHT11PIN 25

int dht11_val[5]={0,0,0,0,0}; 
void dht11_read_val() ;

int main(void) { 
     int i;
     if(wiringPiSetup()==-1) 
          return -1;	
     while(1){ 
          dht11_read_val(); 
          sleep(1); 
     } 
     return 0; 
} 

void dht11_read_val() { 
     uint8_t lststate=HIGH; 
     uint8_t counter=0; 
     uint8_t j=0,i; 
     float farenheit; 

     for (i=0;i<5;i++) 
          dht11_val[i]=0; 
     pinMode(DHT11PIN,OUTPUT); 
     digitalWrite(DHT11PIN,0); 
     delay(18); 
     digitalWrite(DHT11PIN,1); 
     delayMicroseconds(40); 
     pinMode(DHT11PIN,INPUT);

      for  (i=0;  i<MAX_TIME;  i++) { 
          counter=0; 
          while  (  digitalRead(DHT11PIN) ==lststate  )  { 
               counter++;   // pulse duration 
               delayMicroseconds(1); 
               if (counter==255) 
                     break; } 
          lststate = digitalRead(DHT11PIN);
          if (counter==255) // check if is time out (bus no response)
               break; 
          // top 3 transitions are ignored, complete read bits should be even
          if  (  (i>=4)  &&  (i%2==0)  )  { 
               dht11_val[j/8]<<=1; 
               if(counter>16) 
                    dht11_val[j/8]|=1; 
               j++; 
          } 
     }  // for

      // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
      if((j>=40)&&(dht11_val[4]==((dht11_val[0]+dht11_val[1]+dht11_val[2]+dht11_val[3])& 0xFF))){ 
          farenheit = dht11_val[2]*9./5.+32; 
          printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",dht11_val[0],dht11_val[1],dht11_val[2],dht11_val[3],farenheit); 
     }else {
          printf( "Invalid Data!!" );
     }
}  // dht11_read_val()  
