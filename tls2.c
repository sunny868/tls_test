#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val2 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread long tls_val2 = 12;

int getGlobal2() { return g_val2; }
long getTls2() {  return tls_val2; }

int testAdd2(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++g_val2;
  pthread_mutex_unlock(&mutex);

  ++tls_val2;
  return a + b;
}

#ifdef __cplusplus
}
#endif
