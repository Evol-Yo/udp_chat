#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <strings.h>
#include <map>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

#include "data_pool.h"
#include "my_json.h"
#include "util.h"

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

class server {
	public:
		server()
			:_sock(0), _port(0), _socklen(0)
		{
			bzero(&_sockaddr, sizeof(_sockaddr));
		}

		~server()
		{
			if(_sock > 0){
				close(_sock);
			}
		}

		int init(const string &ip, unsigned short port);
		int run();

	private:
		ssize_t recv_msg(string &msg, client_info &cli);
		ssize_t send_msg(const string &msg, const client_info &cli);
		int broadcast_msg(const string &msg);

	private:
		typedef map<typename client_info::id_type, client_info> \
			client_list_type;

		int						_sock;
		unsigned short 			_port;
		struct sockaddr_in		_sockaddr;
		socklen_t				_socklen;
		client_list_type		_clients_list;

};


int server::init(const string &ip, unsigned short port)
{
	_port = port;
	_socklen = sizeof(_sockaddr);

	_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(_sock < 0){
		return -1;
	}
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(port);
	_sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());

	if(bind(_sock, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) < 0){
		return -2;
	}

	return 0;
}

int server::run()
{

	client_info client;
	string msg;
	if(recv_msg(msg, client) < 0){
		log_error("recv_msg error");
		return -1;
	}

	_clients_list[client._id] = client;

	if(broadcast_msg(msg) < 0){
		log_error("broadcast_msg error");
		return -2;
	}

	return 0;

}

ssize_t server::recv_msg(string &msg, client_info &cli)
{
	char buf[BUF_SIZE];
	bzero(buf, sizeof(buf));

	struct sockaddr_in cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	socklen_t len = sizeof(cliaddr);
	ssize_t size = recvfrom(_sock, buf, sizeof(buf), 0, 
			(struct sockaddr*)&cliaddr, &len);

	if(size >= 0){
		cli._id = client_info::hash(cliaddr);
		cli._name = "";
		cli._socklen = len;
		memcpy(&cli._sockaddr, &cliaddr, len);
		msg = buf;
	}

	return size;
}

ssize_t server::send_msg(const string &msg, const client_info &cli)
{
	ssize_t size = sendto(_sock, msg.c_str(), msg.length(), 0,
			(struct sockaddr *)&cli._sockaddr, cli._socklen);
	return size;
}

int server::broadcast_msg(const string &msg)
{
	client_list_type::iterator iter = _clients_list.begin();
	while(iter != _clients_list.end()){
		ssize_t size = send_msg(msg, iter->second);
		if(size < 0){
			//do something
		}
	}
	return 0;
}

#endif
