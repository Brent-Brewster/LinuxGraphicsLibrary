typedef unsigned short color_t;

void clear_screen();
void exit_graphics();
void init_graphics();
char getkey();
void sleep_ms(long ms);
void draw_rect(int x1, int y1, int width, int height, color_t c);
void draw_text(int x, int y, const char *text, color_t c);

int main(int argc, char** argv)
{
	color_t c = 0;
	int i, x, y;
	char *ptr2txt = "Press s key to begin the game...";
	init_graphics();
	x = 0;
	y = 1;
	clear_screen();
	char key_pressed = 'e';
	while(key_pressed != 's')
	{
	key_pressed = getkey();
	//upper left square in circle
	draw_rect(0, 0, 50, 50, 30);
	draw_circle(25, 25, 25, 29);
	//bottm left square in circle
	draw_rect(420, 0, 50, 50, 28);
	draw_circle(420+25, 25, 25, 29);
	//bottom right
	draw_rect(420, 585, 50, 50, 27);
	draw_circle(420+25, 585+25, 25, 27);
	//upper right
	draw_rect(0, 585, 50, 50, 26);
	draw_circle(25, 585+25, 25, 27);
	//middle circle
	for(i = 50; i > 0; i = i--)
		draw_circle(240, 320, i, 63488);
//	draw_rect(240, 320, 50, 50, 20);
	draw_text(100, 100, ptr2txt, 63488);
	sleep_ms(1000);
	}//end of while
	clear_screen();
	ptr2txt = "CONGRATS!!!, you started the game with the s key that is the hardest part";
	draw_text(0, 0, ptr2txt, 30);
	sleep_ms(5000);

	clear_screen();

	ptr2txt = "CONGRATULATIONS YOU WON THE GAME!";
	draw_text(640/2, 480/2, ptr2txt, 50000);

	sleep_ms(10000);

	for(i = 100; i > 0; i--)
	{
	draw_rect(300, 300, i, i, (color_t)i);
	draw_rect(200, 20, i, 50, i);
	draw_circle(100, 100, i, 6000);
	}
	exit_graphics();
}
