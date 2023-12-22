// Copyright 2023 vahid mardani
/*
 * This file is part of eoip.
 *  eoip is free software: you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation, either version 3 of the License, or (at your option)
 *  any later version.
 *
 *  eoip is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with eoip. If not, see <https://www.gnu.org/licenses/>.
 *
 *  Author: Vahid Mardani <vahid.mardani@gmail.com>
 */
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
                strcpy(destination + len,
                        (*path2 == DIR_SEPARATOR) ? (path2 + 1) : path2);
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


bool
endswith(const char *str, const char *suffix) {
    if (!str || !suffix) {
        return 0;
    }

    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr) {
        return 0;
    }

    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
