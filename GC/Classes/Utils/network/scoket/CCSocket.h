/*
 * define file about portable socket class. 
 * description:this sock is suit both windows and linux
 * 
 */

#ifndef _CCSocket_H_
#define _CCSocket_H_
#include "spthread.h"
#include "cocos2d.h"

#ifdef WIN32
	#include <winsock.h>
	#define socklen_t int
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <fcntl.h>

	typedef int				SOCKET;

	//#pragma region define win32 const variable in linux
	#define INVALID_SOCKET	-1
	#define SOCKET_ERROR	-1
	//#pragma endregion
#endif

#define	MAX_SOCKETMSG_BUFF	1024 * 64 //缓存

using namespace cocos2d;

class CCSocket : public CCObject{

public:
	CCSocket(SOCKET sock = INVALID_SOCKET);
	~CCSocket();
	
	sp_thread_mutex_t  m_mutex;
	sp_thread_t m_mutexThreadId;
	
	//typedef void (CALLBACK* NOTIFYPROC)(const char* pBuffer, int nLen);

	//Data callback
	//NOTIFYPROC m_pNotifyProc;
	virtual void OnRecv(const char* pData, int nLen) = 0;
	//Connect socket suess
	virtual void OnConnection();

	//Connect socket failed
	virtual void OnDisConnect();

	//Dis socket 
	virtual void OnDisConnection();

	virtual void Run();
	//
	bool isConnect();

	bool isDisConnect();

	virtual int PackPacket(const char* pInData, int nInLen, char* pOutData, int& nOutLen);

	virtual int UnpackPacket(const char* pInData, int nInLen, char* pOutData, int& nOutLen);

	int	m_nRecvBufLen;
	char m_szRecvBuf[MAX_SOCKETMSG_BUFF + 1];	/*接收缓存区*/

	// Create socket object for snd/recv data
	bool Create(int af, int type, int protocol = 0);

	// Connect socket
	bool Connect(const char* ip, unsigned short port);
	//#region server
	// Bind socket
	bool Bind(unsigned short port);

	// Listen socket
	bool Listen(int backlog = 5);

	// Accept socket
	bool Accept(CCSocket& s, char* fromip = NULL);
	//#endregion
	
	// Send socket
	int Send(const char* buf, int len, int flags = 0);

	// Send Pack socket
	int SendPack(const char* buf, int len, int flags = 0);

	// Recv socket
	int Recv(char* buf, int len, int flags = 0);
	
	// Close socket
	int Close();

	// Get errno
	int GetError();
	
	//#pragma region just for win32
	// Init winsock DLL 
	static int Init();
	// Clean winsock DLL
	static int Clean();
	//#pragma endregion
	
	// Domain parse
	static bool DnsParse(const char* domain, char* ip);

	CCSocket& operator = (SOCKET s);

	operator SOCKET ();

protected:
	SOCKET m_sock;
	bool bConnect;//是否连接
	bool disConnect;//是否掉线
	unsigned short m_shSeed;
	static sp_thread_result_t SP_THREAD_CALL RecvFunc(void *arg);

};

#endif
