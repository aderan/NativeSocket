package com.herewhite.sdk.nativesocket;

import java.net.Socket;
import java.net.SocketException;
import java.net.SocketImpl;

/**
 * @author fenglibin
 */
public class RtnsSocket extends Socket {
    public RtnsSocket(SocketImpl impl) throws SocketException {
        super(impl);
    }
}
