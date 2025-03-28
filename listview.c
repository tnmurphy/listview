#include <curses.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h> /* Added 2024 by Tim Murphy */
#include <unistd.h>
#include <utmp.h>

#define MAXELEM 100
#define KEY_LV_MARK ' '
#define LV_NPARAMS 3
#define LV_TITLE_PARAM 1
#define LV_COMMAND_PARAM 2

const char LV1_DELIM[] = ",\n";
const char LV2_DELIM[] = ", \n";

void unshowbar(WINDOW *w, int line);
void showbar(WINDOW *w, int line);

typedef struct dispElementStruct {
  char display[MAXELEM];
  char variable[MAXELEM];
  int marked;
  struct dispElementStruct *next;
} dispElement;

int appendToList(dispElement **list, dispElement *item);

int inline appendToList(dispElement **list, dispElement *item) {
  dispElement *find;
  item->next = (dispElement *)0;
  if (*list == (dispElement *)0) {
    *list = item;
  } else {
    find = *list;
    while (find->next != (dispElement *)0)
      find = find->next;
    find->next = item;
  }
}

dispElement *scanTo(dispElement *list, int pos) {
  dispElement *find;
  int fpos;

  if (list == (dispElement *)0)
    return list;

  fpos = 0;
  find = list;
  while (find->next != (dispElement *)0 && fpos < pos) {
    find = find->next;
    fpos++;
  }

  return find;
}

int main(int _argc, char *_argv[]) {
  dispElement *dirList = (dispElement *)0;
  char inStr[MAXELEM];
  char *tok1Ptr;
  char *tok2Ptr;
  FILE *console = (FILE *)0;
  FILE *ttyInput = (FILE *)0;
  int rowPos = 0;
  int cursor = 0;
  int keyPress;
  int maxX, maxY, begX, begY;
  int nRows, nCols;
  int padRefreshRows, padRefreshCols;
  int padStartY;
  int oldCursor, oldRowPos;
  dispElement *newItem;
  dispElement *currentItem;
  dispElement *showItem;
  int markCount = 0;
  WINDOW *mainWin;
  SCREEN *mainScr;

  int childStatus, childID;
  char **executeArgs;
  int addItem, countExtraArgs;
  int execArgsPos, argvPos;
  int argsInserted = 0;

  WINDOW *mainPad;

  if (_argc < LV_NPARAMS) {
    fprintf(stderr,
            "Too few arguments!\nUsage: %s [titleString] [command] [<args>] $ "
            "[<args>]\n",
            _argv[0]);
    exit(-1);
  }

  ttyInput = fopen("/dev/tty", "rw");
  console = fopen("/dev/tty", "w");

  nRows = 0;
  while (!feof(stdin)) {
    fgets(inStr, MAXELEM, stdin);
    tok1Ptr = strtok(inStr, LV1_DELIM);
    tok2Ptr = strtok(NULL, LV2_DELIM);
    if (tok1Ptr && tok2Ptr) {
      /* remove leading  spaces on variable */
      while (*tok2Ptr == ' ')
        tok2Ptr++;

      newItem = (dispElement *)malloc(sizeof(dispElement));
      if (newItem) {
        strncpy(newItem->display, tok1Ptr, MAXELEM);
        strncpy(newItem->variable, tok2Ptr, MAXELEM);
        newItem->marked = FALSE;
        appendToList(&dirList, newItem);
        nRows++;
      } else {
        fprintf(stderr, "Couldn't allocate memory.\n");
        exit(-1);
      }
    }
  }

  if (!dirList) {
    fprintf(stderr, "No display,variable pairs found in the input\n");
    exit(1);
  }

  /* Now try to switch to ncurses etc */
  mainScr = newterm(NULL, console, ttyInput);
  set_term(mainScr);
  mainWin = newwin(0, 0, 0, 0);
  fprintf(stderr, "lines %d cols %d\n", LINES, COLS);
  endwin();

  start_color();
  cbreak();
  noecho();
  nonl();

  beep();
  getmaxyx(mainWin, maxY, maxX);
  getbegyx(mainWin, begY, begX);
  nCols = maxX - begX + 1;
  padRefreshRows = (nRows > maxY - 2) ? maxY - 2 : nRows;
  padRefreshCols = (nCols > maxX) ? maxX - 1 : nCols - 1;
  mainPad = newpad(nRows, nCols);
  intrflush(mainPad, FALSE);
  keypad(mainPad, TRUE);
  nodelay(mainPad, FALSE);

  showItem = dirList;
  while (showItem != (dispElement *)0) {
    wprintw(mainPad, "%s\n", showItem->display);
    showItem = showItem->next;
  }

  /* Initialiase the pad input settings & draw it for the first time */
  cursor = 0;
  currentItem = dirList;
  keypad(mainPad, TRUE);
  showbar(mainPad, cursor + rowPos);
  padStartY = begY + 1;
  prefresh(mainPad, rowPos, 0, padStartY, 0, padRefreshRows, padRefreshCols);

  /* print the first status line below */
  wattr_on(mainWin, WA_REVERSE, NULL);
  mvwprintw(mainWin, maxY - 1, begX, "%s , %d  ", currentItem->variable,
            maxY - 1);

  mvwprintw(mainWin, begY, begX, "LISTVIEW - %s ", _argv[LV_TITLE_PARAM]);
  wrefresh(mainWin);

  keyPress = '\0';
  while (keyPress != '\r') {
    //	fprintf(stderr, "curpos %d, rowpos: %d, maxy: %d.\n", cursor, rowPos,
    //maxY);
    oldCursor = cursor;
    oldRowPos = rowPos;
    switch (keyPress) {
    case KEY_LV_MARK: /*  User wishes to select an item */
      if (currentItem->marked == FALSE) {
        currentItem->marked = TRUE;
        mvwaddch(mainPad, rowPos + cursor, maxX - 1, '*');
        markCount++;
      } else {
        currentItem->marked = FALSE;
        mvwaddch(mainPad, rowPos + cursor, maxX - 1, ' ');
        markCount--;
      }
      break;

    case KEY_PPAGE: /* Page up (moves 1/2 a screen) */
      if (rowPos + cursor > maxY / 2) {
        cursor -= maxY / 2;
        if (cursor < 0) {
          rowPos += cursor;
          cursor = 0;
        }
      } else
        beep();
      break;

    case KEY_NPAGE: /* Page Down (moves 1/2 a screen) */
      if (rowPos + cursor + maxY / 2 < nRows - 1) {
        cursor += maxY / 2;
        if (cursor >= maxY - 1) {
          rowPos += cursor - (maxY - 1);
          cursor = maxY - 1;
        }
      } else
        beep();
      break;

    case KEY_UP: /* Move up one item. */
      if (cursor > 0)
        cursor--;
      else if (rowPos > 0)
        rowPos--;
      else
        beep();
      break;

    case KEY_DOWN: /* Move down one item */
      if (rowPos + cursor < nRows - 1) {
        if (cursor < maxY - 1)
          cursor++;
        else
          rowPos++;
      } else
        beep();
      break;

    } /* End switch */

    /* Display the cursor bar */
    unshowbar(mainPad, oldCursor + oldRowPos);
    showbar(mainPad, cursor + rowPos);

    /* Display the variable associated with the current item. */
    currentItem = scanTo(dirList, rowPos + cursor);
    wmove(mainWin, maxY - 1, begX);
    wclrtoeol(mainWin);
    mvwprintw(mainWin, maxY - 1, begX, "%s , yb %d , ym %d , c %d ",
              currentItem->variable, begY, maxY, cursor);
    wrefresh(mainWin);

    /*  Update the Pad */
    prefresh(mainPad, rowPos, 0, padStartY, 0, padRefreshRows, padRefreshCols);

    /* Get next keystroke */
    keyPress = wgetch(mainPad);
  }

  werase(mainWin);
  werase(mainPad);
  wrefresh(mainWin);
  endwin();
  fclose(console);
  fclose(ttyInput);

  if (keyPress != KEY_CANCEL) {

    /*
     * Find out how many extra arguments were tacked on to the
     * the end of the command line.
     */

    countExtraArgs = 0;
    while (_argv[LV_NPARAMS + countExtraArgs] != NULL)
      countExtraArgs++;

    executeArgs =
        (char **)malloc(sizeof(char *) * countExtraArgs + markCount + 2);
    *executeArgs = _argv[LV_COMMAND_PARAM];
    argvPos = LV_NPARAMS;
    execArgsPos = 1;
    argsInserted = FALSE;
    while (_argv[argvPos] != NULL) {
      //			printf("! %d, %s\n", countExtraArgs,
      //_argv[argvPos]);
      if (*_argv[argvPos] == '$' && !argsInserted) {
        argsInserted = TRUE;
        currentItem = dirList;
        while (currentItem != (dispElement *)0) {
          if (currentItem->marked == TRUE) {
            //						printf("! %d, %s\n",
            //countExtraArgs, currentItem->variable);
            *(executeArgs + execArgsPos) = currentItem->variable;
            execArgsPos++;
          }
          currentItem = currentItem->next;
        }
      } else {
        *(executeArgs + execArgsPos) = _argv[argvPos];
        execArgsPos++;
      }

      argvPos++;
    }

    *(executeArgs + execArgsPos) = NULL;

    execArgsPos = 0;
    while (*(executeArgs + execArgsPos) != 0) {
      //		printf("arg %d, \'%s\'\n", execArgsPos,
      //*(executeArgs+execArgsPos));
      execArgsPos++;
    }
    childID = fork();
    if (childID == 0) {
      if (execvp(_argv[LV_COMMAND_PARAM], executeArgs) == -1) {
        fprintf(stderr, "Could not execute: %s\n", _argv[LV_COMMAND_PARAM]);
        exit(-1);
      }
    } else {
      wait(&childStatus);
    }
  }

  exit(0);
}

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
