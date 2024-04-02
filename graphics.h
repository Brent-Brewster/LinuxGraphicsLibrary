/*
Brent Brewster graphics.h header file to assist with use of the
library.c graphics library happy coding!
*/

/*
macro used to set the color_t value given individual red, r green g, and
blue b values can be used to set color_t.
*/
#define set_rgb(r, g, b);

typedef struct color_t
{
	unsigned short rgb;
}color_t;

/*
initalize graphics library functions and
disables output to the terminal
*/
void init_graphics();

/*
close graphics library functions and clean up memory
restores output to the terminal
*/
void exit_graphics();

/*
non blocking function returns the key pressed
*/
char getkey();

/*
Make program wait/sleep for a given number of ms
*/
void sleep_ms(long ms);

/*
clears everything from the screen
*/
void clear_screen();

/*
draws a rectangle on the screen of a given color c
with upper corner of rectangle located at (x1, y1) and
has a height of height and a width of width
*/
void draw_rect(int x1, int y1, int width, int height, color_t c)

/*
draws a given string or array of charecters *text onto the screen
with color c and the upper corner of the first charecter at
(x,y)
*/
void draw_text(int x, int y, const char *text, color_t c)

/*
draws a circle onto the screen with midpoint located at (x,y), and
radius r with color c
*/
void draw_circle(int x, int y, int r, color_t c)
