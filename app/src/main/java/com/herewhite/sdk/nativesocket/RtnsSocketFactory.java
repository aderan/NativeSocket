package com.herewhite.sdk.nativesocket;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;

import javax.net.SocketFactory;

/**
 * @author fenglibin
 */
public class RtnsSocketFactory extends SocketFactory {
    NativeSocketHelper helper = new NativeSocketHelper();

    @Override
    public Socket createSocket() throws IOException {
        return new RtnsSocket(new RtnsSocketImpl(helper));
    }

    @Override
    public Socket createSocket(String host, int port) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    public Socket createSocket(String host, int port, InetAddress localHost, int localPort) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    public Socket createSocket(InetAddress host, int port) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    public Socket createSocket(InetAddress address, int port, InetAddress localAddress, int localPort) throws IOException {
        throw new UnsupportedOperationException();
    }
}
