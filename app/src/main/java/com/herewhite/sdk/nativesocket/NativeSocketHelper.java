package com.herewhite.sdk.nativesocket;

/**
 * @author fenglibin
 */
public class NativeSocketHelper {
    static {
        System.loadLibrary("native-lib");
    }

    public native long createContext();

    public native int connect(long context, int chainId);
}
