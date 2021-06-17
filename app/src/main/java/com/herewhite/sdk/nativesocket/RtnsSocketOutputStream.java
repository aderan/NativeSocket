package com.herewhite.sdk.nativesocket;

import java.io.FileOutputStream;
import java.io.IOException;

/**
 * @author fenglibin
 */
public class RtnsSocketOutputStream extends FileOutputStream {
    private final RtnsSocketImpl impl;

    public RtnsSocketOutputStream(RtnsSocketImpl impl) {
        super(impl.getFD());
        this.impl = impl;
    }

    @Override
    public void write(byte[] b, int off, int len) throws IOException {
        impl.nativeSocketHelper.socketWrite(getFD(), b, off, len);
    }
}
