package com.herewhite.sdk.nativesocket;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

import javax.net.SocketFactory;

/**
 * @author fenglibin
 */
public class RtnsSocketFactory extends SocketFactory {
    NativeSocketHelper helper = new NativeSocketHelper();
    SocketFactory factory = SocketFactory.getDefault();

    @Override
    public Socket createSocket(String host, int port) throws IOException, UnknownHostException {
        if (host.equals("121.196.198.83")) {
            return new RtnsSocket(new RtnsSocketImpl(helper));
        } else {
            return factory.createSocket(host, port);
        }
    }

    @Override
    public Socket createSocket(String host, int port, InetAddress localHost, int localPort) throws IOException, UnknownHostException {
        return null;
    }

    @Override
    public Socket createSocket(InetAddress host, int port) throws IOException {
        return null;
    }

    @Override
    public Socket createSocket(InetAddress address, int port, InetAddress localAddress, int localPort) throws IOException {
        return null;
    }
}
