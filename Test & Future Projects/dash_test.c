// first OpenVG program

//gcc -Wall -Wextra -O3 -I/opt/vc/include -lshapes dash_test.c -o dash_test



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Libraries for OpenVG
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "fontinfo.h"
#include "shapes.h"

// Defines to help set the screen size, set to current screen size
#define SCREEN_WIDTH	1280
#define SCREEN_HEIGHT	720

int main(void) {
	
	char s[3];

	// Set up the window size variables
	int width = SCREEN_WIDTH / 2;
	int height = SCREEN_HEIGHT / 2;
	//int width2 = SCREEN_WIDTH;
	//int height2 = SCREEN_HEIGHT;

	// Set up the window with a start point (origin at top left) and a size
	initWindowSize(0, 0, width, height);

	// Initialization the graphics with the window size
	init(&width, &height);

	// Check the size of the screen variables
	//printf("Width: %d\n",width);
	//printf("Height: %d\n",height);

	// Start a drawing scession
	Start(width, height);

	// Black background
	Background(0, 0, 0);
	
	// Set fill to bring grey
	StrokeWidth(10);
	//Fill(163, 163, 163, 0.58);
	Stroke(163, 163, 163, 0.58);
	RectOutline((width-500)/2, (height-50)/2, 500, 50);
	//Rect((width-500)/2, (height-50)/2, 500, 50);

	// Red
	StrokeWidth(0);
	Fill(240, 29, 33, 1);
	Rect((width-500)/2 + 5, (height-50)/2 + 5, 250 - 10, 50 - 10);	

	// Big blue marble
	//Fill(44, 77, 232, 1);
	//Circle(width / 2, 0, width);

	// White text
	Fill(255, 255, 255, 1);
	
	TextMid(width / 2, (height * 0.9), "Speed", SansTypeface, width / 25);
	End();

	// end with [RETURN]
	fgets(s, 2, stdin);
	// Graphics cleanup
	finish();
	
	return(0);
}
