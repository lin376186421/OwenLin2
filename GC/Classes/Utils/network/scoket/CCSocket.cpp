/*
 * Source file about portable socket class. 
 *
 * 
 */
#include "CCsocket.h"
#include <stdio.h>
#include <queue>
#include "cocos2d.h"

using namespace cocos2d;

#ifdef WIN32
	#pragma comment(lib, "wsock32")
#endif


typedef struct _AsyncRecvStruct
{
	int		m_nRecvBufLen;
	char	    m_szRecvBuf[MAX_SOCKETMSG_BUFF + 1];/*接收缓存区*/
} AsyncRecvStruct;

static std::queue<AsyncRecvStruct*>		*s_pAsyncStructQueue;

CCSocket::CCSocket(SOCKET sock)
{
	m_sock = sock;
	//m_pNotifyProc = NULL;
	bConnect = false;
	disConnect = false;
	m_nRecvBufLen = 0;
	m_shSeed = 1606;
	memset(m_szRecvBuf,0,MAX_SOCKETMSG_BUFF+1);

	s_pAsyncStructQueue = new std::queue<AsyncRecvStruct*>();

	sp_thread_mutex_init(&m_mutex, NULL);
	sp_thread_attr_t attr;

	// 发送线程
	sp_thread_attr_init(&attr);
	sp_thread_attr_setdetachstate(&attr, SP_THREAD_CREATE_DETACHED);
	sp_thread_create(&m_mutexThreadId, &attr, CCSocket::RecvFunc, this);
	sp_thread_attr_destroy(&attr);
    
	//Init();
}

CCSocket::~CCSocket()
{
	sp_thread_mutex_destroy(&m_mutex);
}

int CCSocket::Init()
{
#ifdef WIN32
	/*
	http://msdn.microsoft.com/zh-cn/vstudio/ms741563(en-us,VS.85).aspx

	typedef struct WSAData { 
		WORD wVersion;								//winsock version
		WORD wHighVersion;							//The highest version of the Windows Sockets specification that the Ws2_32.dll can support
		char szDescription[WSADESCRIPTION_LEN+1]; 
		char szSystemStatus[WSASYSSTATUS_LEN+1]; 
		unsigned short iMaxSockets; 
		unsigned short iMaxUdpDg; 
		char FAR * lpVendorInfo; 
	}WSADATA, *LPWSADATA; 
	*/
	WSADATA wsaData;
	//#define MAKEWORD(a,b) ((WORD) (((BYTE) (a)) | ((WORD) ((BYTE) (b))) << 8)) 
	WORD version = MAKEWORD(2, 0);
	int ret = WSAStartup(version, &wsaData);//win sock start up
	if ( ret ) {
		//cerr << "Initilize winsock error !" << endl;
		return -1;
	}
#endif
	
	return 0;
}
//this is just for windows
int CCSocket::Clean()
{
#ifdef WIN32
		return (WSACleanup());
#endif
		return 0;
}

CCSocket& CCSocket::operator = (SOCKET s)
{
	m_sock = s;
	return (*this);
}

CCSocket::operator SOCKET ()
{
	return m_sock;
}
//create a socket object win/lin is the same
// af:
bool CCSocket::Create(int af, int type, int protocol)
{
	m_sock = socket(af, type, protocol);
	if ( m_sock == INVALID_SOCKET ) {
		return false;
	}
	//m_pNotifyProc = pNotifyProc;
	return true;
}

bool CCSocket::Connect(const char* ip, unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip);
	svraddr.sin_port = htons(port);
	int ret = connect(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if ( ret == SOCKET_ERROR ) {
		return false;
	}

	#ifdef WIN32
	unsigned long flag=1;
	if (ioctlsocket(m_sock,FIONBIO,&flag)!=0)
	{
		Close();
		return false;
	} 
	#else
	fcntl(m_sock, F_SETFL, O_NONBLOCK);
	#endif

	bConnect = true;

	return true;
}

bool CCSocket::Bind(unsigned short port)
{
	struct sockaddr_in svraddr;
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = INADDR_ANY;
	svraddr.sin_port = htons(port);

	int opt =  1;
	if ( setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0 ) 
		return false;

	int ret = bind(m_sock, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if ( ret == SOCKET_ERROR ) {
		return false;
	}
	return true;
}
//for server
bool CCSocket::Listen(int backlog)
{
	int ret = listen(m_sock, backlog);
	if ( ret == SOCKET_ERROR ) {
		return false;
	}
	return true;
}

bool CCSocket::Accept(CCSocket& s, char* fromip)
{
	struct sockaddr_in cliaddr;
	socklen_t addrlen = sizeof(cliaddr);
	SOCKET sock = accept(m_sock, (struct sockaddr*)&cliaddr, &addrlen);
	if ( sock == SOCKET_ERROR ) {
		return false;
	}

	s = sock;
	if ( fromip != NULL )
		sprintf(fromip, "%s", inet_ntoa(cliaddr.sin_addr));

	return true;
}

int CCSocket::Send(const char* buf, int len, int flags)
{
	int bytes;
	int count = 0;

	if (bConnect)
	{
		while ( count < len ) {

			bytes = send(m_sock, buf + count, len - count, flags);
			if ( bytes == -1 || bytes == 0 )
				return -1;
			count += bytes;
		} 

	}

	return count;
}

int CCSocket::SendPack(const char* buf, int len, int flags)
{
	int nOutLen;
	char* pOutData = (char*)malloc(len+6);
	memset(pOutData,0,len+6);
	int outlen = PackPacket(buf, len, pOutData, nOutLen);
	//#ifdef WIN32
	Send(pOutData, outlen, 0);
	//#else
	//Send(pOutData, outlen, MSG_NOSIGNAL);
	//#endif

	free(pOutData);

	return outlen;
}

int CCSocket::Recv(char* buf, int len, int flags)
{
	return (recv(m_sock, buf, len, flags));
}

int CCSocket::Close()
{
	bConnect = false;
#ifdef WIN32
	return (closesocket(m_sock));
#else
	return (close(m_sock));
#endif
}

int CCSocket::GetError()
{
#ifdef WIN32
	return (WSAGetLastError());
#else
	return (errno);
#endif
}

bool CCSocket::DnsParse(const char* domain, char* ip)
{
	struct hostent* p;
	if ( (p = gethostbyname(domain)) == NULL )
		return false;
		
	sprintf(ip, 
		"%u.%u.%u.%u",
		(unsigned char)p->h_addr_list[0][0], 
		(unsigned char)p->h_addr_list[0][1], 
		(unsigned char)p->h_addr_list[0][2], 
		(unsigned char)p->h_addr_list[0][3]);
	
	return true;
}


sp_thread_result_t SP_THREAD_CALL CCSocket::RecvFunc(void *arg)
{
	CCSocket *pSocket = (CCSocket *)arg;
	int count = 0;
	int nLen = 0;
	short usPacketLen = 0;
	CCLOG(" pthread suess");
	while (true)
	{
		//CCLOG(" bConnect =  [%d]", pSocket->bConnect);
		if(pSocket->bConnect)
		{
			//CCLOG(" bConnect suess");
			//sp_thread_mutex_lock(&pSocket->m_mutex);
			memset(pSocket->m_szRecvBuf, 0, sizeof(pSocket->m_szRecvBuf));
			nLen = pSocket->Recv(pSocket->m_szRecvBuf, 2, 0);
			//CCLOG(" RecvLen =  [%d]", nLen);
			int nErr = pSocket->GetError();
			if (nLen > 0)
			{
				count = 0;
				memcpy(&usPacketLen, pSocket->m_szRecvBuf, 2);
				usPacketLen = ntohs(usPacketLen); //主机序
				CCLOG(" usPacketLen = ntohs(usPacketLen); = [%d]", usPacketLen);
				if (usPacketLen > 0)
				{
					usPacketLen += 6;
					count += nLen;

					while (count < usPacketLen)
					{
						nLen = pSocket->Recv(pSocket->m_szRecvBuf + count, usPacketLen - count, 0);
						if (nLen > 0)
						{
							count += nLen;
						}
					}

					int nErr = pSocket->GetError();
					if(count > 0)//正常接收数据
					{
						AsyncRecvStruct *data = new AsyncRecvStruct();
						data->m_nRecvBufLen = MAX_SOCKETMSG_BUFF;
						int outlen = pSocket->UnpackPacket
						(
							pSocket->m_szRecvBuf,
							count, 
							data->m_szRecvBuf,
							data->m_nRecvBufLen
						);

						// add async struct into queue
						s_pAsyncStructQueue->push(data);
						count = 0;

					}
					else if (count == 0)//服务器断开连接
					{
						pSocket->bConnect = false;
						pSocket->disConnect = true;
					}
				}
			}
			else if(nLen == 0)
			{
				pSocket->bConnect = false;
				pSocket->disConnect = true;
			}

			//sp_thread_mutex_unlock(&pSocket->m_mutex);
		}
		else 
		{
#ifdef WIN32
			Sleep(50);
#else
			sleep(1);
#endif
		}

	}
	return 0;
}


void CCSocket::Run()
{
    AsyncRecvStruct *data = NULL;
    
    if (!s_pAsyncStructQueue->empty())
    {
        data = s_pAsyncStructQueue->front();
        s_pAsyncStructQueue->pop();
        
        OnRecv(data->m_szRecvBuf,data->m_nRecvBufLen);
        
        free(data);
    }
    
    if (isDisConnect()) {
        OnDisConnection();
    }
}

//连接成功
void CCSocket::OnConnection()
{

}

//连接失败
void CCSocket::OnDisConnect()
{

}

//服务器断开连接
void CCSocket::OnDisConnection()
{
	
}

bool CCSocket::isConnect()
{
	return bConnect;
}

bool CCSocket::isDisConnect()
{
	return disConnect;
}

//数据打包
int CCSocket::PackPacket(const char* pInData, int nInLen, char* pOutData, int& nOutLen)
{
	int nPacketLen = MAX_SOCKETMSG_BUFF;
	char cCompress = 0;
	char cCRC = 0;
	short sInLen = 0;
	unsigned short sVerifyCode = 0;

	//压缩数据包
	char* pData = (char*)(pOutData + 6);

	//if(nInLen > 1024)
	//{
	//	int ret = compress((Bytef*)pData, (uLongf*)&nPacketLen, (Bytef*)pInData, nInLen);
	//	if(ret != Z_OK)
	//	{
	//		glog.log("zlib compress failed ret %d", ret);
	//		return 0;
	//	}
	//	sInLen = (short)nPacketLen;
	//	sVerifyCode = sInLen ^ m_shSeed;
	//	cCompress = 1;
	//}
	//else
	{
		memcpy(pData, pInData, nInLen);
		nPacketLen = nInLen;
		sInLen = (short)nPacketLen;
		sVerifyCode = sInLen ^ m_shSeed;
		for(int i = 0; i < nPacketLen; ++i)
		{
			cCRC += (pData[i]/* << 1*/);
			//pData[i] = (i % 2 == 0) ? ((pData[i] << 2) | (pData[i] >> 6)) : ((pData[i] << 5) | (pData[i] >> 3));
		}
		//cCRC ^= m_symmetricKey[12];

	}

	sInLen = htons(sInLen); //网络序
	sVerifyCode = htons(sVerifyCode);
	memcpy(pOutData, &sInLen, 2);
	memcpy(pOutData + 2, &sVerifyCode, 2);
	memcpy(pOutData + 4, &cCompress, 1);//压缩标记
	memcpy(pOutData + 5, &cCRC, 1);		//校验标记

	nOutLen = nPacketLen + 6;
	return nOutLen;
}

//数据解包
int CCSocket::UnpackPacket(const char* pInData, int nInLen, char* pOutData, int& nMaxLen)
{
	//glog.log("recv buffer %d", nInLen);
	if(nInLen < 6)
		return 0;
	char cCompress = 0;
	char cCRC = 0;
	short usPacketLen = 0;
	unsigned short usVerifyCode = 0;
	memcpy(&usPacketLen, pInData, 2);
	memcpy(&usVerifyCode, &pInData[2], 2);
	memcpy(&cCompress, &pInData[4], 1);//压缩标记
	memcpy(&cCRC, &pInData[5], 1);//校验标记
	usPacketLen = ntohs(usPacketLen); //主机序
	usVerifyCode = ntohs(usVerifyCode);

	int kk = usPacketLen ^ m_shSeed;
	if(kk != usVerifyCode || usPacketLen > nMaxLen)
	{
		nMaxLen = 0; //数据包错误
		return 6; //4个字节作废
	}
	else if(nInLen - 6 < usPacketLen ) //数据包不完整
	{
		nMaxLen = 0;
		return 0;
	}
	else
	{
		if(cCompress != 1)
		{
			nMaxLen = usPacketLen;
			memcpy(pOutData, &pInData[6], usPacketLen);
			//char cValiate = 0;

			//char* pData = (char*)pOutData;


			//for(int i = 0; i < usPacketLen; ++i)
			//{
			//	//pData[i] = (i % 2 == 0) ? ((pData[i] << 6) | (pData[i] >> 2)) :	((pData[i] << 3) | (pData[i] >> 5));
			//	cValiate += (pData[i]/* << 1*/);
			//}
			//cValiate ^= m_symmetricKey[12];
			//if(cCRC != cValiate)
			//{
			//	nMaxLen = 0; //数据包错误
			//	return 6; //4个字节作废
			//}
		}
		else
		{
			//unsigned char* pData = (unsigned char*)&pInData[6];
			//解压数据
// 			int ret = uncompress((Bytef*)pOutData, (uLongf*)&nMaxLen, (Bytef*)pData, usPacketLen);
// 			if(ret != Z_OK)
// 			{
// 				//glog.log("zlib compress failed ret %d", ret);
// 				return 6;
// 			}
		}
		return usPacketLen;
	}
}
