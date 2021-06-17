package com.herewhite.sdk.nativesocket;

import android.util.Log;

import java.io.FileDescriptor;
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
    private FileDescriptor fileDescriptor;
    private int timeout;
    private InputStream inputStream;
    private OutputStream outputStream;

    public RtnsSocketImpl(NativeSocketHelper helper) {
        this.nativeSocketHelper = helper;
    }

    @Override
    protected void create(boolean stream) throws IOException {
        context = nativeSocketHelper.createContext();
        fileDescriptor = new FileDescriptor();
    }

    @Override
    protected void connect(String host, int port) throws IOException {
        int ret = nativeSocketHelper.connect(context, 117, fileDescriptor);
        if (ret > 0) {
            Log.e("NativeSocket", "connect success");
        }
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
        return 0;
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
