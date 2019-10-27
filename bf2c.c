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

#define NO_CHANGE_ON_EOF 0
#define ZERO_ON_EOF 1
#define EOF_ON_EOF 2

#define STR(x) STR_(x)
#define STR_(x) #x

#define cast(x) (x)
#define exit_if_true(x, y)	\
    do {			\
        if(x)			\
        {			\
            perror_and_exit(y);	\
        }			\
    } while(0)

void convert_brainfuck(FILE *out, const unsigned char *in, size_t insz, int opt);
void perror_and_exit(const char *);
unsigned char *read_file(const char *, size_t *);

int main(int argc, char **argv)
{
    FILE *out = stdout;
    size_t file_size;
    unsigned char *file;

    if((unsigned)argc - 2 > 1)
    {
        fprintf(stderr, "Error: Incorrect number of arguments\n"
                        "  Usage: %s bf-file [out-file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argc == 3)
    {
        out = fopen(argv[2], "w");
    }

    file = read_file(argv[1], &file_size);

    fprintf(out, "#include <stdio.h>\n"
                 "unsigned char tape[%u];\n"
                 "int main(void)\n"
                 "{\n"
                 "unsigned char *ptr = tape;\n"
                 "int c;\n",
                 DEF_MEM_SIZE);

    /* TODO: Add an argument system that changes the compilation mode */
    convert_brainfuck(out, file, file_size, EOF_ON_EOF);

    fputs("return 0;\n"
          "}\n",
          out);

    free(file);

    return 0;
}

void convert_brainfuck(FILE *out, const unsigned char *ip, size_t ip_size, int opt)
{
    long ipc = 0;

    while(cast(size_t)ipc < ip_size)
    {
        switch(ip[ipc])
        {
            case '>':
            {
                fputs("ptr++;\n", out);
                break;
            }
            case '<':
            {
                fputs("ptr--;\n", out);
                break;
            }
            case '+':
            {
                fputs("(*ptr)++;\n", out);
                break;
            }
            case '-':
            {
                fputs("(*ptr)--;\n", out);
                break;
            }
            case '.':
            {
                fputs("putchar(*ptr);\n", out);
                break;
            }
            case ',':
            {
                if(opt & ZERO_ON_EOF)
                    fputs("*ptr = (c = getchar()) == -1 ? 0 : c;\n", out);
                else if(opt & EOF_ON_EOF)
                    fputs("*ptr = getchar();\n", out);
                else
                    fputs("if((c = getchar()) != EOF) *ptr = c;\n", out);
                break;
            }
            case '[':
            {
                fputs("while(*ptr) {\n", out);
                break;
            }
            case ']':
            {
                fputs("}\n", out);
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
    unsigned long bytes_read, sz;
    unsigned char *file;
    FILE *fp;

    fp = fopen(argv1, "r");
    exit_if_true(fp == NULL, "fopen()");

    exit_if_true(fseek(fp, 0L, SEEK_END) == -1, "fseek()");

    sz = cast(unsigned long)ftell(fp);
    exit_if_true(sz == -1UL, "ftell()");

    *size = sz;

    file = cast(unsigned char *)malloc(sz);
    exit_if_true(file == NULL, "malloc()");

    exit_if_true(fseek(fp, 0L, SEEK_SET) == -1, "fseek()");

    bytes_read = fread(file, 1, sz, fp);
    if(bytes_read != sz)
    {
        fprintf(stderr, "Unable to read file.\n"
                        "Read %lu bytes when %lu bytes were expected\n",
                        bytes_read, sz);
        exit(1);
    }

    exit_if_true(fclose(fp) == EOF, "fclose()");

    return file;
}

void perror_and_exit(const char *str)
{
    perror(str);
    exit(1);
}
