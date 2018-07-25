# mali-memtester
Memory testing tool, based on the idea of https://github.com/ssvb/lima-memtester but using 
MALI instead of LIMA, combining it with http://pyropus.ca/software/memtester/

Licensed under the terms of the GNU General Public License version 2 (only)
because the license of memtester applies to this work as a whole.

### Background
The original lima-memtester has been a great tool for me to test RAM for ARM-based boards (in particular
 Allwinner-based), but with MALI support being constantly improved I needed something newer. 
In fact with latest kernels I have been unable to get the original lima-memtester working at all.

For that purpose I decided to build a new version that uses the MALI drivers.

### Building
Building is simple - just call make and two executables will be produced - textured-cube-demo and 
mali-memtester. There is no install (at the moment).
To do that you need libEGL and libGLESv2 (or actually libMali). On Sunxi I have used the following
guide: https://bootlin.com/blog/mali-opengl-support-on-allwinner-platforms-with-mainline-linux/. Apart 
from that you need sane build system (gcc, headers, etc) and working MALI with a framebuffer.

### Use
Same as the original memtester:
	mali-memtester <memsize> [runs]

For example:
	mali-memtester 12M 10

My goal has been to use this as replacement for DRAM training procedure as described at 
http://linux-sunxi.org/A10_DRAM_Controller_Calibration. 

The `textured-cube-demo executable will just display a spinning cube - you can use that to verify your 
driver and display are working properly.
