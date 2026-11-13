#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "asserts.h"
// Necessary due to static functions in game.c
#include "game.c"

/* Look at asserts.c for some helpful assert functions */

int greater_than_forty_two(int x) { return x > 42; }

bool is_vowel(char c) {
  char *vowels = "aeiouAEIOU";
  for (int i = 0; i < strlen(vowels); i++) {
    if (c == vowels[i]) {
      return true;
    }
  }
  return false;
}

/*
  Example 1: Returns true if all test cases pass. False otherwise.
    The function greater_than_forty_two(int x) will return true if x > 42. False otherwise.
    Note: This test is NOT comprehensive
*/
bool test_greater_than_forty_two() {
  int testcase_1 = 42;
  bool output_1 = greater_than_forty_two(testcase_1);
  if (!assert_false("output_1", output_1)) {
    return false;
  }

  int testcase_2 = -42;
  bool output_2 = greater_than_forty_two(testcase_2);
  if (!assert_false("output_2", output_2)) {
    return false;
  }

  int testcase_3 = 4242;
  bool output_3 = greater_than_forty_two(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  return true;
}

/*
  Example 2: Returns true if all test cases pass. False otherwise.
    The function is_vowel(char c) will return true if c is a vowel (i.e. c is a,e,i,o,u)
    and returns false otherwise
    Note: This test is NOT comprehensive
*/
bool test_is_vowel() {
  char testcase_1 = 'a';
  bool output_1 = is_vowel(testcase_1);
  if (!assert_true("output_1", output_1)) {
    return false;
  }

  char testcase_2 = 'e';
  bool output_2 = is_vowel(testcase_2);
  if (!assert_true("output_2", output_2)) {
    return false;
  }

  char testcase_3 = 'i';
  bool output_3 = is_vowel(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  char testcase_4 = 'o';
  bool output_4 = is_vowel(testcase_4);
  if (!assert_true("output_4", output_4)) {
    return false;
  }

  char testcase_5 = 'u';
  bool output_5 = is_vowel(testcase_5);
  if (!assert_true("output_5", output_5)) {
    return false;
  }

  char testcase_6 = 'k';
  bool output_6 = is_vowel(testcase_6);
  if (!assert_false("output_6", output_6)) {
    return false;
  }

  return true;
}

/* Task 4.1 */

bool test_is_tail() {
  for (int i = 0; i < 256; i++) {
    char c = (char)i;
    bool expected = i == 'w' || i == 'a' || i == 's' || i == 'd';
    if (is_tail(c) != expected) return false;
  }
  return true;
}

bool test_is_head() {
  for (int i = 0; i < 256; i++) {
    bool expected = i == 'W' || i == 'A' || i == 'S' || i == 'D' || i == 'x';
    if (is_head((char)i) != expected) return false;
  }
  return true;
}

bool test_is_snake() {
  for (int i = 0; i < 256; i++) {
    const unsigned char chars[] = {'w','a','s','d','^','<','v','>','W','A','S','D','x'};
    bool expected = false;
    for (size_t j = 0; j < sizeof(chars); j++) if (i == chars[j]) expected = true;
    if (is_snake((char)i) != expected) return false;
  }
  return true;
}

bool test_body_to_tail() {
  const char in[] = "^<v>", out[] = "wasd";
  for (size_t i = 0; i < 4; i++) if (body_to_tail(in[i]) != out[i]) return false;
  return true;
}

bool test_head_to_body() {
  const char in[] = "WASD", out[] = "^<v>";
  for (size_t i = 0; i < 4; i++) if (head_to_body(in[i]) != out[i]) return false;
  return true;
}

bool test_get_next_row() {
  const unsigned int positions[] = {0, 1, 17, UINT_MAX - 1, UINT_MAX};
  for (size_t j = 0; j < sizeof(positions) / sizeof(positions[0]); j++) {
    for (int i = 0; i < 256; i++) {
      unsigned int expected = positions[j];
      if (i == 'v' || i == 's' || i == 'S') expected++;
      if (i == '^' || i == 'w' || i == 'W') expected--;
      if (get_next_row(positions[j], (char)i) != expected) return false;
    }
  }
  return true;
}

bool test_get_next_col() {
  const unsigned int positions[] = {0, 1, 17, UINT_MAX - 1, UINT_MAX};
  for (size_t j = 0; j < sizeof(positions) / sizeof(positions[0]); j++) {
    for (int i = 0; i < 256; i++) {
      unsigned int expected = positions[j];
      if (i == '>' || i == 'd' || i == 'D') expected++;
      if (i == '<' || i == 'a' || i == 'A') expected--;
      if (get_next_col(positions[j], (char)i) != expected) return false;
    }
  }
  return true;
}

/* Independent allocation/stream edge fixtures beyond the original unit tests. */
static bool test_stream_edges(void) {
  FILE *fp = tmpfile();
  if (!fp) return false;
  for (unsigned int i = 0; i < 1048576; i++) fputc('Q', fp);
  fputs("\nlast-without-newline", fp); rewind(fp);
  char *line = read_line(fp);
  if (!line || strlen(line) != 1048577 || line[1048576] != '\n') return false;
  for (size_t i = 0; i < 1048576; i++) if (line[i] != 'Q') return false;
  free(line); line = read_line(fp);
  if (!line || strcmp(line, "last-without-newline")) return false;
  free(line); if (read_line(fp) != NULL) return false;
  fclose(fp);
  fp = tmpfile(); if (!fp) return false;
  game_t *empty = load_board(fp); fclose(fp);
  if (!empty || empty->num_rows || empty->num_snakes || empty->snakes) return false;
  if (!initialize_snakes(empty)) return false;
  free_game(empty);
  fp = tmpfile(); if (!fp) return false;
  fputs("#########\n# d>D   #\n# d>>D  ###\n###########\n", fp); rewind(fp);
  game_t *game = load_board(fp); fclose(fp);
  if (!game || game->num_rows != 4 || game->snakes || game->num_snakes) return false;
  if (!initialize_snakes(game) || game->num_snakes != 2) return false;
  if (game->snakes[0].tail_row != 1 || game->snakes[0].head_col != 4 ||
      game->snakes[1].tail_row != 2 || game->snakes[1].head_col != 5) return false;
  free_game(game); free_game(NULL);
  return true;
}

bool test_customs() {
  if (!test_stream_edges()) return false;
  if (!test_greater_than_forty_two()) {
    printf("%s\n", "test_greater_than_forty_two failed.");
    return false;
  }
  if (!test_is_vowel()) {
    printf("%s\n", "test_is_vowel failed.");
    return false;
  }
  if (!test_is_tail()) {
    printf("%s\n", "test_is_tail failed");
    return false;
  }
  if (!test_is_head()) {
    printf("%s\n", "test_is_head failed");
    return false;
  }
  if (!test_is_snake()) {
    printf("%s\n", "test_is_snake failed");
    return false;
  }
  if (!test_body_to_tail()) {
    printf("%s\n", "test_body_to_tail failed");
    return false;
  }
  if (!test_head_to_body()) {
    printf("%s\n", "test_head_to_body failed");
    return false;
  }
  if (!test_get_next_row()) {
    printf("%s\n", "test_get_next_row failed");
    return false;
  }
  if (!test_get_next_col()) {
    printf("%s\n", "test_get_next_col failed");
    return false;
  }
  return true;
}

int main(int argc, char *argv[]) {
  init_colors();

  if (!test_and_print("custom", test_customs)) {
    return 1;
  }

  return 0;
}
