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
    BYTE* smallBuffer;
    size_t readResult;
    size_t writeResult;
    
    // attempt to open the file for reading
    file = fopen("card.raw", "r");
    
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
        
        // loop through the bytes and search initial 4-byte blocks
        // that might match the jpg signature(s)
        if (jpgSignatureMatch(b))
        {
            printf("\t\t-> Found beginning of a jpg!\n");
            
            jpgSequenceNum++;
            
            char* newFilename = getJpgFilename(jpgSequenceNum);
            printf("\t\t-> New filename = %s\n", newFilename);
            
            jpgFilePtr = fopen(newFilename, "w");
            
            // free up what was essentially malloc'd
            
            
            if (jpgFilePtr == NULL)
            {
                printf("Could not create %s for writing.\n", newFilename);
                return 4;
            }
            
            // write the initial jpg signature
            fwrite(&b, 1, sizeof(FourByteBlock), jpgFilePtr);
            
            bool isMatch;
            
            smallBuffer = (BYTE*) malloc(sizeof(BYTE) * 4);
            
            
            do {
                //readResult = fread(buffer, 1, 512, file);
                readResult = fread(smallBuffer, 1, 4, file);
                
                // pull the first 4 bytes of the 512 block
                b.byte1 = smallBuffer[0];//buffer[0];
                b.byte2 = smallBuffer[1];//buffer[1];
                b.byte3 = smallBuffer[2];//buffer[2];
                b.byte4 = smallBuffer[3];//buffer[3];
                
                //printf("four bytes %x %x %x %x\n", b.byte1, b.byte2, b.byte3, b.byte4);
                
                // test if they are the jpg signature
                //isMatch = jpgSignatureMatch(b);
                
                isMatch = jpgSignatureMatch(b);
                
                if (!isMatch)
                {
                    // if no jpg sig, continue to write the 512 bytes to file
                    //writeResult = fwrite(buffer, 1, 512, jpgFilePtr);
                    //printf("writing 512 bytes...\n");
                    writeResult = fwrite(smallBuffer, 1, 4, jpgFilePtr);
                    
                }

            } while (!jpgSignatureMatch(b));
            
            printf("\t\t-> Closed %s file.\n", newFilename);
            fclose(jpgFilePtr);
            free(newFilename);
        }
    }
    
    fclose(file);
    free(buffer);
    free(smallBuffer);
    
    return 0;
}

char* getJpgFilename(int jpgNum)
{
    string jpgFilename = malloc(sizeof(char) * 7);
    
    string zeros;
    
    // add preceding zeros as string, if needed
    if (jpgNum <= 9)
    {
        zeros = "00";
    } else if (jpgNum >= 10 && jpgNum <= 99)
    {
        zeros = "0";
    } else
    {
        zeros = "";
    }
    
    sprintf(jpgFilename, "%s%i.jpg", zeros, jpgNum);
    
    return jpgFilename;
}

// compare incoming block against first 4 jpg hex values
bool jpgSignatureMatch(FourByteBlock block)
{
    bool match = false;
    
    FourByteBlock sig;
    sig.byte1 = 0xff;
    sig.byte2 = 0xd8;
    sig.byte3 = 0xff;
    sig.byte4 = 0xe0;
    
    if (block.byte1 == sig.byte1 && block.byte2 == sig.byte2 && block.byte3 == sig.byte3)
    {
        if (block.byte4 == sig.byte4 || block.byte4 == 0xe1)
        {
            match = true;
        }
    }
    
    return match;
}
