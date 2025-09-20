// This file is a place holder for the future code.

#include <ncurses.h>

int main()
{
   initscr();                       // Start curses
   move(10, 10);                    // Set the print location
   addstr("Hello, PatriciaSQL!");   // Print hello
   refresh();                       // Print it onto the screen
   getch();                         // Wait for user input
   endwin();                        // End curses

   return 0;
}
