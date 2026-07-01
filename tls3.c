#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <dlfcn.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val3 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// PT_ELF segment info: 
// p_filesz(12B) = sizeof(int) + sizeof(long)
// p_memsz(14B) =  sizeof(int) + sizeof(long) + sizeof(short)
// p_align(8B): the largest var(long tls_val3_3)
// 实际申请大小：16B（14B对齐到8B的倍数）
static __thread short tls_val3 __attribute__((tls_model("local-dynamic")));
static __thread int tls_val3_2 __attribute__((tls_model("local-dynamic"))) = 13;
static __thread long tls_val3_3 __attribute__((tls_model("local-dynamic"))) = 13;

int getGlobal3() { return g_val3; }
long getTls3() {  return tls_val3 + tls_val3_2 + tls_val3_3; }

void test_free_invalid(void* p) {
  free(p);  // 错误：free 栈上变量，非堆地址
}
int testAdd3(int a, int b) {
  pthread_mutex_lock(&mutex);
  ++g_val3;
  pthread_mutex_unlock(&mutex);
   ++tls_val3;
   ++tls_val3_2;
   ++tls_val3_3;
   //test_free_invalid(&a);
  return a + b;
}

#ifdef __cplusplus
}
#endif
