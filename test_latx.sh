#for latx
export NBL_LOG=0
export LATX_KZT=1
export LATX_AOT=0
export LATX_CLOSE_PARALLEL=1
export LATX_DUMP=00000

## test kzt
gcc -fPIC -shared -o local/libkzt.so kzt.c

export JAVA_HOME=/home/sunguoyun/jdk8u-ls/build/linux-loongarch64-normal-server-release/images/j2sdk-image/jre
export X64_JAVA_HOME=/home/sunguoyun/x86_jdk
#export LD_LIBRARY_PATH=$X64_JAVA_HOME/lib:$X64_JAVA_HOME/lib/amd64/server:${JAVA_HOME}/lib/loongarch64/server:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=$X64_JAVA_HOME/lib:$X64_JAVA_HOME/lib/amd64/server:${JAVA_HOME}/lib/loongarch64/server:/home/sunguoyun/kzt_java/local:./x86:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=${JAVA_HOME}/lib/loongarch64/server:/home/sunguoyun/kzt_java/local:./x86:$LD_LIBRARY_PATH
clear; gdb --args ~/lat/build64/latx-x86_64 ./jvm_launcher


