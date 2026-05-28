#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val5 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread long tls_val5 = 15;
static __thread long tls_val5_2 = 15;
static __thread long tls_val5_3 = 15;
static __thread long tls_val5_4 = 15;
static __thread long tls_val5_5 = 15;

int getGlobal5() { return g_val5; }
long getTls5() {  return tls_val5_5; }

int testAdd5(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++g_val5;
  pthread_mutex_unlock(&mutex);

  ++tls_val5_5;
  return a + b;
}

#ifdef __cplusplus
}
#endif
