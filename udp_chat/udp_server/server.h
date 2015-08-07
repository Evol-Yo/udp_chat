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
#include <pthread.h>

#include "data_pool.h"
#include "my_json.h"
#include "util.h"

void *cosumer_run(void *ser);
void *productor_run(void *ser);

class msg_info {
	private:
		int		msg_len;
		string	msg;
};

class server {
	friend void *cosumer_run(void *ser);
	friend void *productor_run(void *ser);

	public:
		server()
			: _sock(0), _port(0), _socklen(0)
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
		data_pool				_msg_queue;
		client_list_type		_online_users;
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

	pthread_t cosumer, productor;
	if(pthread_create(&productor, NULL, productor_run, this) != 0){
		log_error("create productor error");
		return -1;
	}
	if(pthread_create(&cosumer, NULL, cosumer_run, this) != 0){
		log_error("create cosumer error");
		return -2;
	}

	pthread_join(productor, NULL);
	pthread_join(cosumer, NULL);

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
	client_list_type::iterator iter = _online_users.begin();
	while(iter != _online_users.end()){
		ssize_t size = send_msg(msg, iter->second);
		if(size < 0){
			//do something
		}
		iter++;
	}
	return 0;
}

void *cosumer_run(void *ser)
{	
	server *serv = (server*)ser;
	client_info client;
	string msg;
	data_pool::data_type data;
	while(1){
		data = serv->_msg_queue.pop();
		if(serv->broadcast_msg(data.second) < 0){
			log_error("broadcast_msg error");
			return (void*)-2;
		}
	}
	return NULL;
}

void *productor_run(void *ser)
{
	server *serv = (server *)ser;
	client_info client;
	string msg;
	data_pool::data_type data;
	while(1){
		if(serv->recv_msg(msg, client) < 0){
			log_error("recv_msg error");
			return (void*)-1;
		}
		data.first = client._id;
		data.second = msg;

		log_message(msg.c_str());

		serv->_msg_queue.push(data);
		serv->_online_users[client._id] = client;
	}

	return NULL;
}


#endif
