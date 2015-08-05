#include "server.h"

int main()
{
	server server;
	if(server.init("127.0.0.1", 49999) < 0){
		log_error("server init error");
		return 1;
	}
	if(server.run() < 0){
		log_error("server run error");
		return 2;
	}

	return 0;
}
