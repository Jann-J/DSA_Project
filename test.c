#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t djb2_hash(char *);

int main()
{
    char *id = "public-id-0034";
    printf("%d\n", djb2_hash(id));
    return 0;
}

uint32_t sbj4_hash(char *key)
{
    int len = strlen(key);
    char value[3];
    value[0] = key[len - 2], value[1] = key[len - 1], value[2] = '\0';
    int combined = atoi(value);
    return combined;
}