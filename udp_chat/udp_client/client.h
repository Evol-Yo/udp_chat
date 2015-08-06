#ifndef _CLIENT_H_
#define _CLIENT_H_

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
#include <vector>

#include "my_json.h"
#include "data_pool.h"
#include "util.h"


class client {
	public:
		client()
			: _name(), _sock(0), _serport(0), _sersocklen()
		{
			bzero(&_sersockaddr, sizeof(_sersockaddr));
		}	

		~client()
		{
			if(_sock > 0){
				close(_sock);
			}
		}

		int init(const string &ser_ip, unsigned short ser_port);
		int run();

	private:
		//注册用户
		//int regeister();

		ssize_t recv_msg(string &msg);
		ssize_t send_msg(const string &msg);

	private:
		//client info
		string 				_name;
		vector<client_info>	_friends;

	private:
		//connection info
		int					_sock;
		unsigned short		_serport;
		struct sockaddr_in	_sersockaddr;
		socklen_t			_sersocklen;
};


int client::init(const string &ser_ip, unsigned short ser_port)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock < 0){
		return -1;
	}
	_sock = sock;
	_serport = ser_port;
	_sersockaddr.sin_family = AF_INET;
	_sersockaddr.sin_port = htons(ser_port);
	_sersockaddr.sin_addr.s_addr = inet_addr(ser_ip.c_str());
	_sersocklen = sizeof(_sersockaddr);

	return 0;
}
		
int client::run()
{
	string send_msg = "hello world!";
	string recv_msg;
	while(1){
		if(this->send_msg(send_msg) < 0){
			return -1;
		}
		if(this->recv_msg(recv_msg) < 0){
			return -2;
		}
		
		log_message(recv_msg.c_str());

		sleep(1);
		recv_msg = "";
	}
	return 0;
}

ssize_t client::send_msg(const string &msg)
{
	ssize_t size = sendto(_sock, msg.c_str(), msg.length(), 0,
		(struct sockaddr*)&_sersockaddr, _sersocklen);
	return size;
}

ssize_t client::recv_msg(string &msg)
{
	char buf[BUF_SIZE];
	bzero(buf, sizeof(buf));
	socklen_t len = _sersocklen;
	ssize_t size = recvfrom(_sock, buf, sizeof(buf), 0,
			(struct sockaddr*)&_sersockaddr, &len);
	if(size >= 0){
		msg = buf;
	}
	return size;
}

#endif
