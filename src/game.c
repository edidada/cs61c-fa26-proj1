#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  game_t *game = calloc(1, sizeof(*game));
  if (!game) return NULL;
  game->board = calloc(18, sizeof(*game->board));
  if (!game->board) { free(game); return NULL; }
  for (unsigned int row = 0; row < 18; row++) {
    char *line = malloc(22);
    if (!line) { free_game(game); return NULL; }
    memset(line, row == 0 || row == 17 ? '#' : ' ', 20);
    line[0] = line[19] = '#'; line[20] = '\n'; line[21] = '\0';
    game->board[game->num_rows++] = line;
  }
  memcpy(game->board[2] + 2, "d>D", 3);
  game->board[2][9] = '*';
  game->snakes = malloc(sizeof(*game->snakes));
  if (!game->snakes) { free_game(game); return NULL; }
  game->num_snakes = 1;
  game->snakes[0] = (snake_t){2, 2, 2, 4, true};
  return game;
}

/* Task 2 */
void free_game(game_t *game) {
  if (!game) return;
  for (unsigned int row = 0; row < game->num_rows; row++) free(game->board[row]);
  free(game->board); free(game->snakes); free(game);
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  for (unsigned int row = 0; row < game->num_rows; row++) fputs(game->board[row], fp);
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  return c != '\0' && strchr("wasd", c) != NULL;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  return c != '\0' && strchr("WASDx", c) != NULL;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return c != '\0' && strchr("wasd^<v>WASDx", c) != NULL;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  switch (c) { case '^': return 'w'; case '<': return 'a'; case 'v': return 's'; case '>': return 'd'; default: return '?'; }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  switch (c) { case 'W': return '^'; case 'A': return '<'; case 'S': return 'v'; case 'D': return '>'; default: return '?'; }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (c == 'v' || c == 's' || c == 'S') return cur_row + 1;
  if (c == '^' || c == 'w' || c == 'W') return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if (c == '>' || c == 'd' || c == 'D') return cur_col + 1;
  if (c == '<' || c == 'a' || c == 'A') return cur_col - 1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];
  char head = get_board_at(game, snake->head_row, snake->head_col);
  unsigned int row = get_next_row(snake->head_row, head);
  unsigned int col = get_next_col(snake->head_col, head);
  if (row >= game->num_rows || col >= strlen(game->board[row])) return '#';
  return get_board_at(game, row, col);
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];
  char head = get_board_at(game, snake->head_row, snake->head_col);
  set_board_at(game, snake->head_row, snake->head_col, head_to_body(head));
  snake->head_row = get_next_row(snake->head_row, head);
  snake->head_col = get_next_col(snake->head_col, head);
  set_board_at(game, snake->head_row, snake->head_col, head);
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];
  char tail = get_board_at(game, snake->tail_row, snake->tail_col);
  set_board_at(game, snake->tail_row, snake->tail_col, ' ');
  snake->tail_row = get_next_row(snake->tail_row, tail);
  snake->tail_col = get_next_col(snake->tail_col, tail);
  char body = get_board_at(game, snake->tail_row, snake->tail_col);
  set_board_at(game, snake->tail_row, snake->tail_col, body_to_tail(body));
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  /* Sequential snake-number order is observable when snakes interact. */
  for (unsigned int i = 0; i < game->num_snakes; i++) {
    snake_t *snake = &game->snakes[i];
    if (!snake->live) continue;
    char cell = next_square(game, i);
    if (cell == '#' || is_snake(cell)) {
      snake->live = false;
      set_board_at(game, snake->head_row, snake->head_col, 'x');
    } else {
      update_head(game, i);
      if (cell == '*') add_food(game); else update_tail(game, i);
    }
  }
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  /* fgets chunks support pipes/stdin without seeking; shrink each stored row. */
  size_t len = 0, capacity = 128;
  char *line = malloc(capacity), chunk[128];
  if (!line) { errno = ENOMEM; return NULL; }
  line[0] = '\0';
  while (fgets(chunk, sizeof(chunk), fp)) {
    size_t n = strlen(chunk);
    if (n > SIZE_MAX - len - 1) { free(line); errno = ENOMEM; return NULL; }
    if (len + n + 1 > capacity) {
      size_t required = len + n + 1;
      size_t grown = capacity <= SIZE_MAX / 2 ? capacity * 2 : required;
      if (grown < required) grown = required;
      char *tmp = realloc(line, grown);
      if (!tmp) { free(line); errno = ENOMEM; return NULL; }
      line = tmp; capacity = grown;
    }
    memcpy(line + len, chunk, n + 1); len += n;
    if (n && chunk[n - 1] == '\n') break;
  }
  if (ferror(fp) || !len) { free(line); return NULL; }
  char *trimmed = realloc(line, len + 1);
  return trimmed ? trimmed : line;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  game_t *game = calloc(1, sizeof(*game));
  size_t capacity = 0;
  if (!game) return NULL;
  for (;;) {
    errno = 0;
    char *line = read_line(fp);
    if (!line) {
      if (ferror(fp) || errno == ENOMEM) { free_game(game); return NULL; }
      return game;
    }
    if (game->num_rows == UINT_MAX) { free(line); free_game(game); return NULL; }
    if ((size_t)game->num_rows == capacity) {
      size_t grown = capacity ? capacity * 2 : 8;
      if (grown < capacity || grown > SIZE_MAX / sizeof(*game->board)) { free(line); free_game(game); return NULL; }
      char **tmp = realloc(game->board, grown * sizeof(*game->board));
      if (!tmp) { free(line); free_game(game); return NULL; }
      game->board = tmp; capacity = grown;
    }
    game->board[game->num_rows++] = line;
  }
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  snake_t *snake = &game->snakes[snum];
  unsigned int row = snake->tail_row, col = snake->tail_col;
  char cell = get_board_at(game, row, col);
  while (!is_head(cell)) {
    row = get_next_row(row, cell); col = get_next_col(col, cell);
    cell = get_board_at(game, row, col);
  }
  snake->head_row = row; snake->head_col = col;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  unsigned int count = 0;
  for (unsigned int row = 0; row < game->num_rows; row++)
    for (size_t col = 0; game->board[row][col]; col++)
      if (is_tail(game->board[row][col])) {
        if (count == UINT_MAX) return NULL;
        count++;
      }
  snake_t *snakes = count ? calloc(count, sizeof(*snakes)) : NULL;
  if (count && !snakes) return NULL;
  /* load_board leaves snakes NULL; replacing also permits safe reinitialization. */
  free(game->snakes); game->snakes = snakes; game->num_snakes = count;
  unsigned int i = 0;
  for (unsigned int row = 0; row < game->num_rows; row++) {
    for (size_t col = 0; game->board[row][col]; col++) if (is_tail(game->board[row][col])) {
      if (col > UINT_MAX) return NULL;
      snakes[i].tail_row = row; snakes[i].tail_col = (unsigned int)col;
      find_head(game, i);
      snakes[i].live = get_board_at(game, snakes[i].head_row, snakes[i].head_col) != 'x';
      i++;
    }
  }
  return game;
}
