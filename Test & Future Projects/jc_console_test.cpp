
/*
 jc_console_test.cpp

 Created by Jonathan Crossley on 10/13/19.
 Copyright © 2019 Jonathan Crossley. All rights reserved.
*/
 
// g++ -Wall -Wextra -O3 /home/pi/jc_main/src/jc_console_test.cpp -o /home/pi/jc_main/jc_test -lncurses

#include <iostream>
#include <ncurses.h>

using namespace std;

int main(void) {
    
    // Initilize curses variables
    initscr();
    //nodelay(stdscr, TRUE);
    //clear();
    curs_set(0);

    // Try printing to the console
    int row = 1;
    
    mvprintw(row++, 1, "Thottle (%%):");
    mvprintw(row++, 1, "Thottle flag:");
    mvprintw(row++, 1, "Thottle Comp:");
    
    refresh();
    
    // Wait for user to hit enter
    getchar();
    
    // Reset screen and show the curser
    curs_set(1);
    endwin();

    return 0;
}
