#include "stdafx.h"
#include "Server.h"


Server::Server()
{
}


Server::~Server()
{
}


// ����������
void Server::start()
{
	//��һ����ע�ᡢ��ʼ��Winsock.dll
	WSADATA wsda;
	if (WSAStartup(MAKEWORD(2, 2), &wsda))
	{
		puts("not accpet windows socket version");
		exit(0);
	}
	//�ڶ����������׽���
	if ((server_socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		puts("open socket error");
		exit(0);
	}
	//�����������÷�������Ϣ���󶨵�ַ���׽���
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

	//���Ĳ����������ͬʱ����������ʼ����
	if (listen(server_socket_id, listen_num) != 0)
	{
		puts("listen error!");
		exit(0);
	}
	printf("������������.......\n");
}


// ��������
void Server::running()
{
	//���岽��������
	client_socket_len = sizeof(client_addr);
	while (true)
	{
		client_socket = accept(server_socket_id, (struct sockaddr*)&client_addr, &client_socket_len);
		if (client_socket == INVALID_SOCKET)
		{
			printf("Accept error!");
			break;
		}

		//TODO: ���봦��TCP����ģ��
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


// �رշ�����
void Server::end()
{
	//���������ر��׽���
	closesocket(server_socket_id);
	WSACleanup();
}
