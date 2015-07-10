#pragma once
#include "stdafx.h"
using std::string;
enum Method{GET, POST, HEAD, UNKNOWN};
class TCPconnect
{
public:
	// 打开一个线程，处理TCP连接
	static void beginConnect(SOCKET t_client_socket);
	static DWORD WINAPI connecting(LPVOID t_client_socket);

private:
	static const int request_buffer_size = 1 << 10;//1k
	static const int reponse_buffer_size = 1 << 10;//1KB

public:
	TCPconnect();
	TCPconnect(SOCKET socket);
	~TCPconnect();
	//解析报文
	void analyze();
	// 发送响应报文
	bool reponse();
private:
	// 打开URL所申请的资源文件
	bool openFile();
	// 获得所请求资源的MIME类型
	string getFileType();
private:
	SOCKET client_socket;//该TCP连接的客户端socket
	char *request_buffer;//请求报文的buffer
	char *reponse_buffer;//响应报文的buffer
	string status_line;//状态行
	string reponse_header;//响应头域
	HANDLE hThread;//线程handle
	
	Method method;//请求的方法
	string abs_path;//请求的据对路径
	FILE *file;
};

