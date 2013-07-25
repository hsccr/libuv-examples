#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uv.h>

void signal_handler(uv_signal_t *handle, int signum) {
  printf("Signal received: %d\n", signum);
  uv_signal_stop(handle);
}

void thread1_worker(void *userp) {
  uv_loop_t *loop1 = uv_loop_new();

  uv_signal_t sig1a, sig1b;
  uv_signal_init(loop1, &sig1a);
  uv_signal_start(&sig1a, signal_handler, SIGUSR1);

  uv_signal_init(loop1, &sig1b);
  uv_signal_start(&sig1b, signal_handler, SIGUSR1);

  uv_run(loop1, UV_RUN_DEFAULT);
}

void thread2_worker(void *userp) {
  uv_loop_t *loop2 = uv_loop_new();
  uv_loop_t *loop3 = uv_loop_new();

  uv_signal_t sig2;
  uv_signal_init(loop2, &sig2);
  uv_signal_start(&sig2, signal_handler, SIGUSR1);

  uv_signal_t sig3;
  uv_signal_init(loop3, &sig3);
  uv_signal_start(&sig3, signal_handler, SIGUSR1);

  while (uv_run(loop2, UV_RUN_NOWAIT) || uv_run(loop3, UV_RUN_NOWAIT)) {}
}
int main() {
  printf("PID %d\n", getpid());

  uv_thread_t thread1, thread2;

  uv_thread_create(&thread1, thread1_worker, 0);
  uv_thread_create(&thread2, thread2_worker, 0);

  uv_thread_join(&thread1);
  uv_thread_join(&thread2);
}
