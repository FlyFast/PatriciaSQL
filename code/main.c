// This file is a place holder for the future code.

#include <mysql.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: move this into a file with curses utility functions.
//       the file will need to be unique to each target client.
int prompt_credentials(char *userid, size_t userid_size,
                        char *password, size_t password_size);

// TODO: move this into a file with mariadb utility functions
int mariadb_conn(MYSQL *conn, char *userid, char *password);

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
int prompt_credentials(char *userid, size_t userid_size,
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

   return 0;
}

int mariadb_conn(MYSQL *conn, char *userid, char* password)
{
   if (!mysql_real_connect(
         conn,                      // Connection
         "127.0.0.1",               // Host
         userid,                    // User
         password,       // Pwd
         "bookstore",               // Database
         3306,                      // Port
         NULL,                      // Path to socket file
         0                          // Additional options
      ))
   {
      // Report the connection failure error and close the connection
      fprintf(stderr, "Error connecting to the Server: %s\n", 
               mysql_error(conn));
      mysql_close(conn);
      exit(1);
   }

   return 0;
}

int main()
{
   char userid[128];
   char pwd[128];

   MYSQL *conn;

   initscr();                       // Start curses
   prompt_credentials(userid, sizeof(userid), pwd, sizeof(pwd));
   mariadb_conn(conn, userid, pwd);
   getch();
   endwin();                        // End curses

   printf("Collected User ID:  %s\n", userid);
   printf("Collected Password: %s\n", pwd);

   return 0;

}
