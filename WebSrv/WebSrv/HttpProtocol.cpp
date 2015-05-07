#include "HttpProtocol.h"

#define DATA_BUFSIZE 2048
#define IPADDR "192.168.191.3"
#define HTTPPORT 80
#define ROOTDIR "F:\\WebTest"

// 格林威治时间的星期转换
char *week[] = {		
	"Sun,",  
	"Mon,",
	"Tue,",
	"Wed,",
	"Thu,",
	"Fri,",
	"Sat,",
};
 
// 格林威治时间的月份转换
char *month[] = 
{	
	"Jan",  
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

CHttpProtocol::~CHttpProtocol(void)
{
}

bool CHttpProtocol::StartHttpSrv()
{
	WORD wVersionRequested = WINSOCK_VERSION;
	WSADATA wsaData;
	int iResult = WSAStartup(wVersionRequested, &wsaData);
	if(iResult)
	{//错误处理
		printf("WSAStartup failed: %d\n", iResult);
        return false;
	}
	//检测版本
	if(wsaData.wVersion != wVersionRequested)
	{//错误处理
		printf("Wrong WinSock Version\n");
		return false;
	}

	SOCKADDR_IN service;
	 // Create a SOCKET for listening for 
    // incoming connection requests
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket function failed with error: %d\n", WSAGetLastError());
        WSACleanup();
        return false;
    }
    //----------------------
    // The sockaddr_in structure specifies the address family,
    // IP address, and port for the socket that is being bound.
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(IPADDR);//change it!!
    service.sin_port = htons(HTTPPORT);
	sprintf_s(strRootDir, sizeof(strRootDir), ROOTDIR);
	printf("Server IP: %s  Port: %d\n", IPADDR, HTTPPORT);
	printf("Root Directory: %s\n", strRootDir);
	//初始化content-type和文件后缀对应关系的map
	CreateTypeMap();
	//bind the socket
	iResult = bind(ListenSocket, (SOCKADDR *) &service, sizeof (service));
    if (iResult == SOCKET_ERROR) 
	{
        printf("bind failed with error %u\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }
	iResult = listen(ListenSocket, SOMAXCONN);
	if(iResult == SOCKET_ERROR) 
	{
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
	printf("The web server starting...\n\n");
	   
	return true;
}


void CHttpProtocol::CreateTypeMap()
{
	// 初始化map
    typeMap[".doc"]	= "application/msword";
	typeMap[".bin"]	= "application/octet-stream";
	typeMap[".dll"]	= "application/octet-stream";
	typeMap[".exe"]	= "application/octet-stream";
	typeMap[".pdf"]	= "application/pdf";
	typeMap[".class"] = "application/x-java-class";
	typeMap[".zip"]	= "application/zip";
	typeMap[".aif"]	= "audio/aiff";
	typeMap[".au"]	= "audio/basic";
	typeMap[".snd"]	= "audio/basic";
	typeMap[".mid"]	= "audio/midi";
	typeMap[".rmi"]	= "audio/midi";
	typeMap[".mp3"]	= "audio/mpeg";
	typeMap[".vox"]	= "audio/voxware";
	typeMap[".wav"]	= "audio/wav";
	typeMap[".ra"]	= "audio/x-pn-realaudio";
	typeMap[".ram"]	= "audio/x-pn-realaudio";
	typeMap[".bmp"]	= "image/bmp";
	typeMap[".gif"]	= "image/gif";
	typeMap[".jpeg"] = "image/jpeg";
	typeMap[".jpg"]	= "image/jpeg";
	typeMap[".tif"]	= "image/tiff";
	typeMap[".tiff"] = "image/tiff";
	typeMap[".xbm"]	= "image/xbm";
	typeMap[".wrl"]	= "model/vrml";
	typeMap[".htm"]	= "text/html";
	typeMap[".html"] = "text/html";
	typeMap[".c"] = "text/plain";
	typeMap[".cpp"]	= "text/plain";
	typeMap[".def"]	= "text/plain";
	typeMap[".h"] = "text/plain";
	typeMap[".txt"]	= "text/plain";
	typeMap[".rtx"]	= "text/richtext";
	typeMap[".rtf"]	= "text/richtext";
	typeMap[".java"] = "text/x-java-source";
	typeMap[".css"]	= "text/css";
	typeMap[".mpeg"] = "video/mpeg";
	typeMap[".mpg"]	= "video/mpeg";
	typeMap[".mpe"]	= "video/mpeg";
	typeMap[".avi"]	= "video/msvideo";
	typeMap[".mov"]	= "video/quicktime";
	typeMap[".qt"]	= "video/quicktime";
	typeMap[".shtml"] = "wwwserver/html-ssi";
	typeMap[".asa"]	= "wwwserver/isapi";
	typeMap[".asp"]	= "wwwserver/isapi";
	typeMap[".cfm"]	= "wwwserver/isapi";
	typeMap[".dbm"]	= "wwwserver/isapi";
	typeMap[".isa"]	= "wwwserver/isapi";
	typeMap[".plx"]	= "wwwserver/isapi";
	typeMap[".url"]	= "wwwserver/isapi";
	typeMap[".cgi"]	= "wwwserver/isapi";
	typeMap[".php"]	= "wwwserver/isapi";
	typeMap[".wcgi"] = "wwwserver/isapi";
}

bool CHttpProtocol::Connect()
{
	SOCKADDR_IN	SockAddr;
	PREQUEST pReq = new REQUEST;
	if (pReq == NULL)
	{   
		// 处理错误
		printf("No memory for request\n");
		return false;
	}
	pReq->Socket = INVALID_SOCKET;
	pReq->hFile = INVALID_HANDLE_VALUE;
	pReq->dwRecv = 0;
	pReq->dwSend = 0;

	// Accept a client socket
	pReq->Socket = accept(ListenSocket, (LPSOCKADDR)&SockAddr, NULL);
	// No longer need server socket
    closesocket(ListenSocket);

	if (pReq->Socket == INVALID_SOCKET)
	{
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
	// 将客户端网络地址转换为用点分割的IP地址
	printf("%s Connecting on socket:%d\n", inet_ntoa(SockAddr.sin_addr), pReq->Socket);

	char buf[DATA_BUFSIZE] = "";
	bool nRet;

	// 接收request data
	if (!RecvRequest(pReq, buf, sizeof(buf)))
	{
		Disconnect(pReq);
		delete pReq;
		return false;
	}
	 // 分析request信息
	Analyse(pReq, buf);
	if(!strcmp(HTTP_STATUS_NOTIMPLEMENTED, pReq->StatuCodeReason))
	{
		printf("Request method not implemented\n");
		return false;
	}
	// 生成并返回头部
	if(!SendHeader(pReq))
	{
		printf("Send header failed\n");
		return false;
	}

	// 向client传送数据
	if(pReq->nMethod == METHOD_GET)
		SendFile(pReq);
	Disconnect(pReq);
	delete pReq;
	return true;
}

bool CHttpProtocol::RecvRequest(PREQUEST pReq, char* pBuf, DWORD dwBufSize)
{
	int iResult = recv(pReq->Socket, pBuf, DATA_BUFSIZE, 0);
	//printf("%s\n", pBuf);
	if (iResult >= 0)
		pReq->dwRecv += iResult;
    else  
	{
		printf("recv failed with error: %d\n", WSAGetLastError());
		return false;
	}
	return true;
}

// 分析request信息
void  CHttpProtocol::Analyse(PREQUEST pReq, char* pBuf)
{
	// 分析接收到的信息
	strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_OK);
	char szSeps[] = " \n";
	char *cpToken;
	char *p;
	// 防止非法请求
	if (strstr((const char *)pBuf, "..") != NULL)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_BADREQUEST);
		return;
	}
	// 判断request的method	
	cpToken = strtok_s(pBuf, szSeps, &p);	// 缓存中字符串分解为一组标记串。	
	if (0 == strcmp(cpToken, "GET"))	// GET命令
		pReq->nMethod = METHOD_GET;
	else if (0 == strcmp(cpToken, "HEAD")) // HEAD命令
		pReq->nMethod = METHOD_HEAD;  
	else  
    {
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_NOTIMPLEMENTED);
		return;
	}
    
	// 获取Request-URL
	cpToken = strtok_s(NULL, szSeps, &p);
	if (cpToken == NULL)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_BADREQUEST);
		return;
	}

	strcpy_s(pReq->szFileName, strRootDir);
	if (strlen(cpToken) > 1)// "\***"
		strcat_s(pReq->szFileName, cpToken);	// 把该文件名添加到结尾处形成路径
	else
		strcat_s(pReq->szFileName, "/index.html");
	return;
}

//发送头部
bool CHttpProtocol::SendHeader(PREQUEST pReq)
{
	int iResult;
	char Header[2048];
	while(false == FileExist(pReq))
	{//文件不存在，更改到错误标志文件路径
		strcpy_s(pReq->szFileName, strRootDir);
		strcat_s(pReq->szFileName, "/error.html");
	}
	GetCurTime(Time);
	//取得文件长度
	DWORD length;
	length = GetFileSize(pReq->hFile, NULL);
	//取得文件的last-modified时间
	char last_modified[100];
	GetLastModified(pReq->hFile, (char*)last_modified);
	//取得文件的类型
	char ContenType[100];
	GetContenType(pReq, (char*)ContenType);
	sprintf_s(Header, sizeof(Header), "HTTP/1.0 %s\r\nDate: %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nLast-Modified: %s\r\n\r\n",  HTTP_STATUS_OK, 
			Time,				    // Date
			"My Web Server", // Server
			ContenType,				// Content-Type
			length,					// Content-length
			last_modified);			// Last-Modified
	//发送头部
	iResult = send(pReq->Socket, Header, strlen(Header), 0);
	pReq->dwSend += iResult;
	if (iResult == SOCKET_ERROR) 
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		return false;
    }
	return true;
}

bool CHttpProtocol::FileExist(PREQUEST pReq)
{
	pReq->hFile = CreateFile(pReq->szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	//如果文件不存在，则返回出错信息
	if(pReq->hFile == INVALID_HANDLE_VALUE)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_NOTFOUND);
		return false;
	}
	return true;
}

//发送文件
void CHttpProtocol::SendFile(PREQUEST pReq)
{
	static char buf[2048]= {""};
	DWORD dwRead;
	BOOL fRet;
	//读写数据直到完成
	while (true)
	{//从file中读入到buffer中
		fRet = ReadFile(pReq->hFile, buf, sizeof(buf), &dwRead, NULL);
		if(!fRet)
		{
			static char szMsg[512];
			wsprintf(szMsg, "%s", HTTP_STATUS_SERVERERROR);
			//向客户端发送出错信息
			send(pReq->Socket, szMsg, strlen(szMsg), 0);
			break;
		}		
		//完成
		if(0 == dwRead)
			break;
		//将bufffer内容传送给client
		if(!SendBuffer(pReq, buf, dwRead))
			break;		
		pReq->dwSend += dwRead;
	}
	// 关闭文件
	if (CloseHandle(pReq->hFile))
	{
		pReq->hFile = INVALID_HANDLE_VALUE;
	}
	else
		printf("Error occurs when closing file\n");
	return;
}

bool CHttpProtocol::SendBuffer(PREQUEST pReq, char* pBuf, DWORD dwBufSize)
{//发送缓存中的内容
	int iResult = send(pReq->Socket, pBuf, strlen(pBuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        return false;
    }
    pReq->dwSend += iResult;
	return true;
}

bool CHttpProtocol::GetContenType(PREQUEST pReq, LPSTR type)
{// 取得文件的类型
    char* cpToken;
    cpToken = strstr(pReq->szFileName, ".");
    strcpy_s(pReq->postfix, cpToken);
	// 遍历搜索该文件类型对应的content-type
	map<char*, char *>::iterator it = typeMap.find(pReq->postfix);
	if(it != typeMap.end())
	{
		wsprintf(type,"%s",(*it).second);
	}
	return true;
}

void CHttpProtocol::Disconnect(PREQUEST pReq)
{// 关闭套接字：释放所占有的资源
	int	nRet;
	printf("Closing socket: %d\n", pReq->Socket);
	nRet = closesocket(pReq->Socket);
	WSACleanup();
	if (nRet == SOCKET_ERROR)
	{// 处理错误
		printf("closesocket() error: %d\n", WSAGetLastError());
	}
	printf("\nBytes received: %u\n", pReq->dwRecv);
	printf("Bytes Send: %u\n\n", pReq->dwSend);
	return;
}

// 活动本地时间
void CHttpProtocol::GetCurTime(LPSTR Time)
{
	// 活动本地时间
	SYSTEMTIME st;
	GetLocalTime(&st);
	// 时间格式化
    wsprintf(Time, "%s %02d %s %d %02d:%02d:%02d GMT",week[st.wDayOfWeek], st.wDay,month[st.wMonth-1],
     st.wYear, st.wHour, st.wMinute, st.wSecond);
}

bool CHttpProtocol::GetLastModified(HANDLE hFile, LPSTR Time)
{
	// 获得文件的last-modified 时间
	FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stCreate;
	FILETIME ftime;
	// 获得文件的last-modified的UTC时间
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
		return false;
	FileTimeToLocalFileTime(&ftWrite,&ftime);
	// UTC时间转化成本地时间
    FileTimeToSystemTime(&ftime, &stCreate);
	// 时间格式化
	wsprintf(Time, "%s %02d %s %d %02d:%02d:%02d GMT", week[stCreate.wDayOfWeek],
		stCreate.wDay, month[stCreate.wMonth-1], stCreate.wYear, stCreate.wHour,
		stCreate.wMinute, stCreate.wSecond);
}
