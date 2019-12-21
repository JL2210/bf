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

void convert_brainfuck(FILE *out, const unsigned char *in, size_t insz, int opt);
unsigned char *read_file(const char *, size_t *);

int main(int argc, char **argv)
{
    unsigned argctr = 1,
             flag = 0;
    FILE *out = stdout;
    size_t file_size;
    unsigned char *file;

    if((unsigned)argc - 2 > 2)
    {
        fprintf(stderr, "Error: Incorrect number of arguments\n"
                        "  Usage: %s [-nze] bf-file [out-file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(argv[argctr][0] == '-' || argv[argctr][0] == '/')
    {
        switch(argv[argctr][1])
        {
            case 'n':
                flag = NO_CHANGE_ON_EOF;
                break;
            case 'z':
                flag = ZERO_ON_EOF;
                break;
            case 'e':
                /* default */
                break;
        }
        argctr++;
    }

    file = read_file(argv[argctr++], &file_size);
    if(file == NULL)
    {
        perror("read_file");
        return EXIT_FAILURE;
    }

    if(argc >= argctr)
    {
        out = fopen(argv[argctr], "w");
        if(out == NULL)
        {
            perror("fopen");
            return EXIT_FAILURE;
        }
    }

    fprintf(out, "#include <stdio.h>\n"
                 "unsigned char tape[%u];\n"
                 "int main(void)\n"
                 "{\n"
                 "unsigned char *ptr = tape;\n"
                 "int c;\n",
                 DEF_MEM_SIZE);

    convert_brainfuck(out, file, file_size, flag);

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
                if(opt & NO_CHANGE_ON_EOF)
                    fputs("if((c = getchar()) != EOF) *ptr = c;\n", out);
                else if(opt & ZERO_ON_EOF)
                    fputs("*ptr = (c = getchar()) == -1 ? 0 : c;\n", out);
                else /* if(opt & EOF_ON_EOF) */
                    fputs("*ptr = getchar();\n", out);
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
