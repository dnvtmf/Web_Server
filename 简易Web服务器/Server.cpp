#include "stdafx.h"
#include "Server.h"


Server::Server()
{
}


Server::~Server()
{
}


// 启动服务器
void Server::start()
{
	//第一步：注册、初始化Winsock.dll
	WSADATA wsda;
	if (WSAStartup(MAKEWORD(2, 2), &wsda))
	{
		puts("not accpet windows socket version");
		exit(0);
	}
	//第二步：创建套接字
	if ((server_socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		puts("open socket error");
		exit(0);
	}
	//第三步：配置服务器信息，绑定地址到套接字
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(host_port);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//bind
	if (bind(server_socket_id, (sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		puts("bind error!");
		exit(0);
	}

	//第四步：设置最大同时连接数，开始监听
	if (listen(server_socket_id, listen_num) != 0)
	{
		puts("listen error!");
		exit(0);
	}
	printf("服务器已启动.......\n");
}


// 服务器中
void Server::running()
{
	//第五步：监听中
	client_socket_len = sizeof(client_addr);
	while (true)
	{
		client_socket = accept(server_socket_id, (struct sockaddr*)&client_addr, &client_socket_len);
		if (client_socket == INVALID_SOCKET)
		{
			printf("Accept error!");
			break;
		}

		//TODO: 插入处理TCP连接模块
		/*HANDLE hThread;
		hThread = CreateThread(NULL, 0, &Server::Deal, (LPVOID)client_socket, 0, NULL);
		if (hThread == NULL)
		{
			puts("Create Thread Error!");
			closesocket(client_socket);
			break;
		}*/
	}
}


// 关闭服务器
void Server::end()
{
	//第六步：关闭套接字
	closesocket(server_socket_id);
	WSACleanup();
}
