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
#include <limits.h>
#include <unistd.h>

#include "listfuncs.h"

#define KEY_LV_MARK ' '
#define LV_NPARAMS 2
#define LV_TITLE_PARAM 1

void unshowbar(WINDOW *w, int line, int marked);
void showbar(WINDOW *w, int line, int marked);

void showbar(WINDOW *w, int line, int marked) {
  int bx, by, mx, my;
  getmaxyx(w, my, mx);
  getbegyx(w, by, bx);

  if (marked) {
    mvwchgat(w, line, bx, -1, (attr_t)A_NORMAL, 1, NULL);
  } else {
    mvwchgat(w, line, bx, -1, (attr_t)A_REVERSE, COLOR_BLACK, NULL);
  }
}

void unshowbar(WINDOW *w, int line, int marked) {
  int bx, by, mx, my;
  getmaxyx(w, my, mx);
  getbegyx(w, by, bx);

  if (marked) {
    mvwchgat(w, line, bx, -1, (attr_t)A_NORMAL, 1, NULL);
  } else {
    mvwchgat(w, line, bx, -1, (attr_t)A_NORMAL, COLOR_BLACK, NULL);
  }
}

void dump_selected(DisplayElement *list) {
  DisplayElement *current = list;
  while (current) {
    if (current->marked == TRUE) {
      if (current->output[0] != '\0') {
        printf("%s\n", current->output);
      } else {
        printf("%s\n", current->display);
      }
    }
    current = current->next;
  }
}


int show_menu(char title[], DisplayElement *menu_list, int num_rows, int mark_limit) {
  DisplayElement *old_item;
  DisplayElement *current_item;
  DisplayElement *show_item;
  FILE *console = (FILE *)0;
  FILE *tty_input = (FILE *)0;
  int row_pos = 0;
  int cursor = 0;
  int key_press;
  int max_x, max_y, beg_x, beg_y;
  int num_cols;
  int pad_refresh_rows, pad_refresh_cols;
  int pad_start_y;
  int old_cursor, old_row_pos;
  int mark_count = 0;
  int old_marked = 0;
  WINDOW *main_win;
  SCREEN *main_screen;
  WINDOW *main_pad;

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
  main_pad = newpad(num_rows, num_cols);
  intrflush(main_pad, FALSE);
  keypad(main_pad, TRUE);
  nodelay(main_pad, FALSE);

  show_item = menu_list;
  while (show_item != (DisplayElement *)0) {
    wprintw(main_pad, "%s\n", show_item->display);
    show_item = show_item->next;
  }

  /* Initialiase the pad input settings & draw it for the first time */
  cursor = 0;
  current_item = menu_list;
  old_item = NULL;
  keypad(main_pad, TRUE);
  showbar(main_pad, cursor + row_pos, 0);
  pad_start_y = beg_y + 1;
  prefresh(main_pad, row_pos, 0, pad_start_y, 0, pad_refresh_rows,
           pad_refresh_cols);

  /* print the first status line below */
  wattr_on(main_win, WA_REVERSE, NULL);
  mvwprintw(main_win, max_y - 1, beg_x, "%s , %d  ", current_item->output,
            max_y - 1);

  mvwprintw(main_win, beg_y, beg_x, "LISTVIEW - %s ", title);
  wrefresh(main_win);

  /* mark colour */
  init_pair(1, COLOR_BLACK, COLOR_CYAN);

  key_press = '\0';
  while (key_press != '\r') {
    switch (key_press) {
    case KEY_LV_MARK: /*  User wishes to select an item */
      if (current_item->marked == FALSE) {
	if (mark_count < mark_limit) {
          current_item->marked = TRUE;
          mvwaddch(main_pad, row_pos + cursor, max_x - 1, '*');
          mark_count++;
	}
      } else {
        current_item->marked = FALSE;
        mvwaddch(main_pad, cursor, max_x - 1, ' ');
        mark_count--;
      }
      break;

    case KEY_PPAGE: /* Page up (moves 1/2 a screen) */
      if (row_pos + cursor > max_y / 2) {
        old_cursor = cursor;
        old_row_pos = row_pos;
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
        old_cursor = cursor;
        old_row_pos = row_pos;
        cursor += max_y / 2;
        if (cursor >= max_y - 1) {
          row_pos += cursor - (max_y - 1);
          cursor = max_y - 1;
        }
      } else
        beep();
      break;

    case KEY_UP: /* Move up one item. */
      old_cursor = cursor;
      old_row_pos = row_pos;
      if (cursor > 0)
        cursor--;
      else if (row_pos > 0)
        row_pos--;
      else
        beep();
      break;

    case KEY_DOWN: /* Move down one item */
      old_cursor = cursor;
      old_row_pos = row_pos;
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
    int old_index = old_row_pos + old_cursor;
    old_item = scan_to(menu_list, old_index);
    unshowbar(main_pad, old_index, old_item->marked);
    int current_index = row_pos + cursor;

    /* Display the variable associated with the current item. */
    current_item = scan_to(menu_list, current_index);
    showbar(main_pad, current_index, current_item->marked);

    wmove(main_win, max_y - 1, beg_x);
    wclrtoeol(main_win);
    mvwprintw(main_win, max_y - 1, beg_x, "%s , yb %d , ym %d , c %d ",
              current_item->output, beg_y, max_y, cursor);
    wrefresh(main_win);

    /*  Update the Pad */
    prefresh(main_pad, row_pos, 0, pad_start_y, 0, pad_refresh_rows,
             pad_refresh_cols);

    /* Get next keystroke */
    key_press = wgetch(main_pad);
  }

  werase(main_win);
  werase(main_pad);
  wrefresh(main_win);
  endwin();
  fclose(console);
  fclose(tty_input);

  return key_press;
}


int main(int _argc, char *_argv[]) {
  DisplayElement *menu_list = (DisplayElement *)0;
  int num_rows;


  if (_argc < LV_NPARAMS) {
    fprintf(stderr, "Too few arguments!\nUsage: %s [titleString]", _argv[0]);
    exit(-1);
  }


  int mark_limit = INT_MAX;
  int opt = 0;

  while ((opt = getopt(_argc, _argv, "u")) != -1) {
      switch (opt) {
      case 'u':
          mark_limit = 1;
          break;
      default: /* '?' */
          fprintf(stderr, "Usage: %s [-u]\n  -u: unique mark",
                  _argv[0]);
          exit(EXIT_FAILURE);
      }
  }


  num_rows = parse_input_into_list(&menu_list);

  if (!menu_list) {
    fprintf(stderr, "No display,variable pairs found in the input\n");
    exit(1);
  }

  if (show_menu(_argv[LV_TITLE_PARAM],
			  menu_list, num_rows, mark_limit) != KEY_CANCEL) {
    dump_selected(menu_list);
  }

  exit(0);
}
