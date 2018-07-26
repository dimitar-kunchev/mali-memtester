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

#include "textured-cube.h"

int main(int argc, char *argv[])
{
	int stop_flag = 0;
	return textured_cube_main(&stop_flag);
}
