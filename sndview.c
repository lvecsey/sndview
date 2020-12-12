/*
  Sound View
  Copyright (C) 2020  Lester Vecsey

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "mini_gxkit.h"

#include <sys/mman.h>

int main(int argc, char *argv[]) {

  image_t img;

  long int num_pixels;

  size_t img_sz;

  int retval;

  pixel_t fill_color;

  long int xpos, ypos;

  float *samples;

  long int num_samples;

  long int samplerate;

  long int sampleno;
  
  int fd;

  struct stat buf;
  
  void *m;

  char *inputraw_fn;

  point_t pt;
  
  img.xres = 1920;
  img.yres = 1080;

  num_pixels = img.xres * img.yres;
  img_sz = num_pixels * sizeof(pixel_t);

  img.rgb = malloc(img_sz);
  if (img.rgb == NULL) {
    perror("malloc");
    return -1;
  }
  
  inputraw_fn = argc>2 ? argv[2] : NULL;

  fd = open(inputraw_fn, O_RDWR);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  retval = fstat(fd, &buf);
  if (retval == -1) {
    perror("fstat");
    return -1;
  }
  
  m = mmap(NULL, buf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (m == MAP_FAILED) {
    perror("mmap");
    return -1;
  }

  samples = (float*) m;
  
  num_samples = buf.st_size / sizeof(float);

  fill_color = (pixel_t) { .r = 65535, .g = 65535, .b = 65535 };

  samplerate = 48000;

  memset(img.rgb, 0, img_sz);
  
  for (sampleno = 0; sampleno < num_samples; sampleno++) {

    xpos = sampleno * img.xres / num_samples;
    
    pt.y = samples[sampleno];

    ypos = pt.y * (img.yres >> 1); ypos += img.yres >> 1;

    img.rgb[ypos * img.xres + xpos] = fill_color;
	
  }

  retval = munmap(m, buf.st_size);
  retval = close(fd);

  {

    int out_fd;

    ssize_t bytes_written;
    
    out_fd = 1;
    
    bytes_written = write(out_fd, img.rgb, img_sz);
    if (bytes_written != img_sz) {
      perror("write");
      return -1;
    }

  }

  free(img.rgb);
	 
  return 0;

}
