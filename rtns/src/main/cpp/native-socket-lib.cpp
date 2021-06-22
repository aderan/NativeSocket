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

agora_socket_context *create_context(const char *app_id, const char *token, const char *log_path);

void token_will_expire_event(const char *token) {
    LOGE("[rtns-tcp-demo] token will expire %s\n", token);
}

void network_change(int type) {
    LOGE("[rtns-tcp-demo] network type change to %d\n", type);
}

#ifndef MAX_BUFFER_LEN
#define MAX_BUFFER_LEN 8192
#define MAX_HEAP_BUFFER_LEN 65536
#endif

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

extern "C" JNIEXPORT jlong JNICALL
Java_com_herewhite_sdk_rtns_NativeSocketHelper_createContext(JNIEnv *env, jobject helper,
                                                             jstring app_id,
                                                             jstring token) {
    const char *native_app_id = env->GetStringUTFChars(app_id, 0);
    const char *native_token = env->GetStringUTFChars(token, 0);

    agora_socket_context *ctx = create_context(native_app_id, native_token, "");

    env->ReleaseStringUTFChars(app_id, native_app_id);
    env->ReleaseStringUTFChars(token, native_token);

    return reinterpret_cast<jlong>(ctx);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_rtns_NativeSocketHelper_connect(JNIEnv *env,
                                                       jobject helper,
                                                       jlong context,
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

agora_socket_context *create_context(const char *app_id, const char *token, const char *log_path) {
    agora_socket_conf cfg;
    cfg.app_id = app_id;
    cfg.token = token;
    cfg.log_file_path = log_path;
    cfg.file_size_in_kb = 1024;
    cfg.log_level = 0x0001;
    cfg.event_handlers.on_token_will_expire = token_will_expire_event;
    cfg.event_handlers.on_network_type_changed = network_change;
    agora_socket_context *ctx = agora_socket_context_new(&cfg);
    if (ctx) {
        LOGD("[rtns-tcp-demo] agora socket context Initialized %p\n", ctx);
    } else {
        LOGE("[rtns-tcp-demo] agora socket context cannot created");
    }
    return ctx;
}

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_rtns_NativeSocketHelper_close(JNIEnv *env,
                                                     jobject helper,
                                                     jobject fdObj) {
    if (fdObj == NULL) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return -1;
    } else {
        int fd = getFD(env, fdObj);
        if (fd == -1) {
            RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
            return -1;
        }
        return close(fd);
    }
}

extern "C" JNIEXPORT jint JNICALL
Java_com_herewhite_sdk_rtns_NativeSocketHelper_socketRead(JNIEnv *env,
                                                          jobject helper,
                                                          jobject fdObj,
                                                          jbyteArray data,
                                                          jint off,
                                                          jint len) {
    LOGD("[rtns-tcp-demo] socketRead call");
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
Java_com_herewhite_sdk_rtns_NativeSocketHelper_socketWrite(
        JNIEnv *env,
        jobject helper,
        jobject fdObj,
        jbyteArray data,
        jint off,
        jint len) {
    LOGD("[rtns-tcp-demo] socketWrite call");

    if (fdObj == NULL) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return;
    }

    int fd = getFD(env, fdObj);
    if (fd == -1) {
        RTNS_ThrowByName(env, "java/net/SocketException", "Socket closed");
        return;
    }

    char *bufP;
    char BUF[MAX_BUFFER_LEN];
    int buflen;

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
