// This file is a place holder for the future code.

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include <ncurses.h>

int main()
{
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

   printf("Successfully opened and closed the database connection.\n");


   // TODO - REMOVE the following after testing
   return(0); // End for now - just testing the connection.

/*************************************************

   initscr();                       // Start curses
   move(10, 10);                    // Set the print location
   addstr("Hello, PatriciaSQL!");   // Print hello
   refresh();                       // Print it onto the screen
   getch();                         // Wait for user input
   endwin();                        // End curses

   return 0;

*************************************************/
}
