#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <asm/prctl.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
__thread long tls_val_2 __attribute__((tls_model("initial-exec"))) = 10;
__thread long tls_val __attribute__((tls_model("initial-exec"))) = 11;

static unsigned long get_fs_base() {
    unsigned long fs_base = 0;
    if (syscall(SYS_arch_prctl, ARCH_GET_FS, &fs_base) == 0) {
        return fs_base;
    }
    return 0;
}

int getGlobal() { return g_val; }
long getTls() {
  //fprintf(stderr, "tls.c fs.base=0x%lx\n", get_fs_base());
  //asm volatile("int3\n\t":::"memory");
  return tls_val + tls_val_2;
 }

void incTls() {
  ++tls_val;
  ++tls_val_2;
}
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
