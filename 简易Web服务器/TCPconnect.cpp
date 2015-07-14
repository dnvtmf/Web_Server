#include "stdafx.h"
#include "TCPconnect.h"
using std::to_string;
using std::cout;
using std::endl;

TCPconnect::TCPconnect()
{
}

TCPconnect::TCPconnect(SOCKET socket)
{
	client_socket = socket;
	request_buffer = new char[default_buffer_size + 50];
	response_buffer = new char[default_buffer_size + 50];
	memset(request_buffer, 0, sizeof(request_buffer));
	memset(response_buffer, 0, sizeof(response_buffer));
	request_buffer_size = default_buffer_size;
	response_buffer_size = default_buffer_size;
}

TCPconnect::~TCPconnect()
{
	closesocket(client_socket);
	delete[] request_buffer;
	delete[] response_buffer;
}

// 打开一个线程，处理TCP连接
void TCPconnect::beginConnect(SOCKET t_client_socket)
{
	if (CreateThread(NULL, 0, &connecting, (LPVOID)t_client_socket, 0, NULL) == NULL)
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
		ret = recv(mTCP.client_socket, mTCP.request_buffer, mTCP.request_buffer_size, 0);
		if (ret == 0 || ret == SOCKET_ERROR)
		{
			puts("client closed");
			break;
		}
		mTCP.analyze();//解析请求报文
		if (!mTCP.response())//返回响应报文
		{
			puts("client closed");
			break;
		}
	}
	return 0;
}

void TCPconnect::analyze()
{
	set_default();//将参数设为初始参数
	string request = string(request_buffer);
	string request_line = request.substr(0, request.find("\r\n"));
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
	url = request_line.substr(i + 1, request_line.find("HTTP/") - i - 2);
	if (method == GET || method == HEAD)
	{
		if (url[0] != '/')
		{//"http://"host[":" port][abs_path ["?" query]]
			url = url.substr(url.find('/', 7), string::npos);
		}
		//去掉询问部分
		url = url.substr(0, url.find('?'));
		if (url == "/")
		{//如果路径为服务器根目录
			url += "index.htm";
		}
	}
	string header;
	int header_begin = request.find("\r\n"), header_next = header_begin;
	//printf("header_begin: %d\n", header_begin);
	while (true)
	{
		header_begin += 2;
		header_next = request.find("\r\n", header_begin);
		int len = header_next - header_begin;
		if (len < 2) break;//请求头部已结束
		header = request.substr(header_begin, len);
		int pos = header.find(':');
		analyze_request_header(header.substr(0, pos), header.substr(pos + 2));
		header_begin = header_next;
	}
	if (content_length > 0)
	{//请求报文中有实体
		int pos = request.find("\r\n\r\n") + 4, len = request.length();
		//puts(request.substr(pos).data());
		if (len - pos >= content_length)
		{
			entity = request.substr(pos, content_length);
		}
		else
		{
			int sum = 0, readNum;
			entity = request.substr(pos);
			sum += len - pos;
			while (sum < content_length)
			{
				readNum = recv(client_socket, request_buffer, request_buffer_size, 0);
				sum += readNum;
				request_buffer[readNum] = 0;
				entity += request_buffer;
			}
		}
	}
}

// 发送响应报文
bool TCPconnect::response()
{
	if (method == UNKNOWN)
		response_Error("400", "Bad Request", "");
	else if (method == HEAD || method == GET)
	{
		if (openFile())
		{
			if (if_get && (if_none_match.find(Etag) != string::npos || if_modified_since.find(last_modified) != string::npos
				|| if_none_match.find("*") != string::npos))
			{
				response_Not_Modified();
			}
			else if (range == false)
				response_OK();
			else if (range_min < file_length)
				response_Part_Content();
			else
				response_Error("416", "Requested Range Not Satisfiable", "Requested Range Not Satisfiable");
		}
		else
			response_Error("404", "Not Found", "the sources you request <b> Not Found</b>!!");
	}
	else if (method == POST)
	{
		Connect_keep_alive = false;
		if (url == "/FileServer")
		{
			FileServer mFS(entity.data());
			int res = mFS.deal();
			if (res < 0)
				response_OK("the path is not correct!!");
			else
				response_OK(mFS.get_response());
		}
		else
			response_Not_Implemented();
	}
	printf("URL: %s\tResponse Status: %s\n\n", url.data(), status_line.data());
	return Connect_keep_alive;
}

// 打开URL所申请的资源文件
bool TCPconnect::openFile()
{
	fopen_s(&file, ("web_content" + url).data(), "rb");
	if (file == NULL)
		return false;
	if (stat(("web_content" + url).data(), &file_status) < 0)
		return false;
	file_length = file_status.st_size;//得到文件的大小
	//得到文件的最后修改时间
	struct tm gmT;
	gmtime_s(&gmT, &file_status.st_mtime);
	char buf[100];
	strftime(buf, sizeof(buf), "%a, %d %b %Y %X GMT", &gmT);
	last_modified = buf;
	Etag = to_string(file_status.st_mtime);
	return true;
}

void TCPconnect::analyze_request_header(string field_name, string field_value)
{
	//std::cout << type << ' ' << content << std::endl;
	if (field_name == "Range")
	{
		range = true;
		puts(field_value.data());
		int num = sscanf_s(field_value.data(), "bytes=%d-%d", &range_min, &range_max);
		if (num == 0) range_min = 0, range_max = -1;
		else if (num == 1) range_max = -1;
		printf("range: %d %d\n", range_min, range_max);
	}
	else if (field_name == "Connection")
	{//分析是持续连接还是非持续连接
		if (field_value == "close")
			Connect_keep_alive = false;
	}
	else if (field_name == "If-Match" || field_name == "If-None-Match")
	{
		if_get = true;
		if_none_match = field_value;
	}
	else if (field_name == "If-Modified-Since" || field_name == "If-Unmodified-Since")
	{
		if_get = true;
		if_modified_since = field_value;
	}
	else if (field_name == "If-Range")
	{
		if_get = true;
		if_none_match = field_value;
		if_modified_since = field_value;
	}
	else if (field_name == "Content-Length")
	{
		sscanf_s(field_value.data(), "%d", &content_length);
	}
}

string TCPconnect::getFileType()
{
	string type = url.substr(url.find_last_of(".") + 1);
	for (unsigned i = 0; i < type.length(); i++)
		if ('A' <= type[i] && type[i] <= 'Z')
			type[i] -= 'A' - 'a';
	string mine_type = "";
	if (type == "htm" || type == "html")
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
	else if (type == "mp3")
		mine_type = "audio/mpeg";
	else if (type == "css")
		mine_type = "text/css";
	else if (type == "png")
		mine_type = "image/webp";
	else
		mine_type = "*/*";
	return mine_type;
}

// 请求头部的参数设为默认值
void TCPconnect::set_default()
{
	method = UNKNOWN;
	range = false;
	range_min = 0;
	Connect_keep_alive = true;
	if_get = false;
	if_none_match = "";
	if_modified_since = "";
	entity = "";
	content_length = 0;
	response_header = "";
}

void TCPconnect::response_OK()
{
	status_line = "HTTP/1.1 200 OK\r\n";
	response_header += "Content-Length: " + to_string(file_length) + "\r\n";
	response_header += "Content-Type: " + getFileType() + "\r\n";
	response_header += "Content-Loction: " + url + "\r\n";
	if (Connect_keep_alive)
		response_header += "Connection: keep-alive\r\n";
	else
		response_header += "Connection: close\r\n";
	response_header += getDate();
	response_header += "Last-Modified: " + last_modified + "\r\n";
	response_header += SERVER;
	response_header += "\r\n";
	sprintf_s(response_buffer, response_buffer_size, "%s%s", status_line.data(), response_header.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);
	//puts(response_buffer);

	if (method != HEAD)
	{
		fseek(file, 0, SEEK_SET);
		int readNum;
		while (!feof(file))
		{
			readNum = fread_s(response_buffer, response_buffer_size + 50, sizeof(char), response_buffer_size, file);
			if (send(client_socket, response_buffer, readNum, 0) == SOCKET_ERROR)
			{
				puts("send data error");
			}
		}
	}
	fclose(file);
}

void TCPconnect::response_OK(const char *mEntity)
{
	file_length = strlen(mEntity);
	status_line = "HTTP/1.1 200 OK\r\n";
	response_header += "Content-Length: " + to_string(file_length) + "\r\n";
	response_header += "Content-Type: text/html;charset=UTF-8\r\n";
	if (Connect_keep_alive)
		response_header += "Connection: keep-alive\r\n";
	else
		response_header += "Connection: close\r\n";
	response_header += getDate();
	response_header += SERVER;
	response_header += "\r\n";
	sprintf_s(response_buffer, response_buffer_size, "%s%s", status_line.data(), response_header.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);
	//puts(response_buffer);

	if (send(client_socket, mEntity, file_length, 0) == SOCKET_ERROR)
	{
		puts("send data error");
	}
}
void TCPconnect::response_Error(string error_no, string error_descript, string error_msg)
{
	status_line = "HTTP/1.1 " + error_no + " " + error_descript + "\r\n";
	response_header += "Content-Type: text/html;charset=UTF-8\r\n";
	string entity = "<html><head><title>" + error_descript + "</title></head><body><b>" + error_no + " " + error_descript + "</b><br>" + error_msg + "</body></html>";
	response_header += "Content-Length: " + std::to_string(entity.length()) + "\r\n";
	response_header += "Connection: close\r\n";
	response_header += getDate();
	response_header += SERVER;
	//puts(response_buffer);
	response_header += "\r\n";
	if (method == HEAD)
		entity = "";
	memset(response_buffer, 0, sizeof(response_buffer));
	sprintf_s(response_buffer, response_buffer_size, "%s%s%s", status_line.data(), response_header.data(), entity.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);

	Connect_keep_alive = false;
}

void TCPconnect::response_Part_Content()
{
	if (range_max == -1)
		range_max = file_length - 1;
	int content_length = range_max - range_min + 1;
	status_line = "HTTP/1.1 206 Part Content\r\n";
	response_header += "Content-Length: " + to_string(content_length) + "\r\n";
	response_header += "Content-Type: " + getFileType() + "\r\n";
	response_header += "Content-Range: bytes " + to_string(range_min) + "-" + to_string(range_max) + "/" + to_string(file_length) + "\r\n";
	response_header += "Content-Loction: " + url + "\r\n";
	response_header += getDate();
	response_header += "Last-Modified: " + last_modified + "\r\n";
	response_header += SERVER;
	response_header += "Vary: Content-Location\r\n";
	if (Connect_keep_alive)
		response_header += "Connection: keep-alive\r\n";
	else
		response_header += "Connection: close\r\n";
	response_header += "\r\n";
	sprintf_s(response_buffer, response_buffer_size, "%s%s", status_line.data(), response_header.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);

	printf("Send Date: %d-%d/%d\n", range_min, range_max, file_length);

	if (method == GET)
	{
		fseek(file, range_min, SEEK_SET);
		int readNum, fpos = ftell(file);
		while (fpos <= range_max)
		{
			if (range_max - fpos + 1 >= response_buffer_size)
			{
				readNum = fread_s(response_buffer, response_buffer_size + 50, sizeof(char), response_buffer_size, file);
			}
			else
			{
				readNum = fread_s(response_buffer, response_buffer_size + 50, sizeof(char), range_max - fpos + 1, file);
			}
			if (send(client_socket, response_buffer, readNum, 0) == SOCKET_ERROR)
			{
				puts("send data error");
			}
			fpos = ftell(file);
		}
		fclose(file);
	}
}

void TCPconnect::response_Not_Modified()
{
	status_line = "HTTP/1.1 304 Not Modified\r\n";
	response_header += "Content-Type: text/html;charset=UTF-8\r\n";
	response_header += "Content-Length: 0\r\n";
	response_header += "Connection: close\r\n";
	response_header += getDate();
	response_header += SERVER;
	response_header += "\r\n";
	sprintf_s(response_buffer, response_buffer_size, "%s%s", status_line.data(), response_header.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);
}

void TCPconnect::response_Not_Implemented()
{
	status_line = "HTTP/1.1 501 Not Implemented\r\n";
	response_header += "Content-Type: text/html;charset=UTF-8\r\n";
	response_header += "Content-Length: 0\r\n";
	response_header += "Connection: close\r\n";
	response_header += getDate();
	response_header += SERVER;
	response_header += "\r\n";
	sprintf_s(response_buffer, response_buffer_size, "%s%s", status_line.data(), response_header.data());
	send(client_socket, response_buffer, strlen(response_buffer), 0);
}
// 获得当前服务器时间
string TCPconnect::getDate()
{
	struct tm gmTime;
	time_t curTime;
	time(&curTime);
	gmtime_s(&gmTime, &curTime);
	char date_buf[100];
	strftime(date_buf, sizeof(date_buf), "Date: %a, %d %b %Y %X GMT\r\n", &gmTime);
	return date_buf;
}