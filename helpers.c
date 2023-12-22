#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "helpers.h"


void
joinpath(char *destination, const char *path1, const char *path2) {
    if (path1 && *path1) {
        int len = strlen(path1);
        strcpy(destination, path1);

        if (destination[len - 1] == DIR_SEPARATOR) {
            if (path2 && *path2) {
                strcpy(destination + len, (*path2 == DIR_SEPARATOR) ? (path2 + 1) : path2);
            }
        }
        else {
            if (path2 && *path2) {
                if (*path2 == DIR_SEPARATOR) {
                    strcpy(destination + len, path2);
                }
                else {
                    destination[len] = DIR_SEPARATOR;
                    strcpy(destination + len + 1, path2);
                }
            }
        }
    }
    else if (path2 && *path2) {
      strcpy(destination, path2);
    }
    else {
      destination[0] = '\0';
    }
}


int
isfile(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
