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

    public native String stringFromJNI();

    public native long createContext();

    public native int connect(long context,
                              int chainId,
                              FileDescriptor fileDescriptor);

    public native int socketRead(FileDescriptor fd,
                                 byte b[],
                                 int off,
                                 int len,
                                 int timeout);

    public native void socketWrite(FileDescriptor fd,
                                   byte[] b,
                                   int off,
                                   int len) throws IOException;
}
