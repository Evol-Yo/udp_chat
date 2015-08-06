#ifndef _UTIL_H_
#define _UTIL_H_

#include <iostream>
#include <string>
#include <map>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

void log_error(const char*fmt, ...);
void log_message(const char*fmt, ...);

#define BUF_SIZE 512

typedef struct client_info {
	
	typedef in_addr_t id_type; 

	id_type				_id;
	string 				_name;
	struct sockaddr_in	_sockaddr;
	socklen_t			_socklen;

	client_info()
	{
		_id = 0;
		bzero(&_sockaddr, sizeof(_sockaddr));
		_socklen = 0;
	}

	static id_type hash(const struct sockaddr_in &sockaddr){
		return (id_type)sockaddr.sin_addr.s_addr;
	}

} client_info;


#endif
