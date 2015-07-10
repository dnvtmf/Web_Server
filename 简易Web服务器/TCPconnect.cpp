#include "stdafx.h"


TCPconnect::TCPconnect()
{
}
TCPconnect::TCPconnect(SOCKET socket)
{
	client_socket = socket;
	request_buffer = new char[request_buffer_size + 50];
	reponse_buffer = new char[reponse_buffer_size + 50];
}

TCPconnect::~TCPconnect()
{
	closesocket(client_socket);
	delete[] request_buffer;
	delete[] reponse_buffer;
}


// 打开一个线程，处理TCP连接
void TCPconnect::beginConnect(SOCKET t_client_socket)
{
	if(CreateThread(NULL, 0, &connecting, (LPVOID)t_client_socket, 0, NULL) == NULL)
	{
		puts("Create Thread Error!");
		closesocket(t_client_socket);
	}
}
DWORD WINAPI TCPconnect::connecting(LPVOID t_client_socket)
{
	TCPconnect mTCP((SOCKET)t_client_socket);
	int ret = 0;
	while (true)
	{
		ret = recv(mTCP.client_socket, mTCP.request_buffer, request_buffer_size, 0);
		if (ret == 0 || ret == SOCKET_ERROR)
		{
			puts("client closed");
			break;
		}
		mTCP.analyze();//解析请求报文
		if (mTCP.reponse())//返回响应报文
			continue;
		else
		{
			puts("client closed");
			break;
		}
	}
	return 0;
}

void TCPconnect::analyze()
{
	string request = string(request_buffer);
	string request_line = request.substr(0, request.find('\n'));
	int i = request_line.find(' ');
	//解析方法
	string mmethod = request_line.substr(0, i);
	if (mmethod == "GET")
		method = GET;
	else if (mmethod == "POST")
		method = POST;
	else if (mmethod == "HEAD")
		method = HEAD;
	else
		method = UNKNOWN;
	//解析URL
	abs_path = request_line.substr(i+1, request_line.find("HTTP/")- i - 2);
	if (abs_path[0] != '/')
	{//"http://"host[":" port][abs_path ["?" query]]
		abs_path = abs_path.substr(abs_path.find('/', 7), string::npos);
	}
	//去掉询问部分
	abs_path = abs_path.substr(0, abs_path.find('?'));
	if (abs_path == "/")
	{//如果路径为服务器根目录
		abs_path += "index.htm";
	}
	abs_path = "web_content" + abs_path;
	//puts(request_buffer);
	std::cout << "path: " << abs_path << std::endl;
}


// 发送响应报文
bool TCPconnect::reponse()
{
	if (method == UNKNOWN)
	{
		status_line = "HTTP/1.1 400 Bad Request\r\n\r\n";
		send(client_socket, status_line.data(), status_line.length(), 0);
		return false;
	}
	else if (method == HEAD || method == GET)
	{
		reponse_header = "";
		if (openFile())
		{
			status_line = "HTTP/1.1 200 OK\r\n";
			fseek(file, 0, SEEK_END);
			int file_length = ftell(file);
			reponse_header += "Content-Length: " + std::to_string(file_length) + "\r\n";
			reponse_header += "Content-Type: " + getFileType() + "\r\n";
			reponse_header += "Connection: keep-alive\r\n";
			reponse_header += "\r\n";
			memset(reponse_buffer, 0, sizeof(reponse_buffer));
			sprintf_s(reponse_buffer, reponse_buffer_size, "%s%s", status_line.data(), reponse_header.data());
	puts(reponse_buffer);
			send(client_socket, reponse_buffer, strlen(reponse_buffer), 0);
			if (method == HEAD)
				return true;
			fseek(file, 0, SEEK_SET);
			int readNum;
			while (!feof(file))
			{
				readNum= fread_s(reponse_buffer, reponse_buffer_size + 50, sizeof(char), reponse_buffer_size, file);
				send(client_socket, reponse_buffer, readNum, 0);
			}
			fclose(file);
		}
		else
		{
			status_line = "HTTP/1.1 404 Not Found\r\n";
			reponse_header += "Content-Type: text/html;charset=UTF-8\r\n";
			string entity = "<html><head><title>Not Found</title></head><body><b>404</b><br>Not Found Request Source</body></html>";
			reponse_header += "Content-Length: " + std::to_string(entity.length());
			reponse_header += "\r\n";
			reponse_header += "Connection: close\r\n";
			reponse_header += "\r\n";
			if (method == HEAD)
				entity = "";
			memset(reponse_buffer, 0, sizeof(reponse_buffer));
			sprintf_s(reponse_buffer, reponse_buffer_size, "%s%s%s", status_line.data(), reponse_header.data(), entity.data());
			puts(reponse_buffer);
			send(client_socket, reponse_buffer, strlen(reponse_buffer), 0);
			return false;
		}
	}
	puts("end a request!!");
	return true;
}


// 打开URL所申请的资源文件
bool TCPconnect::openFile()
{
	fopen_s(&file, abs_path.data(), "rb");
	if (file == NULL)
		return false;
	return true;
}

string TCPconnect::getFileType()
{
	int pos = abs_path.find_last_of(".");
	string type = abs_path.substr(pos + 1);
	for (unsigned i = 0; i < type.length(); i++)
		if ('A' <= type[i] && type[i] <= 'Z')
			type[i] -= 'A' - 'a';
	//std::cout << "file type: " << type << std::endl;
	string mine_type = "";
	if (type == "htm")
		mine_type = "text/html;charset=UTF-8";
	else if (type == "js")
		mine_type = "application/x-javascript";
	else if (type == "gif")
		mine_type = "image/gif";
	else if (type == "bmp")
		mine_type = "image/bmp";
	else if (type == "jpg")
		mine_type = "image/jpeg";
	else if (type == "ico")
		mine_type = "image/x-icon";
	else if (type == "html")
		mine_type = "text/html";
	else if (type == "mp3")
		mine_type = "audio/mpeg";
	else if (type == "css")
		mine_type = "text/css";
	else
		mine_type = "*/*";
	return mine_type;
}