//
//  main.c
//  quiz0test
//
//  Created by LISA ROSSELLI on 10/14/13.
//  Copyright (c) 2013 LISA ROSSELLI. All rights reserved.
//

#include <stdio.h>
#include <ctype.h>
#include "cs50.h"

// Counting the length of an input string via pointer arithmetic

int str_len(char* s);

int main(int argc, const char * argv[])
{
    char *mystring = GetString();
    int length = str_len(mystring);
    printf("length=%i\n", length);
    
    return 0;
}

int str_len(char* s)
{
    // they wanted us to use pointer arithmetic here, specifically
    if (s == NULL)
    {
        return 0;
    }
    
    int count = 0;
    
    do {
        count++;
        s++; // moving the pointer forward a memory address at a time
        // *s is the ascii value of what is held in memory, ie, the character
        // s is the memory address
    } while (*s != '\0');
    
    return count;
}