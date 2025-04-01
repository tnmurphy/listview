/* listfuncs_test.c
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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "listfuncs.h"

typedef struct FindTokensResultStruct {
  char input[MAXELEM];
  char count;
} FindTokensResult;
#define MAX_TOKENS 100

int test_parse_1(void) {

  int token_count = 0;
  Token tokens[MAX_TOKENS];

  FindTokensResult tests[] = {{"descronly", 1},
                              {"descr,output", 2},
                              {"descr and spaces,output", 2},
                              {"", -1}};

  for (int i = 0; tests[i].count != -1; i++) {
    token_count = find_tokens(tests[i].input, tokens, MAX_TOKENS);
    fprintf(stderr, "test_parse_1: test %d ", i, tests[i].input, token_count,
            tests[i].count);
    if (token_count != tests[i].count) {
      fprintf(stderr, "failed: %s: %d!=%d tokens\n", i, tests[i].input,
              token_count, tests[i].count);
      for (int j = 0; j < token_count; j++) {
        char temptoken[MAXELEM];
        strncpy(temptoken, tokens[0].ptr, tokens[0].len);
        temptoken[tokens[0].len] = '\0';

        fprintf(stderr, "token %d: '%s'\n", j, temptoken);
      }
    } else {
      fprintf(stderr, "ok\n");
    }
  }

  t
}

int main(int _argc, char *_argv[]) {

  if (!test_parse_1()) {
    fprintf(stderr, "test_parse_1 failed\n");
    exit(-1);
  }

  exit(0);
}
