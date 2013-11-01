/**
 * resize.c
 * L.Rosselli
 * lisarosselli@g.harvard.edu
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Copies a BMP piece by piece, just because.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "cs50.h" //TODO fix to <cs50.h>
#include "bmp.h"


#define BASE 100


typedef struct
{
    BITMAPFILEHEADER bmpFileHeader;
    BITMAPINFOHEADER bmpInfoHeader;
    char* filename;
    double scaleBy;
    int scaleFactor;
    int padding;
} bmpObject;


bmpObject inObject;
bmpObject outObject;

bool inputIsAcceptible(double d);
int initOrigBmpObject(char* incomingFile);
int initDestBmpObject(char* filename, double userScaleValue);


int main(int argc, char* argv[])
{
    // do we have the correct number of arguments
    if (argc != 4)
    {
        printf("Usage: scale by a float, original BMP, destination BMP\n");
        return 1;
    }
    
    // is the float number entered acceptible
    double userFloat = atof(argv[1]);
    if (!inputIsAcceptible(userFloat))
    {
        printf("Usage: requires float ranging 1.0 to 100.0\n");
        return 2;
    }
    
    // init the originating bmp file object
    int readResult = initOrigBmpObject(argv[2]);
    if (readResult != 0)
    {
        return 3; // Maybe do something nicer here?
    }
    
    // init the destination bmp file
    int writeResult = initDestBmpObject(argv[3], userFloat);
    if (writeResult != 0)
    {
        return 4;
    }

    
    // Do EPIC SHit!
    FILE* inFile = fopen(inObject.filename, "r");
    FILE* outFile = fopen(outObject.filename, "a");
    
    if (inFile == NULL || outFile == NULL)
    {
        printf("Failed to open file.\n");
        return 9;
    }
    
    fseek(inFile, inObject.bmpFileHeader.bfOffBits, 0);
    fseek(outFile, outObject.bmpFileHeader.bfOffBits, 0);
    
    
    RGBTRIPLE tripleBuffer;
    int testSize = sizeof(RGBTRIPLE) * (outObject.bmpInfoHeader.biWidth + outObject.padding); // this is correct
    RGBTRIPLE* rowBuffer = (RGBTRIPLE*) malloc(testSize);
    // Something's hinky with my buffer! It's not holding things right!
    
    // setup a padding triple
    RGBTRIPLE paddingTriple;
    paddingTriple.rgbtBlue = '\0';
    paddingTriple.rgbtGreen = '\0';
    paddingTriple.rgbtRed = '\0';
    
    //RGBTRIPLE* rowBufferLocationPointer = rowBuffer;
    
    for (int row = 0; row < abs(inObject.bmpInfoHeader.biHeight); row++) {
        // for each row as a whole
        printf("row = %i\n", row);
        
        for (int origRowIndex = 0; origRowIndex < inObject.bmpInfoHeader.biWidth; origRowIndex++)
        {
            // for each pixel in a row
            printf("\torigRowIndex = %i\n", origRowIndex);
            fread(&tripleBuffer, 1, sizeof(RGBTRIPLE), inFile); // read triple

            
            for (int scaleFactorCount = 0; scaleFactorCount < outObject.scaleFactor; scaleFactorCount++) {
                // for what WILL BE each pixel in the new, larger row
                printf("\t\tscaleFactorCount = %i\n", scaleFactorCount);
                
                //write triple n times here, into rowBuffer
                //rowBuffer[scaleFactorCount] = tripleBuffer;
                
                // this is better
                int blah = origRowIndex * outObject.scaleFactor;
                RGBTRIPLE* rowBufferNextAddress = rowBuffer + blah; //rowBuffer + scaleFactorCount;
                *rowBufferNextAddress = tripleBuffer;
            }
        }
        
        // seek past any padding that may exist in the original file
        if (inObject.padding > 0)
        {
            long streamSpot = ftell(inFile);
            long paddingOffset = inObject.padding * sizeof(RGBTRIPLE);
            fseek(inFile, (paddingOffset + streamSpot), 0);
            streamSpot = ftell(inFile);
        }
        
        // add any padding to our rowBuffer, consistent with the new dimensions
        if (outObject.padding > 0)
        {
            for (int paddingCount = 0; paddingCount < outObject.padding; paddingCount++) {
                printf("\tpadding = %i\n", paddingCount);
                int b = outObject.bmpInfoHeader.biWidth + paddingCount;
                //rowBuffer[b] = paddingTriple;
                RGBTRIPLE* rowBufferNextAddress = rowBuffer + b;
                *rowBufferNextAddress = paddingTriple;
            }
        }
        
        for (int replicatedRow = 0; replicatedRow < outObject.scaleFactor; replicatedRow++) {
            printf("\treplicatedRow = %i\n", replicatedRow);
            
            // write rowBuffer (the entire row) n times into file
            // this is not right. but i do want to write the entire chunk in rowBuffer (rowBuffer+1, rowBuffer+2)
            int size = sizeof(RGBTRIPLE) * (outObject.bmpInfoHeader.biWidth + outObject.padding);
            fwrite(rowBuffer, 1, size, outFile);
        }
        
    }
    
    
    free(rowBuffer);
    fclose(inFile);
    fclose(outFile);
    
    return 0;
}

// ensures the input is within 1.0 to 100.0 range
bool inputIsAcceptible(double d)
{
    bool isAcceptible = false;
    int a = (int) (d * 100);
    
    if (a >= (BASE * 1) && a <= (BASE * BASE))
    {
        isAcceptible = true;
    }
    
    return isAcceptible;
}

// initialize the incoming file's bmpObject
int initOrigBmpObject(char* incomingFile)
{
    FILE* inFile = fopen(incomingFile, "r");
    if (inFile == NULL)
    {
        printf("Could not open initial bmp file.\n");
        return 3;
    } else
    {
        fseek(inFile, 0, 0);
    }
    
    BITMAPFILEHEADER inBFH;
    BITMAPINFOHEADER inBIH;
    
    fread(&inBFH, 1, sizeof(BITMAPFILEHEADER), inFile);
    fread(&inBIH, 1, sizeof(BITMAPINFOHEADER), inFile);
    
    // ensure the incoming file is a 24-bit BMP 4.0
    if (inBFH.bfType != 0x4d42 || inBFH.bfOffBits != 54 || inBIH.biSize != 40 || inBIH.biBitCount != 24 || inBIH.biCompression != 0)
    {
        printf("Unsupported file format.\n");
        return 4;
    }
    
    inObject.bmpFileHeader = inBFH;
    inObject.bmpInfoHeader = inBIH;
    
    int a = (inObject.bmpInfoHeader.biWidth % 4);
    int padding = (a == 0) ? a : (4 - a);
    
    inObject.filename = incomingFile;
    inObject.scaleBy = 0.0;
    inObject.scaleFactor = -1;
    inObject.padding = padding;
    
    fclose(inFile);
    
    return 0;
}

// initialize the destination bmp file and object
int initDestBmpObject(char* filename, double userScaleValue)
{
    outObject.filename = filename;
    
    FILE* writeFile = fopen(outObject.filename, "w");
    
    if (writeFile == NULL)
    {
        printf("Could not create/open file %s\n", outObject.filename);
        return 5;
    }
    
    // initially set the headers equal, will do math in a second
    outObject.bmpFileHeader = inObject.bmpFileHeader;
    outObject.bmpInfoHeader = inObject.bmpInfoHeader;
    
    // calculate new width/height
    long estimatedWidth = outObject.bmpInfoHeader.biWidth * userScaleValue;
    int a = (estimatedWidth % 4);
    int padding = (a == 0) ? 0 : (4 - a);
    long estimatedHeight = outObject.bmpInfoHeader.biHeight * userScaleValue;
    outObject.padding = padding;
    
    outObject.bmpInfoHeader.biWidth = (LONG) estimatedWidth;
    outObject.bmpInfoHeader.biHeight = (LONG) estimatedHeight;
    
    // determine file size
    int rawBmpSize = (outObject.bmpInfoHeader.biWidth + outObject.padding) * abs(outObject.bmpInfoHeader.biHeight) * sizeof(RGBTRIPLE);
    outObject.bmpInfoHeader.biSizeImage = rawBmpSize;
    outObject.bmpFileHeader.bfSize = rawBmpSize + outObject.bmpFileHeader.bfOffBits;
    
    // determine scale values
    outObject.scaleFactor = round(userScaleValue);
    outObject.scaleBy = userScaleValue;
    
    // write header and info to the new file
    BITMAPFILEHEADER bf;
    bf = outObject.bmpFileHeader;
    fwrite(&bf, 1, sizeof(BITMAPFILEHEADER), writeFile);
    
    BITMAPINFOHEADER bi;
    bi = outObject.bmpInfoHeader;
    fwrite(&bi, 1, sizeof(BITMAPINFOHEADER), writeFile);
    
    fclose(writeFile);
    
    return 0;
}
