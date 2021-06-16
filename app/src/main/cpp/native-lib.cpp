#include <jni.h>
#include <string>
#include <unistd.h>
#include <thread>

#include <iostream>

#include "IAgoraRtnsService.h"
#include "agora_socket.h"

#include  <android/log.h>

// log标签
#define TAG  "NativeSocket"

// 定义info信息
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)

// 定义debug信息
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

// 定义error信息
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

static const int BUF_SIZE = 65536;
static unsigned char buf[BUF_SIZE];
//static char data[
//        1024 * 1024] = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: Close\r\n\r\n";

void token_will_expire_event(const char *token) {
    LOGE("[rtns-tcp-demo] token will expire %s\n", token);
}

void network_change(int type) {
    LOGE("[rtns-tcp-demo] network type change to %d\n", type);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_herewhite_sdk_nativesocket_MainActivity_stringFromJNI(JNIEnv *env, jobject) {
    std::string hello = "Hello from C++";
    std::string error = "Hello from Error Web";

//    char data[1024] = "GET / HTTP/1.1\n"
//                      "Host: 121.196.198.83\n"
//                      "Connection: keep-alive\n";

    char data[1024] = "GET /index.html HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: Close\r\n\r\n";
    char connectId[1024] = {0};
    agora_socket_conf cfg;
//    cfg.app_id = "7e8224ffaec64a2dac57b5d3e25f3953";
//    cfg.token = "007eJxTYFjGaP5wvaCEPuuaW5qbBVcYV/Vavrow/6OWtszRUqGGzV8VGMxTLYyMTNLSElOTzUwSjVISk03Nk0xTjFONTNOMLU2NX0adTDjwmYHh2WErBmYGRgYIYIbSLEAMAFLDHfw=";
    cfg.app_id = "80e54398fed94ae8a010acf782f569b7";
    cfg.token = "007eJxTYPgnr7E2TmzN7IANjgaBLuf7oy9uWXvoOZNUR+3O055cKqkKDBYGqaYmxpYWaakpliaJqRaJBoYGiclp5hZGaaZmlknm+YknEw58ZmCoXbGThZmBkQECmKE0CxADAFknHXY=";
    cfg.log_file_path = "/mnt/sdcard/Download/rtns_test.log";
    cfg.file_size_in_kb = 1024;
    cfg.log_level = 0x0001;
    cfg.event_handlers.on_token_will_expire = token_will_expire_event;
    cfg.event_handlers.on_network_type_changed = network_change;
    agora_socket_context *ctx = agora_socket_context_new(&cfg);
    if (ctx) {
        LOGE("[rtns-tcp-demo] agora socket context Initialized %p\n", ctx);
    } else {
        LOGE("[rtns-tcp-demo] agora socket context cannot created");
        exit(1);
    }

    int address_size = 1;
    const char **addresses = (const char **) malloc(address_size * sizeof(char *));
    addresses[0] = "114.236.137.40";
    agora_socket_context_set_ap_addresses(ctx, addresses, address_size, 8443);
    free(addresses);
    addresses = NULL;

    // int fd_0 = agora_socket_tcp_connect(ctx, 331, 0, "", 0, connectId);
    int fd_0 = agora_socket_tcp_connect(ctx, 170, 0, "", 0, connectId);

    LOGE("[rtns-tcp-demo] agora socket  connectId %s\n", connectId);
    int recv_length = 0;
    size_t send_length = strlen(data);
    size_t res = write(fd_0, data, send_length);
    if (res < 0) {
        LOGE("[rtns-tcp-demo] wrapper fd write operation failed, errno: %d\n", errno);
        close(fd_0);
        return env->NewStringUTF(error.c_str());
    }
    LOGE("[rtns-tcp-demo] wrapper fd write %d bytes\n", (int) res);

    while (recv_length < 500) {
        res = read(fd_0, &buf[recv_length], BUF_SIZE);
        if (res < 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                LOGE("[rtns-tcp-demo] wrapper fd read failed, errno: %d, recv_length: %d\n",
                     errno, recv_length);
                close(fd_0);
                return env->NewStringUTF(hello.c_str());
            }
        }
        if (res == 0) {
            break;
        }
        recv_length += res;
        LOGE("[rtns-tcp-demo] recv_length: %d, res: %u\n", recv_length, (int) res);
    }

    LOGE("[rtns-tcp-demo] job done with recv_length: %d\n", recv_length);
    LOGE("[rtns-tcp-demo] job done with buf: %s\n", buf);
    close(fd_0);

    agora_socket_context_free(ctx);

    return env->NewStringUTF(hello.c_str());
}

/**
 * jni调用java方法
 * @param env
 * @param thiz
 * @return
 */
//jstring stringFromJava(JNIEnv *env, jobject /* this */thiz) {
//    jclass cls = env->GetObjectClass(thiz);
//    jmethodID methodId = env->GetMethodID(cls, "stringFromJava", "()Ljava/lang/String;");
//    jobject ob = env->AllocObject(cls);
//    jstring str = static_cast<jstring>(env->CallObjectMethod(ob, methodId));
//    return str;
//}
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_hai_cmake_MainActivity_callJni(JNIEnv *env, jobject instance) {
//    return stringFromJava(env, instance);
//}