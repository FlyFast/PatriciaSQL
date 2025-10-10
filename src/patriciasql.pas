program patriciasql;

{$mode objfpc}{$H+}

uses
  ncurses,
  menu,
  panel,
  SysUtils;

const
  MainChoices: array[0..6] of PChar = (
    'File', 'Edit', 'Search', 'Query', 'Tools', 'Help', 'Exit'
  );
  FileChoices: array[0..8] of PChar = (
    'Connect to...', 'New Query Ctl-t', 'Close Query', 'Close All Queries',
    'Load SQL File Ctl-o', 'Run SQL File Ctl-r', 'Save... Ctl-s', 'Save As...',
    'Back'
  );
  EditChoices: array[0..4] of Pchar =
  (
    'Copy', 'Paste', 'Cut', 'Select All', 'Back'
  );
  SearchChoices: array[0..3] of Pchar =
  (
    'Find...', 'Find/Replace Again', 'Replace', 'Back'
  );
  QueryChoices: array[0..8] of Pchar =
  (
    'New Query Tab', 'Close Query Tab', 'Rename Query Tab', 'Clear',
    'Run', 'Run Selection', 'Set Delimiter', 'Un/comment', 'Back'
  );
  ToolsChoices: array[0..2] of PChar = (
    'Options', 'Preferences', 'Back'
  );
  HelpChoices: array[0..2] of PChar =
  (
    'SQL Help', 'About', 'Back'
  );

var
  Items: PPItem;
  MainMenu: PMenu;
  FileMenu, EditMenu, SearchMenu, QueryMenu, ToolsMenu, HelpMenu: PMenu;
  FileItems, EditItems, SearchItems, QueryItems, ToolsItems, HelpItems: PPItem;
  WinQuery, WinResults, WinSub: PWindow;
  PanQuery, PanResults, PanSub: PPanel;
  Rows, Cols: LongInt;
  C, I, NChoices: Integer;

procedure BuildMenu(var M: PMenu;
                        var Arr: PPItem;
                        const Choices: array of PChar);
var
  i, Count: Integer;
begin
  Count := Length(Choices);
  GetMem(Arr, (Count+1) * SizeOf(PItem));
  for i := 0 to Count-1 do
    Arr[i] := new_item(Choices[i], '');
  Arr[Count] := nil;
  M := new_menu(@Arr[0]);
  set_menu_mark(M, ' > ');
end;

procedure RunSubmenu(M: PMenu; Title: String; XPos: Integer);
var
  ch: Integer;
  name: PChar;
  width, height: Integer;
begin
  height := 11; // TODO: Make this dynamic based on number of entries
  width := 20;

  // Create submenu directly under main menu item
  WinSub := newwin(height, width, 1, XPos);
  PanSub := new_panel(WinSub);
  box(WinSub, 0, 0);
  mvwprintw(WinSub, 0, 2, PChar(Title));
  set_menu_win(M, WinSub);
  set_menu_sub(M, derwin(WinSub, height-2, width-2, 1, 1));

  post_menu(M);
  wrefresh(WinSub);

  repeat
    ch := getch();
    case ch of
      KEY_DOWN: menu_driver(M, REQ_DOWN_ITEM);
      KEY_UP:   menu_driver(M, REQ_UP_ITEM);
      27: // ESC closes submenu
        break;
      10: // Enter
        begin
          name := item_name(current_item(M));
          if StrComp(name, 'Back') = 0 then
            break
          else
          begin
            mvprintw(15, 2, 'Submenu %s selected: %s    ', PChar(Title), name);
            refresh();
          end;
        end;
    end;
    wrefresh(WinSub);
  until False;

  // Cleanup submenu
  unpost_menu(M);
  del_panel(PanSub);
  delwin(WinSub);
  update_panels();
  doupdate();
end;

function GetMenuItemXPos(M: PMenu; Item: PItem): Integer;
var
  idx, i, x: Integer;
begin
  idx := item_index(Item);
  x := 1;
  for i := 0 to idx-1 do
    x := x + Length(StrPas(item_name(menu_items(M)[i]))) + 2; // spacing
  Result := x;
end;

begin
  // Init curses
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, True);
  getmaxyx(stdscr, Rows, Cols);

  // Create two stacked windows
  WinQuery := newwin(Rows div 2, Cols, 1, 0);
  WinResults := newwin(Rows - (Rows div 2), Cols, Rows div 2, 0);

  PanQuery := new_panel(WinQuery);
  PanResults := new_panel(WinResults);

  box(WinQuery, 0, 0);
  mvwprintw(WinQuery, 0, 2, 'Query');
  box(WinResults, 0, 0);
  mvwprintw(WinResults, 0, 2, 'Results');
  update_panels();
  doupdate();

  // Build main menu
  NChoices := Length(MainChoices);
  GetMem(Items, (NChoices+1)*SizeOf(PItem));
  for I := 0 to NChoices-1 do
    Items[I] := new_item(MainChoices[I], '');
  Items[NChoices] := nil;

  MainMenu := new_menu(@Items[0]);
  set_menu_format(MainMenu, 1, NChoices);
  set_menu_mark(MainMenu, '');
  post_menu(MainMenu);
  refresh();

  // Build submenus
  BuildMenu(FileMenu, FileItems, FileChoices);
  BuildMenu(EditMenu, EditItems, EditChoices);
  BuildMenu(SearchMenu, SearchItems, SearchChoices);
  BuildMenu(QueryMenu, QueryItems, QueryChoices);
  BuildMenu(ToolsMenu, ToolsItems, ToolsChoices);
  BuildMenu(HelpMenu, HelpItems, HelpChoices);

  while True do
  begin
    C := getch();
    case C of
      KEY_RIGHT: menu_driver(MainMenu, REQ_RIGHT_ITEM);
      KEY_LEFT:  menu_driver(MainMenu, REQ_LEFT_ITEM);
      10: // Enter
        begin
          if StrComp(item_name(current_item(MainMenu)), 'Exit') = 0 then
            Break
          else if StrComp(item_name(current_item(MainMenu)), 'File') = 0 then
            RunSubmenu(FileMenu, 'File', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else if StrComp(item_name(current_item(MainMenu)), 'Edit') = 0 then
            RunSubmenu(EditMenu, 'Edit', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else if StrComp(item_name(current_item(MainMenu)), 'Search') = 0 then
            RunSubmenu(SearchMenu, 'Search', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else if StrComp(item_name(current_item(MainMenu)), 'Query') = 0 then
            RunSubmenu(QueryMenu, 'Query', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else if StrComp(item_name(current_item(MainMenu)), 'Tools') = 0 then
            RunSubmenu(ToolsMenu, 'Tools', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else if StrComp(item_name(current_item(MainMenu)), 'Help') = 0 then
            RunSubmenu(HelpMenu, 'Help', GetMenuItemXPos(MainMenu, current_item(MainMenu)))
          else
          begin
            mvprintw(Rows-2, 2, 'You selected: %s   ', item_name(current_item(MainMenu)));
            refresh();
          end;
        end;
    end;
    update_panels();
    doupdate();
  end;

  // Cleanup main menu
  unpost_menu(MainMenu);
  free_menu(MainMenu);
  for I := 0 to NChoices-1 do
    free_item(Items[I]);
  FreeMem(Items);

  // Free submenus
  free_menu(FileMenu);
  free_menu(EditMenu);
  free_menu(SearchMenu);
  free_menu(ToolsMenu);
  free_menu(QueryMenu);
  free_menu(HelpMenu);

  for I := 0 to High(FileChoices) do free_item(FileItems[I]);
  for I := 0 to High(EditChoices) do free_item(EditItems[I]);
  for I := 0 to High(SearchChoices) do free_item(SearchItems[I]);
  for I := 0 to High(ToolsChoices) do free_item(ToolsItems[I]);
  for I := 0 to High(QueryChoices) do free_item(QueryItems[I]);
  for I := 0 to High(HelpChoices) do free_item(HelpItems[I]);

  FreeMem(FileItems);
  FreeMem(EditItems);
  FreeMem(SearchItems);
  FreeMem(ToolsItems);
  FreeMem(QueryItems);
  FreeMem(HelpItems);

  del_panel(PanQuery);
  del_panel(PanResults);
  delwin(WinQuery);
  delwin(WinResults);
  endwin();
end.

