
char* jpeg::Encoder::convToHex (int number) 

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



void jpeg::Encoder::setUpfunc (uint8 lumsf, uint16 encWidth, uint16 encHeight, uint32 myfilesize, uint8 QT, string imName) 
  { 
    filesize                 = myfilesize==0? filesize:myfilesize; 
    lumID                    = lumID == 0? 0:lumID; 

    lumHsf                   = lumsf>>4; 
    lumVsf                   = lumsf&0xf; 
   

    compInfo[lumID][0]       = (lumHsf<<4) | lumVsf;   

    width                    = encWidth ==0 ? orWidth:encWidth;    
    height                   = encHeight==0 ? orHeight:encHeight; 

    orWidth                  = width; 
    orHeight                 = height; 

    height                   = height % 8 ? orHeight+(8-(orHeight%8)) : orHeight;               // CONVERT TO MULTIPLES OF 8
    width                    = width % 8 ? orWidth+(8-(orWidth%8)) : orWidth;
    
    mcuPerH                  = height/8; 
    mcuPerW                  = width/8; 

    orMcuPerH               = mcuPerH; 
    orMcuPerW               = mcuPerW; 

    mcuPerH                  = mcuPerH%lumVsf? mcuPerH+(lumVsf-(mcuPerH%lumVsf)) : mcuPerH; 
    mcuPerW                  = mcuPerW%lumHsf? mcuPerW+(lumHsf-(mcuPerW%lumHsf)) : mcuPerW; 
 
    lastMcuCol               = orWidth/8;
    lastMcuRow               = orHeight/8; 
     
    pixelRemainder           = orWidth%8;
    rowRemainder             = orHeight%8; 

   //cout << " lastMcuCol: "  << lastMcuCol << " lastMcuRow: " << lastMcuRow << " pixelRemainder: " << pixelRemainder << " rowRemainder: " << rowRemainder << " mcuPerW: "<< mcuPerW << " mcuPerH: " << mcuPerH << " lumHsf: "  << lumVsf+0 << endl; 

    char compSF              = lumHsf * lumVsf;       
   
    numberOfMCUs[lumID]      =  mcuPerH*mcuPerW;
    numberOfMCUs[lumID+1]    =  numberOfMCUs[lumID]/compSF;                                                       
    numberOfMCUs[lumID+2]    =  numberOfMCUs[lumID]/compSF; 

    if (QT == encQT) 
      {
        char tempQT[2][64]  = { 
                                0xa,0x7,0x15,0x18,0x1c,0x1c,0x18,0x19, 
                                0x8,0x16,0x16,0x1a,0x1a,0x18,0x19,0x18, 
                                0x16,0x16,0x18,0x18,0x1a,0x18,0x23,0x18, 
                                0x16,0x19,0x18,0x19,0x18,0x2b,0x26,0x1a, 
                                0x19,0x18,0x1a,0x1a,0x1e,0x38,0x35,0x24, 
                                0x1a,0x18,0x1a,0x1c,0x26,0x35,0x3b,0x2e, 
                                0x18,0x1c,0x25,0x2b,0x35,0x40,0x3f,0x34, 
                                0x21,0x2e,0x2f,0x31,0x3b,0x34,0x35,0x31, 
                         
                                0xc,0xc,0x6,0x10,0x31,0x31,0x31,0x31, 
                                0xc,0x6,0x6,0x1d,0x31,0x31,0x31,0x31, 
                                0x6,0x6,0x16,0x31,0x31,0x31,0x31,0x31, 
                                0x10,0x1d,0x31,0x31,0x31,0x31,0x31,0x31, 
                                0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31, 
                                0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31, 
                                0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31, 
                                0x31,0x31,0x31,0x31,0x31,0x31,0x31,0x31,  
                              };                  

        for (uint8 a=0; a<2; a++) 
           {
              for (uint8 b=0; b<64; b++) 
                {
                  jpeg::QT[a][b]= tempQT[a][b]; 
                }
           }    
        
        imageName            = imName; 
        compCount            = 2;    
        
        compInfo[lumID][1]   =0;                             // QT ADDRESS
        compInfo[lumID+1][1] =1;
        compInfo[lumID+2][1] =1;  
        
      }

  }

 

void jpeg::Encoder::readRgbContent() 
   
 {      
    cout << "READ RGB CONTENT..."<<endl;    

    fstream myfile; 
    char byte=0; 
    unsigned int  address=0; 
    unsigned char value; 

    
    myfile.open ("code6.txt", ios::in | ios::out); 
     

    while (address < filesize) 
     {
      myfile >> noskipws >> byte; 

      if (byte==0x3a)                                              // 0x3a = ':'                                                                
        {            
            myfile >> noskipws >> byte; 
          
            if (byte == 0x20)                                      // 0x20 = ' '
              {                
                char channel =lumID;    

                while (channel<= compCount) 
                   {   
                      unsigned char  rgbArray [3] = {0};  
                      char           numberOfchars=0;
   
                      while (1) 
                        { 
                            myfile >> noskipws >> byte; 
                            
                            if (byte==0x2c)                             // 0x2c = ','
                              {   
                                  numberOfchars-=1; 
                                  break; 
                              }

                            rgbArray[numberOfchars] = byte; 
                            numberOfchars+=1; 
                        }                      

                            if (numberOfchars==2) 
                               {
                                 value = ((rgbArray[0]-48)*100) + ((rgbArray[1]-48)*10) + (rgbArray[2]-48); 
                               } 
                            
                            if (numberOfchars==1) 
                               {
                                 value = ((rgbArray[0]-48)*10) + (rgbArray[1]-48); 
                               }                               

                            if (numberOfchars==0) 
                              {
                                 value = rgbArray[0]-48; 
                              }   

                        
                            rgbValues [channel][address] = value;  
                            channel+=1; 

                    }
                            address+=1; 
                              
              } 
        }
   }
}








void jpeg::Encoder::computeYcbCr () 

 {  
        int mcuPOS = numberOfMCUs[lumID]; 
            
        for (int POS=0; POS<mcuPOS; POS++) 
            {
              for (char bytePOS = 0; bytePOS<64; bytePOS++) 
                  {
                      unsigned char R  =  MCU[lumID][POS][bytePOS];              
                      unsigned char G  =  MCU[lumID+1][POS][bytePOS];      
                      unsigned char B  =  MCU[lumID+2][POS][bytePOS];  

                      float Y  =   0.299*R + 0.587*G + 0.114*B;
                      float Cb =  -0.1687*R - 0.3313*G + 0.5*B + 128;
                      float Cr =   0.5*R - 0.4187*G - 0.0813*B + 128;     

                      MCU[lumID][POS][bytePOS] = Y; 
                      MCU[lumID+1][POS][bytePOS]= Cb; 
                      MCU[lumID+2][POS][bytePOS] = Cr;    

                  }

            }
      
 }







void jpeg::Encoder::convToMCU () 
  { 
     
    cout << "CONVERT TO MCUs..."<<endl;    

    float              value; 
    unsigned int       address=0; 
    uint8              a_loopEnd; 
    uint8              b_loopEnd; 

    
    for (char compID=lumID; compID<=compCount; compID++) 
      {
        unsigned int mcuPOS   = numberOfMCUs[lumID]; 

          for (int pos=0; pos<mcuPOS; pos++) 
            {                                            
              for (char bytePOS=0; bytePOS<64; bytePOS++) 
                {  
                  MCU[compID][pos][bytePOS]=0;                         
                }                                        
            }
     }

   // cout << " lastMcuCol: "  << lastMcuCol << " lastMcuRow: " << lastMcuRow << " pixelRemainder: " << pixelRemainder << " rowRemainder: " << rowRemainder << " orMcuPerW: " << orMcuPerW << " nbrOfMcus: " << numberOfMCUs[lumID] <<endl; 
   
    for (unsigned short h=0; h<orMcuPerH; h++)                           
      {    
        a_loopEnd = h==lastMcuRow? rowRemainder:8; 

        for (char a=0; a<a_loopEnd; a++)
          {  
            for (unsigned short w=0; w<orMcuPerW; w++) 
              {        
                b_loopEnd = w==lastMcuCol? pixelRemainder:8;  

                for (char b=0;b<b_loopEnd; b++) 
                  { 
                   // address=  b + (w*8) + (width*a) + (h*width*8); 

                    for (char compID=lumID; compID<=compCount; compID++) 
                     {                                                                                 
                        value = rgbValues [compID][address]; 
                                                    
                        MCU[compID][(h*mcuPerW) +w][a*8+b] = value;  
                     }
                     
                        address+=1;  
                  }
              }
           }
      }                                 
  }




void jpeg::Encoder::computeDCT ()                            // use u and v to go from one freq to the next
                                                            // use x and y to go from one pos within a freq to the next
 { 

   cout << "COMPUTE DCT..."<<endl;     

   int   mcuPOS;
   float  temp[64]; 


   for (char compID = lumID; compID <=compCount; compID++)
    {    
       mcuPOS=numberOfMCUs[lumID]; 

       for (int pos=0; pos<mcuPOS; pos++) 
         {  
          for (char u = 0; u<8; u++)                                                       //  U AND V SELECT ONE COEFF TABLE
            {                                                                              // WHILE X AND Y GO THROUGH ALL THE 
              for (char v = 0; v<8; v++)                                                   // ELTS IN THE MCU AND IN THE COEFF TABLE
                 { 
                    float coeff = 0.0;
                
                     for (char x = 0; x<8; x++)
                        {
                          for (char y=0; y<8; y++) 
                            {
                               float cu = u == 0 ? 1.0 / sqrt(2.0) : 1.0;
                               float cv = v == 0 ? 1.0 / sqrt(2.0) : 1.0;
                        
                               coeff += cu * cv * (MCU[compID][pos][8*x+y]-128) * cos((2 * x + 1) * u * M_PI / 16.0) *
                                          cos((2 * y + 1) * v * M_PI / 16.0);
                            }
                        }     
                              

                               temp[8*u+v] = coeff/4;
                  }
            }
                               for (char bytePOS=0;bytePOS<64; bytePOS++) 
                                 {
                                   MCU[compID][pos][bytePOS] = temp[bytePOS]; 

                                     if (compID==1 && pos == 0)
                                      {
                                        //  cout << " bytePOS: " <<  bytePOS+0 << "  " << temp[bytePOS] << endl; 
                                      }  
                                 }       

                             
                               
         }
    }
}




char* jpeg::Encoder::orderZigzagPOS (char orgArr[500], int size)
   
  {     
    char  tempArr[500]; 
  
    char zigzagPos [64] = {
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


    for (char bytePOS = 0; bytePOS<size; bytePOS++) 
      {
        tempArr[bytePOS] = orgArr[bytePOS];
      }

    for (char bytePOS=0; bytePOS<size; bytePOS++) 
      { 
        char temp2; 
        temp2           = zigzagPos [bytePOS]; 
        orgArr[bytePOS] = tempArr[temp2]; 
      } 

     return orgArr;  
 }



void jpeg::Encoder::orderZigzag ()
   
  {     
    cout << "ORDER IN ZIGZAG..."<<endl;      
  
    int     mcuPOS; 
    float   temp [64]; 

    char zigzagPos [64] = {
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


    for (char compID=lumID; compID<=compCount; compID++) 
      { 
        mcuPOS = numberOfMCUs[lumID]; 
            
        for (int pos=0; pos<mcuPOS; pos++) 
            {
              for (char bytePOS = 0; bytePOS<64; bytePOS++) 
                  {
                    temp[bytePOS] = MCU[compID][pos][bytePOS];
                  }

              for (char bytePOS=0; bytePOS<64; bytePOS++) 
                 { 
                   char temp2; 
                   temp2 = zigzagPos [bytePOS]; 
                   MCU[compID][pos][bytePOS] = temp[temp2]; 
                 }
            }
      }
 }



void jpeg::Encoder::divideByQT () 
    {  
          
          cout << "DIVIDE BY QT VALUES..."<<endl;    

          char QTNumber;  
          int  mcuPOS; 
          int val; 
   

          for (char compID=lumID; compID<=compCount; compID++) 
            {
               mcuPOS   = numberOfMCUs[lumID]; 
               QTNumber = compInfo [compID][1];
             
               for (int pos=0; pos<mcuPOS; pos++) 
                   {                                            
                      for (char bytePOS=0; bytePOS<64; bytePOS++) 
                         {  
                            val = MCU[compID][pos][bytePOS]; 
                            MCU[compID][pos][bytePOS] = val / QT[QTNumber][bytePOS];                          
                         }                     
                   }
            }
     }



void jpeg::Encoder::findDCdiff () 
   {        
          cout << "COMPUTE DC DIFFERENCES..."<<endl;    
          
          int  mcuPOS; 
          int  nextLine; 
          int  dcCoeff; 
          int  previousDC; 
          char iterH; 
          char iterV; 
          unsigned short POS; 
          unsigned short nextMCU; 
          unsigned short endOfLine;

          for (char compID=lumID; compID<=compCount; compID++) 
            {    
                Hsf             = compID==lumID? lumHsf:1; 
                Vsf             = compID==lumID? lumVsf:1; 
                mcuPOS          = numberOfMCUs[lumID];   
                nextMCU         =0;  
                POS             =0;
                previousDC      = 0; 
            
               
                while (POS<mcuPOS) 
                  {       
                     iterV=0; 
                     nextMCU =0; 

                     while (iterV<Vsf) 
                        {      
                            iterH = 0; 
                           
                            while (iterH<Hsf)  
                                {
                                  dcCoeff =  MCU[compID][POS+nextMCU][0];                          
                                  MCU[compID][POS+nextMCU][0] = dcCoeff-previousDC; 

                                  //cout << "compID: " << compID+0 << "mcuPOS: " << POS+nextMCU << " dcCoeff: " << dcCoeff << " previousDC: "<< previousDC << " diff: "<<  MCU[compID][POS+nextMCU][0] << endl;
                     
                                  previousDC=dcCoeff; 
                                  iterH+=1;
                                  nextMCU+=1;                              
                                } 
                                  nextMCU-=iterH; 
                                  nextMCU+=mcuPerW; 
                                  iterV+=1; 
                        }                    
                                  POS+=lumHsf; 
                                  endOfLine = POS%mcuPerW; 

                                 if (!endOfLine) 
                                    {
                                      POS+=(mcuPerW*(lumHsf-1)); 
                                    }                                                      
                  }

            }     

       
   }



void jpeg::Encoder::removeBits (char bits) 
  {
        cout << "REMOVE BITS : "  << bits+0 << endl;  

        int  mcuPOS;   
        int  value;  

        for (char compID=lumID; compID<=compCount; compID++) 
          {
            mcuPOS   = numberOfMCUs[lumID]; 
             
            for (int pos=0; pos<mcuPOS; pos++) 
              {                                            
                for (char bytePOS=0; bytePOS<64; bytePOS++) 
                         
                    {  
                      value = MCU[compID][pos][bytePOS]; 
                      
                       if (value!=0x80000000) 
                         {  
                            if (value<0 && value>-3)
                              {
                                value=0; 
                              }

                            MCU[compID][pos][bytePOS] = value >> bits; 
                         }

                       else 
                         {
                           cout << "0x80000000 Found!" << " CompID: "<< compID <<" pos: "<< pos <<" bytePOS: " << bytePOS << endl; 
                         }  
                        
                    }                     
                   
              }
          }
  }





void jpeg::Encoder::revConvCoeff () 
  {  
     
     cout << "CONVERT COEFFS..."<<endl;    

     int number; 
     int mcuPOS; 
     unsigned int  cat; 
     unsigned int  value; 
    
     for (char compID=lumID; compID<=compCount; compID++) 
        {
          mcuPOS   = numberOfMCUs[lumID]; 
             
          for (int pos=0; pos<mcuPOS; pos++) 
            {                       
               for (char bytePOS=0; bytePOS<64; bytePOS++) 
                   {               
                      number = MCU[compID][pos][bytePOS]; 

                      if (number >=0) 
                        {
                           for (char b=0; b<32; b++) 
                            {
                              cat = number & (0x80000000>>b);
                              
                              if (cat) 
                               {
                                 cat = 32-b; 
                                 Categories [compID][pos][bytePOS]=cat; 
                                 break; 
                               }
                            }
                        }

                      else  
                        {  
                           number = (~number)+1;                               // TURN NEGATIVE NUMBER INTO POSITIVE

                           for (char b=0; b<32; b++) 
                            {
                              cat = number & (0x80000000>>b);
                              
                              if (cat) 
                               {
                                 cat = 32-b;
                                 value = 0xffffffff >> b;                   
                                 value = value - number; 
                                 Categories [compID][pos][bytePOS]=cat; 
                                 MCU[compID][pos][bytePOS]=value; 


                                 if (pos== 2498 && compID==1) 
                                    {
                                  //   cout << "b  " <<  b+0 << " bytePOS " << bytePOS+0 << " MCu number: "<< pos << " numberToConv: " << number << " cat: " << cat+0 << " val "  << value << endl;  
                                    }

                                 break; 
                               }
                            }
                        }
                                                                           
                    } 
            
            }

        }
  }
 



void jpeg::Encoder::countZeros () 
  {
        cout << "COUNT ZEROS...."  << endl;  

        int  mcuPOS;   
        int  value;  
        

        for (char compID=lumID; compID<=compCount; compID++) 
          {
            mcuPOS   = numberOfMCUs[lumID]; 
             
            for (int pos=0; pos<mcuPOS; pos++) 
              {                                    
                unsigned char numbOfzeros=0; 
                unsigned char bytePOS=1; 
                unsigned char bridgePOS=0; 
                unsigned char previousPOS=0;
                unsigned char bridgeZeros=0; 

                while (bytePOS<64)                          
                    {  
                      value = MCU[compID][pos][bytePOS]; 

                       while (value==0 && bytePOS<64) 
                         { 
                           bytePOS+=1;
                           numbOfzeros+=1;   
                           value = MCU[compID][pos][bytePOS]; 
                         }
                        
                        if (bytePOS>=64) 
                          {
                             break; 
                          }

                        if (numbOfzeros>=16) 
                          {  
                              while (numbOfzeros>=16) 
                                {
                                   numbOfzeros -= 16; 
                                   bridgePOS  =  previousPOS+16;
                                   bridgeZeros = 15; 
                                   zeroCountArr[compID][pos][bridgePOS]=bridgeZeros; 
                                   previousPOS = bridgePOS;  
                                } 
                          }

                        zeroCountArr[compID][pos][bytePOS]=numbOfzeros; 
                        numbOfzeros=0; 
                        previousPOS=bytePOS; 
                        bytePOS+=1;                            
                        
                    }                     
                                
              }    
              
          }
  }




void jpeg::Encoder::hftSymb () 
  {
        cout << "GENERATE HFT SYMBOLS...."  << endl;  

        int  mcuPOS;         
        unsigned char zeroCount =0; 
        unsigned char cat =0;                  
        unsigned char symbol=0; 

        for (char compID=lumID; compID<=compCount; compID++) 
          {
            mcuPOS   = numberOfMCUs[lumID]; 
             
            for (int pos=0; pos<mcuPOS; pos++) 
              {                
                for (char bytePOS=0; bytePOS<64; bytePOS++) 
                         
                    {  
                      zeroCount = zeroCountArr[compID][pos][bytePOS]; 
                      cat       = Categories[compID][pos][bytePOS]; 
                      symbol    = (zeroCount<<4)|cat; 
                 
                      hftSymbArr[compID][pos][bytePOS]= symbol;  
                    }                     
                   
              }
          }
  }  



void jpeg::Encoder::hftValues () 
  {
        cout << "GENERATE HFT VALUES...."  << endl;  

        unsigned int  mcuPOS;                  
        unsigned char symbol=0; 
        unsigned char zeroCount =0; 
        unsigned char cat =0;
        unsigned char lumOrChr=0;  
        unsigned long freqDC[2][256] ={0}; 
        unsigned long freqAC[2][256] ={0}; 
        unsigned char eltsPerLenArr[17] ={0,0,2,2,1,3,3,3,2,3,5,2,5,2,4,4,87};  
        unsigned int  temp [300]={0}; 
       

        for (char compID=lumID; compID<=compCount; compID++) 
          {
            mcuPOS   = numberOfMCUs[lumID]; 
            lumOrChr = compID==lumID? 0:1; 
            
            for (int pos=0; pos<mcuPOS; pos++) 
              {                
                for (char bytePOS=0; bytePOS<64; bytePOS++) 
                    {  
                      if (bytePOS==0) 
                        {    
                           symbol = hftSymbArr[compID][pos][bytePOS]; 
                           freqDC [lumOrChr][symbol]+=1; 
                        } 

                      else  
                       {    
                           symbol = hftSymbArr[compID][pos][bytePOS]; 
                           freqAC [lumOrChr][symbol]+=1; 
                       } 

                    }                                     
              }
          }
     

    for (lumOrChr=0; lumOrChr<=1; lumOrChr++) 
      {     
        unsigned long maxFreq; 
        unsigned short maxFreqPOS; 
        unsigned short huffCode=0; 
        unsigned short huffCodeLen=2;
        unsigned char  elements =0; 
        unsigned short b; 
        unsigned char  eltsPerLen;  
        unsigned short symbCount=0; 
        
       
        for (int a=0; a<=255; a++) 
          {  
            b=0; 
            maxFreq=0; 

            while (b<=255) 
              {  
                if (freqDC[lumOrChr][b]>maxFreq) 
                  {   
                      maxFreq= freqDC[lumOrChr][b]; 
                      maxFreqPOS = b; 
                  }                        
                      b+=1;   
              }

            if (maxFreq==0) 
              {
                break; 
              }  
     

            temp [maxFreqPOS]                    =    freqDC[lumOrChr][maxFreqPOS];  

            freqDC[lumOrChr][maxFreqPOS]         =0; 
            dcHFTvalues[lumOrChr][maxFreqPOS][0] = huffCode;  
            dcHFTvalues[lumOrChr][maxFreqPOS][1] = huffCodeLen;
            huffCode+=1;
           // cout << " maxFreqPOS: "<< maxFreqPOS << " maxFreq: " << temp [maxFreqPOS]  << " huffCode: " << dcHFTvalues [lumOrChr][maxFreqPOS][0] << " codeLen: " << huffCodeLen << endl; 
                            
            elements+=1; 
            eltsPerLen = eltsPerLenArr[huffCodeLen];

            if (elements == eltsPerLen) 
               {                  
                  huffCode = huffCode*2; 
                  huffCodeLen+=1; 
                  elements=0; 
               }    

            symbCount =  nbrOfSymbols[0][lumOrChr]; 
            hftSymbols [0][lumOrChr][symbCount]= maxFreqPOS; 
            symbCount+=1; 
            nbrOfSymbols[0][lumOrChr]=symbCount;            
                        
          }
         
        huffCodeLen =2;  
        elements    =0; 
        huffCode    =0; 
        
        for (int a=0; a<255; a++) 
          {  
            b=0; 
            maxFreq=0; 

            while (b<255) 
              {  
                if (freqAC[lumOrChr][b]>maxFreq) 
                  {   
                      maxFreq= freqAC[lumOrChr][b]; 
                      maxFreqPOS = b; 
                  }                        
                      b+=1;   
              }

            if (maxFreq==0) 
              {
                break; 
              }  
  
            temp [maxFreqPOS]        =    freqAC[lumOrChr][maxFreqPOS];        

            freqAC[lumOrChr][maxFreqPOS]         =0; 
            acHFTvalues[lumOrChr][maxFreqPOS][0] = huffCode;  
            acHFTvalues[lumOrChr][maxFreqPOS][1] = huffCodeLen;
            huffCode+=1;

            //cout << " maxFreqPOS: "<< maxFreqPOS << " maxFreq: " << temp [maxFreqPOS]  << " huffCode: " << acHFTvalues [lumOrChr][maxFreqPOS][0] << " codeLen: " << huffCodeLen << endl; 
                         
            elements+=1;     
            eltsPerLen = eltsPerLenArr[huffCodeLen]; 

            if (elements==eltsPerLen) 
              { 
                huffCode = huffCode*2; 
                huffCodeLen+=1; 
                elements=0; 
              }

            symbCount =  nbrOfSymbols[1][lumOrChr]; 
            hftSymbols [1][lumOrChr][symbCount]= maxFreqPOS; 
            symbCount+=1; 
            nbrOfSymbols[1][lumOrChr]=symbCount;                
                        
          }

      }       

  }  


void jpeg::Encoder::sendBytes (char arr[500000], int arrSize) 
     {
        fstream myfile2; 
        string  name = "copy_" + imageName; 
        myfile2.open (name, ios::app); 
        int size=0;  

        while (size< arrSize) 
            {
               char byte = arr[size];  
               myfile2 << byte; 
               size+=1; 
            }              
     }  


unsigned short jpeg::Encoder::prepareBytes (char len, unsigned short value, unsigned short retVal) 
   {   
      
      unsigned char byteSize = retVal>>8;
      unsigned char byte     = retVal&0xff;  

      for (char s=0; s<len; s++) 
        {
            unsigned short bit = value & (0x8000>>(0x10-(len-s)));               

            if (bit==0) 
              {
                byte = (byte<<1)|0; 
                
              }

            else    
              {
                byte = (byte<<1)|1; 
                
              }

            byteSize+=1;   

            if (byteSize==8) 
              {        
                char byteArr[1]={byte}; 
                 
                sendBytes(byteArr, 1); 

               if (byte==0xff) 
                 { 
                   byte=0; 
                   byteArr[0]=byte; 
                   sendBytes(byteArr, 1); 
                 }

                byte=0; 
                byteSize=0;
              }
        } 

              retVal = byteSize; 
              retVal = (retVal<<8)| byte; 

             return retVal; 
   } 
 

void jpeg::Encoder::generateJPEG() 
    {

       cout << "CREATE FINAL JPEG FILE, PLEASE WAIT..."  << endl;  

       int mcuPOS2             =0; 
       
       int  POS                = numberOfMCUs [lumID]; 
       int  mcuPOS             =0; 
       int  mcuPositions[5]    ={0};

       char temp[500]          = {0};
       char SOI [20]           = {0xff,0xd8,0xff,0xe0,0,0x10,0x4a,0x46,0x49,0x46,0,0x01,0x01,0,0,0x01,0,0x01,0,0};
       char QT1[5]             = {0xff,0xdb,0,0x43,0}; 
       char QT2[5]             = {0xff,0xdb,0,0x43,0x1}; 
       char H1                 = orHeight>>8;
       char H2                 = orHeight&0xff; 
       char W1                 = orWidth>>8;
       char W2                 = orWidth&0xff; 
       char chrbID             = lumID+1; 
       char chrrID             = lumID+2;     
       char lumSF              = (lumHsf<<4)|lumVsf; 
       char SOF [19]           =  {0xff,0xc0,0,0x11,0x8,H1,H2,W1,W2,0x3,lumID,lumSF,0,chrbID,0x11,0x1,chrrID,0x11,0x1};     

       sendBytes  (SOI,20); 
       sendBytes  (QT1,5); 
       
       for (int n=0; n<64; n++) 
          {                      
            temp[n] = QT[0][n];               
          }

      char* ptr = orderZigzagPOS (temp,64); 

      sendBytes  (ptr,64);      
      sendBytes  (QT2,5); 
      
      for (int n=0; n<64; n++) 
          {                      
            temp[n] = QT[1][n];               
          }

      ptr = orderZigzagPOS (temp,64);
      sendBytes  (ptr,64); 
      sendBytes  (SOF,19); 
      
                                            // HFT
      char HFTtypeANDid             = 0;  
      char sgmtlen1                 = 0; 
      char sgmtlen2                 = 0; 
      char lumOrChr; 
      unsigned short HFTlen         = 0;  
      unsigned short sgmtlen        = 0;  
     

      for (char a; a<=1; a++) 
        {          
          for (char HFTtype=0; HFTtype<=1; HFTtype++) 
             {
                lumOrChr                       = a; 
                char eltsPerLen[17]           = {0}; 

                for (int x=0; x<=255;x++) 
                  {
                    HFTlen = HFTtype==0 ? dcHFTvalues[lumOrChr][x][1]:acHFTvalues[lumOrChr][x][1];
                    eltsPerLen [HFTlen]+=1;
                  }     

                
                HFTlen           =  nbrOfSymbols[HFTtype][lumOrChr];   
                sgmtlen          =  HFTlen+3+16; 
                sgmtlen1         =  (sgmtlen>>8); 
                sgmtlen2         =  (sgmtlen&0xff);  
                HFTtypeANDid     =  HFTtype; 
                HFTtypeANDid     =  (HFTtypeANDid<<4)|lumOrChr; 
                char HFT[5]      = {0xff,0xc4,sgmtlen1, sgmtlen2, HFTtypeANDid};
                
                for (int n=0; n<HFTlen; n++) 
                   {                      
                      temp[n] = hftSymbols[HFTtype][lumOrChr][n];               
                   }

                sendBytes(HFT,5);  
                sendBytes(eltsPerLen+1,16);   
                sendBytes(temp,HFTlen);                 
             }  
        }        
        
       
    char SOS [14]  =  {0xff,0xda,0,0xc,0x3,lumID,0,chrbID,0x11,chrrID,0x11,0,0x3f,0};   
    sendBytes(SOS,14); 
    
    char           compID=0; 
    unsigned short retVal=0; 
  
        
    while (mcuPOS<POS)
      {       
        for (compID=lumID; compID<=compCount; compID++) 
          {              
            Hsf      = compID==lumID? lumHsf:1;                     // FOR CONTROLLING ITERH AND ITERV LOOPS
            Vsf      = compID==lumID? lumVsf:1;  
            mcuPOS   = mcuPositions[compID];

            unsigned short iterH =0; 
            unsigned short iterV=1;  
            unsigned char  HFTcodeLen;  
            unsigned short HFTcode;  
            unsigned short coeff;  
            unsigned char  symbol; 
            unsigned char  EOBflag; 
            unsigned char  coeffSize;              
            unsigned short nextMCUs=0;

          
            while (iterH < Hsf) 
              { 
                lumOrChr = compID== lumID? 0:1;  
                      
                for (char bytePOS=0; bytePOS<64; bytePOS++) 
                  {                               
                      EOBflag       =0; 
                      symbol        = hftSymbArr[compID][mcuPOS+nextMCUs][bytePOS];     
                            
                      if (bytePOS==0) 
                        {                              
                          HFTcode    = dcHFTvalues[lumOrChr][symbol][0]; 
                          HFTcodeLen = dcHFTvalues[lumOrChr][symbol][1];
                        } 

                      else  
                        {    
                          HFTcode    = acHFTvalues[lumOrChr][symbol][0]; 
                          HFTcodeLen = acHFTvalues[lumOrChr][symbol][1];
                        } 

                      if (symbol!=0 || bytePOS==0) 
                        {
                          retVal   = prepareBytes (HFTcodeLen, HFTcode, retVal);
                            
                          coeff     = MCU[compID][mcuPOS+nextMCUs][bytePOS]; 
                          coeffSize = Categories [compID][mcuPOS+nextMCUs][bytePOS];   
                             
                          retVal   = prepareBytes (coeffSize, coeff, retVal);     
                          EOBflag  =1;               
                         
                         if (compID != lumID && bytePOS==0) 
                            {
                             // cout << " compID: " << compID+0  <<  " symb: " << symbol+0 << "bytePOS: " << bytePOS+0 << "MCU: " << mcuPOS2 << " nextMCUs: " << nextMCUs << " HFTcode: " << HFTcode << " Len: "<< HFTcodeLen+0 << " coeff: "<< coeff << " coeffsize: " << coeffSize+0 << endl;  
                            }                         
                        }  

                      if (bytePOS==63 && EOBflag==0)                             // IF EOBFLAG==1, THERE IS A COEFF AT 63 AND EOB IS NOT NEEDED
                        {
                          char EOB = 0; 
                          retVal   = prepareBytes (2, 0, retVal);  
                        }       
                      
                   }                                                              // FOR, BYTEPOS
                      
                       iterH+=1;  

                       if (compID==lumID+1) 
                       { 
                         mcuPOS2+=1; 
                       }

                       mcuPOS = compID == lumID? mcuPOS+1:mcuPOS+lumHsf;      // TAKE ONE CHR MCU FOR EVERY lumHsf MCUs
                       
                       if (iterV<Vsf && iterH==Hsf)
                          {                                    
                            nextMCUs+=mcuPerW;  
                            iterV+=1; 
                            iterH=0; 
                            mcuPOS-=Hsf; 
                          }                                
              }                                                                 // WHILE (iterH)
                          
                       int nbrOflines = mcuPOS%mcuPerW;                        // COMES AFTER COMPID LOOP
                               
                        if (!nbrOflines) 
                            { 
                              if (compID==lumID) 
                               {
                                  mcuPOS+=nextMCUs;                                 // SKIP NEXT LINES
                               }

                              else  
                                {
                                   mcuPOS= mcuPOS+(mcuPerW*(lumVsf-1));          // SKIP lumVsf LINES FOR CHR 
                                }
                            } 
                                
                        mcuPositions[compID] = mcuPOS;                                                                                                               
                             
         }                                                                      // FOR, COMPID
                   
                            
                                                
     }                                                                          //  WHILE MCUPOS
   

                        char EOI[2] ={0xff,0xd9};
                        sendBytes(EOI,2);    
} 



    

void jpeg::Encoder::printMcuContent (string name) 
 
  {  

    cout << "PRINT VALUES IN "<< name << "...." << endl;    

    fstream myfile2; 
    myfile2.open (name, ios::app); 
    
    char            horzSF   = 0;  
    char            vertSF   = 0;  
    char            iterH; 
    char            iterV;   
    char            iter; 
    char            nbrOflines;
    char            compID;
    char            sign; 
    bool            isnegative=0; 
    unsigned short  mcuPerW = width/8;
    unsigned short  printedPOS=0;  
    unsigned short  mcuCount[4]={0};
    unsigned short  printedPosArr[4]={0}; 
    string          compNames[6]={"Cr","Cb","Y "}; 
    int             counter=0; 
     
  

     for (compID=lumID; compID<=compCount;compID++) 
        {    
          
          int mcuPOS = numberOfMCUs[lumID]; 
           
          for (int pos = 0; pos<mcuPOS; pos++) 
            { 
               for (char bytePOS=0; bytePOS<64; bytePOS++) 
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
          horzSF     = (compInfo[compID][0]>>4);  
          vertSF     = (compInfo[compID][0]&0xf); 
          nbrOflines =0;  
          printedPOS = printedPosArr[compID]; 

          
          myfile2 << endl; 
          myfile2 << endl; 
          myfile2 << "COMPONENT            : "<< compNames[compCount-compID]<<endl; 
          myfile2 << endl;        
        
          while (iterH<horzSF) 
            
            {     

                 myfile2 << "MCU NUMBER           : "<< mcuCount[compID] <<endl; 
                                   
                          for (char bytePOS=0; bytePOS<64; bytePOS++) 
                                {    
                                          if (bytePOS%8==0)                                                                               
                                              {
                                                myfile2 <<endl; 
                                                myfile2 <<"                      "; 
                                              }   
                                      
                                                isnegative = printableMCU[compID][printedPOS][bytePOS] & 0x80000000;

                                                //int x = (MCU[compID][printedPOS][bytePOS]);  //signed int x   = printableMCU[compID][printedPOS][bytePOS]; 

                                             //   if (compID==1 && x!=0)   //bytePOS!=0 && x!=0) 
                                             //     { 
                                                  // cout << mcuCount[compID] << ": " << x+0 << " bytePOS: " <<bytePOS+0<< endl; 
                                             //     }
                                                                      

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

                                                  iterH+=1; 
                                                  printedPOS+=1;   

                                                 if (iterV < vertSF && iterH==horzSF) 
                                                  {
                                                    iterH=0; 
                                                    iterV+=1; 
                                                    printedPOS-=horzSF; 
                                                    printedPOS+=mcuPerW; 
                                                    nbrOflines+=mcuPerW;                                                     
                                                  }    

                                                 

                }

                                                  if (compID==lumID)
                                                   {
                                                      printedPOS=  vertSF>1? printedPOS-nbrOflines:printedPOS;                          // GO BACK TO THE LINE ABOVE
                                                      printedPOS=  printedPOS%mcuPerW==0? printedPOS+=nbrOflines:printedPOS;            // SKIP LINES PRINTED WITH VERTSF  
                                                      printedPosArr[lumID]=printedPOS; 
                                                   }

                                                  else 
                                                    { 
                                                      horzSF     = (compInfo[lumID][0]>>4);                                             // APPLY LUM SF TO CHR (TAKE ONE OUT OF 4 OR MORE CHR MCU)
                                                      vertSF     = (compInfo[compID][0]&0xf); 
                                                      printedPOS =  printedPOS+(horzSF-1); 
                                                      printedPOS =   printedPOS%mcuPerW==0 ? printedPOS+(mcuPerW*vertSF-1):printedPOS;  
                                                      printedPosArr[compID]=printedPOS; 
                                                    }
                                                  
                                         
        }
    }
 }


void jpeg::Encoder::encodeImage () 
  {  
     cout << endl; 
     cout << endl; 
     cout << "ENCODING " << imageName << "...."  << endl;
     cout << endl;   
     
    setUpfunc(0x22,1222,1630,1991525,decQT, "me.jpeg");                  //   setUpfunc (sf,encWidth,encHeight,myfilesize,QT,filename)  
   // readRgbContent ();
   // convToMCU (); 
    computeYcbCr (); 
    computeDCT ();
    divideByQT (); 
    orderZigzag (); 
    countZeros();   
    findDCdiff(); 
    revConvCoeff();
    hftSymb(); 
    hftValues(); 
    generateJPEG(); 

    for (int x=0; x<=10000; x++) 
       {
          for (int y=0; y<64; y++) 
            {
               int a = MCU[lumID][x][y]; 
               int b = MCU[lumID+1][x][y]; 
               int c = MCU[lumID+2][x][y]; 

               if (a!=0) 
                 {
                  //cout << "MCU: " << x << " bytePOS: "<< y << " val : "<< a << " , " << b << " , "<< c << endl; 
                 }

            }

     }
   

   for (int y=0; y<700; y++) 
     {              
     //  cout << "addr: "  << y  << "  val: " << rgbValues [lumID][y]+0 << endl; 
     }



  
   // string fileName = "ENC_"+imageName+".text"; 
   // printMcuContent (fileName);   
  }

