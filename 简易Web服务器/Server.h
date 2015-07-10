#pragma once
#include "stdafx.h"
#pragma comment(lib, "ws2_32.lib")

class Server
{
public:
	Server();
	~Server();
	// 启动服务器
	void start();
	// 服务器中
	void running();
	// 关闭服务器
	void end();

private:
	SOCKET server_socket_id;//服务器的socket
	SOCKET client_socket;//客服端的链接socket
	int client_socket_len;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	//const char *host_ip = "172.0.0.1";
	const short host_port = 80;//服务器端口: 80
	const int listen_num = 10;//监听数量：10
};

