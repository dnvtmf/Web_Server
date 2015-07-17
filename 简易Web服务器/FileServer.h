#pragma once
class FileServer
{
public:
	FileServer();
	FileServer(const char* dir);
	~FileServer();
	int deal();
	const char* get_response();
private:
	//�õ���ǰ·���µ���������
	void findAllFile();
	void wcharTochar(const WCHAR* tch, char * ch);
private:
	string CurDir;//��ǰĿ¼
	WIN32_FIND_DATA finder_data;
	HANDLE finder;
	struct stat file_status;//�ļ�״̬
	string dir_content;//��ǰĿ¼�е�����
};

