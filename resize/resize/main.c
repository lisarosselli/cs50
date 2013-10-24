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
    
    int writeResult = initDestBmpObject(argv[3], userFloat);
    
    printf("check objects for values\n");

    
    
    
    
    printf("hello world!\n");
    
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
    
    inObject.filename = incomingFile;
    inObject.scaleBy = 0.0;
    inObject.scaleFactor = -1;
    
    fclose(inFile);
    
    return 0;
}

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
    long estimatedWidth = outObject.bmpInfoHeader.biWidth * round(userScaleValue);
    int a = (estimatedWidth % 4);
    long paddedWidth = (a == 0) ? estimatedWidth : (estimatedWidth + (4 - a));
    long estimatedHeight = outObject.bmpInfoHeader.biHeight * round(userScaleValue);
    
    outObject.bmpInfoHeader.biWidth = (LONG) paddedWidth;
    outObject.bmpInfoHeader.biHeight = (LONG) estimatedHeight;
    
    // determine file size
    int rawBmpSize = outObject.bmpInfoHeader.biWidth * abs(outObject.bmpInfoHeader.biHeight) * sizeof(RGBTRIPLE);
    outObject.bmpInfoHeader.biSizeImage = rawBmpSize;
    outObject.bmpFileHeader.bfSize = rawBmpSize + outObject.bmpFileHeader.bfOffBits;
    
    // determine scale values
    int proposedScaleFactor = outObject.bmpInfoHeader.biWidth % inObject.bmpInfoHeader.biWidth;
    outObject.scaleFactor = (proposedScaleFactor == 0) ? round(userScaleValue) : proposedScaleFactor;
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
