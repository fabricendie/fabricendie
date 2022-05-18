
#include "/Users/macretina/Desktop/Programming/JPEG_PROJECT/JPEG/include/jpegDecEnc.h"
 

char* jpeg::Decoder:: convToHex (uint32 number) 

 {  
    int a=0;
    int c=0;
    static int  temp1 [2];                       // must be static 
    static char temp2 [2];

  for (int i=0;i<8;i++) 

    {  
      a = number & (0x80>>i) ? 1:0;
      c = (c<<1) | a; 
    } 
       
    temp1[0] = c >> 4;
    temp1[1] = c & 0xf;

  for (int i=0;i<2;i++) 
  
    {
       if (temp1[i] < 10) 
          {
             temp2[i] = temp1[i] +  48;                   //  number 
          }
       else
          {
            temp2[i] = temp1[i]  + 87;                    // letter 
          }    
    }

   return temp2; 
}



uint8* jpeg::Decoder :: convToDec (uint32 number) 
    { 
     static uint8 temp[7];  

     temp[0] = number/1000000;
     temp[1] = (number%1000000)/100000;
     temp[2] = ((number%1000000)%100000)/10000;
     temp[3] = (((number%1000000)%100000)%10000)/1000;
     temp[4] = ((((number%1000000)%100000)%10000)%1000)/100;
     temp[5] = (((((number%1000000)%100000)%10000)%1000)%100)/10;
     temp[6] = ((((((number%1000000)%100000)%10000)%1000)%100)%10);
     
     for (int i = 0; i<7;i++) 
       {
         temp[i] = temp[i]+48;       
       } 
     
      return temp;
    }


uint32 jpeg::Decoder::findNextMarker (uint32 i, uint32 filesize)  
    {
      int j;  

      for (j = i; j<filesize; j++) 
         {  
            if (globalArr[j]==0xff && globalArr[j+1]!=0) 
                {
                  if ( (globalArr[j+1] & 0xD8) == 0xD0)              //  EXCLUDE RESTART MARKERS, D0-D7
                     {

                     } 

                  else 
                     {
                       break; 
                     }    

                }
          }
    
      return j-1;     
    }


    
sint32 jpeg::Decoder::convCoeff (uint8 category, sint32 coeff)                // CONVERT COEFF BASED ON CATEGORY
   
   {
      sint32 number = 0xffff;  
             number = number >> (16-category);
                         
          if (coeff >= (number+1)/2) 
              {  
                return coeff; 
              }
             
          else 
              {  
                number = (0-number)+coeff;                               // SUBSTRACT 1 ? 
                return number;
              }             
    }     




void jpeg::Decoder::printMcuContent () 

 {  
    fstream myfile2; 
    myfile2.open (name, ios::app); 
    
    uint8   iterH; 
    uint8   iterV;   
    uint8   compID;
    uint16  printedPOS=0; 
    uint8   sign; 
    bool    isnegative=0; 
    string  compNames[6]={"Cr","Cb","Y "}; 
    uint16  mcuCount[4]={0};
    uint16  printedPosArr[4]={0}; 
    uint8   iter; 
    uint8   nbrOflines; 
  

     for (compID=lumID; compID<=compCount;compID++) 
        {    
          
          uint32 mcuPOS = numberOfMCUs[lumID]; 
           
          for (uint32 pos = 0; pos<mcuPOS; pos++) 
            { 
               for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                 { 
                     printableMCU[compID][pos][bytePOS] = MCU[compID][pos][bytePOS]; 
                 }
            }
        }


    while (printedPosArr[lumID]<numberOfMCUs[lumID]) 
    {

      for (compID =lumID; compID<=compCount; compID++)                            
      
        {    
          iterH      =0;        
          iterV      =1; 
          Hsf        = (compInfo[compID][0]>>4);  
          Vsf        = (compInfo[compID][0]&0xf); 
          nbrOflines =0;  
          printedPOS = printedPosArr[compID]; 

          
          myfile2 << endl; 
          myfile2 << endl; 
          myfile2 << "COMPONENT            : "<< compNames[compCount-compID]<<endl; 
          myfile2 << endl;        
        
          while (iterH<Hsf) 
            
            {     

                 myfile2 << "MCU NUMBER           : "<< mcuCount[compID] <<endl; 
                                   
                          for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                                {    
                                          if (bytePOS%8==0)                                                                               
                                              {
                                                myfile2 <<endl; 
                                                myfile2 <<"                      "; 
                                              }   
                                      
                                                isnegative = printableMCU[compID][printedPOS][bytePOS] & 0x80000000;
                                                                      

                                          if (isnegative) 
                                              {  
                                                  printableMCU[compID][printedPOS][bytePOS] = (~printableMCU[compID][printedPOS][bytePOS])+1;
                                                  sign = 0x2d; 
                                              }
                                          else 
                                              {
                                                  sign = ' ';  
                                              }   

                                          if (printableMCU[compID][printedPOS][bytePOS] <= 0xf)
                                              { 
                                                  myfile2 <<"    "<< sign << convToHex(printableMCU[compID][printedPOS][bytePOS])[1]; 
                                              } 

                                          if (printableMCU[compID][printedPOS][bytePOS] >0xf && printableMCU[compID][printedPOS][bytePOS] <= 0xff)
                                              {
                                                  myfile2 <<"   "<< sign << convToHex(printableMCU[compID][printedPOS][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS])[1]; 
                                              }    

                                          if (printableMCU[compID][printedPOS][bytePOS] >0xff && printableMCU[compID][printedPOS][bytePOS] <= 0xfff)
                                              { 
                                                  myfile2 <<"  "<< sign << convToHex(printableMCU[compID][printedPOS][bytePOS]>>8)[1];
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS])[1]; 
                                              }    
                                                    
                                          if (printableMCU[compID][printedPOS][bytePOS] >0xfff && printableMCU[compID][printedPOS][bytePOS] <= 0x7fff)
                                              {  
                                                  myfile2 << sign << convToHex(printableMCU[compID][printedPOS][bytePOS]>>8)[0];
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS]>>8)[1];
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][printedPOS][bytePOS])[1]; 
                                              }                                                                                                                                                   
 
                                }         
                                                  
                                                  myfile2<<endl;
                                                  myfile2<<endl;  

                                                  mcuCount[compID]+=1; 
                                                  printedPOS+=1;

                                                  iterH+=1; 
                                                           

                                                  if (iterV < Vsf && iterH==Hsf) 
                                                    {
                                                      iterH=0; 
                                                      iterV+=1; 
                                                      printedPOS-=Hsf; 
                                                      printedPOS+=mcuPerW; 
                                                      nbrOflines+=mcuPerW;                                                     
                                                    }    
                                                 

                }

                                                  if (compID==lumID)
                                                   {
                                                      printedPOS=  Vsf>1? printedPOS-nbrOflines:printedPOS;                          // GO BACK TO THE LINE ABOVE
                                                      printedPOS=  printedPOS%mcuPerW==0? printedPOS+=nbrOflines:printedPOS;            // SKIP LINES PRINTED WITH VERTSF  
                                                      printedPosArr[lumID]=printedPOS; 
                                                   }

                                                  else 
                                                    {
                                                      printedPosArr[compID]=printedPOS; 
                                                    }
                                                  
                                         
        }
    }
 }



void  jpeg::Decoder::printMcuRGBContent ()       
   
  {     
      fstream myfile2; 
      myfile2.open (name, ios::app); 
      uint8 compID;
      uint32 mcuPOS;  

      for (compID =lumID; compID<=compCount;compID++) 
        {  
           mcuPOS = numberOfMCUs[lumID]; 
           
          for (uint32 pos = 0; pos<mcuPOS; pos++) 
            { 
               for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                 { 
                     printableMCU[compID][pos][bytePOS] = MCU[compID][pos][bytePOS]; 
                 }
            }
        }


    uint8   sign; 
    bool    isnegative;    
    string  compNames[5]={"B ","G ","R "}; 

        
    for (uint16 pos=0; pos<numberOfMCUs[lumID+1]; pos++) 
      {               
          myfile2 << endl; 
          myfile2 << endl; 

        for (compID =lumID; compID<=compCount; compID++)                                    // PRINT MCU ARRAY CONTENT
           {    

             myfile2 << "COMPONENT            : "<< compNames[compCount-compID]<<endl; 
             myfile2 << endl;               
              
           
                 myfile2 << "MCU NUMBER           : "<< pos <<endl; 
                                   
                          for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                                {    
                                          if (bytePOS%8==0)                                   // PRINT A NEW LINE                                                         
                                              {
                                                myfile2 <<endl; 
                                                myfile2 <<"                      "; 
                                              }   
                                      
                                                isnegative = printableMCU[compID][pos][bytePOS] & 0x80000000;
                                                                      

                                          if (isnegative) 
                                              {  
                                                  printableMCU[compID][pos][bytePOS] = (~printableMCU[compID][pos][bytePOS])+1;
                                                  sign = 0x2d; 
                                              }
                                          else 
                                              {
                                                  sign = ' ';  
                                              }   

                                          if (printableMCU[compID][pos][bytePOS] <= 0xf)
                                              { 
                                                  myfile2 <<"    "<< sign << convToHex(printableMCU[compID][pos][bytePOS])[1]; 
                                              } 

                                          if (printableMCU[compID][pos][bytePOS] >0xf && printableMCU[compID][pos][bytePOS] <= 0xff)
                                              {
                                                  myfile2 <<"   "<< sign << convToHex(printableMCU[compID][pos][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS])[1]; 
                                              }    

                                          if (printableMCU[compID][pos][bytePOS] >0xff && printableMCU[compID][pos][bytePOS] <= 0xfff)
                                              { 
                                                  myfile2 <<"  "<< sign << convToHex(printableMCU[compID][pos][bytePOS]>>8)[1];
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS])[1]; 
                                              }    
                                                    
                                          if (printableMCU[compID][pos][bytePOS] >0xfff && printableMCU[compID][pos][bytePOS] <= 0x7fff)
                                              {  
                                                  myfile2 << sign << convToHex(printableMCU[compID][pos][bytePOS]>>8)[0];
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS]>>8)[1];
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS])[0]; 
                                                  myfile2 << convToHex(printableMCU[compID][pos][bytePOS])[1]; 
                                              }                                                      
                                           
                                        
                                }
                                                  myfile2<<endl;
                                                  myfile2<<endl;                                                  
                                                  
            }                           
        }
    }
    




void  jpeg::Decoder::zigzagTonormalPOS ()
   
  {
    uint8  compID;  
    uint32 mcuPOS; 
    sint32 temp [64]; 

    uint8 zigzagPos [64] = {
                            0,1,8,16,9,2,3,10,
                            17,24,32,25,18,11,
                            4,5,12,19,26,33,40,
                            48,41,34,27,20,13,
                            6,7,14,21,28,35,42,
                            49,56,57,50,43,36,
                            29,22,15,23,30,37,
                            44,51,58,59,52,45,
                            38,31,39,46,53,60,
                            61,54,47,55,62,63,   
                           };


    for (compID =lumID; compID<=compCount; compID++) 
      { 
        mcuPOS = numberOfMCUs[compID]; 
            
        for (uint32 pos=0; pos<mcuPOS; pos++)  
            {   
              for (uint8 n=0; n<64; n++) 
                 {
                    temp[n] = MCU[compID][pos][n];
                 }  
              
              for (uint8 bytePOS = 0; bytePOS<64; bytePOS++) 
                  {  
                     uint8 tempPOS=0; 
                     tempPOS = zigzagPos[bytePOS]; 
                     MCU[compID][pos][tempPOS] =  temp[bytePOS];                                                                                                                     
                  }

            }
      }
 }    
  






void jpeg::Decoder::multiplyByQT () 
      
    {  
          uint8  QTNumber;  
          uint32 mcuPOS; 
          uint8  compID; 

          for (compID = lumID; compID<=compCount; compID++) 
            {
               mcuPOS   = numberOfMCUs[compID]; 
               QTNumber = compInfo[compID][1];
             
               for (uint32 pos=0; pos<mcuPOS; pos++) 
                   {                                            
                      for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                         {  
                            MCU[compID][pos][bytePOS] = MCU[compID][pos][bytePOS] * QT[QTNumber][bytePOS];                            
                         }                     
                   }

            }
          
     }




void jpeg::Decoder::computeIDCT ()

 { 
   uint16 mcuPOS;
   uint8  compID;  
   float  temp1[64]; 

   for (compID = lumID; compID <=compCount; compID++)
    {    
      mcuPOS=numberOfMCUs[compID]; 

      for (uint16 pos=0; pos<mcuPOS; pos++)                                        // x and y select one pos within a table
         {                                                                         // u and v move from one table to the next
          for (uint8 x = 0; x<8; ++x) 
            {   
              for (uint8 y = 0; y<8; ++y) 
                 { 
                    float sum = 0.0;
                
                     for (uint8 u = 0; u<8; ++u)
                        {
                          for (uint8 v=0; v<8; ++v) 
                            {
                               float cu = u == 0 ? 1.0 / sqrt(2.0) : 1.0;
                               float cv = v == 0 ? 1.0 / sqrt(2.0) : 1.0;

                               float val = MCU[compID][pos][8*u+v]; 

                               if (v==7 && val==0) 
                                 {
                                    break; 
                                 }
   
                               while (val==0)                                       // SKIP COEFFS EQUAL TO ZERO TO REDUCE
                                 {                                                  // PROCESSING TIME
                                    v+=1; 

                                    if (v==7) 
                                      {
                                        if (u==7) 
                                          {
                                            break; 
                                          }

                                            u+=1; 
                                            v=0;  
                                      } 

                                     val = MCU[compID][pos][8*u+v];   
                                 }
                                 

                                if  (u==7 && v==7 && val==0) 
                                 {
                                   break; 
                                 }

                              //  cout << MCU[compID][pos][8*u+v] << endl;

                               sum += cu * cv * MCU[compID][pos][8*u+v] * cos((2 * x + 1) * u * M_PI / 16.0) *
                                          cos((2 * y + 1) * v * M_PI / 16.0);
                            }
                        }   
                              sum = (0.25 * sum)+128; 

                              if (sum > 255)
                                {
                                   sum = 255; 
                                }
                              

                              temp1[8*x+y] = sum;
                  }
            }
                              for (uint8 bytePOS=0;bytePOS<64; bytePOS++) 
                                 {
                                    MCU[compID][pos][bytePOS] = temp1[bytePOS]; 
                                 }       

                               
         }
    }
}



void jpeg::Decoder::computeRGB () 

 {  
    uint8   iterH=0; 
    uint8   iterV=0; 
    uint32  crPOS  =0; 
    uint16  newLine =0; 
    uint16  crStartPOS=0;

   for (uint16 pos=0; pos<numberOfMCUs[lumID]; pos++) 
      {           
        for (uint8 bytePOS=0;bytePOS<64; bytePOS++) 
          {  
            float Y  =  MCU[lumID][pos][bytePOS];              
            float Cb =  MCU[lumID+1][crPOS][bytePOS]; 
            float Cr =  MCU[lumID+2][crPOS][bytePOS];     

            float  R =  Y + 1.402 *(Cr-128.0);
            float  G =  Y - 0.344136 * (Cb-128.0) - 0.714136 *(Cr-128);
            float  B =  Y + 1.772 * (Cb-128.0);   

            R= R>255? 255:R;   
            G= G>255? 255:G;
            B= B>255? 255:B;

            R= R<0? 0:R;   
            G= G<0? 0:G;
            B= B<0? 0:B;
           
            MCU_RGB [lumID][pos][bytePOS]   = R;   
            MCU_RGB [lumID+1][pos][bytePOS] = G;  
            MCU_RGB [lumID+2][pos][bytePOS] = B;  
           
          }

             

            iterH+=1; 
            newLine+=1;
          
            Hsf = lumHsf;  
            Vsf = lumVsf;
                                                

            if (iterH==Hsf)                                                                 // AFTER TWO LUM MCUs 
              {                                                                             // MOVE TO THE NEXT Chr MCU
                crPOS+=1; 
                iterH=0; 
              }                          
            
                                                                                          // USE THE SAME CHR VALUES FOR TWO LUM LINES
            if (!(newLine%mcuPerW))                                                       // IF NEW LUM LINE 
                  {
                     iterV+=1; 

                     if (iterV==Vsf)                                                      // AFTER TWO LUM LINES
                      {                                                                   //  MOVE TO THE NEXT CHR LINE
                          iterV=0;
                         crStartPOS  = crPOS;                                            // SAVE FIRST ADDRESS OF THE NEW CHR LINE
                      }

                      else 
                      {
                         crPOS=crStartPOS; 
                      }

                  }    
                  
                      
      }
            
      for (uint8 compID=lumID; compID<=compCount;compID++) 
        {     
          for (uint32 pos = 0; pos<numberOfMCUs[lumID]; pos++) 
            { 
               for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                 { 
                    MCU[compID][pos][bytePOS] = MCU_RGB[compID][pos][bytePOS]; 
                 }
            }
        } 
 }




void jpeg::Decoder::printRGBbase10 () 
  
{ 
    fstream myfile2; 
    myfile2.open (name, ios::app); 
    uint32  address=0; 
    uint8   value; 
    uint8   *value2; 
    uint8   compID; 
    uint16  mcuPOS;
    uint8   a_loopEnd; 
    uint8   b_loopEnd; 

   
    myfile2<<endl;    
    myfile2<< "RGB VALUES IN BASE 10: " << endl;  
    myfile2<<endl; 


    for (compID =lumID; compID<=compCount;compID++) 
      {  
        mcuPOS = numberOfMCUs[lumID];                                                      // lumID BECAUSE THERE IS NO SF FOR RGB VALUES
           
          for (uint32 pos = 0; pos<mcuPOS; pos++) 
            { 
              for (uint8 bytePOS=0; bytePOS<64; bytePOS++) 
                { 
                    printableMCU[compID][pos][bytePOS] = MCU[compID][pos][bytePOS]; 
                }
            }
      }
  
 // cout << " orMcuPerh: " << orMcuPerH << " orMcuPerW: " << orMcuPerW << " mcuPerW: " << mcuPerW << " lastMcuRow: " << lastMcuRow << " rowRemainder: " << " lastMcuCol: " << lastMcuCol << " pixelRemainder: " << pixelRemainder << endl; 
  
    for (uint16 h=0; h<orMcuPerH; h++)                                                   
      {    
        a_loopEnd = h==lastMcuRow? rowRemainder:8; 
        
        for (uint8 a=0; a<a_loopEnd; a++)
          {  
            for (uint16 w=0; w<orMcuPerW; w++) 
              {        
                 b_loopEnd = w==lastMcuCol? pixelRemainder:8;   

                for (uint8 b=0;b<b_loopEnd; b++) 
                 { 
                   //address=  b + (w*8) + (width*a) + (h*width*8);                                             // b=bytePOS,  w=numberOfMcus,  a = numberofLines in each mcu row, h=number of vertical mcus 
                   value2 = convToDec (address);
                   myfile2<<value2[0]<<value2[1]<<value2[2]<<value2[3]<<value2[4]<<value2[5]<<value2[6]<<": ";     
                  

                   for (compID=lumID; compID<=compCount; compID++) 
                     {  
                        value = printableMCU[compID][(h*mcuPerW) +w][a*8+b];                                    // printableMCU[compID][h*w][a*8+b] a : go from one line to the next within the mcu
                        rgbValues [compID][address] = value; 
                        
                                                                                                               // b : go from one value to the next within the mcu
                        if (value<10)                                                                          //  w : go from one horizontal mcu to the next
                          {                                                                                    //  h : go from one vertical mcu to the next
                             myfile2<<convToDec(value)[6]<<",";
                          }
 
                        if (value>=10 && value <100) 
                          {
                            myfile2<<convToDec(value)[5]<<convToDec(value)[6]<<",";
                          } 

                        if (value>=100) 
                          {
                            myfile2<<convToDec(value)[4]<<convToDec(value)[5]<<convToDec(value)[6] << ","; 
                          }
                                                          
                     } 
                         myfile2<<endl; 
                         address+=1;
                         filesize = address; 
                 }
              }                 
          }  
      }
}




uint32 jpeg::Decoder::scanDataBaselineDCT (uint32 i, uint8 numbOfComp, uint32 filesize) 

{        
    uint8  bytePOS;
    uint16 value=0; 
    uint16 bitsize=0;
    uint16 valuesize; 
    uint32 data=0; 
    uint16 lengths; 
    uint8  DCHFT=0; 
    uint8  ACHFT=1; 
    uint8  temp=0; 
    uint16 symbol; 
    uint8  zeroCount; 
    uint8  category; 
    sint32 dcCoeff; 
    sint32 acCoeff; 
    sint32 oldDcCoeff=0; 
    uint8  dcID; 
    uint8  acID; 
    uint8  compID; 
    uint16 mcuPOS; 
    uint8  code_length;
    uint32 nextMarker; 
    uint8  compoIter=0;
    uint8  iterH=0; 
    uint8  iterV=0;
    uint16 nextMCUs=0; 
    uint16 nbrOflines=0;
    uint8  flagRst1 = false; 
    uint8  valRst = 0; 

    uint8  rstBitsCount=0; 
    uint8  rstBitsCountRder=0; 

    uint8  linesCount =0; 
    uint8  flagRst2 = false; 
    uint32 rstData=0; 
    uint32 rstBitsRder=0; 
    uint8  rstDataSize=0; 
    
     
    nextMarker     = findNextMarker (i, filesize); 
     
    
    for (uint8 j = 0; j<4; j++) 
       
       {   
          i+=1;
                 
          if (globalArr[i]==0&&globalArr[i-1]==0xff)                // REMOVE ZERO STUFFING
            {
                i=i+1;               
            }

            data = (data << 8) | globalArr [i];          
       }


 while (i<nextMarker) 
   
  {  
       compoIter=0;
       temp=0;
    
    while (compoIter < numbOfComp) 
          
     { 
        compID   = compoInfo2 [temp];
        dcID     = compoInfo2 [temp+1]; 
        acID     = compoInfo2 [temp+2]; 
        mcuPOS   = mcuPositions [compID]; 
        temp+=3;
        Hsf      = compInfo[compID][0]>>4; 
        Vsf      = compInfo[compID][0]&0xf; 
        iterH    = 0;
        iterV    = 1; 
        nextMCUs = 0; 
        

 
      while (iterH<Hsf) 
  
       {  
         bytePOS=0;
         value=0; 
         valuesize=0; 
          
         while (bytePOS<64)         
          {    
            if (bitsize>8)                                                           // REFILL DATA       
             {                   
                  i+=1;  

              if (globalArr[i]==0&&globalArr[i-1]==0xff)                              // REMOVE ZERO STUFFING
                {
                   i+=1;              
                }     

              if ((globalArr[i] >> 4) == 0xd  && (globalArr[i] & 0x8) == 0  &&  globalArr[i-1]==0xff)         //  READ RESTART MARKERS 0XFFD0 - 0XFFD7
                {
                   flagRst1= true; 
                   valRst = globalArr[i];
                   rstBitsCount=0;
                   rstData = data<<8 | globalArr [i];
                   rstDataSize=bitsize-8; 
                }       
                                                     
                  data = data<<8 | globalArr [i];
                  bitsize=bitsize-8;  
             } 
            
 
            value    = (value<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize));          // TAKE ONE BIT FROM DATA
            valuesize+=1;
            bitsize+=1;
            
            rstBitsCount+=1;
            rstBitsCountRder+=1;  

             
            if (rstBitsCount==(32-rstDataSize)  && flagRst1==true)                                       //(value >> 8) == 0xff  && ((value & 0xff) == valRst)   && flagRst == true) PROCESS RESTART MARKER
              {           
                valuesize =0; 
                flagRst1  = false; 
                dcCoeffArray[lumID]   = 0; 
                dcCoeffArray[lumID+1] = 0;               
                dcCoeffArray[lumID+2] = 0;  
               
                linesCount+=1; 
                flagRst2              = true;
            
               // rstBitsRder           = (rstData >> 16) & (0xffffffff >> (32-(rstBitsCountRder-16))); 

               // cout << " value: " << value <<" RSTmarker :" << valRst+0 << " compID: " << compID+0 << " mcuPOS: " <<  mcuPositions[lumID] << " data: " << data << " rstBitCount: "<< rstBitsCount+0 << " rstData: " << rstData << " rstBitsCountRder: " << rstBitsCountRder+0 <<  " rstBitsRder: " << rstBitsRder+0 << endl; 
                 
                value=0;               
                iterH=Hsf;
                iterV=Vsf; 
                bytePOS=64;
                compoIter=numbOfComp;  
                break; 
              }


            if (valuesize>50 && i<nextMarker) 
             {
               cout << " compID:  "<< compID+0  << " mcuPOS: " << mcuPOS << " bytePOS: " << bytePOS+0 << " START OF SCAN FAILED !"<<  " nextMarker: "<< nextMarker  << endl;              
               i=nextMarker; 
               iterH=Hsf;
               iterV=Vsf; 
               bytePOS=64;
               compoIter=numbOfComp;
               break;
             }
          
        
            if (bytePOS==0) 
              {                          
                lengths = codelen[DCHFT][dcID][value];
                symbol  = HFT[DCHFT][dcID][value];                
              }  
   
            if (bytePOS!=0) 
              {      
                lengths = codelen[ACHFT][acID][value]; 
                symbol  = HFT[ACHFT][acID][value];               
              }   
             
            
            if (valuesize == lengths && symbol == 0  && bytePOS!=0 )                           // END OF BLOCK
              { 
               // cout <<" EOB: " << "ID: "<< compID+0 << " symb: " << symbol << " mcuPOS: "<< mcuPOS+nextMCUs <<  endl;   
                break;       
              }
            
        
            if (valuesize == lengths && symbol !=0x200 && bytePOS==0)                         // EXTRACT DC COEFF
        
              {  
                zeroCount = symbol >> 4; 
                category = symbol & 0xf; 
                rstBitsCount+=category;
                dcCoeff=0; 
                
                for (uint8 a= 0; a<category; a++)                          
                     {
                        dcCoeff = (dcCoeff<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize)); 
                        bitsize+=1;
                     }   
                        float orgCoeffCat =dcCoeff; 
                         
                        bytePOS+=zeroCount; 
                        dcCoeff = convCoeff (category,dcCoeff); 
                        
                        float convCoeff =dcCoeff;

  
                        oldDcCoeff=dcCoeffArray[compID]; 
                        dcCoeff = oldDcCoeff+dcCoeff;
                        dcCoeffArray[compID]=dcCoeff;  
                        MCU[compID][mcuPOS+nextMCUs][bytePOS] = dcCoeff;  

                        //cout << "ID: "<< compID+0 << " dcCoeff: " << dcCoeff << " orgCoeffCat: "<< orgCoeffCat << " convCoeff: "<< convCoeff << " cat: "<< category+0 << " symb: " << symbol << " mcuPOS: " << mcuPOS+nextMCUs<< " bytePOS: "<< bytePOS+0;                       
                        //cout << " Data : " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1];  
                        //cout << " value: "<< value << " valuesize: " << valuesize << endl;
                                              

                        value=0;
                        valuesize=0;
                        bytePOS+=1;       
              }
         

            if (valuesize == lengths && symbol !=0x200 && bytePOS!=0 && bytePOS!=64)       // EXTRACT AC COEFF
        
             {
                zeroCount = symbol >> 4; 
                category  = symbol & 0xf; 
               rstBitsCount+=category;
                acCoeff=0;
                
                
                for (uint8 a= 0; a<category; a++)                          
                  {
                    acCoeff =(acCoeff<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize));; 
                    bitsize+=1; 
                  }

               // cout << "ID: "<< compID+0 << " acCoeff: " << acCoeff << " symb: " << symbol << " mcuPOS: " << mcuPOS+nextMCUs<< " bytePOS: "<< bytePOS+0 << " cat : " << category+0;                       
               // cout << " Data : " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1];  
               // cout << " value: "<< value << " valuesize: " << valuesize << endl;
    

                bytePOS+=zeroCount; 
                acCoeff = convCoeff (category,acCoeff); 
                MCU[compID][mcuPOS+nextMCUs][bytePOS] = acCoeff;                        
                bytePOS+=1;
                value=0; 
                valuesize=0;                          
                        
             }

          }             
                        rstBitsCountRder=0;                                                   // WHILE (BYTEPOS<64)
                        iterH+=1;                                                             // COMES AFTER SAMPLING FACTOR LOOP
                        mcuPOS+=1;   
                                                
                        if (iterV<Vsf && iterH==Hsf)
                          {                                    
                              nextMCUs+=mcuPerW;  
                              iterV+=1; 
                              iterH=0; 
                              mcuPOS-=Hsf; 
                          } 
                                   
        }                                                              
  

                        nbrOflines = mcuPOS%mcuPerW;                            // COMES AFTER NUMBER OF COMPONENTS LOOP
                               
                        if (!nbrOflines && compID==lumID) 
                            { 
                              if (prevPic==false) 
                              {
                               mcuPOS+=nextMCUs;                                // SKIP NEXT LINES
                              }  
                            } 

                        mcuPositions[compID] = mcuPOS;  
                        compoIter+=1;  
                       
                        mcuPositions[lumID]  = flagRst2 == true ? ((linesCount*mcuPerW)*2) : mcuPositions[lumID]; 
                        flagRst2             = false;         
                           
      }

    }                           
                        i = nextMarker;                  
                        return i;
}




uint32 jpeg::Decoder::scanDataProgDCT (uint32 i, uint8 numbOfComp, uint32 filesize) 

{        
    uint8  bytePOS;
    uint16 value=0; 
    uint16 bitsize=0;
    uint16 valuesize=0; 
    uint32 data=0; 
    uint16 lengths; 
    uint8  temp=0; 
    uint16 symbol; 
    uint8  zeroCount; 
    uint8  category; 
    uint8  compID; 
    uint16 mcuPOS=0; 
    uint32 nextMarker; 
    uint8  compoIter=0;
    uint8  acID=0;
    uint8  dcID=0;
    uint8  ACHFT=1;
    uint8  DCHFT=0;
    uint8  bytePOS_start=0; 
    uint8  bytePOS_end=0; 
    uint8  previousScan=0; 
    uint8  bitRemoved=0; 
    sint32 dcCoeff; 
    sint32 acCoeff;
    sint32 oldDcCoeff=0;
    uint8  iterH; 
    uint8  iterV; 
    uint8  nextMCUs=0; 
    uint8  posflag=0; 
    uint16 rowPOS=0; 
    uint16 orgAcCoeff; 
    sint32 tempNumb=0;  
    sint32 OneBit=0;  
    uint32 nbrOfRows=0; 
    uint16  a_nbrOfRows[5]; 
    uint16  a_mcuPerW[5]; 
    uint8  lumVsf; 
    uint8  lumHsf;
    uint8  compSF; 
    uint16 numbToDec; 
    uint16  endOfRow=0; 
    uint8  evenMCUperW = orMcuPerW==mcuPerW ? 1:0; 
    uint16  locMcuPerW=0; 
    
  

    uint8 zigzagPos [64] = {
                            0,1,8,16,9,2,3,10,
                            17,24,32,25,18,11,
                            4,5,12,19,26,33,40,
                            48,41,34,27,20,13,
                            6,7,14,21,28,35,42,
                            49,56,57,50,43,36,
                            29,22,15,23,30,37,
                            44,51,58,59,52,45,
                            38,31,39,46,53,60,
                            61,54,47,55,62,63,   
                           };
     
    numberofscans+=1;
    

    lumVsf               = compInfo[lumID][0]&0xf; 
    lumHsf               = compInfo[lumID][0]>>4; 
             
    a_mcuPerW [lumID]   = mcuPerW; 
    
    a_mcuPerW [lumID+1] = mcuPerW/lumHsf; 
    a_mcuPerW [lumID+2] = mcuPerW/lumHsf;          
    
    
    a_nbrOfRows [0]     = 0;
    a_nbrOfRows [1]     = 0;
    a_nbrOfRows [2]     = 0;
    a_nbrOfRows [3]     = 0;   
 
    nextMarker          = findNextMarker (i, filesize); 
    mcuPositions [0]    = 0;
    mcuPositions [1]    = 0;
    mcuPositions [2]    = 0; 
    mcuPositions [3]    = 0; 

    i+=1; 
    bytePOS_start = globalArr[i];
    i+=1; 
    bytePOS_end = globalArr[i]; 
    i+=1; 
    previousScan = globalArr[i]>>4; 
    bitRemoved = globalArr[i]&0xf; 
 
    for (uint8 j = 0; j<4; j++) 
       
       {      
          i+=1; 
                        
           if (globalArr[i]==0&&globalArr[i-1]==0xff)                // REMOVE ZERO STUFFING
              {
                i+=1;    
              }

            data = (data << 8) | globalArr [i];    
              
       }

while (1)    
 {  

  if (bitsize>=32)                                                        // EXIT FUNCTION BC I>NEXTMARKER                                                       
    { 
        i = nextMarker;                  
        return i;
    }  
    
    compoIter=0;
    temp =0;
 

  while (compoIter < numbOfComp)  
   {
      compID    = compoInfo2 [temp];
      dcID      = compoInfo2 [temp+1]; 
      acID      = compoInfo2 [temp+2]; 

      locMcuPerW = a_mcuPerW[compID]; 
      nbrOfRows  = a_nbrOfRows[compID];

      rowPOS = mcuPositions[compID];
      mcuPOS = (nbrOfRows*locMcuPerW)+rowPOS;  
      iterV  =1; 
      iterH  =0; 

      temp+=3;
      Hsf    = (compInfo[compID][0]>>4); 
      Vsf    = (compInfo[compID][0]&0xf); 
      nextMCUs=0; 
  
      
      while (iterH < Hsf) 
      
        {  
          bytePOS = bytePOS_start; 
         
          while (bytePOS<=bytePOS_end) 
             {                  

                if (bitsize>8 && i < nextMarker)                                                // REFILL DATA       
                   
                  {                   
                        i+=1;  
                     
                        if (globalArr[i]==0&&globalArr[i-1]==0xff)                              // REMOVE ZERO STUFFING
                          {
                              i+=1;                
                          }     

                          data = data<<8 | globalArr [i];
                          bitsize=bitsize-8;                            
                  } 
             
                          value = (value<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize));  // TAKE ONE BIT FROM DATA
                          valuesize+=1;
                          bitsize+=1;

                          if (numberofscans==7) 
                           {
                            // cout << "  Data:   " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1]<< " bitsize: "<< bitsize+0; 
                             //cout << "  Value:  " << convToHex(value>>8)[0]<<convToHex(value>>8)[1]<< convToHex(value)[0]<< convToHex(value)[1] << endl; 
                           }
            
                          

                      if (valuesize>16 && i<nextMarker) 
                          {
                            cout << "ADDRESS " << i << " compID: " << compID+0 << " bytePOS: "<< bytePOS+0 << " numbOfComp: "<< numbOfComp+0 << " START OF SCAN FAILED" << endl;                         
                            cout <<"numberOfscans: "<< numberofscans+0 << " mcuPOS: " << mcuPOS << " numberOfMCUs: "<< numberOfMCUs[compID]; 
                            cout << " Data: " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1]; 
                            cout <<"  value:"<< convToHex(value>>8)[0]<<convToHex(value>>8)[1]<<convToHex(value)[0]<< convToHex(value)[1]; 
                            cout <<"  bytePOS_start: " << bytePOS_start+0 <<" bytePOS_end: "<<  bytePOS_end+0 <<" previousScan: "<<  previousScan+0 <<" bitRemoved: "<< bitRemoved+0<<endl;                          
                            
                            i=nextMarker; 
                            bytePOS=bytePOS_end+1;
                            iterH=Hsf; 
                            iterV=Vsf; 
                            compoIter=5;
                            i = nextMarker; 
                          }


                      if (bytePOS == 0) 
                          {
                            lengths = codelen[DCHFT][dcID][value];                   //codelenProgDCT [numberOfHFT][value];
                            symbol  = HFT[DCHFT][dcID][value];                       //HFTProgDCT [numberOfHFT][value];                
                          }

                      else 
                          {
                            lengths = codelen[ACHFT][acID][value]; 
                            symbol  = HFT[ACHFT][acID][value];               
                          }

            
                      if (valuesize == lengths && symbol == 0 && bytePOS!=0 )        // END OF BLOCK
                        {                          
                             
                            if (previousScan!=0)
                             { 
 
                                uint8  x =0; 
                                sint32 tempArr[64]; 
                                uint8  tempPOS=0; 
                                uint8  zcPlusOne=0; 

                                while (x<64) 
                                  {
                                     tempPOS = zigzagPos[x]; 
                                     tempArr[tempPOS] = MCU[compID][mcuPOS][x]; 
                                     x+=1;
                                  }  

                                while (bytePOS<=63) 
                                  {   
                                    tempPOS = zigzagPos[bytePOS]; 

                                    if (tempArr[tempPOS]==0) 
                                      {
                                         bytePOS+=1; 
                                      }

                                     else  
                                      {  
                                        acCoeff =  (data&(0x80000000>>bitsize)) >> (31-bitsize); 
                                        bitsize+=1; 

                                        if (bitsize>8 && i< nextMarker)                                              // REFILL DATA                          
                                          {                   
                                            i+=1;  
                                            
                                            if (globalArr[i]==0&&globalArr[i-1]==0xff)            // REMOVE ZERO STUFFING
                                              {
                                                 i+=1;                
                                              }     

                                            data = data<<8 | globalArr [i];
                                            bitsize=bitsize-8;                            
                                          }
                                          
                                        if (acCoeff!=0)
                                          {
                                             tempArr[tempPOS] = tempArr[tempPOS] | (0x1<bitRemoved); 
                                             
                                             if (numberofscans==6)
                                              {
                                             //   cout << "EOB bytePOS: " <<  bytePOS+0 << " acCoeff: "<< acCoeff+0 << " val: " <<  tempArr[tempPOS] << endl; 
                                              }  
                                          }   
                                                bytePOS+=1;
                                      }                                                      // IF [tempPOS]==0, ELSE
                                                                              
                                  }                                                          // WHILE BYTEPOS<=64

                                        x=0; 

                                        while (x<64) 
                                          {
                                             tempPOS = zigzagPos[x]; 
                                             MCU[compID][mcuPOS][x] = tempArr[tempPOS]; 
                                             x+=1; 
                                          }
                             }                                                                // IF PREVIOUSCAN


                             if (numberofscans==1) 
                              {                           
                              //  cout <<" EOB "<< " mcuPOS: " << mcuPOS << " value: " << value << " valuesize: " << valuesize << " symbol: " << symbol << " bytePOS: " << bytePOS+0 << endl;  
                              }         

                             value=0; 
                             valuesize=0;  
                             break;                             
                        } 
            

        
                      if ( ((valuesize == lengths && symbol !=0x200) && bytePOS==0) || (previousScan!=0 && bytePOS==0))         // EXTRACT DC COEFF
        
                         {  
                            zeroCount = symbol >> 4; 
                            category = symbol & 0xf; 
                            dcCoeff=0; 
                            
                            category = previousScan==0 ? category : 0;                                        // SKIP NEXT FOR LOOP FOR SUBSEQUENT DC SCAN
                
                            for (uint8 a= 0; a<category; a++)                          
                              {
                                dcCoeff = (dcCoeff<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize)); 
                                bitsize+=1;
                              }       
                                  
                                int orgdcCoeff = dcCoeff;   
                                dcCoeff = convCoeff (category,dcCoeff); 
                                oldDcCoeff=dcCoeffArray[compID]; 
                                dcCoeff = oldDcCoeff+dcCoeff;
                                dcCoeffArray[compID]=dcCoeff;


                            if (previousScan==0) 
                              { 
                                MCU[compID][mcuPOS+nextMCUs][bytePOS] = dcCoeff<<bitRemoved;  
                              }      

                            else 
                              { 
                                tempNumb = MCU[compID][mcuPOS+nextMCUs][bytePOS]; 

                                if (value==1)
                                {
                                   MCU[compID][mcuPOS+nextMCUs][bytePOS] = tempNumb | (0x1 << bitRemoved); 
                                }  
                              }          

                            if (numberofscans==7)  //&& compID==(lumID+2)) 
                              {                               
                              //   cout << " Data : " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1];  
                              //   cout << " value: "<< value << " valuesize: " << valuesize; //<< " symbol: " << symbol << " ZC: "<< zeroCount+0 << " cat: "<< category+0 << " mcuPOS: " << mcuPOS << " bytePOS: " << bytePOS+0 << " orgAcCoeff: "<< orgAcCoeff << " acCoeff: "<< acCoeff << endl;                           
                              //   cout << " compID:  " << compID+0 << " mcuPOS: " << mcuPOS+nextMCUs << " value: "<< value << " symbol: " << symbol << " ZC: "<< zeroCount+0 << " cat: "<< category+0 << " bytePOS: " << bytePOS+0 << " orgDcCoeff: "<< orgdcCoeff << " dcCoeff: "<< dcCoeff << " savedCoeff: "<< MCU[compID][mcuPOS+nextMCUs][bytePOS] << endl;  
                              }    
                                                                                                                                                        
                            
                            value=0; 
                            bytePOS+=1;
                            valuesize=0;  
                            break;      
                          }
                      


                      if (valuesize == lengths && symbol !=0x200 && bytePOS_start!=0)                     // EXTRACT AC COEFF
        
                        {  
                            zeroCount = symbol >> 4; 
                            category  = symbol & 0xf; 
                            acCoeff=0;
    

                            if (category == 0  && symbol!=0xf0)                                
                            {
                              category = zeroCount; 
                              zeroCount =0; 
                              posflag=1; 
                            }  
                
                
                            for (uint8 a= 0; a<category; a++)                          
                              {
                                acCoeff =(acCoeff<<1) | ((data&(0x80000000>>bitsize)) >> (31-bitsize));; 
                                bitsize+=1; 
                              }  

                            orgAcCoeff = acCoeff;   
                             
                            if (posflag==1)                                                               // CHECK IF COEFF REPRESENTS POSITION
                              {   
                                numbToDec = (0xffff>> (16-category)) + acCoeff; 
                                
                                numbToDec+=1; 

                                while (numbToDec>0) 
                                  {    
                                      if (previousScan!=0)
                                        {                              
                                          uint8  x =0;  
                                          sint32 tempArr[64]; 
                                          uint8  tempPOS=0; 
                                          uint8  zcPlusOne=0; 

                                          while (x<64) 
                                           {
                                               tempPOS = zigzagPos[x]; 
                                               tempArr[tempPOS] = MCU[compID][mcuPOS][x]; 
                                               x+=1; 
                                           }  

                                          while (bytePOS<=63) 
                                           {   
                                             tempPOS = zigzagPos[bytePOS]; 

                                             if (tempArr[tempPOS]==0) 
                                              {
                                                bytePOS+=1; 
                                              }

                                             else  
                                              {  
                                                acCoeff =  (data&(0x80000000>>bitsize)) >> (31-bitsize); 
                                                bitsize+=1; 

                                                if (bitsize>8)                                              // REFILL DATA                          
                                                  {                   
                                                    i+=1;  

                                                    if (globalArr[i]==0&&globalArr[i-1]==0xff)            // REMOVE ZERO STUFFING
                                                     {
                                                       i+=1;                
                                                     }     

                                                    data = data<<8 | globalArr [i];
                                                    bitsize=bitsize-8;                            
                                                  }
                                           
                                                if (acCoeff!=0)
                                                 {
                                                    tempArr[tempPOS] = tempArr[tempPOS] | (0x1<bitRemoved); 
                                             
                                                    if (numberofscans==6)
                                                     {
                                                    //   cout <<" POScase: " << " bytePOS: " <<  bytePOS+0 << " acCoeff: "<< acCoeff+0 << " val: " <<  tempArr[tempPOS] << " mcuPOS:  "<< mcuPOS << endl; 
                                                     }  
                                                 }   
                                                    bytePOS+=1;
                                             }                                               // IF [tempPOS]==0, ELSE
                                                                              
                                          }                                                  // WHILE BYTEPOS<=64
                                                
                                                x=0; 

                                                while (x<64) 
                                                  {
                                                    tempPOS = zigzagPos[x]; 
                                                    MCU[compID][mcuPOS][x] = tempArr[tempPOS]; 
                                                    x+=1; 
                                                  }
                                              
                                        }                                                    // IF PREVIOUSCAN !=0       
                           
                                                 
                                                rowPOS+=1;  

                                                endOfRow = evenMCUperW==1 ? locMcuPerW:orMcuPerW;           
                                                 
                                                 if (compID== lumID && rowPOS== endOfRow)  
                                                  {  
                                                     nbrOfRows+=1; 
                                                     rowPOS=0;  
                                                  } 
                                                 
                                                 if (compID!= lumID && rowPOS== locMcuPerW)   
                                                  {  
                                                     nbrOfRows+=1; 
                                                     rowPOS=0;  
                                                  }                                                   
                                      
                                                mcuPOS = (nbrOfRows*locMcuPerW)+rowPOS;  
                                                
                                                posflag =0;                               
                                                bytePOS = bytePOS_start;  
                                                numbToDec-=1; 
                                               
                                  }                                                        //  WHILE numbToDec >= 0                      
                                 
                              }

                            else                                                           // IF NOT, TREAT IT AS A NORMAL COEFF
                              {  
                      
                               if (previousScan==0) 
                                {
                                  acCoeff = convCoeff (category,acCoeff);  
                                  bytePOS+=zeroCount;
                                  acCoeff = acCoeff<<bitRemoved;  
                                  MCU[compID][mcuPOS][bytePOS] = acCoeff;                                                                 
                                  bytePOS+=1; 
                                }      


                               if (previousScan >= 0x1) 
                                {                                                                                   
                                      OneBit = acCoeff == 1 ? 1 : -1;                      // SAVE THE SIGN OF ONEBIT

                                      uint8  x =0; 
                                      sint32 tempArr[64]; 
                                      uint8  tempPOS=0; 
                                      uint8  zcPlusOne=0; 

                                      while (x<64) 
                                        {
                                          tempPOS = zigzagPos[x]; 
                                          tempArr[tempPOS] = MCU[compID][mcuPOS][x]; 
                                          x+=1; 
                                        }  


                                      while  (1) 
                                        {                                                        
                                            while (1)                                        // SKIP ZERO COEFFS
                                              {      

                                                if (zcPlusOne==0)
                                                {
                                                  zeroCount+=1;
                                                  zcPlusOne=1; 
                                                }

                                                tempPOS = zigzagPos[bytePOS]; 

                                                if (tempArr[tempPOS]==0)
                                                  {
                                                    zeroCount-=1; 
                                                  } 

                                                else 
                                                  { 
                                                   break; 
                                                  } 

                                                if (zeroCount==0) 
                                                 {   
                                                    tempPOS = zigzagPos[bytePOS];
                                                    break;                                                     
                                                 } 

                                                 bytePOS+=1;   

                                                if (bytePOS>=64) 
                                                 {
                                                   cout << " FAIL, 1st WHILE LOOP " << " compID: " << compID << " bytePOS: " << bytePOS+0 << " mcuPOS: "<< mcuPOS+0 << endl; 
                                                   break; 
                                                 }                                                                                              

                                              }
                                            
                                            if (zeroCount==0 || bytePOS>=64) 
                                                {   
                                                   break; 
                                                } 
    
                                              
                                            acCoeff =  (data&(0x80000000>>bitsize)) >> (31-bitsize); 
                                            bitsize+=1; 

                                            if (bitsize>8 && i<nextMarker)                            // REFILL DATA                          
                                              {                   
                                                i+=1;  
                                             
                                                if (globalArr[i]==0&&globalArr[i-1]==0xff)            // REMOVE ZERO STUFFING
                                                  {
                                                    i+=1;                
                                                  }     

                                                 data = data<<8 | globalArr [i];
                                                 bitsize=bitsize-8;                            
                                              }

                                            if (numberofscans== 6) 
                                               {
                                              //   cout << " acCoeff : " << acCoeff << " bytePOS: "<< bytePOS+0 << " zigzagPOS : "<< tempPOS+0 << " zeroC : "<< zeroCount+0 << " nbrOfRows: " << nbrOfRows+0 << endl;  
                                               } 

                                            if (acCoeff!=0)                                           // ADD ONE BIT TO COEFF
                                              { 
                                                tempNumb =  tempArr [tempPOS]; 
                                                tempArr[tempPOS]=  tempNumb | (0x1<<bitRemoved);
                                              }

                                            bytePOS+=1; 

                                            if (bytePOS>=64) 
                                              {
                                                  cout << " FAIL, 2nd WHILE LOOP " << " compID: " << compID << " bytePOS: " << bytePOS+0 << " mcuPOS: "<< mcuPOS+0 << endl; 
                                                  break; 
                                              }                                                                                              

                                        }                                                               // WHILE (1)

                                      if (category==1) 
                                       {  
                                         tempArr[tempPOS]= OneBit<<bitRemoved; 

                                         if (numberofscans==6) 
                                          {
                                          //  cout << " mcuPOS: " << mcuPOS << " OneBit  : " << OneBit << " bytePOS : " << bytePOS+0 << " temPOS: "<< tempPOS+0 << endl; 
                                          }
                                       }   
                                      
                                      bytePOS+=1;

                                      x=0; 

                                      while (x<64) 
                                        {
                                          tempPOS = zigzagPos[x]; 
                                          MCU[compID][mcuPOS][x] = tempArr[tempPOS]; 
                                          x+=1;                                          
                                        }

                                }                                                                      // PREVIOUSSCAN==1 
                                          
                             }                                                                         // IF COEFF == POS, ELSE
                                                                                                                                                        
                             if (numberofscans==5)
                               { 
                                //  cout << " Data : " << convToHex(data>>24)[0]<<convToHex(data>>24)[1]<< convToHex(data>>16)[0]<< convToHex(data>>16)[1]<<convToHex(data>>8)[0]<<convToHex(data>>8)[1]<<convToHex(data)[0]<<convToHex(data)[1];  
                                //  cout << " value: "<< value << " valuesize: " << valuesize << " symbol: " << symbol << " ZC: "<< zeroCount+0 << " cat: "<< category+0 << " mcuPOS: " << mcuPOS << " bytePOS: " << bytePOS+0 << " orgAcCoeff: "<< orgAcCoeff << " acCoeff: "<< acCoeff << endl;        
                               }

                            
                             value=0;    
                             valuesize=0;  
                        }                                                                              // EXTRACT ACCOEFF
                        
                            if (bitsize>=32)                                                          // EXIT FUNCTION BC E>NEXTMARKER                                                       
                              { 
                                i = nextMarker;                  
                                return i;
                              }  
              }                                                                                       // WHILE BYTEPOS                                                                  

                                                                                   
                             iterH+=1;                                                              // POSITION TWO MCUs ON THE SECOND LINE  
                            
                             rowPOS+=1;                                                             // INCREMENT rowPOS after sf
                              

                             if (iterV<Vsf && iterH==Hsf && numbOfComp==3)
                              {                                    
                                nextMCUs+=locMcuPerW;  
                                iterV+=1; 
                                iterH=0; 
                                rowPOS-=Hsf; 
                                mcuPOS = (nbrOfRows*locMcuPerW)+rowPOS;
                              } 


                             if (numbOfComp==3)                                               
                              {  
                                if (rowPOS==locMcuPerW) 
                                {                                     
                                   nbrOfRows+=1; 
                                   rowPOS=0; 

                                  if (compID==lumID) 
                                   {  
                                     nbrOfRows= nbrOfRows+(nextMCUs/locMcuPerW);                  // SKIP NEXT LINE
                                   } 
                                }

                              } 

                            endOfRow = evenMCUperW==1 ? locMcuPerW:orMcuPerW; 
                      

                            if (numbOfComp==1 && compID== lumID && rowPOS==endOfRow)          // SKIP LAST MCU OF THE ROW FOR LUM ACCOEFFS
                              {                                       
                                  nbrOfRows+=1; 
                                  rowPOS=0;                                   
                              } 

                            if (numbOfComp==1  && compID!= lumID && rowPOS== locMcuPerW)   
                              {  
                                  nbrOfRows+=1; 
                                  rowPOS=0;  
                              }           
                                   
                             mcuPOS = (nbrOfRows*locMcuPerW)+rowPOS;      
                                                                                                                                   
        }                                                                            // WHILE ITERH
                              mcuPositions[compID]=rowPOS;  
                              a_nbrOfRows [compID] = nbrOfRows; 
                              compoIter+=1;      
    }                                                                                // NUMBER OF COMPO
                                  
  }                                                                                  // I != NEXTMARKER

                              i = nextMarker;                  
                              return i;
}







uint32 jpeg::Decoder::analyseQT (uint32 i) 

 {       
    fstream myfile2;     
    myfile2.open (name,ios::app);

    uint8 byte; 
    uint8 temp;  
    uint16 QTLength;
    uint8 QTPrecision; 
    uint8 QTNumber=0; 
    uint8 QTPrecision2; 
    uint8 QTNumber2=0; 
    uint8 temp2[5][64]={0};  

    uint8 zigzagPos [64] = {
                            0,1,8,16,9,2,3,10,
                            17,24,32,25,18,11,
                            4,5,12,19,26,33,40,
                            48,41,34,27,20,13,
                            6,7,14,21,28,35,42,
                            49,56,57,50,43,36,
                            29,22,15,23,30,37,
                            44,51,58,59,52,45,
                            38,31,39,46,53,60,
                            61,54,47,55,62,63,   
                           };
     
    i+=1;  
    byte = globalArr [i];  
    QTLength     = byte;
    i+=1; 
    byte = globalArr [i]; 
    QTLength     = (QTLength<<8) | byte;            // QT Lenght 

    i+=1;
    byte = globalArr [i]; 
    QTPrecision  = (byte>>4) ? 16:8;  
    QTNumber     = byte & 0xf;


    for (int j = 0; j<64; j++) 
      {  
          i+=1; 
          byte = globalArr [i];  
          QT[QTNumber][j] = byte;
      }
  
    for (uint8 b=0; b<64; b++)                                        //  ZIGZAG TO NORMAL
      { 
          uint8 pos = zigzagPos[b]; 
          temp2[QTNumber][pos] = QT[QTNumber][b];  
      }
        
    for (uint8 b=0; b<64; b++) 
      {
          QT[QTNumber][b]=temp2[QTNumber][b];     
      }  
                 

    if (QTLength>75)                                                  // IF THERE ARE 2 QT
      { 
        i+=1;
        byte = globalArr [i]; 
        QTPrecision2  = (byte>>4) ? 16:8;  
        QTNumber2     = byte & 0xf;

        for (int j = 0; j<64; j++) 
          {  
               i+=1; 
               byte = globalArr [i];  
               QT[QTNumber2][j] = byte;
          }        

        for (uint8 b=0; b<64; b++)                                        // REORDER ZIGZAG
          { 
             uint8 pos = zigzagPos[b]; 
             temp2[QTNumber2][pos] = QT[QTNumber2][b];  
          }
        
        for (uint8 b=0; b<64; b++) 
          {
             QT[QTNumber2][b]=temp2[QTNumber2][b];     
          }  
      }
       

            
     myfile2 << "DQT                "<<endl;
     myfile2 << "QT LENGTH          : "; 
     myfile2 << convToHex(QTLength>>8)[0];                    // first part
     myfile2 << convToHex(QTLength>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(QTLength)[0];                      // second part
     myfile2 << convToHex(QTLength)[1]<<endl; 
     myfile2 << "QT PRECISION       : "; 
     myfile2 << convToHex(QTPrecision)[0]; 
     myfile2 << convToHex(QTPrecision)[1]<<endl; 
     myfile2 << "QT NUMBER          : "; 
     myfile2 << convToHex(QTNumber)[0]; 
     myfile2 << convToHex(QTNumber)[1]<<endl; 
     myfile2 << "QT VALUES          : ";
    
    for (int j =0; j<64; j++)                      
      
      {
        if (j%8==0&&j>0)                                         // START NEW LINE
            {
                myfile2 <<endl; 
                myfile2 <<"                     "; 
            }         
        myfile2 << convToHex(QT[QTNumber][j])[0]; 
        myfile2 << convToHex(QT[QTNumber][j])[1]<<" ";           
      }  

       myfile2 << endl; 
       myfile2 << endl;  
       myfile2 << endl; 
       myfile2 << endl;  


      if (QTLength>75)                                                    // PRINT SECOND QT
       {   
         myfile2 << "DQT                "<<endl;
         myfile2 << "QT PRECISION       : "; 
         myfile2 << convToHex(QTPrecision2)[0]; 
         myfile2 << convToHex(QTPrecision2)[1]<<endl; 
         myfile2 << "QT NUMBER          : "; 
         myfile2 << convToHex(QTNumber2)[0]; 
         myfile2 << convToHex(QTNumber2)[1]<<endl; 
         myfile2 << "QT VALUES          : ";
    
         for (int j =0; j<64; j++)                                 
         {
           if (j%8==0&&j>0)                                         // START NEW LINE
             {
                myfile2 <<endl; 
                myfile2 <<"                     "; 
              }         
                myfile2 << convToHex(QT[QTNumber2][j])[0]; 
                myfile2 << convToHex(QT[QTNumber2][j])[1]<<" ";           
          }  

               myfile2 << endl; 
               myfile2 << endl;  
               myfile2 << endl; 
               myfile2 << endl;  
        }

               return i; 
}





uint32 jpeg::Decoder::analyseSOF (uint32 i) 
  {     
    fstream myfile2;     
    myfile2.open (name,ios::app);
     
    uint8  byte; 
    uint8  temp;  
    uint16 SOFLength;
    uint8  SOFPrecision; 
    uint8  compID; 
    uint8  compSF; 
    uint8  compQT; 
    string names [6] = {"Cr","Cb","Y "}; 

    i+=1;  
    byte = globalArr [i];  
    SOFLength     = byte;
    i+=1; 
    byte = globalArr [i]; 
    SOFLength     = (SOFLength<<8) | byte;          // SOF LENGTH 
    
    i+=1; 
    byte = globalArr [i];                          // DATA PRECISION 
    SOFPrecision = byte; 
    
    i+=1;
    byte = globalArr[i];                           // IMAGE HEIGHT  
    height = byte;
    i+=1;
    byte = globalArr[i];
    height = (height <<8) | byte;

    i+=1;
    byte = globalArr[i];                           // IMAGE WIDTH  
    width = byte;
    i+=1;
    byte = globalArr[i];
    width = (width <<8) | byte;

    i+=1; 
    byte = globalArr [i];                         // NUMBER OF COMPONENTS
    compCount = byte; 

    for (uint8 b=0;b<compCount;b++)
        {      
          i+=1; 
          byte = globalArr [i];                         // COMPONENT ID 
          compID = byte; 
           
          if (b==0) 
            {
              lumID = compID;                          // SAVE ID
            }

          i+=1; 
          byte = globalArr [i];                         // COMPONENT SF 
          compSF = byte; 

          i+=1; 
          byte = globalArr [i];                         // COMPONENT QT
          compQT = byte; 

          compInfo [compID][0]= compSF;
          compInfo [compID][1]= compQT;
        }

     
    compCount                = lumID==0? (compCount-1):compCount;  

    lumVsf                   = compInfo[lumID][0]&0xf; 
    lumHsf                   = compInfo[lumID][0]>>4;   

    orWidth                 = width; 
    orHeight                = height;  

    height                   = height % 8 ? orHeight+(8-(orHeight%8)) : orHeight;               // CONVERT TO MULTIPLES OF 8
    width                    = width % 8 ? orWidth+(8-(orWidth%8)) : orWidth;

    mcuPerH                  = height/8; 
    mcuPerW                  = width/8; 

    orMcuPerH                = mcuPerH;
    orMcuPerW                = mcuPerW;
  
    lastMcuCol               = orWidth/8;
    lastMcuRow               = orHeight/8; 
     
    pixelRemainder           = orWidth%8;
    rowRemainder             = orHeight%8; 

    mcuPerH                  = mcuPerH%lumVsf? mcuPerH+(lumVsf-(mcuPerH%lumVsf)) : mcuPerH; 
    mcuPerW                  = mcuPerW%lumHsf? mcuPerW+(lumHsf-(mcuPerW%lumHsf)) : mcuPerW; 


    //cout << "lastMcuCol: " << lastMcuCol << " lastMcuRow: " << lastMcuRow << " pixelRemainder: " << pixelRemainder << " rowRemainder: "<< rowRemainder<< " orMcuPerW: " << orMcuPerW << " mcuPerW: "<< mcuPerW << endl;  


    mcuColRemainder          = mcuPerW-orMcuPerW;  
    
    compSF                   = (compInfo[lumID][0]>>4) * (compInfo[lumID][0]&0xf);      // ROUND UP NUMBER MCUs    
   
    numberOfMCUs[lumID]      =  mcuPerH*mcuPerW;
    numberOfMCUs[lumID+1]    =  numberOfMCUs[lumID]/compSF;                                                       
    numberOfMCUs[lumID+2]    =  numberOfMCUs[lumID]/compSF;   


     myfile2 << "START OF FRAME"<<endl;
     myfile2 << "SOF LENGTH          : "; 
     myfile2 << convToHex(SOFLength>>8)[0];                    // first part
     myfile2 << convToHex(SOFLength>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(SOFLength)[0];                       // second part
     myfile2 << convToHex(SOFLength)[1]<<endl; 
     myfile2 << "SOF PRECISION       : "; 
     myfile2 << convToHex(SOFPrecision)[0]; 
     myfile2 << convToHex(SOFPrecision)[1]<<endl; 
     myfile2 << "HEIGHT              : "; 
     myfile2 << convToHex(height>>8)[0];                     // first part
     myfile2 << convToHex(height>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(height)[0];                        // second part
     myfile2 << convToHex(height)[1]<<endl; 
     myfile2 << "WIDTH               : "; 
     myfile2 << convToHex(width>>8)[0];                      // first part
     myfile2 << convToHex(width>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(width)[0];                         // second part
     myfile2 << convToHex(width)[1]<<endl; 
     myfile2 << "NUMBER OF COMPONENTS: "; 
     myfile2 << convToHex(compCount)[0]; 
     myfile2 << convToHex(compCount)[1]<<endl; 
    
    for (uint8 j =lumID; j<=compCount; j++) 
        
       {
                myfile2 << names[compCount-j] << "                  : SAMPLING FACTOR " << convToHex(compInfo[j][0])[0]; 
                myfile2 << convToHex(compInfo[j][0])[1]<<" ";  
                myfile2 << endl; 
                myfile2 << names[compCount-j] << "                  : QUANTIZATION TABLE " << convToHex(compInfo[j][1])[0]; 
                myfile2 << convToHex(compInfo[j][1])[1]<<" ";   
                myfile2 << endl; 
       } 
                 
       
               myfile2 << endl; 
               myfile2 << endl;  
               myfile2 << endl; 
               myfile2 << endl;  

               return i; 
}




uint32 jpeg::Decoder::analyseHFT (uint32 i, uint8 lenFlag) 

 {    
    fstream myfile2;     
    myfile2.open (name,ios::app);

    uint8  byte; 
    uint8  numberSymb=0; 
    uint8  HFTid;
    uint8  HFTType;
    string HFTInfo [2] = {"DC","AC"};
    uint8  temp=0; 
    uint8  temp2[256];
    uint8  temp1[16]; 
    uint8  code_length = 0;
    
  

  if (lenFlag==0)                           
   { 
    
     i+=1;                                             // AVOID TAKING THE LENGHT TWICE
     byte = globalArr [i];  
     HFTLength     = byte;
     i+=1; 
     byte = globalArr [i]; 
     HFTLength     = (HFTLength<<8) | byte;           // HFT LENGTH 
                 
    if (HFTLength<20)
            
        {
            return i; 
        }
   }

    i+=1; 
    byte = globalArr [i];                          // HFT INFO 
    HFTType = (byte>>0x4)&0x1; 
    HFTid = byte&0xf;
   

    
    for (int b=0;b<16;b++)                        // ELEMENTS PER SYMBOL LENGTH 
        {      
          i+=1; 
          byte = globalArr [i];                       
          temp1[b] = byte; 
        }       


    for (int b=0;b<16;b++) 
    
        {
          numberSymb += temp1[b]; 
        }    


    for (int b=0;b<numberSymb;b++)                         // SYMBOLS 
        {      
          i+=1; 
          byte = globalArr [i];                       
          temp2[b] = byte; 
        }
         
        uint16  huffcode=0;
        uint8   symbol=0;
      
      
    for (uint32 hftCode=0;hftCode<=0xffff;hftCode++) 
        {
            HFT[HFTType][HFTid][hftCode] = 0x200; 
            codelen[HFTType][HFTid][hftCode] = 0x200; 
        }   
                   



    for (int b=0;b<16;b++) 
           
    {    
          code_length+=1;
          
            
          for (uint8 c=0;c<temp1[b];c++)                                            // TEMP1 ELEMENTS PER SYMBOL LENGTH 
            {
               HFT[HFTType][HFTid][huffcode]= temp2[symbol];                         
               codelen[HFTType][HFTid][huffcode] = code_length; 

               huffcode+=1;
               symbol+=1;      
            } 
            
               huffcode=huffcode*2;                                                     
    }
                
         


     myfile2 << "HUFFMAN TABLE"<<endl;
     myfile2 << "HFT LENGTH          : "; 
     myfile2 << convToHex(HFTLength>>8)[0];                    // first part
     myfile2 << convToHex(HFTLength>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(HFTLength)[0];                       // second part
     myfile2 << convToHex(HFTLength)[1]<<endl; 
     myfile2 << "HFT NUMBER          : "; 
     myfile2 << convToHex(HFTid)[0];
     myfile2 << convToHex(HFTid)[1]<<endl; 
     myfile2 << "HFT INFO            : ";
     myfile2 << HFTInfo[HFTType]<<endl;
     myfile2 << "NUMBER OF SYMBOLS   : ";
     myfile2 << convToHex(numberSymb)[0];                       // second part
     myfile2 << convToHex(numberSymb)[1]<<endl; 
     myfile2 << "ELEMENTS PER LENGTH : ";


    for (int j =0; j<16; j++) 
        
        {                       
            myfile2 << convToHex(temp1[j])[0]; 
            myfile2 << convToHex(temp1[j])[1]<<" ";           
        } 
            myfile2 << endl; 

     myfile2 << "SYMBOLS             : ";

    
    for (uint8 j =0; j<numberSymb; j++) 
        
       {    
           if (j%16==0&&j>0)
              {
                myfile2 <<endl; 
                myfile2 <<"                      "; 
              }         

            myfile2 << convToHex(temp2[j])[0]; 
            myfile2 << convToHex(temp2[j])[1]<<" ";          
       } 
                 
    myfile2 << endl; 

    myfile2 << "HFT CONTENT         : "; 
                  
    for (uint16 j =0; j<0xffff; j++) 
        
       {   
           if ((HFT[HFTType][HFTid][j]) != 0x200)
            {  

                if (temp%8==0&&temp>0)
                   {
                      myfile2 <<endl; 
                      myfile2 <<"                      "; 
                   }         

                myfile2 << convToHex(j>>8)[0]; 
                myfile2 << convToHex(j>>8)[1];   
                myfile2 << convToHex(j)[0]; 
                myfile2 << convToHex(j)[1] <<" ";  
                myfile2 << convToHex(HFT[HFTType][HFTid][j])[0]; 
                myfile2 << convToHex(HFT[HFTType][HFTid][j])[1] <<"    "; 
                temp+=1;  
              
            }
       }    
             
            myfile2 << endl; 
            myfile2 << endl; 
            myfile2 << endl; 
            myfile2 << endl; 

    return i; 
}



uint32 jpeg::Decoder::analyzeSOS (uint32 i, uint32 filesize) 

 {     
    fstream myfile2;     
    myfile2.open (name,ios::app);

    uint8  byte; 
    uint8  numbOfCompo; 
    uint16 SOSLength;
   
    
    

    i+=1;  
    byte = globalArr [i];  
    SOSLength     = byte;
    i+=1; 
    byte = globalArr [i]; 
    SOSLength     = (SOSLength<<8) | byte;               // SOS Lenght 

    i+=1;
    byte = globalArr [i];                                // NUMBER OF COMPO
    numbOfCompo = byte; 
    uint8  temp=0; 
    
    for (uint8 a =0; a<numbOfCompo; a++) 
    
       {         
         i+=1;
         byte = globalArr [i]; 
         compoInfo2[temp] = byte;                         // COMPO ID 
         i+=1;
         byte = globalArr [i]; 
         compoInfo2[temp+1] =  byte>>4;                   //  HFT FOR DC  
         compoInfo2[temp+2] =  byte&0xf;                  //  HFT FOR AC 
         temp+=3;                 
       }

     
     myfile2 << "START OF SCAN  "<<endl;
     myfile2 << "SOS LENGTH           : "; 
     myfile2 << convToHex(SOSLength>>8)[0];                    // first part
     myfile2 << convToHex(SOSLength>>8)[1];
     myfile2 << " "; 
     myfile2 << convToHex(SOSLength)[0];                      // second part
     myfile2 << convToHex(SOSLength)[1]<<endl; 
     myfile2 << "COMPO COUNT          : "; 
     myfile2 << convToHex(numbOfCompo)[0]; 
     myfile2 << convToHex(numbOfCompo)[1]<<endl; 
     myfile2 << "COMPONENTID          :";
     temp=0; 

    for (uint8 a=0;a<numbOfCompo;a++) 
    
      { 
        myfile2 <<" "<< convToHex(compoInfo2[temp]);  
        temp+=3;  
      } 

     myfile2 << endl; 
     myfile2 << "DC HFT               :"; 
     temp=1; 

    for (uint8 a=0;a<numbOfCompo;a++) 
    
      { 
        myfile2 <<" "<< convToHex(compoInfo2[temp]);  
        temp+=3;  
      } 
    

     myfile2 << endl; 
     myfile2 << "AC HFT               :";
     temp=2;  

     for (uint8 a=0;a<numbOfCompo;a++) 
    
      { 
        myfile2 <<" "<< convToHex(compoInfo2[temp]);  
        temp+=3;  
      } 


     myfile2 << endl; 
     myfile2 << endl;  
     myfile2 << endl; 
     myfile2 << endl;  

    cout << "       Scan: " << numberofscans +0<<"      NumberOfCompo:   "<< numbOfCompo+0 << " compID: " << compoInfo2[0]+0 <<" Three bytes: "; 

    for (uint8 a=1; a<4; a++) 
        { 
          cout << convToHex(globalArr[i+a])[0]<<convToHex(globalArr[i+a])[1]<<' ';
        }
          cout << endl; 

        
    if (DctMarkerType==0xc0) 
        { 
          i+=3; 
          i=scanDataBaselineDCT (i,numbOfCompo,filesize);       
        }

    if (DctMarkerType==0xc2) 
       {
         i=scanDataProgDCT (i,numbOfCompo,filesize);       
       }   

          return i; 
}





void jpeg::Decoder::parseImage (const string filename, uint32 picsize)  

  {
      
    const int SOF0  =  0xc0;                           // Start of Frame, Baseline DCT        
    const int SOF1  =  0xc1;                           // Start of Frame 1, Extended Sequential DCT               
    const int SOF2  =  0xc2;                           // Start of Frame 2, Progressive DCT                       
    const int SOF3  =  0xc3;                           // Start of Frame 3, Lossless (Sequential)
    const int HFT   =  0xc4;                           // Huffmam Table
    const int SOF5  =  0xc5;                           // Start of Frame 5, Differential Sequential DCT           
    const int SOF6  =  0xc6;                           // Start of Frame 6, Differential Progressive DCT          
    const int SOF7  =  0xc7;                           // Start of Frame 7, Differential Loessless (Sequential)   
    const int SOF9  =  0xc9;                           // Extended Sequential DCT, Arithmetic Coding              
    const int SOF10 =  0xca;                           // Progressive DCT, Arithmetic Coding                      
    const int SOF11 =  0xcb;                           // Lossless (Sequential), Arithmetic Coding                
    const int SOF13 =  0xcd;                           // Differential Sequential DCT, Arithmetic Coding          
    const int SOF14 =  0xce;                           // Differential Progressive DCT, Arithmetic Coding         
    const int SOF15 =  0xcf;                           // Differential Lossless (Sequential), Arithmetic Coding
     
    const int SOI   =  0xd8;                           // Start of image 
    const int EOI   =  0xd9;                           // end of image   
    const int SOS   =  0xda;                           // start of scan 
    const int QT    =  0xdb;                           // Quantization table
    const int HDR   =  0xe0;                           // Header
    const int COM   =  0xfe;                           // COMMENT
      

    fstream myfile; 
    myfile.open  (filename, ios::in | ios::out);

    imageName = filename; 

    name = filename+".txt";   
      
    uint8   byte;
    uint16  value;
    uint32  nextMarkerFlag=0;
    uint8   lengFlag; 

    numberofscans=0;   


    for (int i=0; i<picsize; i++) 
    
        {
            myfile >> noskipws >> byte;
            globalArr[i] = byte; 
        }


    for (int i=0;i<picsize;i++)
       
        {     
          if (globalArr[i] == 0xff) 
            { 
               i+=1;

               switch (globalArr[i]) 

                {
                   case  SOI : 
                         cout << "ADDRESS " << i << ": START OF IMAGE" << endl;
                         break; 
                   case  HDR : 
                         cout << "ADDRESS " << i << ": IMAGE HEADER" << endl; 
                         break;
                   case  QT : 
                         cout << "ADDRESS " << i << ": QUANTIZATION TABLE" << endl;
                         i = analyseQT(i);   
                         break; 
                   case  SOF0 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: BASELINE DCT" << endl;
                         DctMarkerType = SOF0; 
                         i = analyseSOF(i); 
                         break; 
                   case  SOF1 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: EXTENDED SEQUENTIAL DCT" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF2 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: PROGRESSIVE DCT" << endl;
                         DctMarkerType = SOF2; 
                         i = analyseSOF(i); 
                         break; 
                   case  SOF3 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: LOSSLESS (SEQUENTIAL)" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF5 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL SEQUENTIAL DCT" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF6 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL PROGRESSIVE DCT" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF7 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL LOSSLESS (SEQUENTIAL)" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF9 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: EXTENDED SEQUENTIAL DCT, ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break; 
                   case  SOF10 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: PROGRESSIVE DCT, ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break; 
                    case SOF11 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: LOSSLESS SEQUENTIAL, ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break; 
                    case SOF13 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL SEQUENTIAL, ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break; 
                    case SOF14 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL PROGRESSIVE DCT, ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break;
                    case SOF15 : 
                         cout << "ADDRESS " << i << ": START OF FRAME: DIFFERENTIAL LOSSLESS (SEQUENTIAL), ARITHMETIC CODING" << endl;
                         i = analyseSOF(i); 
                         break; 
                    case  HFT: 
                         cout << "ADDRESS " << i << ": HUFFMAN TABLE" << endl;

                             lengFlag = 0; 
                             nextMarkerFlag = findNextMarker(i, picsize);
                             i= analyseHFT(i,lengFlag);  
                         
                             if (i+5<nextMarkerFlag)
                               {  
                                 cout << "ADDRESS " << i << ": HUFFMAN TABLE" << endl;
                                 lengFlag=1; 
                                 i= analyseHFT(i,lengFlag); 
                               }
                                 break;
                   
                    case  COM: 
                         cout << "ADDRESS " << i << ": COMMENT" << endl; 
                         break;     
       
                   case  SOS: 
                         cout << "ADDRESS " << i << ": START OF SCAN";  
                         i = analyzeSOS (i, picsize);
                         break;

                   case  EOI:      
                              
                                zigzagTonormalPOS ();                                   
                                multiplyByQT ();
                                computeIDCT  ();                                   
                                computeRGB   (); 
                              // printMcuContent ();
                              // printMcuRGBContent();
                               // printRGBbase10 ();    


                   for (int x=0; x<=10000; x++) 
                        {
                            for (int y=0; y<64; y++) 
                                {
                                    int a = MCU[lumID][x][y]; 

                                    int b = MCU[lumID+1][x][y]; 

                                    int c = MCU[lumID+2][x][y];

                                    if (a!=0) 
                                        {
                                        // cout << "MCU: " << x << " bytePOS: "<< y << " val : "<< a << endl;  //<< "," << b << "," << c << endl; 
                                        }
                                 }
                        }



                          cout << "ADDRESS " << i << ": END OF IMAGE" << endl;
                          break; 
                }
            }
       }
  }

  
  