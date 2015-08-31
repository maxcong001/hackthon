

typedef enum {
	NOT_CONNECTED = 0,
	PENDING = 1,
	CONNECTED = 2
} IM_CONNECT_STATUS;

typedef struct{
	IMPeerID	PeerID;
	IM_CONNECT_STATUS 		connect_status;
	void					*sock_state;
}ConMgrtbl;

class IMTCPMgr
{




};