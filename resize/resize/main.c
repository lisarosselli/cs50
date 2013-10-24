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
void readWriteHeaders(bmpObject readObject, bmpObject writeObject);


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
    int result = initOrigBmpObject(argv[2]);
    if (result != 0)
    {
        return 3; // Maybe do something nicer here?
    }
    
    printf("check inObject for values\n");

    
    
    
    
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

int initOrigBmpObject(char* incomingFile)
{
    printf("initOrigBmpObject\n");
    
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
    fread(&inBFH, 1, sizeof(BITMAPFILEHEADER), inFile);
    inObject.bmpFileHeader = inBFH;
    
    BITMAPINFOHEADER inBIH;
    fread(&inBIH, 1, sizeof(BITMAPINFOHEADER), inFile);
    inObject.bmpInfoHeader = inBIH;
    
    inObject.filename = incomingFile;
    inObject.scaleBy = 0.0;
    inObject.scaleFactor = -1;
    
    return 0;
}

void readWriteHeaders(bmpObject readObject, bmpObject writeObject)
{
    
}
