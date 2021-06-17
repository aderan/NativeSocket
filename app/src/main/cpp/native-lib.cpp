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

agora_socket_context *create_context(const char *app_id, const char *token);

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

jfieldID file_descriptor_id;

jfieldID FileDescriptor_descriptor(JNIEnv *env) {
    if (!file_descriptor_id) {
        jclass fdclass = env->FindClass("java/io/FileDescriptor");
        file_descriptor_id = env->GetFieldID(fdclass, "descriptor", "I");
    }
    return file_descriptor_id;
}

JNIEXPORT int getFD(JNIEnv *env, jobject fileDescriptor) {
    return env->GetIntField(fileDescriptor, FileDescriptor_descriptor(env));
}

JNIEXPORT void setFD(JNIEnv *env, jobject fileDescriptor, int fd) {
    env->SetIntField(fileDescriptor, FileDescriptor_descriptor(env), fd);
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_stringFromJNI(JNIEnv *env, jobject) {
    std::string hello = "Hello from C++";
//    const char *argv[] = {"PN", "335", "",
//                          "80e54398fed94ae8a010acf782f569b7",
//                          "007eJxTYNgy9/k1n/CE6413J95+xrsgacGWwKM9cR+8vlzu+HDBo6hLgcHCINXUxNjSIi01xdIkMdUi0cDQIDE5zdzCKM3UzDLJfBHv6YQDnxkYqv4ksjIzMDJAADOUZgFiAOTjIhE="};
    const char *argv[] = {"PN", "331", "",
                          "7e8224ffaec64a2dac57b5d3e25f3953",
                          "007eJxTYPjvH2Fs/VUn09zPMyOi8qmrt5tVWxLz/aXC/T92TTSblqzAYJ5qYWRkkpaWmJpsZpJolJKYbGqeZJpinGpkmmZsaWpsJ3A64cBnBobWEFVWZgZGBghghtIsQAwAA9obvg=="};
    testRtns(5, argv);
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_createContext(JNIEnv *env, jobject) {
    agora_socket_context *ctx = create_context("80e54398fed94ae8a010acf782f569b7",
                                               "007eJxTYChxUeO/+DKyaEbfjfNn+r8c28MirvBld9zPnpslXBM1m/8qMFgYpJqaGFtapKWmWJokplokGhgaJCanmVsYpZmaWSaZL95/KuHAZwaG7W2rmJkZGBkggBlKswAxAEJwILc=");
    // set_ap_addressed(ctx);
    return reinterpret_cast<jlong>(ctx);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_connect(JNIEnv *env, jobject, jlong context,
                                                               jint chain_id,
                                                               jobject fileDescriptor) {
    char connection_id[64] = {};
    int fd = agora_socket_tcp_connect(reinterpret_cast<agora_socket_context *>(context), chain_id,
                                      0,
                                      "", 0, connection_id);
    setFD(env, fileDescriptor, fd);
    return fd;
}


agora_socket_context *create_context(const char *app_id, const char *token) {
    agora_socket_conf cfg;
    cfg.app_id = app_id;
    cfg.token = token;
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
    }
    return ctx;
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

    agora_socket_context *ctx = create_context(argv[3], argv[4]);
    if (ctx) {
        LOGE("[rtns-tcp-demo] agora socket context Initialized %p\n", ctx);
    } else {
        LOGE("[rtns-tcp-demo] agora socket context cannot created");
        return -2;
    }

    // set_ap_addressed(ctx);

    struct timespec send_time;
    clock_gettime(CLOCK_REALTIME, &send_time);

    char connection_id[64] = {};
    int fd_0 = agora_socket_tcp_connect(ctx, chan_id, 0, "", 0, connection_id);
    if (fd_0 < 0) {
        LOGE("[rtns-tcp-demo] connect server failed, errno: %d\n", fd_0);
    }
    LOGE("[rtns-tcp-demo] connection_id %s\n", connection_id);

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

#ifndef MAX_BUFFER_LEN
#define MAX_BUFFER_LEN 65536
#define MAX_HEAP_BUFFER_LEN 131072
#endif

extern "C"
JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_socketRead(JNIEnv *env, jobject thiz,
                                                                  jobject fdObj,
                                                                  jbyteArray data,
                                                                  jint off,
                                                                  jint len,
                                                                  jint timeout) {
    char BUF[MAX_BUFFER_LEN];
    char *bufP;
    jint fd, newfd, nread;

    if (!fdObj) {
        return -1;
    }
    fd = getFD(env, fdObj);
    if (fd == -1) {
        return -2;
    }

    if (len <= MAX_BUFFER_LEN) {
        bufP = BUF;
    } else {
        if (len > MAX_HEAP_BUFFER_LEN) {
            len = MAX_HEAP_BUFFER_LEN;
        }
        bufP = (char *) malloc((size_t) len);
        if (bufP == NULL) {
            bufP = BUF;
            len = MAX_BUFFER_LEN;
        }
    }

    nread = read(fd, bufP, len);
    if (nread > 0) {
        env->SetByteArrayRegion(data, off, nread, (jbyte *) bufP);
    } else {
        return -3;
    }
    if (bufP != BUF) {
        free(bufP);
    }
    return nread;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_socketWrite(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jobject file_descriptor,
                                                                   jbyteArray b,
                                                                   jint off,
                                                                   jint len) {
    int fd = getFD(env, file_descriptor);

    sprintf(data, "GET /%s HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: Close\r\n\r\n", "");
    size_t send_length = strlen(data);
    write(fd, data, send_length);
}