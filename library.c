/*
Brent Brewster Project 1 library.c
*/
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include "iso_font.h"
#include <sys/select.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
//#define NULL 0
#define set_rgb(r, g, b)(((r&0b0000000000011111)<<12)|((b&0b0000000000111111)<<5)|(b&0b0000000000011111)))

static void *mapped_space;
static int fd_fb;
static int fb_size;
static int linlength;
void exit_graphics();

typedef struct color_t
{
	unsigned short rgb;
}color_t;

/*
USED SYSCALLS: open, ioctl, mmap
init_graphics opens the /dev/fb0 zeroth frame buffer and stores it's
file descriptor in var fd. Then using fd maps the file to an adress
using mmap and stores a ptr to the adress in mapped_space.
*/
void init_graphics()
{
//int *mapped_space;
int yres_virtual;
/*
Structs used in IOCTL
*/
struct fb_var_screeninfo fb_vscreeninfo;
struct fb_fix_screeninfo fb_fscreeninfo;
struct termios terminal_settings;

//open the fb for read and write
fd_fb = open("/dev/fb0", O_RDWR);
if(fd_fb == -1)
{
	return;
}

// get virtual screen info stuct that contains the yres and save it
if(ioctl(fd_fb, FBIOGET_VSCREENINFO, &fb_vscreeninfo) == -1)
{
	return;
}
//get fixed screen info that contains the line length
if(ioctl(fd_fb, FBIOGET_FSCREENINFO, &fb_fscreeninfo) == -1)
{
	return;
}

/*
calculate the screens resolution and size by using
yres*linelength = size;
*/
yres_virtual = fb_vscreeninfo.yres_virtual;
linlength = fb_fscreeninfo.line_length;
fb_size = yres_virtual * linlength;

//get the settings structure for the terminal to buffer key press
ioctl(0, TCGETS, &terminal_settings);

//hopefully echo off and Icanon off maybe?
terminal_settings.c_lflag &= ~ECHO;
terminal_settings.c_lflag &= ~ICANON;
//applay changes
ioctl(0, TCSETS, &terminal_settings);
//map the space in mem
mapped_space = mmap(NULL, fb_size, PROT_WRITE, MAP_SHARED, fd_fb, 0);
if(!mapped_space)
{
	exit_graphics();
}

}

/*
Used system call: ioctl
Closes and resotres terminal to original state and settings
*/
void exit_graphics()
{
struct termios terminal_settings;
//retrive the termios struct containing the controls
ioctl(0, TCGETS, &terminal_settings);
//set the flags back to there original state
terminal_settings.c_lflag |= (ICANON | ECHO);
//apply the changes back to the term
ioctl(0, TCSETS, &terminal_settings);
//un-map the pages from memory
munmap(mapped_space, fb_size);
//close the frame buffer file
close(fd_fb);
}

/*
Used syscalls: select, read:
gets user input from user presing key using select a non-
blocking linux system call to see when somthing has been typed then
reading it from the stdin file fd = 0;
*/
char getkey()
{
int nfds;
char keygot;
size_t size_of_input = 1;
struct timeval t = {0L, 0L};
fd_set fdset;
nfds = 0 + 1;
FD_SET(0, &fdset);
if(select(nfds, &fdset, NULL, NULL, &t) > 0)
{
	read(0, &keygot, size_of_input);
	return keygot;
}
	return 'e';
}

/*
sleepms:
Syscalls - nanosleep
makes program sleep for a specified number of miliseconds (ms)
*/
void sleep_ms(long ms)
{
int time_ms;
struct timespec request;
time_ms = ms / 1000;
request.tv_sec = time_ms;
request.tv_nsec = ms%1000 * 1000000;
nanosleep(&request, NULL);
}

/*
Function draw pixel:
Syscalls used:
*/
void draw_pixel(int x, int y, color_t color)
{
if(y >= 640)
	return;
if(x >= 480)
	return;
if(x < 0)
	return;
if(y < 0)
	return;

//get the pixel to change
color_t *pixel = mapped_space + x * linlength + y * 2;
//set that pixel at the location aboves to the provided color
*pixel = color;
}

/*
Draw rectangle draw a rectangle on screen using corners -
 (x1,y1), (x1+width,y1), (x1+width,y1+height), (x1,y1+height).
 using draw pixel
*/
void draw_rect(int x1, int y1, int width, int height, color_t c)
{
int temp_x = x1;
int temp_y = y1;

//draw top line connecting (x1,y1+height) to (x1+width, y1+height)
for(temp_x; temp_x <= x1+width; temp_x++)
{
draw_pixel(temp_x, y1+height, c);
}

//draw bottom line connecting (x1,y1) to (x1+width,y1)
for(temp_x = x1; temp_x <= x1+width; temp_x++)
{
draw_pixel(temp_x, y1, c);
}

//draw left line connecting (x1, y1) to (x1,y1+height)
for(temp_y; temp_y <= y1+height; temp_y++)
{
draw_pixel(x1, temp_y, c);
}

//draw final line connecting (x1+width, y1) to x1+width
for(temp_y = y1; temp_y <= y1+height; temp_y++)
{
draw_pixel(x1+width, temp_y, c);
}

}

void clear_screen()
{
write(1, "\033[2J",8);
}

/*
draw_text()
text drawing function using font.h file to take text and print it to the
terminal chars in the font are encoded in 8 bit ints using a mask, & and
shifting, pull the letter out from the number
*/
void draw_text(int x, int y, const char *text, color_t c)
{
int i, j;
int k = 0;
int start_index;
int char_spacing = 0;
char pointed;
unsigned short array_char;
unsigned short mask = 0b0000000000000001;
while(text[k] != '\0')
{
start_index = text[k] * 16 + 0;
for(i = 0; i < 16; i++)
{
	array_char = (unsigned short)iso_font[start_index + i];
		for(j = 0; j < 8; j++)
		{
			if(array_char&mask)
			{
				draw_pixel(y+i, x+j + char_spacing, c);
			}
		array_char = array_char>>1;
		}
}
k++;
char_spacing += 8;
//printf("\npointed: %c", text[k]);
}
}
/*
MidPoint cirlce algo modified from Wikipedia.org
*/
void draw_circle(int x0, int y0, int radius, color_t c)
{
int x = radius-1;
int y =0;
int dx = 1;
int dy = 1;
int err = dx - (radius <<1);

while(x >= y)
{
	draw_pixel(x0 + x, y0 + y, c);
	draw_pixel(x0 + y, y0 + x, c);
	draw_pixel(x0 - y, y0 + x, c);
	draw_pixel(x0 - x, y0 + y, c);
	draw_pixel(x0 - x, y0 - y, c);
	draw_pixel(x0 - y, y0 - x, c);
	draw_pixel(x0 + y, y0 - x, c);
	draw_pixel(x0 + x, y0 - y, c);

	if(err <= 0)
	{
		y++;
		err += dy;
		dy += 2;
	}
	if(err > 0)
	{
		x--;
		dx += 2;
		err += dx - (radius << 1);
	}
}
}
