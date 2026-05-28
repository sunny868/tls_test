#include <jni.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif
static int g_val2 = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread int tls_val2 = 2;

JNIEXPORT jint JNICALL Java_NativeTest_getGlobal2(JNIEnv *env, jclass cls) {
  return g_val2;
}

JNIEXPORT jint JNICALL Java_NativeTest_getTls2(JNIEnv *env, jclass cls) {
  return tls_val2;
}

JNIEXPORT jint JNICALL Java_NativeTest_add2(JNIEnv *env, jclass cls, jint a, jint b) {
  pthread_mutex_lock(&mutex);
  ++g_val2;
  pthread_mutex_unlock(&mutex);

  ++tls_val2;
  //fprintf(stderr, "[%ld] Java_NativeTest_add in\n", syscall(SYS_gettid));
  //fprintf(stderr, "==\n"); //ok
  return a + b;
}

#ifdef __cplusplus
}
#endif
