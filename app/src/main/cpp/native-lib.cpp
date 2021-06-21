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

JNIEXPORT void JNICALL
RTNS_ThrowByName(JNIEnv *env, const char *name, const char *msg) {
    jclass cls = env->FindClass(name);

    if (cls != 0) /* Otherwise an exception has already been thrown */
        env->ThrowNew(cls, msg);
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
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_runTest(JNIEnv *env, jobject) {
    std::string hello = "Hello from C++";
    const char *argv[] = {"PN", "336", "",
                          "7e8224ffaec64a2dac57b5d3e25f3953",
                          "007eJxTYIhtuZm5xnnacUfPVUldMqH1ynLLIxxjN5aueOnGM6vnwwQFBvNUCyMjk7S0xNRkM5NEo5TEZFPzJNMU41Qj0zRjS1PjsDfnEw58ZmAo/LiBlZmBkQECmKE0CxADAJDnHrk="};
    testRtns(5, argv);
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_createContext(JNIEnv *env, jobject helper,
                                                                     jstring app_id,
                                                                     jstring token) {
    const char *native_app_id = env->GetStringUTFChars(app_id, 0);
    const char *native_token = env->GetStringUTFChars(token, 0);

    agora_socket_context *ctx = create_context(native_app_id, native_token);

    env->ReleaseStringUTFChars(app_id, native_app_id);
    env->ReleaseStringUTFChars(token, native_token);

    return reinterpret_cast<jlong>(ctx);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_connect(JNIEnv *env, jobject, jlong context,
                                                               jint chain_id,
                                                               jobject fileDescriptor) {
    char connection_id[64] = {};
    int fd = agora_socket_tcp_connect(reinterpret_cast<agora_socket_context *>(context),
                                      chain_id,
                                      0,
                                      "", 0, connection_id);
    LOGI("[rtns-tcp-demo] connection_id %s\n", connection_id);
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
    unsigned int chainId;

    if (argc != 5) {
        LOGE("Usage: ./rtns chan_id name token\n");
        return -1;
    }

    chainId = atoi(argv[1]);
    LOGE("chan_id: %u, name: %s, appid: %s, token: %s\n", chainId, argv[2], argv[3], argv[4]);

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
    int fd_0 = agora_socket_tcp_connect(ctx, chainId, 0, "", 0, connection_id);
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

#ifndef MAX_BUFFER_LEN
#define MAX_BUFFER_LEN 8192
#define MAX_HEAP_BUFFER_LEN 65536
#endif

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_socketRead(JNIEnv *env, jobject thiz,
                                                                  jobject fdObj,
                                                                  jbyteArray data,
                                                                  jint off,
                                                                  jint len) {
    LOGE("[rtns-tcp-demo] READ call");

    if (fdObj == NULL) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return -1;
    }
    jint fd = getFD(env, fdObj);
    if (fd == -1) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return -1;
    }

    char BUF[MAX_BUFFER_LEN];
    char *bufP;

    if (len > MAX_BUFFER_LEN) {
        if (len > MAX_HEAP_BUFFER_LEN) {
            len = MAX_HEAP_BUFFER_LEN;
        }
        bufP = (char *) malloc((size_t) len);
        if (bufP == NULL) {
            bufP = BUF;
            len = MAX_BUFFER_LEN;
        }
    } else {
        bufP = BUF;
    }

    jint nread = read(fd, bufP, len);
    if (nread <= 0) {
        if (nread < 0) {
            switch (errno) {
                case ECONNRESET:
                case EPIPE:
                    RTNS_ThrowByName(env, "sun/net/ConnectionResetException", "Connection reset");
                    break;

                case EBADF:
                    RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
                    break;

                case EINTR:
                    RTNS_ThrowByName(env, "java/io/InterruptedIOException",
                                     "Operation interrupted");
                    break;
                default:
                    // TODO JNU_ThrowByNameWithMessageAndLastError
                    RTNS_ThrowByName(env, "java/net/SocketException", "Read failed");
            }
        }
    } else {
        env->SetByteArrayRegion(data, off, nread, (jbyte *) bufP);
    }

    if (bufP != BUF) {
        free(bufP);
    }
    return nread;
}

#define min(a, b) ((a) < (b) ? (a) : (b))

extern "C" JNIEXPORT void JNICALL
Java_com_herewhite_sdk_nativesocket_NativeSocketHelper_socketWrite(
        JNIEnv *env,
        jobject helper,
        jobject fdObj,
        jbyteArray data,
        jint off,
        jint len) {
    LOGE("[rtns-tcp-demo] write call");

    char *bufP;
    char BUF[MAX_BUFFER_LEN];
    int buflen;
    int fd;

    if (fdObj == NULL) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return;
    } else {
        fd = getFD(env, fdObj);
        if (fd == -1) {
            RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
            return;
        }
    }

    if (len <= MAX_BUFFER_LEN) {
        bufP = BUF;
        buflen = MAX_BUFFER_LEN;
    } else {
        buflen = min(MAX_HEAP_BUFFER_LEN, len);
        bufP = (char *) malloc((size_t) buflen);

        /* if heap exhausted resort to stack buffer */
        if (bufP == NULL) {
            bufP = BUF;
            buflen = MAX_BUFFER_LEN;
        }
    }

    while (len > 0) {
        int chunkLen = min(buflen, len);
        int llen = chunkLen;
        env->GetByteArrayRegion(data, off, chunkLen, (jbyte *) bufP);

        if (env->ExceptionCheck()) {
            break;
        } else {
            int loff = 0;
            while (llen > 0) {
                int n = write(fd, bufP + loff, llen);
                if (n > 0) {
                    llen -= n;
                    loff += n;
                    continue;
                }
                // TODO
                RTNS_ThrowByName(env, "java/net/SocketException", "Write failed");
                if (bufP != BUF) {
                    free(bufP);
                }
                return;
            }
            len -= chunkLen;
            off += chunkLen;
        }
    }

    if (bufP != BUF) {
        free(bufP);
    }
}
