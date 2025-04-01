/* listfuncs.c
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

#ifndef _LISTFUNCS_H_
#define _LISTFUNCS_H_

#include <curses.h>
#include <malloc.h>
#include <unistd.h>
#include <utmp.h>

#define MAXELEM 100

typedef struct DisplayElementStruct {
  char display[MAXELEM];
  char output[MAXELEM];
  int marked;
  struct DisplayElementStruct *next;
} DisplayElement;

typedef struct TokenStruct {
  char *ptr;
  int len;
} Token;

extern void append_display_element(DisplayElement **list, DisplayElement *item);
extern void add_item(DisplayElement **list, const char *description,
                     const char *output);
extern DisplayElement *scan_to(DisplayElement *list, int pos);
extern int parse_input_into_list(DisplayElement **list);
extern int find_tokens(char *string, Token tokens[], int max_tokens);

#endif
