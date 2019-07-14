#include <stdio.h>
#include <uv.h>

int64_t counter = 0;

void wait_for_a_while(uv_idle_t* handle) {
  counter++;

  if (counter >= 10e6)
    uv_idle_stop(handle);
}

int main() {
  uv_idle_t idler;
  uv_loop_t *loop = uv_default_loop();

  uv_idle_init(loop, &idler);
  uv_idle_start(&idler, wait_for_a_while);

  printf("Size of uv_idle_t: %zu\n", sizeof(uv_idle_t));
  printf("Idling...\n");
  uv_run(loop, UV_RUN_DEFAULT);

  uv_loop_close(uv_default_loop());
  return 0;
}
