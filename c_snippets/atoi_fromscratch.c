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

// make atoi

int ato_i(char* s);

int main(int argc, const char * argv[])
{
    char *strNum = GetString();
    
    if (!isdigit(*strNum)) {
        return 1;
    }
    
    int n = ato_i(strNum);
    printf("n=%i\n", n);
    
    return 0;
}

int ato_i(char* s)
{
    if (s == NULL)
    {
        return 0;
    }
    
    int sum = 0;
    int len = strlen(s);
    
    for (int i = 0; i < len; i++)
    {
        char thisChar = s[i];
        if (!isdigit(thisChar))
        {
            return 0;
        }
        
        sum = 10 * sum + s[i] - '0';
    }
    
    return sum;
}