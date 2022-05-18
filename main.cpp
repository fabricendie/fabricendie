#include <iostream>
#include <cmath>
#include "/Users/macretina/Desktop/Programming/JPEG_PROJECT/JPEG/src/jpegDecoder.cpp"
#include "/Users/macretina/Desktop/Programming/JPEG_PROJECT/JPEG/src/jpegEncoder.cpp"
 
using namespace jpeg; 

int main(int argc, char *argv[])
{
	cout << "Hello world!" << endl;

	jpeg::Decoder mydecoder; 

	jpeg::Encoder myencoder; 

    mydecoder.parseImage ("me.jpeg",350000);    

// for (int a =34; a<=34; a++)
  // {
	for (int b=0; b<64; b++) 
	 {
	//	  MCU[lumID][34][b] = 0; 
	//	   MCU[lumID+1][34][b] = 0; 
	//	   MCU[lumID+2][34][b] = 0; 
	 }
   //}
   
     myencoder.encodeImage(); 

}