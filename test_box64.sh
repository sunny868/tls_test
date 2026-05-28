#for box64
export BOX64_LOG=1
#export JAVA_HOME=/home/sunguoyun/jdk-ls-sandbox/build/linux-loongarch64-server-fastdebug/jdk
#export LD_LIBRARY_PATH=${JAVA_HOME}/lib/server:/home/sunguoyun/box64/x64lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/home/sunguoyun/box64/x64lib:$LD_LIBRARY_PATH
clear;  ~/box64/build/box64 ./jvm_launcher
