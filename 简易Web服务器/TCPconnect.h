#pragma once
#include "stdafx.h"
using std::string;
enum Method{GET, POST, HEAD, UNKNOWN};
class TCPconnect
{
public:
	// ��һ���̣߳�����TCP����
	static void beginConnect(SOCKET t_client_socket);
	static DWORD WINAPI connecting(LPVOID t_client_socket);

private:
	static const int request_buffer_size = 1 << 10;//1k
	static const int reponse_buffer_size = 1 << 10;//1KB

public:
	TCPconnect();
	TCPconnect(SOCKET socket);
	~TCPconnect();
	//��������
	void analyze();
	// ������Ӧ����
	bool reponse();
private:
	// ��URL���������Դ�ļ�
	bool openFile();
	// �����������Դ��MIME����
	string getFileType();
private:
	SOCKET client_socket;//��TCP���ӵĿͻ���socket
	char *request_buffer;//�����ĵ�buffer
	char *reponse_buffer;//��Ӧ���ĵ�buffer
	string status_line;//״̬��
	string reponse_header;//��Ӧͷ��
	HANDLE hThread;//�߳�handle
	
	Method method;//����ķ���
	string abs_path;//����ľݶ�·��
	FILE *file;
};

