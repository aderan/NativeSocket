package com.herewhite.sdk.rtns;

import java.net.SocketAddress;

/**
 * @author fenglibin
 */
abstract public class RtnsSocketConfProvider {
    final String appId;
    final String token;

    public RtnsSocketConfProvider(String appId, String token) {
        this.appId = appId;
        this.token = token;
    }

    public abstract int getChainIdByAddress(SocketAddress address);
}
