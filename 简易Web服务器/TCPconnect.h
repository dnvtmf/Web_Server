#pragma once
#include "stdafx.h"
using std::string;
enum Method{GET, POST, HEAD, UNKNOWN};
#define SERVER "Server: wan's web server\r\n"
class TCPconnect
{
public:
	// ��һ���̣߳�����TCP����
	static void beginConnect(SOCKET t_client_socket);
	static DWORD WINAPI connecting(LPVOID t_client_socket);

private:
	//buffer_size ������64KB
	static const int buffer_size_1K = 1 << 10;//1KB
	static const int buffer_size_32K = 1 << 15;//32KB
	static const int default_buffer_size = buffer_size_32K;
	static const int send_limit = 1 << 20;//����1MΪ��ֵ������1M��ʹ��206

public:
	TCPconnect();
	TCPconnect(SOCKET socket);
	~TCPconnect();
	//��������
	void analyze();
	// ������Ӧ����
	bool response();
private:
	// ��URL���������Դ�ļ�
	bool openFile();
	// �����������Դ��MIME����
	string getFileType();
	// ��õ�ǰ������ʱ��
	string getDate();
	//��������ͷ��
	void analyze_request_header(string field_name, string field_value);
	// ����ͷ���Ĳ�����ΪĬ��ֵ
	void set_default();
	void response_OK();
	void response_Error(string error_no, string error_descript, string error_msg);
	void response_Part_Content();//�����Ƿ����������ļ�
	void response_Not_Modified();
	void response_Not_Implemented();
private:
	SOCKET client_socket;//��TCP���ӵĿͻ���socket
//----------------  request  -----------------//
	char *request_buffer;//�����ĵ�buffer
	int request_buffer_size;//������buffer��С
	Method method;//����ķ���
	string url;//����ľݶ�·��
	bool range;
	int range_min;
	int range_max;
	bool Connect_keep_alive;//�Ƿ񱣳ֳ�������
	//����GET
	bool if_get;
	string if_none_match;
	string if_modified_since;
	//POST����
	int content_length;
//---------------- reponse  ----------------//
	char *response_buffer;//��Ӧ���ĵ�buffer
	int response_buffer_size;//��Ӧ����buffer��С
	string status_line;//״̬��
	string response_header;//��Ӧͷ��
	FILE *file;
	int file_length;
	struct stat file_status;//�ļ�����ϸ��Ϣ
	string last_modified;//�ļ�������޸�ʱ��
	string Etag;//ʵ���ǩ
};

