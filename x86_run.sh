export JAVA_HOME=/home/sunguoyun/jdk8u-ls/build/linux-x86_64-normal-server-release/images/j2sdk-image

# build java
$JAVA_HOME/bin/javac HelloWorld.java

# env
export LD_LIBRARY_PATH=${JAVA_HOME}/jre/lib/amd64/server:$LD_LIBRARY_PATH

# build launcher
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libnso.so nso.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libnso2.so nso2.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libkzt.so kzt.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libtls.so tls.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libtls2.so tls2.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libtls3.so tls3.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libtls4.so tls4.c
gcc -O2 -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -fPIC -shared -o x86/libtls5.so tls5.c
#g++ -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L${JAVA_HOME}/jre/lib/amd64/server -ljvm -DTEST_STATIC -o jvm_launcher jvm_launcher.cpp
g++ -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux -L./x86 -ltls -ltls2 -ldl -lpthread -o jvm_launcher jvm_launcher.cpp

# run
export LD_LIBRARY_PATH=.:./x86:$LD_LIBRARY_PATH
#strace -e trace=arch_prctl gdb --args ./jvm_launcher
./jvm_launcher
