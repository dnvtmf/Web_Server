#pragma once
#include "stdafx.h"

class Server
{
public:
	Server();
	~Server();
	// ����������
	void start();
	// ��������
	void running();
	// �رշ�����
	void end();

private:
	SOCKET server_socket_id;//��������socket
	SOCKET client_socket;//�ͷ��˵�����socket
	int client_socket_len;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	//const char *host_ip = "172.0.0.1";
	const short host_port = 80;//�������˿�: 80
	const int listen_num = 10;//����������10
};

