
/*
 jc_console_test.cpp

 Created by Jonathan Crossley on 10/13/19.
 Copyright © 2019 Jonathan Crossley. All rights reserved.
*/
 
// g++ -Wall -Wextra -O3 /home/pi/jc_main/src/jc_console_test.cpp -o /home/pi/jc_main/jc_test -lncurses

#include <iostream>
#include <curses.h>
//#include "wiringPi.h"

using namespace std;

/*
void row_mv (int n)
{
    for (int i = 0; i < n; i++)
        cout << endl;
}
*/
int main(void) {
    
    // Initilize curses variables
    initscr();
    nodelay(stdscr, TRUE);
    clear();
    curs_set(0);

    printf("Start\n");

    // Try printing to the console
    int row = 1;
    
    mvprintw(1, 1, "Thottle (%%):");
    mvprintw(2, 1, "Thottle flag:");
    mvprintw(3, 1, "Thottle Comp:");
    /*
    cout << "Start" << row_mv(1);
    cout << "Thottle (%%):" << row_mv(1);
    cout << "Thottle flag:" << row_mv(1);
    cout << "Thottle Comp:" << row_mv(1);
    */
    // Wait for user to hit enter
    getchar();
    
    // Reset screen and show the curser
    endwin();
    curs_set(1);

    return 0;
}
