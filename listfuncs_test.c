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

#define MAX_TOKENS 100
typedef struct FindTokensResultStruct {
  char input[MAXELEM];
  char count;
  char *tokens[MAX_TOKENS];
} FindTokensResult;

int test_parse_1(void) {

  int token_count = 0;
  Token tokens[MAX_TOKENS];
  int test_result = 1; /* default to succeed */

  FindTokensResult tests[] = {
      {"descronly", 1, {"descronly"}},
      {"descronlylf\n", 1, {"descronlylf"}},
      {"descr,output", 2, {"descr", "output"}},
      {"descr and spaces,output", 2, {"descr and spaces", "output"}},
      {"descr and spaces,output,another_output",
       3,
       {"descr and spaces", "output", "another_output"}},
      {"descr and stripped spaces ,output ,another_output",
       3,
       {"descr and stripped spaces", "output", "another_output"}},
      {",", 1, {"", ""}},
      {"", -1, {}}};

  for (int i = 0; tests[i].count != -1; i++) {
    int failed = 0;
    token_count = find_tokens(tests[i].input, tokens, MAX_TOKENS);
    fprintf(stderr, "test_parse_1: test %d: '%s' expecting %d tokens: ", i,
            tests[i].input, token_count);
    if (token_count != tests[i].count) {
      fprintf(stderr, " %d!=%d tokens", token_count, tests[i].count);
      failed = 1;
    }

    fprintf(stderr, "tokens: ");
    for (int j = 0; j < token_count; j++) {
      char temptoken[MAXELEM];
      strncpy(temptoken, tokens[j].ptr, tokens[j].len);
      temptoken[tokens[j].len] = '\0';
      fprintf(stderr, " '%s'", temptoken);
      if (strcmp(tests[i].tokens[j], temptoken) != 0) {
        failed = 1;
        if (j < tests[i].count) {
          fprintf(stderr, " != '%s' ", tests[i].tokens[j]);
        } else {
          fprintf(stderr, " != '' ");
        }
      } else {
        fprintf(stderr, ", ", j, tests[i].tokens[j]);
      }
    }

    if (failed) {
      fprintf(stderr, "failed\n");
      test_result = failed;
    } else {
      fprintf(stderr, "ok\n");
    }
  }

  return test_result;
}

int main(int _argc, char *_argv[]) {

  if (!test_parse_1()) {
    fprintf(stderr, "test_parse_1 failed\n");
    exit(-1);
  }

  exit(0);
}
