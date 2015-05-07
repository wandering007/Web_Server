#ifndef _HTTPPROTOCOL_H
#define _HTTPPROTOCOL_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string.h>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

#define METHOD_GET 0
#define METHOD_HEAD 1

#define HTTP_STATUS_OK				"200 OK"
#define HTTP_STATUS_CREATED			"201 Created"
#define HTTP_STATUS_ACCEPTED		"202 Accepted"
#define HTTP_STATUS_NOCONTENT		"204 No Content"
#define HTTP_STATUS_MOVEDPERM		"301 Moved Permanently"
#define HTTP_STATUS_MOVEDTEMP		"302 Moved Temporarily"
#define HTTP_STATUS_NOTMODIFIED		"304 Not Modified"
#define HTTP_STATUS_BADREQUEST		"400 Bad Request"
#define HTTP_STATUS_UNAUTHORIZED	"401 Unauthorized"
#define HTTP_STATUS_FORBIDDEN		"403 Forbidden"
#define HTTP_STATUS_NOTFOUND		"404 File can not fonund!"
#define HTTP_STATUS_SERVERERROR		"500 Internal Server Error"
#define HTTP_STATUS_NOTIMPLEMENTED	"501 Not Implemented"
#define HTTP_STATUS_BADGATEWAY		"502 Bad Gateway"
#define HTTP_STATUS_UNAVAILABLE		"503 Service Unavailable"
using namespace std;

typedef struct REQUEST
{
	SOCKET Socket; //请求的socket
	int nMethod; //请求的使用方法：GET或HEAD
	DWORD dwRecv; //收到的字节数
	DWORD dwSend; //发送的字节数
	HANDLE hFile; //请求连接的文件
	char szFileName[_MAX_PATH]; //文件的相对路径
	char postfix[10]; //存储扩展名
	char StatuCodeReason[100]; //头部的status code和reason phrase 
	bool permitted; //用户权限判断
	char* authority; //用户提供的认证信息
	char key[1024]; //正确认证信息
}REQUEST, *PREQUEST;

typedef struct HTTPSTATS
{
	DWORD dwRecv; //收到字节数
	DWORD dwSend; //发送字节数
}HTTPSTATS, *PHTTPSTATE;

class CHttpProtocol
{
public:
	SOCKET ListenSocket;
	map<char*, char*> typeMap;
	char strRootDir[50]; //web的根目录
	char Time[50];

public:
	bool StartHttpSrv();
	bool Connect();
	bool RecvRequest(PREQUEST pReq, char* pBuf, DWORD dwBufSize);
	void Analyse(PREQUEST pReq, char* pBuf);
	void Disconnect(PREQUEST pReq);
	void CreateTypeMap();
	bool SendHeader(PREQUEST pReq);
	bool FileExist(PREQUEST pReq);

	void GetCurTime(LPSTR Time);
	bool GetLastModified(HANDLE hFile, LPSTR Time);
	bool GetContenType(PREQUEST pReq, LPSTR type);
	void SendFile(PREQUEST pReq);
	bool SendBuffer(PREQUEST pReq, char* pBuf, DWORD dwBufSize);

public:
	~CHttpProtocol(void);
};

#endif _HTTPPROTOCOL_H;