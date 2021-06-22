package com.herewhite.sdk.rtns;

import java.net.Socket;
import java.net.SocketException;
import java.net.SocketImpl;

/**
 * @author fenglibin
 */
class RtnsSocket extends Socket {
    public RtnsSocket(SocketImpl impl) throws SocketException {
        super(impl);
    }
}
