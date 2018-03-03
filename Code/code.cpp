// Copyright(C) 2018 Iti Shree

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "lodepng.h"
#include <cstdio>
#include <cmath>
#include <stdlib.h>

using namespace std;

const int BUFFERSIZE = 4096;
void TwelveToEightBit(uint32_t * charBuffer, uint32_t * buffer,const int BUFFERSIZE);
int main(int argc, char** argv)

{    // Checking the input
	 if (argc != 2)
    {
     cout <<" Invalid Command!" << endl;
     return -1;
    }

  // Reading Raw12 file
    const char * fName = "filename.raw12";
    std::ifstream file (fName, std::ios::binary|std::ios::in);
    if (!file) {
	printf("Error: could not open file %s\n", fName); return -1; }

	// Reading into Buffer

	uint32_t * buffer = new uint32_t [BUFFERSIZE];
	uint32_t *charBuffer=new uint32_t[4096*3072];

	// Conversion of 12-bit buffer to 8-bit buffer
	TwelveToEightBit(charBuffer,buffer,BUFFERSIZE);

	// 4 Bayer Color Channel and Color Image Arrays
     
     uint8_t * redLayer = new uint8_t[2048*1536];
     uint8_t * green1Layer = new uint8_t[2048*1536];
     uint8_t * green2Layer = new uint8_t[2048*1536];
     uint8_t * blueLayer = new uint8_t[2048*1536];
     uint8_t * colorLayer = new uint8_t[4096*3072*3];

   // Getting 4 required layers

   int count1=0,count2=0;

   for (long long i=0; i<4096*3072; i+=2)
	{
		if (i%8192 < 4096) {
                redLayer[count1] = charBuffer[i];
                green1Layer[count1] = charBuffer[i + 1];
                count1++;
            } else {
                green2Layer[count2] = charBuffer[i];
                blueLayer[count2] = charBuffer[i + 1];
                count2++;
            }
	}

	// Debayring using Linear Interpolation
	// Debayering
        for (long long i = 0; i < 4096*3072; i += 2) {
            if (i%8192 < 4096) {
               colorLayer[i*3] =   charBuffer[i];
               colorLayer[i*3+1] = charBuffer[i + 1];
               colorLayer[i*3+2] = charBuffer[i + 4097];
               colorLayer[i*3+3] = charBuffer[i];
               colorLayer[i*3+4] = charBuffer[i + 1];
               colorLayer[i*3+5] = charBuffer[i + 4097];
            } else {
               colorLayer[i*3] =   charBuffer[i - 4096];
               colorLayer[i*3+1] = charBuffer[i];
               colorLayer[i*3+2] = charBuffer[i + 1];
               colorLayer[i*3+3] = charBuffer[i - 4096];
               colorLayer[i*3+4] = charBuffer[i];
               colorLayer[i*3+5] = charBuffer[i + 1];
            }

}

	//Using LoadPNG to save layers to PNG files

	std::cout << lodepng::encode("redPicture.png", redLayer, 2048,  1536, LCT_GREY) << endl;
        std::cout << lodepng::encode("green1Picture.png", green1Layer, 2048,  1536, LCT_GREY) << endl;
        std::cout << lodepng::encode("green2Picture.png", green2Layer, 2048,  1536, LCT_GREY) << endl;
        std::cout << lodepng::encode("bluePicture.png", blueLayer, 2048,  1536, LCT_GREY) << endl;
        std::cout << lodepng::encode("coloredPicture.png", colorLayer, 4096,  3072, LCT_RGB) << endl;


  return 0;
}


void TwelveToEightBit(uint32_t * charBuffer, uint32_t * shortBuffer,const int BUFFERSIZE)
{

 short shortValue  = 0; // Will contain the two bytes in the shortBuffer.
 double doubleValue  = 0; // Will contain intermediary calculations.

 // Contrast adjustment necessary when converting

 double contrast = pow( ((100.0f + 50.0f) / 100.0f), 2);

 for ( int i = 0, j =0; i < BUFFERSIZE; i++, j += 2 )
 {

  // Bitwise manipulations to fit two chars onto one short.
  shortValue = (shortBuffer[j]<<8);
  shortValue += (shortBuffer[j+1]);

  doubleValue = (double)shortValue;

  // Divide by 16 to bring down to 0-255 from 0-4095 (12 to 8 bits)
  doubleValue /= 16;

  // Flatten it out from 0-1
  doubleValue /= 255;
  // Center pixel values at 0, so that the range is -0.5 to 0.5
  doubleValue -= 0.5f;
  doubleValue *= contrast;

  // Change back to a 0-1 range
  doubleValue += 0.5f;
  // and back to 0-255
  doubleValue *= 255;


  // If the pixel values clip a little, equalize them.
  if (doubleValue >255)
   doubleValue = 255;
  else if (doubleValue<0)
   doubleValue = 0;

  // Finally, put back into the char buffer.
  charBuffer[i] = (( unsigned char)(doubleValue));


 }
}

