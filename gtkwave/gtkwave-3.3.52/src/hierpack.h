/*
 * Copyright (c) Tony Bybell 2008-2011.     
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef WAVE_HIERPACK_H
#define WAVE_HIERPACK_H

#include "globals.h"

#define HIER_DEPACK_ALLOC (0)
#define HIER_DEPACK_STATIC (1)

void init_facility_pack(void);
char *compress_facility(unsigned char *key, int len);
void freeze_facility_pack(void);

char *hier_decompress_flagged(char *n, int *was_packed);

#endif

