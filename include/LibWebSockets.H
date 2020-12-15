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

  virtual void protocolInit(){
    lwsl_notice("LibWebSockets::protcolInit()\n");
  }

  virtual void protocolDestroy(){
    lwsl_notice("LibWebSockets::protcolDestroy()\n");
  }

  virtual void rawRxFile(void *in, size_t len){
    lwsl_notice("LibWebSockets::rawRxFile()\n");
  }

  virtual void clientWriteable(void *in, size_t len){
    lwsl_notice("LibWebSockets::clientWriteable\n");
  }

  virtual void clientReceive(void *in, size_t len){
    lwsl_notice("LibWebSockets::clientReceive\n");
  }

  virtual void serverWriteable(void *in, size_t len){
    lwsl_notice("LibWebSockets::serverWriteable\n");
  }

  virtual void receive(void *in, size_t len){
    lwsl_notice("LibWebSockets::receive\n");
  }

  static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len){
    LibWebSockets *lws = static_cast<LibWebSockets *>(lws_context_user(lws_get_context(wsi)));
  	int n;
    printf("LibWebSockets::callback\n");
    lws->protocolInit();

  	switch (reason) {
  	case LWS_CALLBACK_PROTOCOL_INIT:
      lws->protocolInit();
  		break;
  	case LWS_CALLBACK_PROTOCOL_DESTROY:
      lws->protocolDestroy();
  		break;
  	case LWS_CALLBACK_RAW_RX_FILE:
      lws->rawRxFile(in, len);
  		break;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      lws->clientWriteable(in, len);
      break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      lws->clientReceive(in, len);
      break;
    case LWS_CALLBACK_SERVER_WRITEABLE:
      lws->serverWriteable(in, len);
      break;
    case LWS_CALLBACK_RECEIVE:
      lws->receive(in, len);
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
