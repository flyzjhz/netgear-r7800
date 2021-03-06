// PHGlobal.h: interface for the CBaseThread class.
//
//////////////////////////////////////////////////////////////////////
/*! \file PHGlobal.h
*  \author skyvense
*  \date   2009-09-14
*  \brief PHDDNS 客户端实现	
*/

#ifndef __PHGLOBAL__H__
#define __PHGLOBAL__H__
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#endif

#ifdef WIN32
#define sleep(x) Sleep(x*1000)
#else
#define INVALID_SOCKET (-1)
#define MAX_PATH	260
#endif

#include "phsocket.h"

#define MAX_TCP_PACKET_LEN	8192

//! 客户端状态
enum MessageCodes
{
	okConnected = 0,
	okAuthpassed,
	okDomainListed,
	okDomainsRegistered,
	okKeepAliveRecved,
	okConnecting,
	okRetrievingMisc,
	okRedirecting,

	errorConnectFailed = 0x100,
	errorSocketInitialFailed,
	errorAuthFailed,
	errorDomainListFailed,
	errorDomainRegisterFailed,
	errorUpdateTimeout,
	errorKeepAliveError,
	errorRetrying,
	errorAuthBusy,
	errorStatDetailInfoFailed,
	

	okNormal = 0x120,
	okNoData,
	okServerER,
	errorOccupyReconnect,
};

//! TCP指令表
#define COMMAND_AUTH	"auth router6\r\n"
#define COMMAND_REGI    "regi a"
#define COMMAND_CNFM    "cnfm\r\n"
#define COMMAND_STAT_USER    "stat user\r\n"
#define COMMAND_STAT_DOM     "stat domain\r\n"
#define COMMAND_QUIT    "quit\r\n"

//! 心跳包更新指令
#define UDP_OPCODE_UPDATE_VER2		0x2010
//! 心跳包服务器返回正常
#define UDP_OPCODE_UPDATE_OK		0x2050
//! 心跳包服务器返回错误
#define UDP_OPCODE_UPDATE_ERROR		1000

//! 心跳包注销登录
#define UDP_OPCODE_LOGOUT			11

//! 心跳包加密部分大小
#define KEEPALIVE_PACKET_LEN	20

//! 转换状态码到文本串
const char *convert_status_code(int nCode);

//! 转换IP地址到文本串
const char *my_inet_ntoa(int ip);

//! 心跳包结构
typedef struct  
{
	//! 对话ID
	int lChatID;
	//! 操作码
	int lOpCode;
	//! 包ID
	int lID;
	//! 校验和
	int lSum;
	//! 保留字
	int lReserved;
}DATA_KEEPALIVE;

//! 更新包扩展结构，用于服务器返回时IP地址
typedef struct 
{
	DATA_KEEPALIVE keepalive;
	int ip;
}DATA_KEEPALIVE_EXT;

	typedef void (*CALLBACK_OnStatusChanged)(int status, int data); 
	//! 此函数得到状态变更
	/*! status可能的值：
		enum MessageCodes
		{
			okConnected = 0,
			okAuthpassed,
			okDomainListed,
			okDomainsRegistered,
			okKeepAliveRecved,
			okConnecting,
			okRetrievingMisc,
			okRedirecting,
		
		  errorConnectFailed = 0x100,
		  errorSocketInitialFailed,
		  errorAuthFailed,
		  errorDomainListFailed,
		  errorDomainRegisterFailed,
		  errorUpdateTimeout,
		  errorKeepAliveError,
		  errorRetrying,
		  errorAuthBusy,
		  errorStatDetailInfoFailed,

		  okNormal = 0x120,
		  okNoData,
		  okServerER,
		  errorOccupyReconnect,
		};
		其中：
		1、当status为okDomainsRegistered时，data返回用户级别：0(免费),1(专业),2(商业)
		2、当status为okKeepAliveRecved时，data返回客户端IP地址（整数形式）
		3、其他情况下，data一直为0
	*/
	//! 此函数得到注册的域名，每条域名被执行一次
	typedef void (*CALLBACK_OnDomainRegistered)(char *domainName); 
	//! 此函数得到用户信息，XML格式
	typedef void (*CALLBACK_OnUserInfo)(char *userInfo, int length); 
	//! 此函数得到用户域名信息，XML格式
	typedef void (*CALLBACK_OnAccountDomainInfo)(char *domainInfo, int length); 


//! 全局变量
typedef struct
{
//basic system info
	//! 嵌入式客户端信息，4位客户端ID + 4位版本号
	int clientinfo;
	//! 嵌入式验证码
	int challengekey;

	//! 服务器地址
	char szHost[256];
	//! 服务器端口，6060固定
	short nPort;
	//! 用户账号名
	char szUserID[256];
	//! 用户密码明码
	char szUserPWD[256];
	//! 本地绑定地址，默认填空，用于多个网卡时指定出口
	char szBindAddress[256];
//run-time controll variables
	//! 用户类型
	int nUserType;
	BOOL bTcpUpdateSuccessed;
	char szChallenge[256];
	int nChallengeLen;
	int nChatID,nStartID,nLastResponseID;
	int tmLastResponse;
	int nAddressIndex;
	char szTcpConnectAddress[32];

	int cLastResult;
	int ip;

	int uptime;
	int lasttcptime;

	char szActiveDomains[255][255];

	//! 当前是否需要进行TCP连接
	BOOL bNeed_connect;
	//! 最后一次发送心跳包的时间	
	int tmLastSend;

	int m_tcpsocket,m_udpsocket;

	CALLBACK_OnStatusChanged cbOnStatusChanged;
	CALLBACK_OnDomainRegistered cbOnDomainRegistered;
	CALLBACK_OnUserInfo cbOnUserInfo;
	CALLBACK_OnAccountDomainInfo cbOnAccountDomainInfo;
} PHGlobal;

void init_global(PHGlobal *global);
void set_default_callback(PHGlobal *global);

#endif