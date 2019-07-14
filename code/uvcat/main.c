#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <uv.h>

void on_read(uv_fs_t *);
void on_write(uv_fs_t *);

uv_loop_t *loop;

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;
uv_fs_t close_req;

static char buffer[1024];

static uv_buf_t iov;

void on_open(uv_fs_t *req) {
  // The request passed to the callback is the same as the one the call setup
  // function was passed.
  assert(req == &open_req);
  if (req->result < 0) {
    fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
    return;
  }
  iov = uv_buf_init(buffer, sizeof(buffer));
  uv_fs_read(loop, &read_req, req->result, &iov, 1, -1, on_read);
}

void on_read(uv_fs_t *req) {
  if (req->result < 0) {
    fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    return;
  }
  if (req->result == 0) {
    uv_fs_close(loop, &close_req, open_req.result, NULL); // synchronous
    return;
  }
  iov.len = req->result;
  uv_fs_write(loop, &write_req, STDOUT_FILENO, &iov, 1, -1, on_write);
}

void on_write(uv_fs_t *req) {
  if (req->result < 0) {
    fprintf(stderr, "Write error: %s\n", uv_strerror((int)req->result));
    return;
  }
  // read loop
  uv_fs_read(loop, &read_req, open_req.result, &iov, 1, -1, on_read);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: nvcat <input_file>\n");
    return 1;
  }

  loop = uv_default_loop();

  uv_fs_open(loop, &open_req, argv[1], O_RDONLY, 0, on_open);
  uv_run(loop, UV_RUN_DEFAULT);

  uv_fs_req_cleanup(&open_req);
  uv_fs_req_cleanup(&read_req);
  uv_fs_req_cleanup(&write_req);
  return 0;
}
