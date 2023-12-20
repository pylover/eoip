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
#ifndef OPTIONS_H_
#define OPTIONS_H_


enum command {
    CMD_UNKNOWN,
    CMD_LIST,
    CMD_START,
};


struct options {
    enum command command;
    unsigned char verbosity;
    const char *configpath;
    char *argv[1];
    int argc;
};


extern struct options options;


int
options_parse(int argc, char **argv);


#endif  // OPTIONS_H_
