// HelloWorld.java
class NativeTest extends Thread {
    private int threadId;
    // libnso
    public native int add(int a, int b);
    public native int getGlobal();
    public native int getTls();

    // libnso2
    public native int add2(int a, int b);
    public native int getGlobal2();
    public native int getTls2();
    static {
        try {
            System.loadLibrary("nso");
            System.loadLibrary("nso2");
        } catch (UnsatisfiedLinkError e) {
            System.out.println("could not load native lib: " + e);
        }
    }
    public NativeTest(int threadId) {
        this.threadId = threadId;
    }
    @Override
    public void run() {
        int tls = getTls();
        int result = add(1, threadId);
        result = add(1, result);
        result = add(1, result);
        tls = getTls();
        System.out.println("T" + threadId + ": nso (1+" + threadId + ")+2 = " + result + " global=" + getGlobal() + " tls=" + tls);

        tls = getTls2();
        result = add2(1, threadId);
        result = add2(1, result);
        result = add2(1, result);
        tls = getTls2();
        System.out.println("T" + threadId + ": nso2 (1+" + threadId + ")+2 = " + result + " global2=" + getGlobal2() + " tls2=" + tls);
    }
}
        
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello from Java!");
        NativeTest[] threads = {
            new NativeTest(1),
            new NativeTest(2),
            //new NativeTest(3),
            //new NativeTest(4),
            //new NativeTest(5)
        };
        for (NativeTest thread : threads) {
            thread.start();
        }
        for (NativeTest thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        //HelloWorld hw = new HelloWorld();         
        System.out.println("end");

    }
}
