package com.herewhite.sdk.nativesocket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.SocketImpl;

/**
 * @author fenglibin
 */
public class RtnsSocketImpl extends SocketImpl {
    private static final int chainId = 117;

    NativeSocketHelper nativeSocketHelper;

    private long context;

    public RtnsSocketImpl(NativeSocketHelper helper) {
        this.nativeSocketHelper = helper;
    }

    @Override
    protected void create(boolean stream) throws IOException {
        context = nativeSocketHelper.createContext();
    }

    @Override
    protected void connect(String host, int port) throws IOException {
        nativeSocketHelper.connect(context, 117);
    }

    @Override
    protected void connect(InetAddress address, int port) throws IOException {

    }

    @Override
    protected void connect(SocketAddress address, int timeout) throws IOException {

    }

    @Override
    protected void bind(InetAddress host, int port) throws IOException {

    }

    @Override
    protected void listen(int backlog) throws IOException {

    }

    @Override
    protected void accept(SocketImpl s) throws IOException {

    }

    @Override
    protected InputStream getInputStream() throws IOException {
        return null;
    }

    @Override
    protected OutputStream getOutputStream() throws IOException {
        return null;
    }

    @Override
    protected int available() throws IOException {
        return 0;
    }

    @Override
    protected void close() throws IOException {

    }

    @Override
    protected void sendUrgentData(int data) throws IOException {

    }

    @Override
    public void setOption(int optID, Object value) throws SocketException {

    }

    @Override
    public Object getOption(int optID) throws SocketException {
        return null;
    }
}
