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
#ifndef TUNNELSET_H_
#define TUNNELSET_H_


struct tunnelset {
    struct tunnel *first;
    unsigned char count;
};


int
tunnelset_load(struct tunnelset *tunnels);


void
tunnelset_closeall(struct tunnelset *tunnels);


int
tunnelset_print(struct tunnelset *tunnels);


void
tunnelset_dispose(struct tunnelset *tunnels);


#endif  // TUNNELSET_H_
