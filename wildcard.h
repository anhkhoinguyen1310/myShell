// wildcard.h
#ifndef WILDCARD_H
#define WILDCARD_H

#include <glob.h> // For glob_t

// Function prototype(s)
void expand_wildcards(char *wildcard, char ***expanded_args, int *arg_count);

#endif // WILDCARD_H
