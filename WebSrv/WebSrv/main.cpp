#include "HttpProtocol.h"

int main()
{
	CHttpProtocol Server;
	while(!Server.StartHttpSrv());
	if(Server.Connect())
		printf("成功处理一个HTTP请求，程序结束！\n");
	system("pause");
	return 0;
}