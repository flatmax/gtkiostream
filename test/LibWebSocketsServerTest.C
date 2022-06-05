
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

#include "LibWebSockets.H"
#include <strstream>
using namespace std;

class LWSTest : public LibWebSockets {
	string msgData;

	int receive(void *in, size_t len){
			printf("LWSTest::receive\n");
			string msgC((char*)in,len);
		  cout<<msgC<<endl;
			return 1;
	}

	void serverWriteable(struct lws *wsi){
			printf("LWSTest::serverWritable\n");

			int flags = lws_write_ws_flags(LWS_WRITE_TEXT, 1, 1);

			string msgData;
			for (int i=0; i<LWS_PRE; i++)
				msgData+=' ';
				msgData+="Hi from the terminal";
			/* notice we allowed for LWS_PRE in the payload already */
			int m = lws_write(wsi, ((unsigned char *)msgData.c_str()) +
							LWS_PRE, msgData.size()-LWS_PRE, (lws_write_protocol)flags);
			if (m < (int)msgData.size()-LWS_PRE) {
				lwsl_err("ERROR %d writing to ws socket\n", m);
				// return -1;
			} else
				lwsl_user(" wrote %d: flags: 0x%x\n",
						m, flags);
	}

public:
	LWSTest(int port=0) : LibWebSockets(port) {
		msgData="";
		for (int n=0; n<LWS_PRE; n++)
			msgData+=' ';
		msgData+="Server says hello!";
	}
};

int main(int argc, char *argv[]) {
	LWSTest lws(10001);
	printf("LibWebSockets::LibWebSockets\n");

	lws.run();
	return 0;
}
