/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/

#ifndef LIBWEBSOCKETS_H
#define LIBWEBSOCKETS_H
#include <libwebsockets.h>
#include "Debug.H"

#include <string>
#include <iostream>

#define LIBWEBSOCKETS_CREATECONTEXT_ERROR LIBWEBSOCKETS_ERROR_OFFSET-1

/** Debug class for the FIR class
*/
class LibWebSocketsDebug :  virtual public Debug  {
public:
    LibWebSocketsDebug(){
#ifndef NDEBUG
errors[LIBWEBSOCKETS_CREATECONTEXT_ERROR]=std::string("LWS Couldn't create the context. ");
#endif // NDEBUG
    }
};

/** A websockets class implemented for LibWebSockets (libwebsockets.org)
\example LibWebSocketsTest.C
*/
class LibWebSockets {

  std::string protocolName; ///< The name of the protocol which identifies this websocket
  int interrupted; ///< Set when sigint is received

  lws_context *context; ///< LibWebSocket's context handler

  // static void sigint_handler(int sig) {
  // 	interrupted = 1;
  // }

  int init(int port){
    decommission();
    // signal(SIGINT, sigint_handler);
    lws_protocols protocols[] = { // Add the default protocol here with the callback
      {protocolName.c_str(), callback, 0, 0},
      { NULL, NULL, 0, 0 },
    };
    lws_context_creation_info info;
    memset(&info, 0, sizeof(info));
    // // int argc=0; chat *argv[]=NULL; // handle argument passing later
    // // lws_cmdline_option_handle_builtin(argc, argv, &info);
    info.port=port;
    info.protocols = protocols;
    info.user = this;
    context = lws_create_context(&info);
    if (!context) {
      lwsl_err("lws init failed\n");
      LibWebSocketsDebug().evaluateError(LIBWEBSOCKETS_CREATECONTEXT_ERROR);
      return LIBWEBSOCKETS_CREATECONTEXT_ERROR;
    }
    return 0;
  }

  void decommission(){
    if (context)
      lws_context_destroy(context);
    context=NULL;
    signal(SIGINT, SIG_DFL);
  }

  // virtual void protocolInit(){
  //   lwsl_notice("LibWebSockets::protcolInit()\n");
  // }
  //
  // virtual void protocolDestroy(){
  //   lwsl_notice("LibWebSockets::protcolDestroy()\n");
  // }
  //
  // virtual void rawRxFile(void *in, size_t len){
  //   lwsl_notice("LibWebSockets::rawRxFile()\n");
  // }
  //
  // virtual void clientWriteable(void *in, size_t len){
  //   lwsl_notice("LibWebSockets::clientWriteable\n");
  // }
  //
  // virtual void clientReceive(void *in, size_t len){
  //   lwsl_notice("LibWebSockets::clientReceive\n");
  // }
  //
  // virtual void serverWriteable(void *in, size_t len){
  //   lwsl_notice("LibWebSockets::serverWriteable\n");
  // }
  //
  // virtual void receive(void *in, size_t len){
  //   lwsl_notice("LibWebSockets::receive\n");
  // }

  // definitions for declaring default virtualisable callback functions
  #define VIRTUALFNNAME(name) name##_FN
  // #define VIRTUALFNNAME(name) name##_static
  // callback functions which take no arguments
	#define VIRTUALFNDEF(retType, name) virtual retType VIRTUALFNNAME(name) ()
	#define VIRTUALFNBODY(name) {std::cout<<__func__<<std::endl;}
	#define VIRTUALFN(retType, name) VIRTUALFNDEF(retType, name) VIRTUALFNBODY(name)
  // callback functions which take two variables as inputs typically void * and int
  #define VIRTUALFNDEF2(retType, name, arg1_type, arg1, arg2_type, arg2) static retType VIRTUALFNNAME(name) (arg1_type arg1, arg2_type arg2)
	#define VIRTUALFNBODY2(name, arg1, arg2) {std::cout<<__func__<<std::endl; std::cout<<arg1<<std::endl; std::cout<<arg2<<std::endl;}
	#define VIRTUALFN2(retType, name, arg1_type, arg1, arg2_type, arg2) VIRTUALFNDEF2(retType, name, arg1_type, arg1, arg2_type, arg2) VIRTUALFNBODY2(name, arg1, arg2)

  // Define the default virtualisable callback functions
  VIRTUALFN(void, LWS_CALLBACK_PROTOCOL_INIT)
  VIRTUALFN(void, LWS_CALLBACK_PROTOCOL_DESTROY)
  VIRTUALFN2(void, LWS_CALLBACK_RAW_RX_FILE, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_RECEIVE)
  VIRTUALFN(void, LWS_CALLBACK_SERVER_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_RECEIVE)
  VIRTUALFN(void, LWS_CALLBACK_WSI_CREATE)
  VIRTUALFN(void, LWS_CALLBACK_WSI_DESTRO)
  VIRTUALFN2(void, LWS_CALLBACK_WSI_TX_CREDIT_GET, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS)
  VIRTUALFN(void, LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS)
  VIRTUALFN2(void, LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY)
  VIRTUALFN(void, LWS_CALLBACK_SSL_INFO)
  VIRTUALFN2(void, LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED)
  VIRTUALFN(void, LWS_CALLBACK_HTTP)
  VIRTUALFN2(void, LWS_CALLBACK_HTTP_BODY, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_BODY_COMPLETION)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_FILE_COMPLETION)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_CLOSED_HTTP)
  VIRTUALFN(void, LWS_CALLBACK_FILTER_HTTP_CONNECTION)
  VIRTUALFN2(void, LWS_CALLBACK_ADD_HEADERS, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_VERIFY_BASIC_AUTHORIZATION)
  VIRTUALFN(void, LWS_CALLBACK_CHECK_ACCESS_RIGHTS)
  VIRTUALFN2(void, LWS_CALLBACK_PROCESS_HTML, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_CONFIRM_UPGRADE)
  VIRTUALFN(void, LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP)
  VIRTUALFN(void, LWS_CALLBACK_CLOSED_CLIENT_HTTP)
  VIRTUALFN(void, LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ)
  VIRTUALFN(void, LWS_CALLBACK_RECEIVE_CLIENT_HTTP)
  VIRTUALFN(void, LWS_CALLBACK_COMPLETED_CLIENT_HTTP)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_HTTP_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_HTTP_REDIRECT)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL)
  VIRTUALFN2(void, LWS_CALLBACK_ESTABLISHED, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_CLOSED)
  VIRTUALFN(void, LWS_CALLBACK_SERVER_WRITEABLE)
  VIRTUALFN2(void, LWS_CALLBACK_RECEIVE, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_RECEIVE_PONG)
  VIRTUALFN2(void, LWS_CALLBACK_WS_PEER_INITIATED_CLOSE, void *, in, size_t, len)
  VIRTUALFN2(void, LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION, void *, in, size_t, len)
  VIRTUALFN2(void, LWS_CALLBACK_CONFIRM_EXTENSION_OKAY, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_WS_SERVER_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_WS_SERVER_DROP_PROTOCOL)
  VIRTUALFN2(void, LWS_CALLBACK_CLIENT_CONNECTION_ERROR, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_ESTABLISHED)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_CLOSED)
  VIRTUALFN2(void, LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER, void *, in, size_t, len)
  VIRTUALFN2(void, LWS_CALLBACK_CLIENT_RECEIVE, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_RECEIVE_PONG)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED)
  VIRTUALFN2(void, LWS_CALLBACK_WS_EXT_DEFAULTS, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_FILTER_NETWORK_CONNECTION)
  VIRTUALFN(void, LWS_CALLBACK_WS_CLIENT_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_GET_THREAD_ID)
  VIRTUALFN(void, LWS_CALLBACK_ADD_POLL_FD)
  VIRTUALFN(void, LWS_CALLBACK_DEL_POLL_FD)
  VIRTUALFN(void, LWS_CALLBACK_CHANGE_MODE_POLL_FD)
  VIRTUALFN2(void, LWS_CALLBACK_LOCK_POLL, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_UNLOCK_POLL)
  VIRTUALFN(void, LWS_CALLBACK_CGI)
  VIRTUALFN2(void, LWS_CALLBACK_CGI_TERMINATED, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_CGI_STDIN_DATA)
  VIRTUALFN(void, LWS_CALLBACK_CGI_STDIN_COMPLETED)
  VIRTUALFN(void, LWS_CALLBACK_CGI_PROCESS_ATTACH)
  VIRTUALFN(void, LWS_CALLBACK_SESSION_INFO)
  VIRTUALFN(void, LWS_CALLBACK_GS_EVENT)
  VIRTUALFN(void, LWS_CALLBACK_HTTP_PMO)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_RX)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_RX)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_CLOSE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_CLOSE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_ADOPT)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_ADOPT)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_CLI_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_PROXY_SRV_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_RX)
  VIRTUALFN(void, LWS_CALLBACK_RAW_CLOSE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_ADOPT)
  VIRTUALFN(void, LWS_CALLBACK_RAW_CONNECTED)
  VIRTUALFN(void, LWS_CALLBACK_RAW_SKT_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_SKT_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_ADOPT_FILE)
  VIRTUALFN2(void, LWS_CALLBACK_RAW_RX_FILE, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_RAW_WRITEABLE_FILE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_CLOSE_FILE)
  VIRTUALFN(void, LWS_CALLBACK_RAW_FILE_BIND_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_RAW_FILE_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_TIMER)
  VIRTUALFN(void, LWS_CALLBACK_EVENT_WAIT_CANCELLED)
  VIRTUALFN(void, LWS_CALLBACK_CHILD_CLOSING)
  VIRTUALFN2(void, LWS_CALLBACK_VHOST_CERT_AGING, void *, in, size_t, len)
  VIRTUALFN2(void, LWS_CALLBACK_VHOST_CERT_UPDATE, void *, in, size_t, len)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_NEW_CLIENT_INSTANTIATED)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_IDLE)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_CLIENT_ESTABLISHED)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_SUBSCRIBED)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_CLIENT_WRITEABLE)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_CLIENT_RX)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_UNSUBSCRIBED)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_DROP_PROTOCOL)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_CLIENT_CLOSED)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_ACK)
  VIRTUALFN(void, LWS_CALLBACK_MQTT_RESEND)
  VIRTUALFN(void, LWS_CALLBACK_USER)

  static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len){
    LibWebSockets *lws = static_cast<LibWebSockets *>(lws_context_user(lws_get_context(wsi)));
  	int n;

  	switch (reason) {
  	case LWS_CALLBACK_PROTOCOL_INIT:
      lws->LWS_CALLBACK_PROTOCOL_INIT_FN();
  		break;
  	case LWS_CALLBACK_PROTOCOL_DESTROY:
      lws->LWS_CALLBACK_PROTOCOL_DESTROY_FN();
  		break;
  	case LWS_CALLBACK_WSI_CREATE:
      lws->LWS_CALLBACK_WSI_CREATE_FN();
      break;
  	case LWS_CALLBACK_WSI_DESTRO:
      lws->LWS_CALLBACK_WSI_DESTRO_FN();
      break;
  	case LWS_CALLBACK_WSI_TX_CREDIT_GET:
      lws->LWS_CALLBACK_WSI_TX_CREDIT_GET_FN(in, len);
  		break;
    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to Server TLS -----
    	 */

    	case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS:
        lws->LWS_CALLBACK_OPENSSL_LOAD_EXTRA_CLIENT_VERIFY_CERTS_FN();
        break;

    	case LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS:
        lws->LWS_CALLBACK_OPENSSL_LOAD_EXTRA_SERVER_VERIFY_CERTS_FN();
        break;

    	case LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION:
        lws->LWS_CALLBACK_OPENSSL_PERFORM_CLIENT_CERT_VERIFICATION_FN(in, len);
        break;

    	case LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY:
        lws->LWS_CALLBACK_OPENSSL_CONTEXT_REQUIRES_PRIVATE_KEY_FN();
        break;

    	case LWS_CALLBACK_SSL_INFO:
        lws->LWS_CALLBACK_SSL_INFO_FN();
        break;

    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to Client TLS -----
    	 */

    	case LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION:
        lws->LWS_CALLBACK_OPENSSL_PERFORM_SERVER_CERT_VERIFICATION_FN(in, len);
        break;
    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to HTTP Server  -----
    	 */

    	case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
        lws->LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED_FN();
        break;

    	case LWS_CALLBACK_HTTP:
        lws->LWS_CALLBACK_HTTP_FN();
        break;

    	case LWS_CALLBACK_HTTP_BODY:
        lws->LWS_CALLBACK_HTTP_BODY_FN(in, len);
        break;

    	case LWS_CALLBACK_HTTP_BODY_COMPLETION:
        lws->LWS_CALLBACK_HTTP_BODY_COMPLETION_FN();
        break;

    	case LWS_CALLBACK_HTTP_FILE_COMPLETION:
        lws->LWS_CALLBACK_HTTP_FILE_COMPLETION_FN();
        break;

    	case LWS_CALLBACK_HTTP_WRITEABLE:
        lws->LWS_CALLBACK_HTTP_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_CLOSED_HTTP:
        lws->LWS_CALLBACK_CLOSED_HTTP_FN();
        break;

    	case LWS_CALLBACK_FILTER_HTTP_CONNECTION:
        lws->LWS_CALLBACK_FILTER_HTTP_CONNECTION_FN();
        break;

    	case LWS_CALLBACK_ADD_HEADERS:
        lws->LWS_CALLBACK_ADD_HEADERS_FN(in, len);
        break;
    	case LWS_CALLBACK_VERIFY_BASIC_AUTHORIZATION:
        lws->LWS_CALLBACK_VERIFY_BASIC_AUTHORIZATION_FN();
        break;


    	case LWS_CALLBACK_CHECK_ACCESS_RIGHTS:
        lws->LWS_CALLBACK_CHECK_ACCESS_RIGHTS_FN();
        break;

    	case LWS_CALLBACK_PROCESS_HTML:
        lws->LWS_CALLBACK_PROCESS_HTML_FN(in, len);
        break;

    	case LWS_CALLBACK_HTTP_BIND_PROTOCOL:
        lws->LWS_CALLBACK_HTTP_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_HTTP_DROP_PROTOCOL:
        lws->LWS_CALLBACK_HTTP_DROP_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_HTTP_CONFIRM_UPGRADE:
        lws->LWS_CALLBACK_HTTP_CONFIRM_UPGRADE_FN();
        break;

    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to HTTP Client  -----
    	 */

    	case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
        lws->LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP_FN();
        break;

    	case LWS_CALLBACK_CLOSED_CLIENT_HTTP:
        lws->LWS_CALLBACK_CLOSED_CLIENT_HTTP_FN();
        break;

    	case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
        lws->LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ_FN();
        break;

    	case LWS_CALLBACK_RECEIVE_CLIENT_HTTP:
        lws->LWS_CALLBACK_RECEIVE_CLIENT_HTTP_FN();
        break;

    	case LWS_CALLBACK_COMPLETED_CLIENT_HTTP:
        lws->LWS_CALLBACK_COMPLETED_CLIENT_HTTP_FN();
        break;

    	case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
        lws->LWS_CALLBACK_CLIENT_HTTP_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_CLIENT_HTTP_REDIRECT:
        lws->LWS_CALLBACK_CLIENT_HTTP_REDIRECT_FN();
        break;

    	case LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL:
        lws->LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL:
        lws->LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to Websocket Server -----
    	 */

    	case LWS_CALLBACK_ESTABLISHED:
        lws->LWS_CALLBACK_ESTABLISHED_FN(in, len);
        break;

    	case LWS_CALLBACK_CLOSED:
        lws->LWS_CALLBACK_CLOSED_FN();
        break;

    	case LWS_CALLBACK_SERVER_WRITEABLE:
        lws->LWS_CALLBACK_SERVER_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_RECEIVE:
        lws->LWS_CALLBACK_RECEIVE_FN(in, len);
        break;

    	case LWS_CALLBACK_RECEIVE_PONG:
        lws->LWS_CALLBACK_RECEIVE_PONG_FN();
        break;

    	case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE:
        lws->LWS_CALLBACK_WS_PEER_INITIATED_CLOSE_FN(in, len);
        break;

    	case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
        lws->LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION_FN(in, len);
        break;

    	case LWS_CALLBACK_CONFIRM_EXTENSION_OKAY:
        lws->LWS_CALLBACK_CONFIRM_EXTENSION_OKAY_FN(in, len);
        break;

    	case LWS_CALLBACK_WS_SERVER_BIND_PROTOCOL:
        lws->LWS_CALLBACK_WS_SERVER_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_WS_SERVER_DROP_PROTOCOL:
        lws->LWS_CALLBACK_WS_SERVER_DROP_PROTOCOL_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to Websocket Client -----
    	 */

    	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
        lws->LWS_CALLBACK_CLIENT_CONNECTION_ERROR_FN(in, len);
        break;

    	case LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH:
        lws->LWS_CALLBACK_CLIENT_FILTER_PRE_ESTABLISH_FN();
        break;


    	case LWS_CALLBACK_CLIENT_ESTABLISHED:
        lws->LWS_CALLBACK_CLIENT_ESTABLISHED_FN();
        break;


    	case LWS_CALLBACK_CLIENT_CLOSED:
        lws->LWS_CALLBACK_CLIENT_CLOSED_FN();
        break;

    	case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
        lws->LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER_FN(in, len);
        break;

    	case LWS_CALLBACK_CLIENT_RECEIVE:
        lws->LWS_CALLBACK_CLIENT_RECEIVE_FN(in, len);
        break;

    	case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
        lws->LWS_CALLBACK_CLIENT_RECEIVE_PONG_FN();
        break;

    	case LWS_CALLBACK_CLIENT_WRITEABLE:
        lws->LWS_CALLBACK_CLIENT_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
        lws->LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED_FN();
        break;


    	case LWS_CALLBACK_WS_EXT_DEFAULTS:
        lws->LWS_CALLBACK_WS_EXT_DEFAULTS_FN(in, len);
        break;

    	case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
        lws->LWS_CALLBACK_FILTER_NETWORK_CONNECTION_FN();
        break;

    	case LWS_CALLBACK_WS_CLIENT_BIND_PROTOCOL:
        lws->LWS_CALLBACK_WS_CLIENT_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL:
        lws->LWS_CALLBACK_WS_CLIENT_DROP_PROTOCOL_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to external poll loop integration  -----
    	 */

    	case LWS_CALLBACK_GET_THREAD_ID:
        lws->LWS_CALLBACK_GET_THREAD_ID_FN();
        break;

    	/* external poll() management support */
    	case LWS_CALLBACK_ADD_POLL_FD:
        lws->LWS_CALLBACK_ADD_POLL_FD_FN();
        break;

    	case LWS_CALLBACK_DEL_POLL_FD:
        lws->LWS_CALLBACK_DEL_POLL_FD_FN();
        break;

    	case LWS_CALLBACK_CHANGE_MODE_POLL_FD:
        lws->LWS_CALLBACK_CHANGE_MODE_POLL_FD_FN();
        break;


    	case LWS_CALLBACK_LOCK_POLL:
        lws->LWS_CALLBACK_LOCK_POLL_FN(in, len);
        break;

    	case LWS_CALLBACK_UNLOCK_POLL:
        lws->LWS_CALLBACK_UNLOCK_POLL_FN();
        break;

    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to CGI serving -----
    	 */

    	case LWS_CALLBACK_CGI:
        lws->LWS_CALLBACK_CGI_FN();
        break;


    	case LWS_CALLBACK_CGI_TERMINATED:
        lws->LWS_CALLBACK_CGI_TERMINATED_FN(in, len);
        break;

    	case LWS_CALLBACK_CGI_STDIN_DATA:
        lws->LWS_CALLBACK_CGI_STDIN_DATA_FN();
        break;

    	case LWS_CALLBACK_CGI_STDIN_COMPLETED:
        lws->LWS_CALLBACK_CGI_STDIN_COMPLETED_FN();
        break;

    	case LWS_CALLBACK_CGI_PROCESS_ATTACH:
        lws->LWS_CALLBACK_CGI_PROCESS_ATTACH_FN();
        break;

    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to Generic Sessions -----
    	 */

    	case LWS_CALLBACK_SESSION_INFO:
        lws->LWS_CALLBACK_SESSION_INFO_FN();
        break;

    	case LWS_CALLBACK_GS_EVENT:
        lws->LWS_CALLBACK_GS_EVENT_FN();
        break;

    	case LWS_CALLBACK_HTTP_PMO:
        lws->LWS_CALLBACK_HTTP_PMO_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to RAW PROXY -----
    	 */

    	case LWS_CALLBACK_RAW_PROXY_CLI_RX:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_RX_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_RX:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_RX_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_CLI_CLOSE:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_CLOSE_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_CLOSE:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_CLOSE_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_CLI_WRITEABLE:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_WRITEABLE:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_CLI_ADOPT:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_ADOPT_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_ADOPT:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_ADOPT_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_CLI_BIND_PROTOCOL:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_BIND_PROTOCOL:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_CLI_DROP_PROTOCOL:
        lws->LWS_CALLBACK_RAW_PROXY_CLI_DROP_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_RAW_PROXY_SRV_DROP_PROTOCOL:
        lws->LWS_CALLBACK_RAW_PROXY_SRV_DROP_PROTOCOL_FN();
        break;



    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to RAW sockets -----
    	 */

    	case LWS_CALLBACK_RAW_RX:
        lws->LWS_CALLBACK_RAW_RX_FN();
        break;

    	case LWS_CALLBACK_RAW_CLOSE:
        lws->LWS_CALLBACK_RAW_CLOSE_FN();
        break;

    	case LWS_CALLBACK_RAW_WRITEABLE:
        lws->LWS_CALLBACK_RAW_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_RAW_ADOPT:
        lws->LWS_CALLBACK_RAW_ADOPT_FN();
        break;

    	case LWS_CALLBACK_RAW_CONNECTED:
        lws->LWS_CALLBACK_RAW_CONNECTED_FN();
        break;

    	case LWS_CALLBACK_RAW_SKT_BIND_PROTOCOL:
        lws->LWS_CALLBACK_RAW_SKT_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_RAW_SKT_DROP_PROTOCOL:
        lws->LWS_CALLBACK_RAW_SKT_DROP_PROTOCOL_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to RAW file handles -----
    	 */

    	case LWS_CALLBACK_RAW_ADOPT_FILE:
        lws->LWS_CALLBACK_RAW_ADOPT_FILE_FN();
        break;

    	case LWS_CALLBACK_RAW_RX_FILE:
        lws->LWS_CALLBACK_RAW_RX_FILE_FN(in, len);
        break;

    	case LWS_CALLBACK_RAW_WRITEABLE_FILE:
        lws->LWS_CALLBACK_RAW_WRITEABLE_FILE_FN();
        break;


    	case LWS_CALLBACK_RAW_CLOSE_FILE:
        lws->LWS_CALLBACK_RAW_CLOSE_FILE_FN();
        break;

    	case LWS_CALLBACK_RAW_FILE_BIND_PROTOCOL:
        lws->LWS_CALLBACK_RAW_FILE_BIND_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_RAW_FILE_DROP_PROTOCOL:
        lws->LWS_CALLBACK_RAW_FILE_DROP_PROTOCOL_FN();
        break;

    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to generic wsi events -----
    	 */

    	case LWS_CALLBACK_TIMER:
        lws->LWS_CALLBACK_TIMER_FN();
        break;


    	case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
        lws->LWS_CALLBACK_EVENT_WAIT_CANCELLED_FN();
        break;


    	case LWS_CALLBACK_CHILD_CLOSING:
        lws->LWS_CALLBACK_CHILD_CLOSING_FN();
        break;


    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to TLS certificate management -----
    	 */

    	case LWS_CALLBACK_VHOST_CERT_AGING:
        lws->LWS_CALLBACK_VHOST_CERT_AGING_FN(in, len);
        break;

    	case LWS_CALLBACK_VHOST_CERT_UPDATE:
        lws->LWS_CALLBACK_VHOST_CERT_UPDATE_FN(in, len);
        break;
    	/* ---------------------------------------------------------------------
    	 * ----- Callbacks related to MQTT Client  -----
    	 */

    	case LWS_CALLBACK_MQTT_NEW_CLIENT_INSTANTIATED:
        lws->LWS_CALLBACK_MQTT_NEW_CLIENT_INSTANTIATED_FN_FN();
        break;

    	case LWS_CALLBACK_MQTT_IDLE:
        lws->LWS_CALLBACK_MQTT_IDLE_FN_FN();
        break;

    	case LWS_CALLBACK_MQTT_CLIENT_ESTABLISHED:
        lws->LWS_CALLBACK_MQTT_CLIENT_ESTABLISHED_FN_FN();
        break;

    	case LWS_CALLBACK_MQTT_SUBSCRIBED:
        lws->LWS_CALLBACK_MQTT_SUBSCRIBED_FN_FN();
        break;

    	case LWS_CALLBACK_MQTT_CLIENT_WRITEABLE:
        lws->LWS_CALLBACK_MQTT_CLIENT_WRITEABLE_FN();
        break;

    	case LWS_CALLBACK_MQTT_CLIENT_RX:
        lws->LWS_CALLBACK_MQTT_CLIENT_RX_FN();
        break;

    	case LWS_CALLBACK_MQTT_UNSUBSCRIBED:
        lws->LWS_CALLBACK_MQTT_UNSUBSCRIBED_FN();
        break;

    	case LWS_CALLBACK_MQTT_DROP_PROTOCOL:
        lws->LWS_CALLBACK_MQTT_DROP_PROTOCOL_FN();
        break;

    	case LWS_CALLBACK_MQTT_CLIENT_CLOSED:
        lws->LWS_CALLBACK_MQTT_CLIENT_CLOSED_FN();
        break;

    	case LWS_CALLBACK_MQTT_ACK:
        lws->LWS_CALLBACK_MQTT_ACK_FN();
        break;

    	case LWS_CALLBACK_MQTT_RESEND:
        lws->LWS_CALLBACK_MQTT_RESEND_FN();
        break;

    	case LWS_CALLBACK_USER:
        lws->LWS_CALLBACK_USER_FN();
        break;


  	default:
  		break;
  	}
  	return 0;
  }

public:
  /// Constructor
  LibWebSockets(int port=0){
    context=NULL;
    interrupted=0;
    protocolName="generic-ws";
    init(port);
  }

  /// Destructor
  virtual ~LibWebSockets(){
    decommission();
  }

  virtual int run(){
    while ((lws_service(context, 0) >= 0) && !interrupted)
      ;
    return 0;
  }
};
#endif // LIBWEBSOCKETS_H
