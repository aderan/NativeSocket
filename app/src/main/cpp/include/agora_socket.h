//
//  Agora Rtns SDK
//
//  Created by Yuan Yao in 2021-01.
//  Copyright (c) 2021 Agora IO. All rights reserved.
//

#pragma once

#include "agora_api.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#define AGORA_SOCKET_RECV  1
#define AGORA_SOCKET_SEND  2

#define AGORA_SOCKET_ERROR_TYPE_SYSTEM_ERROR -1
#define AGORA_SOCKET_ERROR_TYPE_OK  0
#define AGORA_SOCKET_ERROR_TYPE_AGAIN  11
#define AGORA_SOCKET_ERROR_TYPE_BROKEN_PIPE  32

#define AGORA_SOCKET_ERROR_TYPE_PERMISSION_DENIED  200
#define AGORA_SOCKET_ERROR_TYPE_TOKEN_EXPIRED  201
#define AGORA_SOCKET_ERROR_TYPE_IN_PROGRESS  202
#define AGORA_SOCKET_ERROR_TYPE_NOT_CONNECTED  203
#define AGORA_SOCKET_ERROR_TYPE_INTERNAL_ERROR  204
#define AGORA_SOCKET_ERROR_TYPE_MSG_TOO_BIG  205
#define AGORA_SOCKET_ERROR_TYPE_SOCKET_ALREADY_CONNECTED  206
#define AGORA_SOCKET_ERROR_TYPE_RECONNECTING_CLIENT  207
#define AGORA_SOCKET_ERROR_TYPE_INVALID_ARGUMENT  208
#define AGORA_SOCKET_ERROR_TYPE_UNKNOWN  209
#define AGORA_SOCKET_ERROR_TYPE_INVALID_APP_ID  210
#define AGORA_SOCKET_ERROR_TYPE_NOT_INITIALIZED  211

#define AGORA_API_C_SOCKET_HDL AGORA_API_C agora_socket* AGORA_CALL_C
#define AGORA_API_C_SOCKET_CTX_HDL AGORA_API_C agora_socket_context* AGORA_CALL_C

typedef void (*agora_socket_callback_func) (void* user);
typedef struct _agora_socket_context agora_socket_context;
typedef struct _agora_socket agora_socket;

/**
 * The IRtnsServiceEventHandler class, which handle the event from agora socket.
 * @ANNOTATION:TYPE:OBSERVER
 */
typedef struct _agora_socket_event_handlers {
  /**
   * Triggered when token privilege will expire
   * @param token The pointer to the token
   */
  void (*on_token_will_expire)(const char* token);
  /**
   * Triggered when network type changed
   * @param type The network type
   * - -1 unknown type
   * - 0 network is disconnected
   * - 1 LAN
   * - 2 WIFI
   * - 3 mobile 2G
   * - 4 mobile 3G
   * - 5 mobile 4G
   */
  void (*on_network_type_changed)(int type);
} agora_socket_event_handlers;

/**
 * Definition of agora socket configuration.
*/
typedef struct _agora_socket_conf {
  /**
   * The pointer to the app ID.
   */
  const char* app_id;
  /**
   * The pointer to the token.
   */
  const char* token;
  /**
   * The pointer to the log file path.
   */
  const char* log_file_path;
  /**
   * The log file size in kbytes.
   */
  int file_size_in_kb;
  /**
   * The log level
   * - 0 indication no log output
   * - 1 information level
   * - 2 warning level
   * - 4 error level
   * - 8 fatal level
   */
  int log_level;
  /**
   * The event handlers for agora socket
   */
  agora_socket_event_handlers event_handlers;
} agora_socket_conf;

/**
 * Interface to create agora socket context
 * @param cfg The configuration for the agora socket context
 * @return
 * - The pointer to agora socket context
 * - nullptr: Failure
 * @ANNOTATION:CTOR:{IRtnsService}
 */
AGORA_API_C_SOCKET_CTX_HDL agora_socket_context_new(agora_socket_conf* cfg);

/**
 * Interface to renew agora socket token
 * @param token The new token user generated to renew service
 * @param ctx The pointer to created aogra socket context
 * @return
 * - =0: Success
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_context_renew_token(agora_socket_context* ctx, const char* token);

/**
 * Interface to set agora socket access point ips
 * @param addresses Two-dimensional array of access point ips
 * @param addresses_size The size of addresses
 * @param port The port of access point ips
 * @param ctx The pointer to created aogra socket context
 * @return
 * - =0: Success
 * - <0: Failure
 */
AGORA_API_C_INT agora_socket_context_set_ap_addresses(agora_socket_context* ctx, const char** addresses, int addresses_size, int port);

/**
 * Interface to release agora socket context
 * @param ctx The pointer to created aogra socket context
 * @return 
 * - =0: Success
 * - <0: Failure 
 * @ANNOTATION:DTOR:{IRtnsService}
 */
AGORA_API_C_INT agora_socket_context_free(agora_socket_context* ctx);

/**
 * Create agora socket instance
 * @param ctx The pointer to created aogra socket context
 * @return
 * - The pointer to agora socket instance
 * - nullptr: Failure
 */
AGORA_API_C_SOCKET_HDL agora_socket_open(agora_socket_context* ctx);
/**
 * Establish connection with specific acceleration chain id
 * @param as The pointer to created agora socket instance
 * @param chainId The acceleration chain id
 * @param type The detail property information type
 *  - 0 means property is ip
 *  - 1 means property is domain
 * @param property The property of this acceleration chain
 * @param port The port of acceleration chain
 * @param connectionId Output parameter, indicate uuid for this connection
 * @return
 * - =0: Success
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_connect(agora_socket* as,
    int chainId, int type, const char* property, int port, char* connectionId);
/**
 * Release agora socket instance and close the connection
 * @param as The pointer to created agora socket instance
 * @return
 * - =0: Success
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_close(agora_socket* as);

/**
 * Read data from agora socket instance
 * @param as The pointer to created agora socket instance
 * @param buf The pointer to the buffer
 * @param len The length of buffer
 * @return
 * - >0: Read bytes
 * - =0: Indicate the connection has been closed
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_read(agora_socket* as, void *buf, int len);
/**
 * Write data with agora socket instance
 * @param as The pointer to created agora socket instance
 * @param buf The pointer to the buffer
 * @param len The length of buffer
 * @return
 * - >=0: Write bytes
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_write(agora_socket* as, void *buf, int len);

/**
 * Register callback to agora socket instance
 * @param as The pointer to created agora socket instance
 * @param type The callback function type
 *  - 1 callback will triggered when agora socket instance can be read again
 *  - 2 callback will triggered when agora socket instance can be write again
 * @param cb The callback function pointer
 * @param user The pointer to the user argument(s)
 * @return
 * - =0: Success
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_reg_callback(agora_socket* as, int type,
    agora_socket_callback_func cb, void* user);
/**
 * Unregister callback to agora socket instance
 * @param as The pointer to created agora socket instance
 * @param type The callback function type
 *  - 1 callback will triggered when agora socket instance can be read again
 *  - 2 callback will triggered when agora socket instance can be write again
 * @return
 * - =0: Success
 * - <0: Failure 
 */
AGORA_API_C_INT agora_socket_unreg_callback(agora_socket* as, int type);

/**
 * Agora socket interface for tcp connection, to instead of tcp connect invocation
 *
 * @param ctx The agora socket context created by agora_socket_context_new
 * @param chainId The acceleration chain id
 * @param type The detail property information type
 *  - 0 means property is ip
 *  - 1 means property is domain
 * @param property The property of this acceleration chain
 * @param port The port of acceleration chain
 * @return
 * - The return value can be treated as normal file descriptor which returned by tcp connect invocation
 */
AGORA_API_C_INT agora_socket_tcp_connect(agora_socket_context* ctx, int chainId, int type, const char* property, int port, char* connectionId);

#ifdef __cplusplus
}
#endif  // __cplusplus
