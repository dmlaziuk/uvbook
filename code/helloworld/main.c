#include <stdio.h>
#include <stdlib.h>
#include <uv.h>

int main() {
  uv_loop_t *loop = malloc(sizeof(uv_loop_t));
  uv_loop_init(loop);

  printf("Size of uv_loop_t: %zu\n", sizeof(uv_loop_t));
  printf("Now quitting.\n");
  uv_run(loop, UV_RUN_DEFAULT);

  uv_loop_close(loop);
  free(loop);
  return 0;
}
