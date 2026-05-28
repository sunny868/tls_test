#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val4 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread long tls_val4 = 14;
static __thread long tls_val4_2 = 14;
static __thread long tls_val4_3 = 14;
static __thread long tls_val4_4 = 14;

int getGlobal4() { return g_val4; }
long getTls4() {  return tls_val4_4; }

int testAdd4(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++g_val4;
  pthread_mutex_unlock(&mutex);

  ++tls_val4_4;
  return a + b;
}

#ifdef __cplusplus
}
#endif
