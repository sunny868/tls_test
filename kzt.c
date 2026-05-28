#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int _g_val = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread short _tls_val = 21;
static __thread short _tls_val_v = 22;

int getKZTGlobal() { return _g_val; }
long getKZTTls() {  return _tls_val + _tls_val_v; }

int testKZT(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++_g_val;
  pthread_mutex_unlock(&mutex);

  ++_tls_val;
  ++_tls_val_v;
  return a + b;
}

#ifdef __cplusplus
}
#endif
