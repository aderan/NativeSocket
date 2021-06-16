#include <jni.h>
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

int testRtns(int argc, const char *argv[]);

void set_ap_addressed(agora_socket_context *ctx);

void token_will_expire_event(const char *token) {
    LOGE("[rtns-tcp-demo] token will expire %s\n", token);
}

void network_change(int type) {
    LOGE("[rtns-tcp-demo] network type change to %d\n", type);
}

unsigned long time_diff(struct timespec *curr, struct timespec *prev) {
    time_t old_sec = prev->tv_sec;
    time_t new_sec = curr->tv_sec;

    long old_nsec = prev->tv_nsec;
    long new_nsec = curr->tv_nsec;

    long diff_msec;

    if (new_sec < old_sec) {
        diff_msec = 0;
    } else if (new_sec == old_sec) {
        if (new_nsec <= old_nsec) {
            diff_msec = 0;
        } else {
            diff_msec = (new_nsec - old_nsec) / 1000000;
        }
    } else {
        diff_msec = (new_sec - old_sec) * 1000;
        diff_msec += (new_nsec - old_nsec) / 1000000;
    }

    if (diff_msec < 0) {
        return 0;
    }

    return (unsigned long) diff_msec;
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_herewhite_sdk_nativesocket_MainActivity_stringFromJNI(JNIEnv *env, jobject) {
    std::string hello = "Hello from C++";
    const char *argv[] = {"PN", "170", "",
                          "80e54398fed94ae8a010acf782f569b7",
                          "007eJxTYAhlXLls3tbkUJe3u5eIL76pkxsbnCL1submi4N/q9eydl5XYLAwSDU1Mba0SEtNsTRJTLVINDA0SExOM7cwSjM1s0wyd+s4mXDgMwNDn6YlKzMDIwMEMENpFiAGAOysHso="};
//    const char *argv[] = {"PN", "331", "",
//                          "7e8224ffaec64a2dac57b5d3e25f3953",
//                          "007eJxTYFh/klc5zF4pLVx6z+V+5h0lqodit6Q4Mfy7PGnrv11TGXwVGMxTLYyMTNLSElOTzUwSjVISk03Nk0xTjFONTNOMLU2NWfpOJhz4zMDwgO0qCzMDIwMEMENpFiAGAFCtHXM="};
    testRtns(5, argv);
    return env->NewStringUTF(hello.c_str());
}


static const int BUF_SIZE = 65536;
static char buf[BUF_SIZE];
static char data[1024 * 1024];

int testRtns(int argc, const char *argv[]) {
    unsigned int chan_id;

    if (argc != 5) {
        LOGE("Usage: ./rtns chan_id name token\n");
        return -1;
    }

    chan_id = atoi(argv[1]);
    LOGE("chan_id: %u, name: %s, appid: %s, token: %s\n", chan_id, argv[2], argv[3], argv[4]);

    sprintf(data, "GET /%s HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: Close\r\n\r\n", argv[2]);

    agora_socket_conf cfg;
    cfg.app_id = argv[3];
    cfg.token = argv[4];
    cfg.log_file_path = "/mnt/sdcard/test.log";
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
    // set_ap_addressed(ctx);

    struct timespec send_time;
    clock_gettime(CLOCK_REALTIME, &send_time);

    char connection_id[64] = {};
    int fd_0 = agora_socket_tcp_connect(ctx, chan_id, 0, "", 0, connection_id);
    if (fd_0 < 0) {
        LOGE("[rtns-tcp-demo] connect server failed, errno: %d\n", fd_0);
    }
    int recv_length = 0;

    size_t send_length = strlen(data);
    size_t res = write(fd_0, data, send_length);
    if (res < 0) {
        LOGE("[rtns-tcp-demo] wrapper fd write operation failed, errno: %d\n", errno);
        close(fd_0);
        return 1;
    }
    LOGE("[rtns-tcp-demo] wrapper fd write %d bytes\n", (int) res);

    int header_len = 0;
    int body_len = 0;
    int content_len = 0;

    while (true) {
        res = read(fd_0, buf + recv_length, BUF_SIZE - recv_length);
        if (res < 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                LOGE("[rtns-tcp-demo] wrapper fd read failed, errno: %d, recv_length: %d\n",
                     errno, recv_length);
                close(fd_0);
                return 1;
            }
        }
        if (res == 0) {
            break;
        }
        recv_length += res;

        char *p = strstr(buf, "\r\n\r\n");
        if (p == NULL) {
            if (recv_length >= BUF_SIZE) {
                LOGE("error: too big http header\n");
                close(fd_0);
                return 1;
            }

            continue;
        }

        header_len = (p - buf) + 4;

        p = strcasestr(buf, "content-length:");
        if (p == NULL) {
            LOGE("error: can not find content-length\n");
            close(fd_0);
            return 1;
        }

        p += sizeof("content-length:") - 1;

        while (*p == ' ') {
            p++;
        }

        while (isdigit(*p)) {
            content_len *= 10;
            content_len += *p - '0';
            p++;
        }

        body_len = recv_length - header_len;
        LOGE("[rtns-tcp-demo] recv_length: %d, res: %u\n", recv_length, (int) res);
        break;
    }

    while (body_len < content_len) {
        res = read(fd_0, buf, BUF_SIZE);
        if (res < 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                LOGE("[rtns-tcp-demo] wrapper fd read failed, errno: %d, recv_length: %d\n",
                     errno, recv_length);
                close(fd_0);
                return 1;
            }
        }
        if (res == 0) {
            break;
        }

        recv_length += res;
        body_len += res;
    }

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    unsigned long diff = time_diff(&now, &send_time);

    if (body_len != content_len) {
        LOGE("error: body_len: %d, content_len: %d\n", body_len, content_len);
    }

    LOGE("[rtns-tcp-demo] job done with recv_body: %s\n", buf);
    LOGE("[rtns-tcp-demo] job done with recv_length: %d, total time: %lums\n", recv_length, diff);
    close(fd_0);

    agora_socket_context_free(ctx);
    return 0;
}

void set_ap_addressed(agora_socket_context *ctx) {
    int address_size = 1;
    const char **addresses = (const char **) malloc(address_size * sizeof(char *));
    addresses[0] = "114.236.137.40";
    agora_socket_context_set_ap_addresses(ctx, addresses, address_size, 8443);
    free(addresses);
    addresses = NULL;
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