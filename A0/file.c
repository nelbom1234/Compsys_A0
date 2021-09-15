#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

enum file_type {
    DATA,
    EMPTY,
    ASCII,
    ISO_8859_1,
    UTF8
};

const char * const FILE_TYPE_STRINGS[] = {
    "data",
    "empty",
    "ascii",
    "ise-8859-1",
    "utf8"
};

enum file_type fileType(FILE *f) {
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0)
        return EMPTY;
    
    else
        for(;;){
            enum file_type first_char;
            
        }
        
        return DATA;
}

int main(int argc, char* argv[]) {
    assert(argc == 2);
    
    FILE *f = fopen(argv[1], "r");

    if (f !=  NULL) {
        enum file_type type = fileType(f);
        printf("%s\n", FILE_TYPE_STRINGS[type]);
        fclose(f);
        return EXIT_SUCCESS;
    }

}