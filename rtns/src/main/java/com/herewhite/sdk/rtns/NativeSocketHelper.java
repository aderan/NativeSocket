package com.herewhite.sdk.rtns;

import java.io.FileDescriptor;
import java.io.IOException;
import java.net.SocketAddress;

/**
 * @author fenglibin
 */
public class NativeSocketHelper {
    static {
        System.loadLibrary("native-socket-lib");
    }

    private final RtnsSocketConfProvider confProvider;
    private long socketContext;

    public NativeSocketHelper(RtnsSocketConf conf) {
        this(new DefaultConfProvider(conf));
    }

    public NativeSocketHelper(RtnsSocketConfProvider confProvider) {
        this.confProvider = confProvider;
    }

    public synchronized long ensureContextCreated() {
        if (socketContext == 0) {
            socketContext = createContext(confProvider.appId, confProvider.token);
        }
        return socketContext;
    }

    public long connect(SocketAddress address, int timeout, FileDescriptor fd) {
        return connect(socketContext, confProvider.getChainIdByAddress(address), fd);
    }

    public native long createContext(String appId, String token);

    public native int connect(long context,
                              int chainId,
                              FileDescriptor fileDescriptor);

    public native int close(FileDescriptor fileDescriptor);


    public native int socketRead(FileDescriptor fd,
                                 byte[] data,
                                 int off,
                                 int len);

    public native void socketWrite(FileDescriptor fd,
                                   byte[] data,
                                   int off,
                                   int len) throws IOException;

    private static class DefaultConfProvider extends RtnsSocketConfProvider {
        private final RtnsSocketConf conf;

        DefaultConfProvider(RtnsSocketConf conf) {
            super(conf.appId, conf.token);
            this.conf = conf;
        }

        @Override
        public int getChainIdByAddress(SocketAddress address) {
            return conf.chainId;
        }
    }
}
