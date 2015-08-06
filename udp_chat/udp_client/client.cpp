#include "client.h"

int main()
{

	client cli;
	if(cli.init("127.0.0.1", 49999) < 0){
		log_error("cli init error");
	}
	if(cli.run() < 0){
		log_error("cli run error");
	}

	return 0;
}
