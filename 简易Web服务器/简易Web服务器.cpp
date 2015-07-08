/**********************************************
*工程名：Web服务器
*作者：万笛文
*学号：2013060102008
*时间：2015年7月
*描述：设计Web服务器：
*	（a）开发多线程服务器通信程序，可接收远程数据，每个线程对应一个TCP链接；
*	（b)定义HTTP请求/响应数据传输格式；
*	（c）开发数据解析模块；
*	（d）开发与HTTP的GET和POST等方法对应的服务器处理程序；
*	（e）根据请求报文处理完成后，发送响应报文到客户端，若服务器出错也发送相应错误响应。
**********************************************/
#include "stdafx.h"
#include "Server.h"

int main(int argc, char* argv[])
{
	Server mServer;
	mServer.start();
	mServer.running();
	mServer.end();
	return 0;
}

