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


FourByteBlock formatFourByteBlock(BYTE* buffer);
void extractJpgFile(char* jpgFilename, long streamIndex, FILE* readFile);
char* getJpgFilename(int jpgNum);
bool jpgSignatureMatch(FourByteBlock block);
void printByteBlock(FourByteBlock block);


FILE* file;
long lSize;


int main(void)
{
    //size_t readResult;
    //size_t writeResult;
    
    // attempt to open the file for reading
    file = fopen("card.raw", "r");
    
    if (file == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    }
    
    // obtain file size:
    fseek (file , 0 , SEEK_END);
    lSize = ftell(file);
    rewind(file);
    
    printf("file size is %ld\n", lSize);
    
    // create a way to store where in the file we are
    long streamMarker;
    
    // create a way to hold 4 bytes at a time to make
    // jpg signature matching easier
    BYTE* buffer;
    buffer = (BYTE*) malloc(sizeof(BYTE) * 4);
    
    // let's count the jpgs we're exporting
    int jpgFileCount = 0;
    
    // DO EPIC STUFF HERE.
    while (!feof(file)) {
        streamMarker = ftell(file);
        
        fread(buffer, 1, (sizeof(BYTE) * 4), file);
        
        FourByteBlock byteBlock = formatFourByteBlock(buffer);
        //printByteBlock(byteBlock);
        
        if (jpgSignatureMatch(byteBlock))
        {
            char* newJpgFilename = getJpgFilename(jpgFileCount);
            extractJpgFile(newJpgFilename, streamMarker, file);
            jpgFileCount++;

            // rewind the stream by 4 bytes to loop back and capture
            // the jpg signature
            long streamSpot = ftell(file);
            fseek(file, (streamSpot - 4), 0);
            streamSpot = ftell(file);
        }
        
    }
    
    free(buffer);
    fclose(file);
    
    return 0;
}

FourByteBlock formatFourByteBlock(BYTE* buffer)
{
    FourByteBlock b;
    
    b.byte1 = buffer[0];
    b.byte2 = buffer[1];
    b.byte3 = buffer[2];
    b.byte4 = buffer[3];
    
    return b;
}

void extractJpgFile(char* jpgFilename, long streamIndex, FILE* readFile)
{
    printf("extractJpgFile!\n");
    
    FILE* jpgFile = fopen(jpgFilename, "w");
    
    if (jpgFile == NULL)
    {
        printf("Could not create file %s\n", jpgFilename);
        return;
    }
    
    BYTE* localBuffer;
    localBuffer = (BYTE*) malloc(sizeof(BYTE) * 4);
    
    fseek(readFile, streamIndex, 0);
    
    // write the first jpg signature block
    fread(localBuffer, 1, (sizeof(BYTE) * 4), readFile);
    fwrite(localBuffer, 1, (sizeof(BYTE) * 4), jpgFile);
    
    FourByteBlock b;
    long startIndex = -1;
    
    // continue reading/writing the rest of the jpg bytes
    // constraint being the end of the file
    while (startIndex < lSize) {
        startIndex = ftell(file);
        fread(localBuffer, 1, (sizeof(BYTE) * 4), file);
        b = formatFourByteBlock(localBuffer);
        
        if (!jpgSignatureMatch(b))
        {
            fwrite(localBuffer, 1, (sizeof(BYTE) * 4), jpgFile);
        } else
        {
            // Uh oh, found a new jpg signature match
            // break outta this!
            break;
        }
    }
    
    fclose(jpgFile);
    free(localBuffer);
}

// manufacture a jpg filename
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

// optional printing of the bytes in a FourByteBlock
void printByteBlock(FourByteBlock block)
{
    printf("%x%x %x%x\n", block.byte1, block.byte2, block.byte3, block.byte4);
}
