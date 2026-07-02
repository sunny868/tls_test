#include <jni.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/syscall.h>
#include <unistd.h> 
#include <pthread.h>
#include <sys/prctl.h>
#include <asm/prctl.h>

__thread long main_tls_val = 51;

void incMainTls(void) {
  ++main_tls_val;
  fprintf(stderr, "[%ld]main main_tls_val=%d\n", syscall(SYS_gettid), main_tls_val);
}
static unsigned long get_fs_base() {
    unsigned long fs_base = 0;
    if (syscall(SYS_arch_prctl, ARCH_GET_FS, &fs_base) == 0) {
        return fs_base;
    }
    return 0;
}

int main() {
    void* handle = dlopen("libadd.so", RTLD_LAZY);
#if 0
    typedef int (*FUNC)(int, int);
    FUNC testAdd = (FUNC)dlsym(handle, "testAdd");
    int ret = testAdd(1, 2);
    fprintf(stderr, "[%ld]main testAdd=%d\n", syscall(SYS_gettid), ret);
#endif 
    typedef void (*FUNC2)(void*);
    FUNC2 testCallback = (FUNC2)dlsym(handle, "testCallback");
    testCallback(incMainTls);

    main_tls_val +=1;
    errno = 0;
    //asm volatile("int3\n\t":::"memory");
    fprintf(stderr, "[%ld]main fs:base=0x%lx main_tls_val=%d\n", syscall(SYS_gettid), get_fs_base(), main_tls_val);
    return 0;
}
