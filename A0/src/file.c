#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

enum file_type {
    UNDEFINED,
    EMPTY,
    DATA,
    ASCII,
    ISO_8859_1,
    UTF8
};

const char * const FILE_TYPE_STRINGS[] = {
    "undefined",
    "empty",
    "data",
    "ASCII text",
    "ISO-8859 text",
    "UTF-8 Unicode text"
};

enum file_type fileType(FILE *f) {
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
        return EMPTY;
    fseek(f,0, SEEK_SET);
    enum file_type first_char = UNDEFINED;
    unsigned char c;
    while(fread(&c, sizeof(char),1,f) == 1) {   
        if ((c >= 7 && c <= 13) || c == 27 || (c >= 32 && c <= 126)) {
            if (first_char == ASCII || first_char == UNDEFINED) {
                first_char = ASCII;
                continue;
            }
            else if(first_char == ISO_8859_1) {
                continue;
            }
            else if(first_char == UTF8) {
                continue;
            }
        }
        else if(c >= 160){
            if((c & 0xe0) == 0xc0) {
                char byte;
                fread(&byte, sizeof(char), 1, f);
                if((byte & 0xc0) == 0x80) {
                    first_char = UTF8;
                    continue;
                }
                else {
                    fseek(f,-1,SEEK_CUR);
                }
            }
            else if((c & 0xf0) == 0xe0) {
                char bytearr[2];
                fread(&bytearr[0],sizeof(char),1,f);
                fread(&bytearr[1],sizeof(char),1,f);
                if((bytearr[0] & 0xc0) == 0x80 && (bytearr[1] & 0xc0) == 0x80) {
                    if (first_char == UTF8 || first_char == UNDEFINED || first_char == ASCII) {
                        first_char = UTF8;
                        continue;
                    }
                }
                else {
                    fseek(f,-2,SEEK_CUR);
                }
            }
            else if ((c & 0xf8) == 0xf0) {
                char bytearr[3];
                fread(&bytearr[0],sizeof(char),1,f);
                fread(&bytearr[1],sizeof(char),1,f);
                fread(&bytearr[2],sizeof(char),1,f);
                if((bytearr[0] & 0xc0) == 0x80 && (bytearr[1] & 0xc0) == 0x80 && (bytearr[2] & 0xc0) == 0x80) {
                    if (first_char == UTF8 || first_char == UNDEFINED || first_char == ASCII) {
                        first_char = UTF8;
                        continue;
                    }
                }
                else {
                    fseek(f, -3, SEEK_CUR);
                }
            }
            if (first_char == ISO_8859_1 || first_char == UNDEFINED || first_char == ASCII) {
                first_char = ISO_8859_1;
                continue;
            }
            else {
                return DATA;
            }
        }
        else if((c & 0x80) == 0x0) {
            if (first_char == UTF8 || first_char == UNDEFINED || first_char == ASCII) {
                 first_char = UTF8;
                 continue;
            }
            return DATA;
        }
        return DATA;
    }
    return first_char;
}

// Assumes: errnum is a valid error number
int print_error(char *path, int errnum) {
    return fprintf(stdout, "%s: cannot determine (%s)\n",
    path, strerror(errnum));
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
    
        FILE *f = fopen(argv[1], "r");

        if (f !=  NULL) {
            enum file_type type = fileType(f);
            printf("%s: %s\n", argv[1], FILE_TYPE_STRINGS[type]);
            fclose(f);
            return EXIT_SUCCESS;
        }
        else {
            fclose(f);
            return print_error(argv[1], errno);
        }
    }
    else if (argc == 1) {
        printf("Usage: file path\n");
        return EXIT_FAILURE;
    }

}