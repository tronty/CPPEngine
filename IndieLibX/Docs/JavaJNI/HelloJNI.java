public class HelloJNI {
    static {
        System.loadLibrary("hello");    // loads libhello.so
    }

    private native void sayHello(String name);

    public static void main(String[] args) {
        new HelloJNI().sayHello("Dave");
    }
}

