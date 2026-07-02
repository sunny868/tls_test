#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/syscall.h>
#include <unistd.h> 
#include <pthread.h>
#include <sys/prctl.h>

static __thread long _tls_val1 = 11;
static __thread long _tls_val2 __attribute__((tls_model("initial-exec"))) = 21;

//extern incMainTls();
void* thread_function(void* arg) {
    ++_tls_val1;
    ++_tls_val2;
    //incMainTls();
    void (*incMainTls)(void) = (void (*)(void)) dlsym(RTLD_DEFAULT, "incMainTls");
    if (incMainTls) {
      incMainTls();
    } else {
      fprintf(stderr, "incMainTls not found in main program.\n");
    }
    fprintf(stderr, "[%ld]native tls: %ld, %ld\n", syscall(SYS_gettid), _tls_val1, _tls_val2);
}

int testAdd(int a, int b) {
  pthread_t thread_id;
  int arg = 100;
  int result = pthread_create(&thread_id, NULL, thread_function, (void*)&arg);
  if (result != 0) {
        fprintf(stderr, "线程创建失败，错误码: %d\n", result);
        exit(EXIT_FAILURE);
  }
  pthread_join(thread_id, NULL);
  return a + b;
}

void* thread_function2(void* func) {
   // extern main void incMainTls(void)
   //asm volatile("break 5\n\t":::"memory");
   ((void (*)(void))func)();
}

void testCallback(void* func) {
  fprintf(stderr, "---------1\n");
  pthread_t thread_id;
  int arg = 100;
  int result = pthread_create(&thread_id, NULL, thread_function2, func);
  if (result != 0) {
        fprintf(stderr, "线程创建失败，错误码: %d\n", result);
        exit(EXIT_FAILURE);
  }
  pthread_join(thread_id, NULL);
}
