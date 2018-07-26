/*
 * textured-cube version 0.1
 * 
 * Part of the MALI Memory Testing tool (https://github.com/dimitar-kunchev/mali-memtester)
 * 
 * Copyright (C) 2018 Dimitar Kunchev <d.kunchev@gmail.com>
 * Licensed under the terms of the GNU General Public License version 2 (only).
 * See the file COPYING for details.
 *
 */

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES/gl.h>

#ifndef __TEXTURED_CUBE_H__
#define __TEXTURED_CUBE_H__

int textured_cube_main(int * stop_flag);

#endif
