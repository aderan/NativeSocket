package com.herewhite.sdk.nativesocket;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.SocketImpl;

/**
 * @author fenglibin
 */
public class RtnsSocketImpl extends SocketImpl {
    NativeSocketHelper nativeSocketHelper;

    private FileDescriptor fileDescriptor;
    private int timeout;

    private InputStream inputStream;
    private OutputStream outputStream;

    public RtnsSocketImpl(NativeSocketHelper helper) {
        this.nativeSocketHelper = helper;

        fileDescriptor = new FileDescriptor();
    }

    @Override
    protected void create(boolean stream) throws IOException {
        nativeSocketHelper.ensureContextCreated();
    }

    @Override
    protected void connect(String host, int port) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    protected void connect(InetAddress address, int port) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    protected void connect(SocketAddress address, int timeout) throws IOException {
        // map address to chainId
        int chainId = 0;
        if (address instanceof InetSocketAddress) {
            InetSocketAddress isa = (InetSocketAddress) address;
            if ("121.196.198.83".equals(isa.getAddress().getHostAddress())) {
                chainId = 336;
            } else if ("echo.websocket.org".equals(isa.getAddress().getHostName())) {
                chainId = 334;
            }
        }
        nativeSocketHelper.connect(nativeSocketHelper.getSocketContext(), chainId, fileDescriptor);
    }

    @Override
    protected void bind(InetAddress host, int port) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    protected void listen(int backlog) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    protected void accept(SocketImpl s) throws IOException {
        throw new UnsupportedOperationException();
    }

    @Override
    protected InputStream getInputStream() throws IOException {
        if (inputStream == null) {
            inputStream = new RtnsSocketInputStream(this);
        }
        return inputStream;
    }

    @Override
    protected OutputStream getOutputStream() throws IOException {
        if (outputStream == null) {
            outputStream = new RtnsSocketOutputStream(this);
        }
        return outputStream;
    }

    @Override
    protected int available() throws IOException {
        try {
            int available = inputStream.available();
            if (available < 0) {
                available = 0;
            }
            return available;
        } catch (IOException exception) {
            throw exception;
        }
    }

    @Override
    protected void close() throws IOException {

    }

    @Override
    protected void sendUrgentData(int data) throws IOException {

    }

    @Override
    public void setOption(int opt, Object value) throws SocketException {
        if (opt == SO_TIMEOUT) {
            timeout = (Integer) value;
        }
    }

    @Override
    public Object getOption(int optID) throws SocketException {
        return null;
    }

    public int getTimeout() {
        return timeout;
    }

    public FileDescriptor getFD() {
        return fileDescriptor;
    }
}
