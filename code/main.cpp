// This file is a place holder for the future code.

// #include <mysql.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: move this into a file with utility functions
void prompt_credentials(char *userid, size_t userid_size,
                        char *password, size_t password_size);

#include <ncurses.h>
#include <string.h>

/**
 * Prompt the user for userid and password using ncurses.
 * 
 * Assumes ncurses has already been initialized (initscr, cbreak, noecho, etc.).
 *
 * @param userid Buffer to store userid.
 * @param userid_size Size of userid buffer.
 * @param password Buffer to store password.
 * @param password_size Size of password buffer.
 */
void prompt_credentials(char *userid, size_t userid_size,
                        char *password, size_t password_size) {
    int ch, i;

    // Prompt for userid
    mvprintw(2, 2, "Enter UserID: ");
    echo(); // enable echo for userid
    getnstr(userid, userid_size - 1);
    noecho();

    // Prompt for password
    mvprintw(4, 2, "Enter Password: ");
    refresh();

    i = 0;
    while ((ch = getch()) != '\n' && ch != '\r' && i < (int)password_size - 1) {
        if (ch == KEY_BACKSPACE || ch == 127) { // handle backspace
            if (i > 0) {
                i--;
                password[i] = '\0';
                mvaddch(4, 16 + i, ' '); // erase last '*'
                move(4, 16 + i);
            }
        } else {
            password[i++] = ch;
            password[i] = '\0';
            mvaddch(4, 16 + i - 1, '*'); // display '*'
        }
        refresh();
    }
    password[i] = '\0';

    // Optional: confirm collection
    mvprintw(6, 2, "Credentials received.");
    refresh();
}


int main()
{
   char userid[128];
   char pwd[128];

   initscr();                       // Start curses
   prompt_credentials(userid, sizeof(userid), pwd, sizeof(pwd));
   mvprintw(8, 2, "Press any key to exit...");
   refresh();
   getch();
   endwin();                        // End curses

   printf("Collected User ID:  %s\n", userid);
   printf("Collected Password: %s\n", pwd);

   return 0;

}


/*************************************************

   // Initialize the database connection
   MYSQL *conn;
   if (!(conn = mysql_init(0)))
   {
      fprintf(stderr, "Unable to initialize the connection struct.\n");
      exit(1);
   }

   // Connect to the database
   if (!mysql_real_connect(
         conn,                      // Connection
         "127.0.0.1",               // Host
         "Put User Here",           // User
         "Put Password Here",       // Pwd
         "bookstore",               // Database
         3306,                      // Port
         NULL,                      // Path to socket file
         0                          // Additional options
      ))
   {
      // Report the connection failure error and close the connection
      fprintf(stderr, "Error connecting to the Server: %s\n", mysql_error(conn));
      mysql_close(conn);
      exit(1);
   }

 
*************************************************/
