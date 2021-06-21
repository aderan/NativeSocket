package com.herewhite.sdk.nativesocket;

import java.io.FileDescriptor;
import java.io.IOException;

/**
 * @author fenglibin
 */
public class NativeSocketHelper {
    static {
        System.loadLibrary("native-lib");
    }

    private static final int chainId = 336;
    private static final String appId = "7e8224ffaec64a2dac57b5d3e25f3953";
    public static final String token = "007eJxTYIhtuZm5xnnacUfPVUldMqH1ynLLIxxjN5aueOnGM6vnwwQFBvNUCyMjk7S0xNRkM5NEo5TEZFPzJNMU41Qj0zRjS1PjsDfnEw58ZmAo/LiBlZmBkQECmKE0CxADAJDnHrk=";

    private long socketContext;

    public synchronized long ensureContextCreated() {
        if (socketContext == 0) {
            socketContext = createContext(appId, token);
        }
        return socketContext;
    }

    public long getSocketContext() {
        return socketContext;
    }

    public native String runTest();

    public native long createContext(String appId, String token);

    public native int connect(long context,
                              int chainId,
                              FileDescriptor fileDescriptor);

    public native int socketRead(FileDescriptor fd,
                                 byte[] data,
                                 int off,
                                 int len);

    public native void socketWrite(FileDescriptor fd,
                                   byte[] data,
                                   int off,
                                   int len) throws IOException;
}
