// This file is a place holder for the future code.

#include <mysql.h>
#include <menu.h>       // ncurses menu support
#include <ncurses.h>    // Only required for character terminal version.
#include <panel.h>      // Used to manage sub-windows in ncurses
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER "127.0.0.1"
#define DATABASE "bookstore"

// TODO: move this into a file with curses utility functions.
//       the file will need to be unique to each target client.
int prompt_credentials(char *userid, size_t userid_size,
                        char *password, size_t password_size);

// TODO: move this into a file with mariadb utility functions
int mariadb_conn(MYSQL *conn, char *server, char *database, char *userid, char *password);

// Struct to hold submenu definitions
typedef struct {
    const char *title;
    char **choices;
    int n_choices;
} SubMenu;

// Example submenu definitions
char *file_choices[] = {"New", "Open", "Save", "Back"};
char *tools_choices[] = {"Options", "Preferences", "Back"};

SubMenu submenus[] = {
    {"File", file_choices, sizeof(file_choices)/sizeof(file_choices[0])},
    {"Tools", tools_choices, sizeof(tools_choices)/sizeof(tools_choices[0])}
};
int n_submenus = sizeof(submenus)/sizeof(submenus[0]);

// Function to run a vertical submenu
void run_submenu(const char *title, char **choices, int n_choices) {
   ITEM **items;
   MENU *submenu;
   WINDOW *submenu_win;
   int c;

   items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
   for (int i = 0; i < n_choices; i++)
   {
      items[i] = new_item(choices[i], "");
   };
   items[n_choices] = (ITEM *)NULL;

   submenu = new_menu((ITEM **)items);
   submenu_win = newwin(n_choices + 4, 20, 2, 2); // height, width, y, x
   keypad(submenu_win, TRUE);

   set_menu_win(submenu, submenu_win);
   set_menu_sub(submenu, derwin(submenu_win, n_choices, 18, 2, 1));
   set_menu_mark(submenu, " > ");

   box(submenu_win, 0, 0);
   mvwprintw(submenu_win, 0, 2, "%s", title);

   post_menu(submenu);
   wrefresh(submenu_win);

   while ((c = wgetch(submenu_win))) 
   {
      switch (c) 
      {
         case KEY_DOWN:
            menu_driver(submenu, REQ_DOWN_ITEM);
            break;
         case KEY_UP:
            menu_driver(submenu, REQ_UP_ITEM);
            break;
         case 10: 
         {
            ITEM *cur = current_item(submenu);
            const char *name = item_name(cur);
            if (strcmp(name, "Back") == 0) 
            {
               goto exit_submenu; // TODO: Eliminate goto
            } 
            else 
            {
               mvprintw(LINES - 3, 0, "You chose %s -> %s", title, name);
               refresh();
            }
            break;
         }
      }
      wrefresh(submenu_win);
   }

exit_submenu:
   unpost_menu(submenu);
   free_menu(submenu);
   for (int i = 0; i < n_choices; i++) 
   {
      free_item(items[i]);
   }
   free(items);
   delwin(submenu_win);
}

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
   mvprintw(3, 2, "Enter Password: ");
   refresh();

   i = 0;
   while ((ch = getch()) != '\n' && ch != '\r' && i < (int)password_size - 1) {
       if (ch == KEY_BACKSPACE || ch == 127) { // handle backspace
           if (i > 0) {
               i--;
               password[i] = '\0';
               mvaddch(3, 18 + i, ' '); // erase last '*'
               move(3, 18 + i);
           }
       } else {
           password[i++] = ch;
           password[i] = '\0';
           mvaddch(3, 18 + i - 1, '*'); // display '*'
       }
       refresh();
   }
   password[i] = '\0';

   // Confirm credentials received
   mvprintw(5, 2, "Credentials received.");
   refresh();

   return 0;
}


/* Connect to a MariaDB server with the provided credentials.
 *
 * Assumes the database connection has been initialized with mysql_init().
 * TODO: Handle sockets (low priority)
 * 
 * Paramters:
 *    conn is a pointer to the database connection created by mysql_init().
 *    server is a string pointer to the mariadb server.
 *    database is a string pointer to the database.
 *    userid is a string pointer to the user's login name.
 *    password is a string pointer to the user's password.
 *
 */    
int mariadb_conn(MYSQL *conn, char *server, char *database, char *userid, char* password)
{
   if (!mysql_real_connect(
         conn,                      // Connection
         server,                    // Host
         userid,                    // User
         password,                  // Pwd
         database,                  // Database
         3306,                      // Port
         NULL,                      // Path to socket file
         0                          // Additional options
      ))
   {
      // Report the connection failure error and close the connection
      mvprintw(12, 2, "Error logging into MariaDB.");
      mvprintw(13, 5, "%s", mysql_error(conn));
      refresh();
      // fprintf(stderr, "Error connecting to the Server: %s\n", 
      //         mysql_error(conn));
      mysql_close(conn);
      getch();
      endwin();                        // End curses

      return(-1);
   }
   // TODO: Change the below message to say:
   //       Logged into <hostname> on <bind_address>
   //       Use SELECT @@hostname, @@bind_address;
   mvprintw(12, 2, "Successfully logged into MariaDB.");
   refresh();

   // TODO: Remove this before adding additional database code.
   mysql_close(conn);

   return 0;
}

/* Main program
 *
 * This will need to be refactored to break things into functions,
 * isolate the ncurses code so it can be replaced with other UIs, etc.
 *
 */

int main()
{
   char userid[128];                // TODO: DEFINE the max value
   char pwd[128];                   // TODO: DEFINE the max value

   // Start ncurses specific menu variables and defines ***********************
   // These should be moved to an ncurses specific file
   ITEM **items;                    // Menu items
   MENU *menu;                      // Menu pointer
   #define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
   #define CTRLD 4
   int c;                           // Working space for one character
   int n_choices;                   // Number of menu choices
   int i;                           // Iterator
   char *choices[] =                // Define the menu options
   {
      "File",
      "Edit",
      "Search",
      "Query",
      "Tools",
      "Help",
      "Exit",
   };

   // Start ncurses specific window variables
   WINDOW *winQuery, *winResults;   // Pointers to the top and bottom windows
   PANEL *panQuery, *panResults;    // Panels for the windows
   int rows, cols;                  // Number of rows/columns in the terminal
   PANEL *top = panQuery;           // Key track of panel with the focus
   // End ncurses specific window variables


printf("A\n");
   winResults = newwin((rows / 2) - 1, cols, (rows / 2) + 1, 0);

   // Create the panels to manage window stacking
   panQuery = new_panel(winQuery);
   panResults = new_panel(winResults);

   // Draw borders and labels
   box(winQuery, 0, 0);
   mvwprintw(winQuery, 0, 2, "Query");
printf("b\n");

   box(winResults, 0, 0);
   mvwprintw(winResults, 0, 2, "Results");
printf("c\n");
   // Show the panels
   update_panels();
   doupdate();
printf("d\n");
   // Create new menu items
   n_choices = ARRAY_SIZE(choices);
   items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
   for (i = 0; i < n_choices; ++i)
   {
      items[i] = new_item(choices[i], ""); // label + description
   };
   items[n_choices] = (ITEM *)NULL;
printf("e\n");
   // Create menu
   // ---------------------------------------------------------------------
   menu = new_menu((ITEM **)items); // TODO - START HERE - Seg faults here!
   // ---------------------------------------------------------------------
printf("f\n");
   // Display menu horizontally with 1 row and n_choices columns
   set_menu_format(menu, 1, n_choices);
printf("g\n");
   // Post the menu at row 0
   // mvprintw(2, 0, "Use LEFT/RIGHT arrow keys, Enter to select");
   set_menu_mark(menu, "");  // no marker (default is "->")
   post_menu(menu);
   refresh();
printf("h\n");
   while ((c = getch()) != KEY_F(1)) 
   {
      switch (c) // TODO: Improve key handling for new complexity
      {
         case KEY_RIGHT:
            menu_driver(menu, REQ_RIGHT_ITEM);
            break;
         case KEY_LEFT:
            menu_driver(menu, REQ_LEFT_ITEM);
            break;
         case '\t': // Tab key - toggle which panel gets focus
            if (top == panQuery)
            {
               top_panel(panResults);
               top = panResults;
            }
            else
            {
               top_panel(panQuery);
               top = panQuery;
            }
            break;
         case 10: // Enter key
            {
               ITEM *cur = current_item(menu);
               const char *name = item_name(cur);
               
               mvprintw(LINES - 4, 0, "You selected: %s   ", name);                      refresh();

               // Check if this menu item has a submenu
               for (int s = 0; s < n_submenus; s++) 
               {
                  if (strcmp(name, submenus[s].title) == 0) 
                  {
                     run_submenu(submenus[s].title, 
                                 submenus[s].choices, 
                                 submenus[s].n_choices);
                  }
               }

               if (strcmp(name, "Exit") == 0) 
               {
                  goto cleanup; // TODO: remove the goto
               }
               break;
            }
      }
      update_panels();
      doupdate();
      refresh();
   }

cleanup:
   // Unpost and free all the memory
   unpost_menu(menu);
   free_menu(menu);
   for (i = 0; i < n_choices; ++i)
   { 
      free_item(items[i]);
   }
   free(items);

   // Cleanup the panels
   del_panel(panQuery);
   del_panel(panResults);
   delwin(winQuery);
   delwin(winResults);
    
   endwin(); // End ncurses
   return 0;
}
