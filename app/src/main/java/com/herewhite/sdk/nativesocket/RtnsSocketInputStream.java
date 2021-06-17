package com.herewhite.sdk.nativesocket;

import java.io.FileInputStream;
import java.io.IOException;

/**
 * @author fenglibin
 */
public class RtnsSocketInputStream extends FileInputStream {
    private final RtnsSocketImpl impl;

    public RtnsSocketInputStream(RtnsSocketImpl impl) throws IOException {
        super(impl.getFD());
        this.impl = impl;
    }

    @Override
    public int read(byte[] b, int off, int len) throws IOException {
        return impl.nativeSocketHelper.socketRead(getFD(), b, off, len, impl.getTimeout());
    }
}
