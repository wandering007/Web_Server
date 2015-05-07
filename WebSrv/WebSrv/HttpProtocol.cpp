#include "HttpProtocol.h"

#define DATA_BUFSIZE 2048
#define IPADDR "192.168.191.3"
#define HTTPPORT 80
#define ROOTDIR "F:\\WebTest"

// ��������ʱ�������ת��
char *week[] = {		
	"Sun,",  
	"Mon,",
	"Tue,",
	"Wed,",
	"Thu,",
	"Fri,",
	"Sat,",
};
 
// ��������ʱ����·�ת��
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
	{//������
		printf("WSAStartup failed: %d\n", iResult);
        return false;
	}
	//���汾
	if(wsaData.wVersion != wVersionRequested)
	{//������
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
	//��ʼ��content-type���ļ���׺��Ӧ��ϵ��map
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
	// ��ʼ��map
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
		// �������
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
	// ���ͻ��������ַת��Ϊ�õ�ָ��IP��ַ
	printf("%s Connecting on socket:%d\n", inet_ntoa(SockAddr.sin_addr), pReq->Socket);

	char buf[DATA_BUFSIZE] = "";
	bool nRet;

	// ����request data
	if (!RecvRequest(pReq, buf, sizeof(buf)))
	{
		Disconnect(pReq);
		delete pReq;
		return false;
	}
	 // ����request��Ϣ
	Analyse(pReq, buf);
	if(!strcmp(HTTP_STATUS_NOTIMPLEMENTED, pReq->StatuCodeReason))
	{
		printf("Request method not implemented\n");
		return false;
	}
	// ���ɲ�����ͷ��
	if(!SendHeader(pReq))
	{
		printf("Send header failed\n");
		return false;
	}

	// ��client��������
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

// ����request��Ϣ
void  CHttpProtocol::Analyse(PREQUEST pReq, char* pBuf)
{
	// �������յ�����Ϣ
	strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_OK);
	char szSeps[] = " \n";
	char *cpToken;
	char *p;
	// ��ֹ�Ƿ�����
	if (strstr((const char *)pBuf, "..") != NULL)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_BADREQUEST);
		return;
	}
	// �ж�request��method	
	cpToken = strtok_s(pBuf, szSeps, &p);	// �������ַ����ֽ�Ϊһ���Ǵ���	
	if (0 == strcmp(cpToken, "GET"))	// GET����
		pReq->nMethod = METHOD_GET;
	else if (0 == strcmp(cpToken, "HEAD")) // HEAD����
		pReq->nMethod = METHOD_HEAD;  
	else  
    {
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_NOTIMPLEMENTED);
		return;
	}
    
	// ��ȡRequest-URL
	cpToken = strtok_s(NULL, szSeps, &p);
	if (cpToken == NULL)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_BADREQUEST);
		return;
	}

	strcpy_s(pReq->szFileName, strRootDir);
	if (strlen(cpToken) > 1)// "\***"
		strcat_s(pReq->szFileName, cpToken);	// �Ѹ��ļ�����ӵ���β���γ�·��
	else
		strcat_s(pReq->szFileName, "/index.html");
	return;
}

//����ͷ��
bool CHttpProtocol::SendHeader(PREQUEST pReq)
{
	int iResult;
	char Header[2048];
	while(false == FileExist(pReq))
	{//�ļ������ڣ����ĵ������־�ļ�·��
		strcpy_s(pReq->szFileName, strRootDir);
		strcat_s(pReq->szFileName, "/error.html");
	}
	GetCurTime(Time);
	//ȡ���ļ�����
	DWORD length;
	length = GetFileSize(pReq->hFile, NULL);
	//ȡ���ļ���last-modifiedʱ��
	char last_modified[100];
	GetLastModified(pReq->hFile, (char*)last_modified);
	//ȡ���ļ�������
	char ContenType[100];
	GetContenType(pReq, (char*)ContenType);
	sprintf_s(Header, sizeof(Header), "HTTP/1.0 %s\r\nDate: %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %d\r\nLast-Modified: %s\r\n\r\n",  HTTP_STATUS_OK, 
			Time,				    // Date
			"My Web Server", // Server
			ContenType,				// Content-Type
			length,					// Content-length
			last_modified);			// Last-Modified
	//����ͷ��
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
	//����ļ������ڣ��򷵻س�����Ϣ
	if(pReq->hFile == INVALID_HANDLE_VALUE)
	{
		strcpy_s(pReq->StatuCodeReason, HTTP_STATUS_NOTFOUND);
		return false;
	}
	return true;
}

//�����ļ�
void CHttpProtocol::SendFile(PREQUEST pReq)
{
	static char buf[2048]= {""};
	DWORD dwRead;
	BOOL fRet;
	//��д����ֱ�����
	while (true)
	{//��file�ж��뵽buffer��
		fRet = ReadFile(pReq->hFile, buf, sizeof(buf), &dwRead, NULL);
		if(!fRet)
		{
			static char szMsg[512];
			wsprintf(szMsg, "%s", HTTP_STATUS_SERVERERROR);
			//��ͻ��˷��ͳ�����Ϣ
			send(pReq->Socket, szMsg, strlen(szMsg), 0);
			break;
		}		
		//���
		if(0 == dwRead)
			break;
		//��bufffer���ݴ��͸�client
		if(!SendBuffer(pReq, buf, dwRead))
			break;		
		pReq->dwSend += dwRead;
	}
	// �ر��ļ�
	if (CloseHandle(pReq->hFile))
	{
		pReq->hFile = INVALID_HANDLE_VALUE;
	}
	else
		printf("Error occurs when closing file\n");
	return;
}

bool CHttpProtocol::SendBuffer(PREQUEST pReq, char* pBuf, DWORD dwBufSize)
{//���ͻ����е�����
	int iResult = send(pReq->Socket, pBuf, strlen(pBuf), 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        return false;
    }
    pReq->dwSend += iResult;
	return true;
}

bool CHttpProtocol::GetContenType(PREQUEST pReq, LPSTR type)
{// ȡ���ļ�������
    char* cpToken;
    cpToken = strstr(pReq->szFileName, ".");
    strcpy_s(pReq->postfix, cpToken);
	// �����������ļ����Ͷ�Ӧ��content-type
	map<char*, char *>::iterator it = typeMap.find(pReq->postfix);
	if(it != typeMap.end())
	{
		wsprintf(type,"%s",(*it).second);
	}
	return true;
}

void CHttpProtocol::Disconnect(PREQUEST pReq)
{// �ر��׽��֣��ͷ���ռ�е���Դ
	int	nRet;
	printf("Closing socket: %d\n", pReq->Socket);
	nRet = closesocket(pReq->Socket);
	WSACleanup();
	if (nRet == SOCKET_ERROR)
	{// �������
		printf("closesocket() error: %d\n", WSAGetLastError());
	}
	printf("\nBytes received: %u\n", pReq->dwRecv);
	printf("Bytes Send: %u\n\n", pReq->dwSend);
	return;
}

// �����ʱ��
void CHttpProtocol::GetCurTime(LPSTR Time)
{
	// �����ʱ��
	SYSTEMTIME st;
	GetLocalTime(&st);
	// ʱ���ʽ��
    wsprintf(Time, "%s %02d %s %d %02d:%02d:%02d GMT",week[st.wDayOfWeek], st.wDay,month[st.wMonth-1],
     st.wYear, st.wHour, st.wMinute, st.wSecond);
}

bool CHttpProtocol::GetLastModified(HANDLE hFile, LPSTR Time)
{
	// ����ļ���last-modified ʱ��
	FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stCreate;
	FILETIME ftime;
	// ����ļ���last-modified��UTCʱ��
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
		return false;
	FileTimeToLocalFileTime(&ftWrite,&ftime);
	// UTCʱ��ת���ɱ���ʱ��
    FileTimeToSystemTime(&ftime, &stCreate);
	// ʱ���ʽ��
	wsprintf(Time, "%s %02d %s %d %02d:%02d:%02d GMT", week[stCreate.wDayOfWeek],
		stCreate.wDay, month[stCreate.wMonth-1], stCreate.wYear, stCreate.wHour,
		stCreate.wMinute, stCreate.wSecond);
}
