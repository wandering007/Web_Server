#include "HttpProtocol.h"

int main()
{
	CHttpProtocol Server;
	while(!Server.StartHttpSrv());
	if(Server.Connect())
		printf("�ɹ�����һ��HTTP���󣬳��������\n");
	system("pause");
	return 0;
}