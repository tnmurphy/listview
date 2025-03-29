/* listview.c 
 *
 * Copyright (c) 2024 Timothy Norman Murphy

 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listfuncs.h"

#define KEY_LV_MARK ' '
#define LV_NPARAMS 2
#define LV_TITLE_PARAM 1

void unshowbar(WINDOW *w, int line);
void showbar(WINDOW *w, int line);

void showbar(WINDOW *w, int line) {
  int bx, by, mx, my;
  getmaxyx(w, my, mx);
  getbegyx(w, by, bx);

  mvwchgat(w, line, bx, -1, (attr_t)A_REVERSE, COLOR_BLACK, NULL);
}

void unshowbar(WINDOW *w, int line) {
  int bx, by, mx, my;
  getmaxyx(w, my, mx);
  getbegyx(w, by, bx);

  mvwchgat(w, line, bx, -1, (attr_t)A_NORMAL, COLOR_BLACK, NULL);
}

int main(int _argc, char *_argv[]) {
  DisplayElement *menu_list = (DisplayElement *)0;
  FILE *console = (FILE *)0;
  FILE *tty_input = (FILE *)0;
  int row_pos = 0;
  int cursor = 0;
  int key_press;
  int max_x, max_y, beg_x, beg_y;
  int num_rows, num_cols;
  int pad_refresh_rows, pad_refresh_cols;
  int pad_start_y;
  int old_cursor, old_row_pos;
  DisplayElement *current_item;
  DisplayElement *show_item;
  int mark_count = 0;
  WINDOW *main_win;
  SCREEN *main_screen;

  WINDOW *mainPad;

  if (_argc < LV_NPARAMS) {
    fprintf(stderr, "Too few arguments!\nUsage: %s [titleString]", _argv[0]);
    exit(-1);
  }

  tty_input = fopen("/dev/tty", "rw");

  if (!tty_input) {
    fprintf(stderr, "couldn't open /dev/tty for read/write\n");
    exit(-1);
  }

  console = fopen("/dev/tty", "w");
  if (!console) {
    fprintf(stderr, "couldn't open /dev/tty for write\n");
    exit(-1);
  }

  num_rows = parse_input_into_list(&menu_list);

  if (!menu_list) {
    fprintf(stderr, "No display,variable pairs found in the input\n");
    exit(1);
  }

  /* Now try to switch to ncurses etc */
  main_screen = newterm(NULL, console, tty_input);
  set_term(main_screen);
  main_win = newwin(0, 0, 0, 0);
  /* fprintf(stderr, "lines %d cols %d\n", LINES, COLS); */
  endwin();

  start_color();
  cbreak();
  noecho();
  nonl();

  beep();
  getmaxyx(main_win, max_y, max_x);
  getbegyx(main_win, beg_y, beg_x);
  num_cols = max_x - beg_x + 1;
  pad_refresh_rows = (num_rows > max_y - 2) ? max_y - 2 : num_rows;
  pad_refresh_cols = (num_cols > max_x) ? max_x - 1 : num_cols - 1;
  mainPad = newpad(num_rows, num_cols);
  intrflush(mainPad, FALSE);
  keypad(mainPad, TRUE);
  nodelay(mainPad, FALSE);

  show_item = menu_list;
  while (show_item != (DisplayElement *)0) {
    wprintw(mainPad, "%s\n", show_item->display);
    show_item = show_item->next;
  }

  /* Initialiase the pad input settings & draw it for the first time */
  cursor = 0;
  current_item = menu_list;
  keypad(mainPad, TRUE);
  showbar(mainPad, cursor + row_pos);
  pad_start_y = beg_y + 1;
  prefresh(mainPad, row_pos, 0, pad_start_y, 0, pad_refresh_rows,
           pad_refresh_cols);

  /* print the first status line below */
  wattr_on(main_win, WA_REVERSE, NULL);
  mvwprintw(main_win, max_y - 1, beg_x, "%s , %d  ", current_item->output,
            max_y - 1);

  mvwprintw(main_win, beg_y, beg_x, "LISTVIEW - %s ", _argv[LV_TITLE_PARAM]);
  wrefresh(main_win);

  key_press = '\0';
  while (key_press != '\r') {
    old_cursor = cursor;
    old_row_pos = row_pos;
    switch (key_press) {
    case KEY_LV_MARK: /*  User wishes to select an item */
      if (current_item->marked == FALSE) {
        current_item->marked = TRUE;
        mvwaddch(mainPad, row_pos + cursor, max_x - 1, '*');
        mark_count++;
      } else {
        current_item->marked = FALSE;
        mvwaddch(mainPad, row_pos + cursor, max_x - 1, ' ');
        mark_count--;
      }
      break;

    case KEY_PPAGE: /* Page up (moves 1/2 a screen) */
      if (row_pos + cursor > max_y / 2) {
        cursor -= max_y / 2;
        if (cursor < 0) {
          row_pos += cursor;
          cursor = 0;
        }
      } else
        beep();
      break;

    case KEY_NPAGE: /* Page Down (moves 1/2 a screen) */
      if (row_pos + cursor + max_y / 2 < num_rows - 1) {
        cursor += max_y / 2;
        if (cursor >= max_y - 1) {
          row_pos += cursor - (max_y - 1);
          cursor = max_y - 1;
        }
      } else
        beep();
      break;

    case KEY_UP: /* Move up one item. */
      if (cursor > 0)
        cursor--;
      else if (row_pos > 0)
        row_pos--;
      else
        beep();
      break;

    case KEY_DOWN: /* Move down one item */
      if (row_pos + cursor < num_rows - 1) {
        if (cursor < max_y - 1)
          cursor++;
        else
          row_pos++;
      } else
        beep();
      break;

    } /* End switch */

    /* Display the cursor bar */
    unshowbar(mainPad, old_cursor + old_row_pos);
    showbar(mainPad, cursor + row_pos);

    /* Display the variable associated with the current item. */
    current_item = scan_to(menu_list, row_pos + cursor);
    wmove(main_win, max_y - 1, beg_x);
    wclrtoeol(main_win);
    mvwprintw(main_win, max_y - 1, beg_x, "%s , yb %d , ym %d , c %d ",
              current_item->output, beg_y, max_y, cursor);
    wrefresh(main_win);

    /*  Update the Pad */
    prefresh(mainPad, row_pos, 0, pad_start_y, 0, pad_refresh_rows,
             pad_refresh_cols);

    /* Get next keystroke */
    key_press = wgetch(mainPad);
  }

  werase(main_win);
  werase(mainPad);
  wrefresh(main_win);
  endwin();
  fclose(console);
  fclose(tty_input);

  if (key_press != KEY_CANCEL) {
    current_item = menu_list;
    while (current_item != (DisplayElement *)0) {
      if (current_item->marked == TRUE) {
        printf("%s\n", current_item->output);
      }
      current_item = current_item->next;
    }
  }

  exit(0);
}
