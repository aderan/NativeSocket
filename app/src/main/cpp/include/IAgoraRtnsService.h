//
//  Agora Rtns SDK
//
//  Created by Yuan Yao in 2021-01.
//  Copyright (c) 2021 Agora IO. All rights reserved.
//
#pragma once

#include <string>
#include "IAgoraLog.h"
#include "AgoraBase.h"

namespace agora {
namespace rtns {

/** Error code.
 */
enum RtnsErrorType : uint16_t {
  /** 0: No error occurs.
   */
  RTNS_ERROR_TYPE_OK = 0,
  /** 1: Resource temporarily unavailable.
   * This is a temporary condition and later calls to the same routine may complete normally.*
   */
  RTNS_ERROR_TYPE_AGAIN = 11,
  /** 32: Broken pipe.
   * A write on a pipe, socket or FIFO for which there is no process to read the data.*
   */
  RTNS_ERROR_TYPE_BROKEN_PIPE = 32,

  /** 200: No permission exists.
   * Check if the user has correct token to access rtns service.*
   */
  RTNS_ERROR_TYPE_PERMISSION_DENIED = 200,

  /** 201: The connection failed since token is expired.
   * Please destroy the old rtns sevice and create a new one if see this error.*
   */
  RTNS_ERROR_TYPE_TOKEN_EXPIRED = 201,

  /** 202: Service temporarily unavailable
   * This is a temporary condition and later calls to the same routine may complete normally.*
   */
  RTNS_ERROR_TYPE_IN_PROGRESS = 202,

  /** 203: Socket not connect
   * Please call Connect() before send or read.*
   */
  RTNS_ERROR_TYPE_NOT_CONNECTED = 203,

  /** 204: Service internal error
   * Please close the old socket and create a new socket if see this error.*
   */
  RTNS_ERROR_TYPE_INTERNAL_ERROR = 204,

  /** 205: The length of sending buffer exceed limitation
   */
  RTNS_ERROR_TYPE_MSG_TOO_BIG = 205,

  /** 206: The socket instance with specify chain id has alread been created
   * Do not connect twice.*
   */
  RTNS_ERROR_TYPE_SOCKET_ALREADY_CONNECTED = 206,

  /** 207: The connection has been reset and service is reconneting
   * This is a temporary condition and later calls to the same routine may complete normally.*
   */
  RTNS_ERROR_TYPE_RECONNECTING_CLIENT = 207,

  /** 208: The argument is invalid
   * Please check the input arguments if see this error.*
   */
  RTNS_ERROR_TYPE_INVALID_ARGUMENT = 208,

  /** 209: Unknown error
   * Please close the old socket and create a new socket if see this error.*
   */
  RTNS_ERROR_TYPE_UNKNOWN = 209,

  /** 210: AppID not set or is invalid
   * Please check the appid if see this error.*
   */
  RTNS_ERROR_TYPE_INVALID_APP_ID = 210,

  /** 211: Service is not initialized or initialize failed
   */
  RTNS_ERROR_TYPE_NOT_INITIALIZED = 211,
};
 
typedef void (*socket_callback_type)(void* user);
typedef void (*event_engine_callback_type)(int fd, short what, void* arg);

/**
 * Definition of rtns socket options.
*/
struct RtnsSocketOptions {
  const char* ip = nullptr;
  const char* domain = nullptr;
  uint16_t port = 0;
};

class RtnsSocketInterface {
 protected:
  virtual ~RtnsSocketInterface() {}

 public:
  virtual int Connect(char* connection_id, uint32_t chain_id, const RtnsSocketOptions& options) = 0;
  virtual int Close() = 0;
  virtual int SendBuffer(const char* data, uint32_t length) = 0;
  virtual int ReadBuffer(char *data, uint32_t length) = 0;
  virtual int RegisterOnWriteCallback(socket_callback_type callback = nullptr, void* user = nullptr) = 0;
  virtual int RegisterOnReadCallback(socket_callback_type callback = nullptr, void* user = nullptr) = 0;
};

class IRtnsServiceEventHandler {
 public:
  virtual ~IRtnsServiceEventHandler() {}
  virtual void OnTokenPrivilegeWillExpire(const char* token) = 0;
  virtual void OnNetworkTypeChanged(rtc::NETWORK_TYPE type) = 0;
};

struct RtnsServiceContext {
  const char* appId;
  const char* token;
  commons::LogConfig logConfig;
  IRtnsServiceEventHandler *eventHandler;
  unsigned int areaCode;
  RtnsServiceContext() : appId(nullptr),
    token(nullptr),
    eventHandler(nullptr) {}
};

class IRtnsService {
 protected:
  virtual ~IRtnsService() {}

 public:
  virtual int Initialize(const RtnsServiceContext& ctx) = 0;
  virtual int Release() = 0;
  virtual int RenewToken(const char* token) = 0;
  virtual RtnsSocketInterface *CreateSocket() = 0;
  virtual int SetApAddresses(const char** addresses, int addresses_size, uint16_t port = 0) = 0;

  virtual int OpenSocketPair(int fd[2]) = 0;
  virtual void MakeSocketNonblocking(int fd) = 0;
  virtual void* EventNew(int fd, int type, event_engine_callback_type callback, void* arg) = 0;
  virtual void EventFree(void* ev) = 0;
  virtual void EventAdd(void* ev) = 0;
  virtual void TimerEventNew(std::function<void()>&& f, uint64_t ms, bool persist = true) = 0;
  virtual void TimerEventFree() = 0;
  virtual void CloseSocketPair(int fd) = 0;

  virtual int SetLogFile(const char *filePath, unsigned int fileSize) = 0;
  virtual int SetLogFilter(unsigned int filter) = 0;
};

} /* rtns */
} /* agora */

AGORA_API agora::rtns::IRtnsService* AGORA_CALL CreateAgoraRtnsService();
