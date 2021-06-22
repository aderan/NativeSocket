package com.herewhite.sdk.rtns;

/**
 * @author fenglibin
 */
public class RtnsSocketConf {
    /**
     * 产品标识
     */
    String appId;
    /**
     * 链路标识
     */
    String token;
    /**
     * 链路标识
     */
    int chainId;

    public RtnsSocketConf(String appId, String token, int chainId) {
        this.appId = appId;
        this.token = token;
        this.chainId = chainId;
    }
}
