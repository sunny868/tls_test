#define _POSIX_C_SOURCE 200809L
#include <cstring>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/syscall.h>
#include <unistd.h> 
#include <pthread.h>

//__thread int main_tls_val = 0;
static int callJVM() {
    JavaVM *jvm;        // JVM实例
    JNIEnv *env;        // JNI环境
    JavaVMInitArgs vm_args; // JVM初始化参数
    JavaVMOption options[1]; // JVM选项
    
    // 设置JVM选项 - 类路径
    //options[0].optionString = strdup("-Djava.class.path=.");
    options[0].optionString = strdup("-Xint");
    
    vm_args.version = JNI_VERSION_1_8;  // JNI版本
    vm_args.nOptions = 1;               // 选项数量
    vm_args.options = options;          // 选项数组
    vm_args.ignoreUnrecognized = JNI_FALSE; // 不忽略无法识别的选项
   
   // main_tls_val +=1; 
    // 创建JVM
    jint res = 0;
#ifdef TEST_STATIC // 方式一：编译时链接
    //g++ -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/lib/server -ljvm -DTEST_STATIC -o jvm_launcher jvm_launcher.cpp
    JavaVM* vm_list[10];
    jsize vm_count = 0;
    jint result = JNI_GetCreatedJavaVMs(vm_list, 10, &vm_count);
    if (result == JNI_OK) {
       printf(" JNI_GetCreatedJavaVMs return OK. count =%d\n", vm_count);
    } else {
       printf(" JNI_GetCreatedJavaVMs return FAILED!\n");
    }
    res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
#else              // 方式二：动态加载
//g++ -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -ldl -o jvm_launcher jvm_launcher.cpp
    void* handle = dlopen("libjvm.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle) { printf("%s\n", dlerror()); exit(0); }
    typedef int (*CreateJavaVM_t)(void* jvm, void** env, void* args);
    CreateJavaVM_t CreateJavaVM = (CreateJavaVM_t)dlsym(handle, "JNI_CreateJavaVM");
    printf("CreateJavaVM=%p\n", CreateJavaVM);
    res = CreateJavaVM(&jvm, (void**)&env, &vm_args);
#endif
    if (res != JNI_OK) {
        printf("Failed to create JVM, error code: %d\n", res);
        return 1;
    }
   // main_tls_val +=1; 
    printf("JVM started successfully!\n");
    
    // 查找Java类
    jclass cls = env->FindClass("HelloWorld");
    printf("FindClass out!\n");
    if (cls == NULL) {
        env->ExceptionDescribe(); // 打印异常信息
        jvm->DestroyJavaVM();
        return 1;
    }
    
    // 查找main方法
    jmethodID mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    if (mid == NULL) {
        printf("Main method not found\n");
        jvm->DestroyJavaVM();
        return 1;
    }
    
    // 调用main方法
    jobjectArray args = env->NewObjectArray(0, env->FindClass("java/lang/String"), NULL);
    env->CallStaticVoidMethod(cls, mid, args);
    
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    
    jvm->DestroyJavaVM();
    //main_tls_val +=1; 
    //printf("[%ld]main_tls_val=%d\n", syscall(SYS_gettid), main_tls_val);
}

typedef int (*TESTFUNC)(int, int);
typedef long (*GETTLS)();
typedef struct {
    int id;
    int a;
    int b;
    TESTFUNC test_fn;
    GETTLS get_tls;
} thread_arg_t_dlopen;

extern "C" {
  extern int testAdd(int a, int b); // tls.c
  extern long getTls();
  extern int testAdd2(int a, int b); // tls2.c
  extern long getTls2();
}
typedef struct {
    int id;
    int a;
    int b;
} thread_arg_t;

void* thread_func_kzt(void* arg) {
    thread_arg_t_dlopen* targ = (thread_arg_t_dlopen*)arg;
    long start_tls = targ->get_tls();
    targ->test_fn(targ->a, targ->b);
    targ->test_fn(targ->a, targ->b);
    long end_tls = targ->get_tls();
    printf("KZT thread %d: tls = %ld -> %ld\n", targ->id, start_tls, end_tls);
    return NULL;
}
void* thread_func_dlopen(void* arg) {
    thread_arg_t_dlopen* targ = (thread_arg_t_dlopen*)arg;
    long start_tls = targ->get_tls();
    targ->test_fn(targ->a, targ->b);
#if 1
    // test resizeTLSData
    if (targ->id == 400) { // libtls4.so
       sleep(1);
    }
#endif
    targ->test_fn(targ->a, targ->b);
    long end_tls = targ->get_tls();
    printf("Dlopen thread  %d: tls = %ld -> %ld\n", targ->id, start_tls, end_tls);
    return NULL;
}

void* thread_func(void* arg) {
    thread_arg_t* targ = (thread_arg_t*)arg;
    printf("Compiled-Linked thread %d: tls = %ld tls2 = %ld\n", targ->id, getTls(), getTls2());
    testAdd(targ->a, targ->b);
#if 1
    // test resizeTLSData
    if (targ->id == 100) { // libtls.so libtls2.so
       sleep(1);
    }
#endif
    testAdd(targ->a, targ->b);
    long tls = getTls();
    testAdd2(targ->a, targ->b);
    testAdd2(targ->a, targ->b);
    long tls2 = getTls2();
    printf("Compiled-Linked thread %d: tls = %ld tls2 = %ld\n", targ->id, tls, tls2);
    return NULL;
}

#define NUM_THREADS 2
static void testBuildLink() {
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    // for linker-static
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i + 100;
        args[i].a = i * 10;
        args[i].b = i * 2;

        if (pthread_create(&threads[i], NULL, thread_func, &args[i]) != 0) {
            perror("pthread_create");
            return;
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
}

static void testDlopen() {
    // test libtls3.so
    void* handle3 = dlopen("libtls3.so", RTLD_LAZY);
    if (!handle3) { fprintf(stderr, "dlopen error: %s\n", dlerror()); return; }

    TESTFUNC test_fn3 = (TESTFUNC)dlsym(handle3, "testAdd3");
    GETTLS gettls3 = (GETTLS)dlsym(handle3, "getTls3");

    pthread_t threads3[NUM_THREADS];
    thread_arg_t_dlopen args3[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args3[i].id = i+300;
        args3[i].a = i * 10;
        args3[i].b = i * 2;
        args3[i].test_fn = test_fn3;
        args3[i].get_tls = gettls3;

        if (pthread_create(&threads3[i], NULL, thread_func_dlopen, &args3[i]) != 0) {
            perror("pthread_create");
            return;
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads3[i], NULL);
    }

    // handle3 TLS module ID be free, and handle4 will be reused it.
    //dlclose(handle3);

    // test libtls4.so
    void* handle4 = dlopen("libtls4.so", RTLD_LAZY);
    TESTFUNC test_fn4 = (TESTFUNC)dlsym(handle4, "testAdd4");
    GETTLS gettls4 = (GETTLS)dlsym(handle4, "getTls4");
    pthread_t threads4[NUM_THREADS];
    thread_arg_t_dlopen args4[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args4[i].id = i+400;
        args4[i].a = i * 10;
        args4[i].b = i * 2;
        args4[i].test_fn = test_fn4;
        args4[i].get_tls = gettls4;

        if (pthread_create(&threads4[i], NULL, thread_func_dlopen, &args4[i]) != 0) {
            perror("pthread_create");
            return;
        }
    }
    // handle3 TLS module ID be free. but handle5 be not used it.
    dlclose(handle3);

    // test libtls5.so
    void* handle5 = dlopen("libtls5.so", RTLD_LAZY);
    TESTFUNC test_fn5 = (TESTFUNC)dlsym(handle5, "testAdd5");
    GETTLS gettls5 = (GETTLS)dlsym(handle5, "getTls5");
    pthread_t threads5[NUM_THREADS];
    thread_arg_t_dlopen args5[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args5[i].id = i+500;
        args5[i].a = i * 10;
        args5[i].b = i * 2;
        args5[i].test_fn = test_fn5;
        args5[i].get_tls = gettls5;

        if (pthread_create(&threads5[i], NULL, thread_func_dlopen, &args5[i]) != 0) {
            perror("pthread_create");
            return;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads4[i], NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads5[i], NULL);
    }

    dlclose(handle5);
    dlclose(handle4);
}

void callKZTTest() {
    // test libkzt.so
    void* handle = dlopen("libkzt.so", RTLD_LAZY);
    if (!handle) { fprintf(stderr, "dlopen error: %s\n", dlerror()); return; }

    TESTFUNC test_fn = (TESTFUNC)dlsym(handle, "testKZT");
    GETTLS gettls = (GETTLS)dlsym(handle, "getKZTTls");

    pthread_t threads[NUM_THREADS];
    thread_arg_t_dlopen args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].id = i+900;
        args[i].a = i * 10;
        args[i].b = i * 2;
        args[i].test_fn = test_fn;
        args[i].get_tls = gettls;

        if (pthread_create(&threads[i], NULL, thread_func_kzt, &args[i]) != 0) {
            perror("pthread_create");
            return;
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    dlclose(handle);
}
int main() {
    callJVM();
    //callKZTTest();
    testDlopen();
    testBuildLink();
    return 0;
}
