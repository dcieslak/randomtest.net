#include <stdlib.h>
#include <stdio.h>

#include "randomtest-common.h"

int main(int, char**) {

    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char rawStacktrace[BUFSIZE];
    FILE* memoryFile = fmemopen(rawStacktrace, BUFSIZE, "wt");

    setvbuf(stdin, (char*) NULL, _IONBF, 0);

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read == 0 || line[0] == '\n') {
            /* End of stacktrace */
            fclose(memoryFile);
            send_stacktrace_by_curl(rawStacktrace);
            memoryFile = fmemopen(rawStacktrace, BUFSIZE, "wt");
        }
        else {
            /* Next stracktrace line */
            fprintf(memoryFile, "%s", line);
        }
    }

    fclose(memoryFile);

    return 0;
}


