#include <stdio.h>
#include <string.h>

#include "snake_utils.h"
#include "game.h"

int main(int argc, char *argv[]) {
  bool io_stdin = false;
  char *in_filename = NULL;
  char *out_filename = NULL;
  game_t *game = NULL;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i < argc - 1) {
      if (io_stdin) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      in_filename = argv[i + 1];
      i++;
      continue;
    } else if (strcmp(argv[i], "--stdin") == 0) {
      if (in_filename != NULL) {
        fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
        return 1;
      }
      io_stdin = true;
      continue;
    }
    if (strcmp(argv[i], "-o") == 0 && i < argc - 1) {
      out_filename = argv[i + 1];
      i++;
      continue;
    }
    fprintf(stderr, "Usage: %s [-i filename | --stdin] [-o filename]\n", argv[0]);
    return 1;
  }

  // Do not modify anything above this line.

  /* Task 7 */

  FILE *input = NULL;
  if (in_filename != NULL) {
    input = fopen(in_filename, "r");
    if (!input) return -1;
    game = load_board(input);
    fclose(input);
  } else if (io_stdin) {
    game = load_board(stdin);
  } else {
    game = create_default_game();
  }
  if (!game) return -1;
  if ((in_filename != NULL || io_stdin) && !initialize_snakes(game)) {
    free_game(game); return -1;
  }
  update_game(game, deterministic_food);
  FILE *output = out_filename ? fopen(out_filename, "w") : stdout;
  if (!output) { free_game(game); return -1; }
  print_board(game, output);
  int failed = ferror(output);
  if (out_filename && fclose(output)) failed = 1;
  free_game(game);
  return failed ? -1 : 0;
}
