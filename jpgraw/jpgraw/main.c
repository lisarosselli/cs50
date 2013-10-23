/**
  * recover.c
  *
  * Computer Science 50
  * Problem Set 5
  *
  * Recovers JPEGs from a forensic image.
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "cs50.h" // TODO <cs50.h>



// define what a 'BYTE' is
typedef uint8_t  BYTE;

// define a block of 4 bytes
typedef struct
{
    BYTE  byte1;
    BYTE  byte2;
    BYTE  byte3;
    BYTE  byte4;
} __attribute__((__packed__))
FourByteBlock;


char* getJpgFilename(int jpgNum);
bool jpgSignatureMatch(FourByteBlock block);


int main(void)
{
    FILE *file;
    long lSize;
    BYTE* buffer;
    size_t readResult;
    size_t writeResult;
    
    // attempt to open the file for reading
    file = fopen("card.raw", "r");
    
    // setting up some bools for first 4 bytes comparison
    bool firstByte = false;
    bool secondByte = false;
    bool thirdByte = false;
    bool fourhByte = false;
    
    if (file == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }
    
    // obtain file size:
    fseek (file , 0 , SEEK_END);
    lSize = ftell (file);
    rewind(file);
    
    printf("file size is %ld\n", lSize);
    
    // defining one of the four-byte signatures i'm looking for
    FourByteBlock sig1;
    sig1.byte1 = 0xff;
    sig1.byte2 = 0xd8;
    sig1.byte3 = 0xff;
    sig1.byte4 = 0xe0;
    
    // defining the other four-byte signature i'm looking for
    FourByteBlock sig2;
    sig2.byte1 = 0xff;
    sig2.byte2 = 0xd8;
    sig2.byte3 = 0xff;
    sig2.byte4 = 0xe1;
    
    
    // allocate memory to contain 512 Bytes (a block)
    buffer = (BYTE*) malloc (sizeof(BYTE) * 512);
    if (buffer == NULL)
    {
        printf("Could not allocate a buffer.\n");
        return 2;
    }
    
    readResult = fread (buffer, 1, 512, file);
    if (readResult != 512) {
        printf("Error reading file.\n");
        return 3;
    }
    
    int jpgSequenceNum = 0;
    
    // setup a pointer that will point to the jpg file
    FILE* jpgFilePtr;
    
    while (!feof(file)) {
        FourByteBlock b;
        fread(&b, 1, sizeof(FourByteBlock), file);
        
        
        // loop through the bytes and search by 4-byte blocks
        // that might match the jpg signature(s) prev. defined
        if (b.byte1 == sig1.byte1 &&
            b.byte2 == sig1.byte2 &&
            b.byte3 == sig1.byte3 &&
            (b.byte4 == sig1.byte4 || b.byte4 == sig2.byte4)) {
            printf("\t\t-> Found beginning of a jpg!\n");
            
            jpgSequenceNum++;
            
            char* newFilename = getJpgFilename(jpgSequenceNum);
            printf("\t\t-> New filename = %s\n", newFilename);
            
            jpgFilePtr = fopen(newFilename, "w");
            
            if (jpgFilePtr == NULL)
            {
                printf("Could not create %s for writing.\n", newFilename);
                return 4;
            }
            
            // write the initial jpg signature
            fwrite(&b, 1, sizeof(FourByteBlock), jpgFilePtr);
            
            bool isMatch;
            
            do {
                readResult = fread(buffer, 1, 512, file);
                
                // pull the first 4 bytes of the 512 block
                b.byte1 = buffer[0];
                b.byte2 = buffer[1];
                b.byte3 = buffer[2];
                b.byte4 = buffer[3];
                
                // test if they are the jpg signature
                isMatch = jpgSignatureMatch(b);
                
                if (!isMatch)
                {
                    // if no jpg sig, continue to write the 512 bytes to file
                    writeResult = fwrite(buffer, 1, 512, jpgFilePtr);
                    printf("writing 512 bytes...\n");
                }

            } while (b.byte1 != sig1.byte1 && b.byte2 != sig1.byte2 && b.byte3 != sig1.byte3 && (b.byte4 != sig1.byte4 || b.byte4 != sig2.byte4));
            
            printf("\t\t-> Closed %s file.\n", newFilename);
            fclose(jpgFilePtr);
        }
    }
    
    free(buffer);
    
    return 0;
}

char* getJpgFilename(int jpgNum)
{
    string blah = malloc(sizeof(char) * 7);
    
    //TODO better format the name
    
    
    sprintf(blah, "%i.jpg", jpgNum);
    
    
    return blah;
}

// compare incoming block against first 4 jpg hex values
bool jpgSignatureMatch(FourByteBlock block)
{
    bool isMatch1 = false;
    bool isMatch2 = false;
    bool isMatch3 = false;
    bool isMatch4 = false;
    bool fullMatch = false;
    
    isMatch1 = (block.byte1 == 0xff) ? true : false;
    isMatch2 = (block.byte2 == 0xd8) ? true : false;
    isMatch3 = (block.byte3 == 0xff) ? true : false;
    isMatch4 = (block.byte4 == 0xe0) ? true : false;
    
    if (!isMatch4)
    {
        isMatch4 = (block.byte4 == 0xe1) ? true : false;
    }
    
    fullMatch = (isMatch1 && isMatch2 && isMatch3 && isMatch4);
    
    return fullMatch;
}
