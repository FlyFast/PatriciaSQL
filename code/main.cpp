#include <ncurses.h>

int main()
{
   initscr();                       // Start curses
   printw("Hello, PatriciaSQL!");   // Print hello
   refresh();                       // Print it onto the screen
   getch();                         // Wait for user input
   endwin();                        // End curses

   return 0;
}
