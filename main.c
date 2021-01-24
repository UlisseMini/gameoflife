#define _XOPEN_SOURCE 500 /* Required for usleep */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "memdebug.h"

/* How big the grid should be, 40 looks good on my split tmux terminal. */
#define N 40

/* A Cell can be alive or dead */
typedef enum Cell { DEAD=0, ALIVE=1 } E_Cell;

/* Print the world in ascii */
void print_world(E_Cell** world) {
  for (int x = 0; x < N; x++) {
    for (int y = 0; y < N; y++) {
      E_Cell cell = world[y][x];
      char c = cell == ALIVE ? '#' : cell == DEAD ? '-' : '?';
      printf("%c ", c);
    }
    printf("\n");
  }
}

/* free() the world, column by column */
void free_world(E_Cell** world) {
  for (int i = 0; i < N; i++) { free(world[i]); }
  free(world);
}

/* malloc a new world, and its columns (NOTE: Not zeroed!) */
E_Cell** new_world() {
  E_Cell** world = malloc(N * sizeof(E_Cell*));
  for (int i = 0; i < N; i++) {
    world[i] = malloc(N * sizeof(E_Cell));
  }
  return world;
}

/* Number of live neighbors for the cell at x,y */
int neighbors(E_Cell** world, int y, int x, E_Cell type) {
  int n = 0;

  // Middle left and right
  n += (x+1 < N) && world[y][x+1] == type;
  n += (x > 0) && world[y][x-1] == type;

  // Bottom (y+1 goes down)
  if (y+1 < N) {
    n += world[y+1][x] == type;
    n += (x+1 < N) && world[y+1][x+1] == type;
    n += (x > 0) && world[y+1][x-1] == type;
  }

  // Top
  if (y > 0) {
    n += world[y-1][x] == type;
    n += (x+1 < N) && world[y-1][x+1] == type;
    n += (x > 0) && world[y-1][x-1] == type;
  }

  return n;
}

/* Make a step in the world, n_world is overwritten with the new world. */
void step_world(E_Cell** world, E_Cell** n_world) {
  for (int x = 0; x < N; x++) {
    for (int y = 0; y < N; y++) {
      int neigh = neighbors(world, y, x, ALIVE);

      if (world[y][x] == ALIVE) {
        /* A live cell with fewer then two, or greater then three live neighbors dies */
        if (neigh < 2 || neigh > 3) { n_world[y][x] = DEAD; }
        else { n_world[y][x] = ALIVE; }
      } else if (world[y][x] == DEAD) {
        /* A dead cell with exactly three alive neighbors becomes alive */
        if (neigh == 3) { n_world[y][x] = ALIVE; }
        else { n_world[y][x] = DEAD; }
      } else {
        /* unreachable */
        fprintf(stderr, "This branch should be unreachable!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

int main() {
  E_Cell** world = new_world();
  E_Cell** n_world = new_world();

  /* Create glider, Note that we index by [col][row], or [y][x] */
  world[19][19] = ALIVE;
  world[20][20] = ALIVE;
  world[21][20] = ALIVE;
  world[21][19] = ALIVE;
  world[21][18] = ALIVE;

  /* Make sure neighbors is working correctly */
  assert(neighbors(world, 21, 18, ALIVE) == 1);
  assert(neighbors(world, 21, 19, ALIVE) == 3);
  assert(neighbors(world, 20, 19, ALIVE) == 5);

  /* Alternate between overwriting world, and n_world */
  for (int steps = 0; steps < 80; steps++) {
    if (steps % 2 == 0) { 
      print_world(world);
      step_world(world, n_world);
    } else {
      print_world(n_world);
      step_world(n_world, world);
    }
    printf("\x1b[%dA", N); /* Escape code for up N lines */
    usleep(1000*100); /* Sleep for 100000microseconds = 100ms */
  }
  print_world(n_world);

  free_world(world);
  free_world(n_world);

  printf("%d allocations %d frees\n", allocs, frees);
  return 0;
}

