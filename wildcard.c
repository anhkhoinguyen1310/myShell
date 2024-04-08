#include "wildcard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Implement the expand_wildcards function
void expand_wildcards(char *wildcard, char ***expanded_args, int *arg_count)
{
    glob_t results;
    int ret;

    ret = glob(wildcard, GLOB_NOCHECK, NULL, &results);
    if (ret == 0)
    {
        *expanded_args = malloc(results.gl_pathc * sizeof(char *));
        *arg_count = results.gl_pathc;
        for (size_t i = 0; i < results.gl_pathc; i++)
        {
            (*expanded_args)[i] = strdup(results.gl_pathv[i]);
        }
    }
    else
    {
        perror("glob error");
    }

    globfree(&results);
}
