#include <stdio.h>
#include <stdlib.h>

#define cast(x) (x)

unsigned char *read_file(const char *argv1, size_t *size)
{
    unsigned long bytes_read, sz;
    unsigned char *file;
    FILE *fp;

    fp = fopen(argv1, "r");

    if(fp == NULL)
        return NULL;

    if(fseek(fp, 0L, SEEK_END) == -1)
        return NULL;

    sz = cast(unsigned long)ftell(fp);
    if(sz == -1UL)
         return NULL;

    *size = sz;

    if(fseek(fp, 0L, SEEK_SET) == -1)
        return NULL;

    file = cast(unsigned char *)malloc(sz);
    if(file == NULL)
        return NULL;

    bytes_read = fread(file, 1, sz, fp);
    if(bytes_read != sz)
    {
        fprintf(stderr, "Unable to read file.\n"
                        "Read %lu bytes when %lu bytes were expected\n",
                        bytes_read, sz);
        goto cleanup;
    }

    if(fclose(fp) == EOF)
        goto cleanup_no_fclose;

    return file;

cleanup:
    fclose(fp);
cleanup_no_fclose:
    free(file);
    return NULL;
}
