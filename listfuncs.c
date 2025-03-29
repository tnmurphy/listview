
#include "listfuncs.h"
#include <ctype.h>
#include <string.h>

inline void append_display_element(DisplayElement **list,
                                   DisplayElement *item) {
  DisplayElement *find;
  item->next = (DisplayElement *)0;
  if (*list == (DisplayElement *)0) {
    *list = item;
  } else {
    find = *list;
    while (find->next != (DisplayElement *)0)
      find = find->next;
    find->next = item;
  }
}

DisplayElement *scan_to(DisplayElement *list, int pos) {
  DisplayElement *find;
  int fpos;

  if (list == (DisplayElement *)0)
    return list;

  fpos = 0;
  find = list;
  while (find->next != (DisplayElement *)0 && fpos < pos) {
    find = find->next;
    fpos++;
  }

  return find;
}

int find_tokens(char *string, Token tokens[], int max_tokens) {
  int token_count = 0;
  char *current;

  current = string;
  while (token_count < max_tokens) {
    /* search for the first non-whitespace character */
    while (isspace(*current)) {
      current++;
    }
    if (*current == '\0') {
      break;
    }
    tokens[token_count].ptr = current;
    tokens[token_count].len = 0;

    int len = 0;
    while (*current != '\0' && *current != ',') {

      current++;
      if (isspace(*current)) {
        tokens[token_count].len = len;
      }
      len++;
    }
    if (*current == '\0') {
      break;
    } else if (*current == ',') {
    }
    /* search for the last non-whitespace character before a separator or before
     * the end of the string */
  }

  return token_count;
}

int parse_input_into_list(DisplayElement **list) {
  int num_rows = 0;
  char input_line[MAXELEM];
  const int max_tokens = 2;

  while (!feof(stdin)) {
    DisplayElement *new_item = NULL;
    Token tokens[max_tokens];
    int token_count = 0;
    fgets(input_line, MAXELEM, stdin);
    input_line[MAXELEM - 1] = '\0';
    token_count = find_tokens(input_line, tokens, max_tokens);

    switch (token_count) {
    case 0:
      break; /* no menu item */
      ;
      ;
    case 1:
      new_item = (DisplayElement *)malloc(sizeof(DisplayElement));
      if (new_item) {
        strncpy(new_item->display, tokens[0].ptr, tokens[0].len);
        new_item->display[tokens[0].len] = '\0';
      }
      break;
      ;
      ;
    case 2:
      new_item = (DisplayElement *)malloc(sizeof(DisplayElement));
      if (new_item) {
        strncpy(new_item->display, tokens[0].ptr, tokens[0].len);
        new_item->display[tokens[0].len] = '\0';
        strncpy(new_item->output, tokens[1].ptr, tokens[1].len);
        new_item->output[tokens[1].len] = '\0';
      }
      break;
      ;
      ;
    default:
      break;
      ;
      ;
    }

    if (new_item) {
      append_display_element(list, new_item);
      num_rows++;
    }
  }
  return num_rows;
}
