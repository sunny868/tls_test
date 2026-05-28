#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
__thread long tls_val __attribute__((tls_model("initial-exec"))) = 11;

int getGlobal() { return g_val; }
long getTls() {  return tls_val; }

int testAdd(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++g_val;
  pthread_mutex_unlock(&mutex);

  ++tls_val;
  return a + b;
}

#ifdef __cplusplus
}
#endif
