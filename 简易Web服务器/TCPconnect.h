#pragma once
#include "stdafx.h"
using std::string;
enum Method{GET, POST, HEAD, UNKNOWN};
#define SERVER "Server: wan's web server\r\n"
class TCPconnect
{
public:
	// 打开一个线程，处理TCP连接
	static void beginConnect(SOCKET t_client_socket);
	static DWORD WINAPI connecting(LPVOID t_client_socket);

private:
	//buffer_size 不超过64KB
	static const int buffer_size_1K = 1 << 10;//1KB
	static const int buffer_size_32K = 1 << 15;//32KB
	static const int default_buffer_size = buffer_size_32K;
	static const int send_limit = 1 << 20;//设置1M为阈值，超过1M将使用206

public:
	TCPconnect();
	TCPconnect(SOCKET socket);
	~TCPconnect();
	//解析报文
	void analyze();
	// 发送响应报文
	bool response();
private:
	// 打开URL所申请的资源文件
	bool openFile();
	// 获得所请求资源的MIME类型
	string getFileType();
	// 获得当前服务器时间
	string getDate();
	//分析请求头部
	void analyze_request_header(string field_name, string field_value);
	// 请求头部的参数设为默认值
	void set_default();
	void response_OK();
	void response_Error(string error_no, string error_descript, string error_msg);
	void response_Part_Content();//返回是否传输完所有文件
	void response_Not_Modified();
	void response_Not_Implemented();
private:
	SOCKET client_socket;//该TCP连接的客户端socket
//----------------  request  -----------------//
	char *request_buffer;//请求报文的buffer
	int request_buffer_size;//请求报文buffer大小
	Method method;//请求的方法
	string url;//请求的据对路径
	bool range;
	int range_min;
	int range_max;
	bool Connect_keep_alive;//是否保持持续连接
	//条件GET
	bool if_get;
	string if_none_match;
	string if_modified_since;
	//POST方法
	int content_length;
//---------------- reponse  ----------------//
	char *response_buffer;//响应报文的buffer
	int response_buffer_size;//响应报文buffer大小
	string status_line;//状态行
	string response_header;//响应头域
	FILE *file;
	int file_length;
	struct stat file_status;//文件的详细信息
	string last_modified;//文件的最后修改时间
	string Etag;//实体标签
};

