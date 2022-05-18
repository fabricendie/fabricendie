            

#include <iostream>
#include <string>
#include <fstream> 
#include <math.h>
using namespace std;

typedef unsigned char   uint8; 
typedef unsigned short  uint16;
typedef unsigned int    uint32;
typedef signed   int    sint32;      
           

namespace jpeg
  {  
    float            MCU [4][40000][64]={0};      
    float            MCU_RGB[4][40000][64]={0};    
    sint32           printableMCU[4][40000][64]={0}; 
    uint8            compInfo [5][2];                        // compInfo [ID][sampFact/QT]
    uint16           numberOfMCUs[4]={0};
    uint16           mcuPositions[4]={0};
    sint32           dcCoeffArray[4]={0}; 
    
    char             hftSymbols [2][2][300]={0}; 
    char             hftSymbArr  [4][40000][64]={0};  
    unsigned short   dcHFTvalues[2][300][2]={0};                              
    unsigned short   acHFTvalues[2][300][2]={0};                              
    unsigned short   nbrOfSymbols[2][2]={0};                                
    unsigned short   zeroCountArr  [4][40000][64]={0};   
    unsigned short   Categories [4][40000][64]={0};   
    

    uint16           QT[5][64]; 
    uint8            rgbValues [4][3000000] = {0};   
    uint32           filesize;
    string           imageName; 
    uint16           width; 
    uint16           height; 
    uint8            compCount; 
    uint8            lumID=0;   
    uint16           orWidth; 
    uint16           orHeight; 
    uint16           mcuPerH; 
    uint16           mcuPerW;
    uint16           orMcuPerH; 
    uint16           orMcuPerW;
    uint16           lastMcuCol; 
    uint16           lastMcuRow; 
    uint16           pixelRemainder; 
    uint16           rowRemainder; 
    uint8            mcuColRemainder;
    uint8            Hsf=0; 
    uint8            Vsf=0; 
    uint8            decQT = false;
    uint8            encQT = true;  
    uint8            prevPic = false;                        // DECODE IMAGE MODIFIED BY PREVIEW
          
      class Decoder 
        {
            public:              

             uint8   globalArr [500000];           
            
             uint8   compoInfo2[20];                         // COMPOID, HFT FOR DC, HFT FOR AC
             uint16  HFT[2][10][0xffff];                     // HFT[AC/DC][10][0xffff];    
             uint16  codelen[2][10][0xffff];                 // colen[AC/DC/CODELENGTH][10][0xffff];     
             uint8   compoIDs[5]; 
             uint16  HFTLength;    
             uint8   DctMarkerType;
             uint8   numberofscans;    
             uint8   evenMCUperW;  

             uint8   lumHsf; 
             uint8   lumVsf;            

             char*  convToHex (uint32 number); 
             uint8* convToDec (uint32 number);
             uint32 findNextMarker (uint32 i, uint32 filesize);  
             sint32 convCoeff (uint8 category, sint32 coeff); 
             uint32 scanDataBaselineDCT (uint32 i, uint8 numbOfComp, uint32 filesize);  
             uint32 scanDataProgDCT (uint32 i, uint8 numbOfComp, uint32 filesize);     
             uint32 analyseQT (uint32 i);  
             uint32 analyseSOF (uint32 i); 
             uint32 analyseHFT (uint32 i, uint8 lenFlag);
             uint32 analyzeSOS (uint32 i, uint32 filesize);
             void printMcuContent (); 
             void printMcuRGBContent (); 
             void reorderZizag ();
             void zigzagTonormalPOS ();
             void multiplyByQT (); 
             void computeIDCT (); 
             void computeRGB (); 
             void printRGBbase10 ();  
             void convCoeffProgDCT (uint8 numbOfComp); 
             void normalOrd ();
             void zigzagOrd(uint8 temp[64]);  
                       
            public:

            string name; 
            void   parseImage (const string filename, uint32 picsize);   
            void   myprint(); 

       };


       class Encoder 
        {
          public:              

            void   readRgbContent();            
            void   computeYcbCr (); 
            char*  convToHex (int number); 
            void   convToMCU ();
            void   computeDCT ();  
            char*  orderZigzagPOS (char orgArr[500], int size); 
            void   orderZigzag (); 
            void   divideByQT (); 
            void   findDCdiff (); 
            void   removeBits (char bits); 
            void   revConvCoeff (); 
            void   countZeros (); 
            void   hftSymb (); 
            void   hftValues (); 
            void   sendBytes (char arr[500000], int arrSize); 
            void   generateJPEG(); 
            void   encodeImage();  
            void   printMcuContent (string name); 
            void   setUpfunc (uint8 sf, uint16 encWidth, uint16 encHeight, uint32 myfilesize, uint8 QT, string imName);  

            uint8   lumHsf; 
            uint8   lumVsf; 
            uint16  mcuPerWsf; 

            unsigned short   prepareBytes (char len, unsigned short value, unsigned short retVal);

        };


  }
