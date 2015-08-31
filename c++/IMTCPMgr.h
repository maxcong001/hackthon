#include "T_ipcfg_service_ips.h"

#define IM_LISTEN_PORT 7500

#define POOL_TYPE_MAX 4
#define POOL_ID_MAX 255
#define POOL_MEMBER_MAX 2
#define IM_MULTI_DESTINATION_CONN_NUM_MAX 255
#define IM_TRANS_RECV_BUFFER_SIZE 32768
#define IM_LISTEN_PORT	7500
#define IM_ACTIVE_SIDE 1
#define IM_STDBY_SIED 0

#define IM_SUCCESS	0
#define IM_FAIL		-1
//#define IM_INSERT2FIX_TABLE 0
//#define IM_INSERT2FLOAT_TABLE 1
#define IMTcp_log IMTCPMgr::IMTcp_log


typedef enum {
	NOT_CONNECTED = 0,
	PENDING = 1,
	CONNECTED = 2
} IM_CONNECT_STATUS;

typedef enum {
	IM_INSERT2FIX_TABLE = 0,
	IM_INSERT2FLOAT_TABLE = 1
}IM_INSERT2TABLE

typedef struct{
	in_addr_t				peerIP;
	IM_CONNECT_STATUS 		connect_status;
	void					*sock_state;
}ConMgrtbl;

class IMTCPMgr
{
public:
	IMTCPMgr();
	~IMTCPMgr();
	
	//API for IM Trans class
	int init();
	int start();
	int send(IMPeerId *peerid, void *message, int length);
	int connect(IMPeerId *peeridp, short num );
	int close(IMPeerId *peeridp);
	int onclose();
	int onconnect();

		//wraper of translib function
	int IMTransInit();
	int IMTransListen();
	int IMTransConnect(IN  OSENDPOINT*    remoteEP);
	int IMTransSend(IN  void*      llc,
					IN  void*       ulc,
					IN  char*       message,
					IN  int         length);		
	
	int IMTransClose(IN  void    *llc,
                      IN  void    *ulc);

    //translib callback functions
	int  onNewConnNotifyCB(IN void *newLlc, IN  void *listeningUlc, OUT void **returnedUlc, IN  OSENDPOINT *remoteAddr);
	int  onConnectNotifyCB( void *llc, void *ulc);
	int  onCloseNotifyCB( void *llc, void *ulc, int errorNum);
	int  onConnectErrorNotifyCB( void *llc, void *ulc, int errorNum);
	int  onListenErrorNotifyCB( void *llc, void *ulc, int errorNum);
	int  onTimeoutNotifyCB( void *llc, void *ulc, int timerId);
	int  onReceived( void *llc, void *ulc, char *msg, int size); 
	int  onSocketError(
			IN	void*				llc,
			IN	void*				ulc,
			IN	int 				error);

	//log function
	void IMTcp_log(char *module, int level, char *format, ...);
	//initTransSockOpts()
	//void initTransSockOpts(TransSockOpts *sockOpts);

	//get IP information function & update connection management table
	int IMGetIPInfo();
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//depends on peerid structure to tell me if poolmember or floater
	void insert_ip_mapping(char pooltype, char poolid, char poolmember, in_addr_t peerip, IM_INSERT2TABLE table );
	// get local internal floating IP 
	//OSENDPOINT IMBldLocalAddr();
	OSENDPOINT IMGetIntFloatIP();
	//this function is called only in active card
	OSENDPOINT IMGetIntFixedIP();

	//convert peerid / IP function
	OSENDPOINT PeerId2IP(IMPeerId peerid);
	IMPeerId	IP2PeeerId(OSENDPOINT peerIP);
	//find llc using peerid
	void *peerid2llc(IMPeerId peerid);

	//struct of translib callback function
	static TransEventT 		IM_trans_event;
	// socket options
	static TransSockOpts 		IM_trans_sock_opts;
	//connection management table for pool_type(AMC->0, IMS->1, CFED->2, DFED->3), pool_id, pool_member
	static ConMgrtbl conmgrtbl_member[POOL_TYPE_MAX][POOL_ID_MAX][POOL_MEMBER_MAX];
	//connection management table for pool_type, pool_id, floater
	static ConMgrtbl conmgrtbl_floater[POOL_TYPE_MAX][POOL_ID_MAX][POOL_MEMBER_MAX];
	// Local address info
	OSENDPOINT local_intfloat;
	OSENDPOINT local_intfixed;
	// active side or standby side
	int IM_active_stdby;
	// server llc
	void *serv_llc_float;
	void *serv_llc_fixed;

	// Just for debug use
	int insert_debug();

};