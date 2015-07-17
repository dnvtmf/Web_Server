#include "stdafx.h"
#include "FileServer.h"

using namespace std;

FileServer::FileServer()
{
}

FileServer::FileServer(const char* dir)
{
	CurDir = dir;
}

FileServer::~FileServer()
{
}

int FileServer::deal()
{
	dir_content = CurDir;
	if (CurDir == "/")
		CurDir = "web_content";
	else
		CurDir = "web_content" + CurDir;
	if (stat(CurDir.data(), &file_status) < 0)
		return -1;
	if (file_status.st_mode & S_IFREG)
	{
		dir_content = CurDir;
		return 0;
	}
	dir_content += "\n<br>\n<table>\n";
	dir_content += "<tr><td>Name</td><td width=150>Last modified</td><td>Size</td></tr>\n<hr>\n";
	findAllFile();
	dir_content += "</table>";
	return 1;
}

void FileServer::findAllFile()
{
	struct stat fs;
	char buf[300];
	WCHAR wbuf[300];
	swprintf_s(wbuf, L"%S/*.*", CurDir.data());
	struct tm mTime;
	finder = FindFirstFile(wbuf, &finder_data);
	if (CurDir != "web_content")
	{
		dir_content += "<tr><td><img src=\"http://releases.ubuntu.com/icons/back.gif\"><button id = \"";
		dir_content += CurDir.substr(11, CurDir.find_last_of("/") - 10);
		dir_content += ("\">..</button></td></tr>");
	}
	while (true)
	{
		if (finder_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{//目录
			if (finder_data.cFileName[0] != '.')
			{
				wcharTochar(finder_data.cFileName, buf);
				string path = CurDir + "/" + buf;
				stat(path.data(), &fs);
				dir_content += "<tr><td><img src=\"http://releases.ubuntu.com/icons/folder.gif\"><button id=\"";
				dir_content += path.substr(11);
				dir_content += "\">";
				dir_content += buf;
				dir_content += "</button></td><td>";
				localtime_s(&mTime, &fs.st_mtime);
				strftime(buf, 300, "%a,%d-%b-%Y", &mTime);
				dir_content += buf;
				dir_content += "</td><td>-</td></tr>\n";
			}
		}
		else if (finder_data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{//文件
			wcharTochar(finder_data.cFileName, buf);
			string path = CurDir + "/" + buf;
			stat(path.data(), &fs);
			dir_content += "<tr><td><img src=\"http://releases.ubuntu.com/cdicons/list.png\"><a href=\"";
			dir_content += path.substr(11);
			dir_content += "\" target=\"_black\">";
			dir_content += buf;
			dir_content += "</a></td><td>";
			localtime_s(&mTime, &fs.st_mtime);
			strftime(buf, 300, "%a,%d-%b-%Y", &mTime);
			dir_content += buf;
			dir_content += "</td><td>";
			sprintf_s(buf, 300, "%dB", fs.st_size);
			dir_content += buf;
			dir_content += "</td></tr>\n";
		}
		if (FindNextFile(finder, &finder_data) == false)
			break;
	}
	FindClose(finder);
}

 const char* FileServer::get_response()
{
	//cout << dir_content << endl;
	return dir_content.data();
}

 void FileServer::wcharTochar(const WCHAR* tch, char* ch)
 {
	 int iLength;
	 //获取字节长度   
	 iLength = WideCharToMultiByte(CP_UTF8, 0, tch, -1, NULL, 0, NULL, NULL);
	 //将tchar值赋给_char    
	 WideCharToMultiByte(CP_UTF8, 0, tch, -1, ch, iLength, NULL, NULL);
 }