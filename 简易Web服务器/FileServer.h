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
	//得到当前路径下的所有内容
	void findAllFile();
	void wcharTochar(const WCHAR* tch, char * ch);
private:
	string CurDir;//当前目录
	WIN32_FIND_DATA finder_data;
	HANDLE finder;
	struct stat file_status;//文件状态
	string dir_content;//当前目录中的内容
};

