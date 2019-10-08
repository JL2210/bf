/*
 *  Copyright © 2019 James Larrowe
 *
 *  This file is part of bf.
 *
 *  bf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with bf.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define DEF_MEM_SIZE 32768

#define ezs(x) (x)
#define exit_if_true(x, y) if(x) exit_perr(y)

void bf_interp(unsigned char *, const unsigned char *, size_t, size_t);
void exit_perr(const char *);
unsigned char *read_file(const char *, size_t *);

int main(int argc, char **argv)
{
    size_t file_size,
           mem_size = DEF_MEM_SIZE;
    unsigned char *file, *mem;

    if(argc != 2)
    {
        fprintf(stderr, "Error: Incorrect number of arguments\n"
                        "  Usage: %s [file] (args...)\n", argv[0]);
        return 1;
    }

    file = read_file(argv[1], &file_size);
    mem = calloc(sizeof(*mem), mem_size);
    exit_if_true(mem == NULL, "calloc()");

    bf_interp(mem, file, mem_size, file_size);

    free(mem);
    free(file);

    return 0;
}

void bf_interp(unsigned char *sp, const unsigned char *ip, size_t sp_size, size_t ip_size)
{
    long spc = 0, ipc = 0;

    while(ezs(size_t)ipc < ip_size)
    {
        /* I would put this inside the switch
         * but clang warns about "unreachable code".
         */
        int nest;
        switch(ip[ipc])
        {
            case '>':
            {
                spc++;
                if(ezs(size_t)spc >= sp_size)
                {
                    unsigned char *tmp;

                    tmp = realloc(sp, sp_size + DEF_MEM_SIZE);
                    exit_if_true(tmp == NULL, "realloc()");

                    memset(tmp+sp_size, 0, DEF_MEM_SIZE);
                    memcpy(tmp, sp, sp_size);

                    sp_size += DEF_MEM_SIZE;

                    free(sp);
                    sp = tmp;
                }
                break;
            }
            case '<':
            {
                spc--;
                if(spc < 0)
                {
                    fputs("Error: spc < 0\n", stderr);
                    exit(1);
                }
                break;
            }
            case '+':
            {
                sp[spc]++;
                break;
            }
            case '-':
            {
                sp[spc]--;
                break;
            }
            case '.':
            {
                putchar(sp[spc]);
                break;
            }
            case ',':
            {
                int c = getchar();
                if(c != EOF)
                    sp[spc] = ezs(unsigned char)c;
                break;
            }
            case '[':
            {
                if(sp[spc] == 0)
                {
                    for( nest = 1; nest; )
                    {
                        ipc++;
                        if(ezs(size_t)ipc >= ip_size)
                        {
                            fputs("Error: ipc >= ip_size\n", stderr);
                            exit(1);
                        }
                        nest += (ip[ipc] == '[');
                        nest -= (ip[ipc] == ']');
                    }
                }
                break;
            }
            case ']':
            {
                if(sp[spc])
                {
                    for( nest = 1; nest; )
                    {
                        ipc--;
                        if(ipc < 0)
                        {
                            fputs("Error: ipc < 0\n", stderr);
                            exit(1);
                        }
                        nest -= (ip[ipc] == '[');
                        nest += (ip[ipc] == ']');
                    }
                }
                break;
            }
            default:
                break;
        }
        ipc++;
    }
}

unsigned char *read_file(const char *argv1, size_t *size)
{
    unsigned char *file;
    size_t was_read, sz;
    FILE *fp;

    fp = fopen(argv1, "r");
    exit_if_true(fp == NULL, "fopen()");

    exit_if_true(fseek(fp, 0L, SEEK_END) == -1, "fseek()");

    sz = ezs(size_t)ftell(fp);
    exit_if_true(sz == ezs(size_t)-1, "ftell()");

    *size = sz;

    file = malloc(sz);
    exit_if_true(file == NULL, "malloc()");

    exit_if_true(fseek(fp, 0L, SEEK_SET) == -1, "fseek()");

    was_read = fread(file, 1, sz, fp);
    if(was_read != sz)
    {
        fprintf(stderr, "Unable to read file.\n"
                        "Read %zu bytes when %zu bytes were expected\n",
                        was_read, sz);
        exit(1);
    }

    exit_if_true(fclose(fp) == EOF, "fclose()");

    return file;
}

void exit_perr(const char *str)
{
    perror(str);
    exit(1);
}
