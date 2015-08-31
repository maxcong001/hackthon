#include "IMTCPMgr.h"

IMTCPMgr::IMTCPMgr()
{
}
IMTCPMgr::~IMTCPMgr()
{
}
/*do the following work:
1. set active stdby code
2. set callback function for translib
3. set trans sockect options
4. set translib config
5. call TransInit
*/
int IMTCPMgr::init()
{
	//mark active side or stadnby side
	if (getDSPrimaryCondition() == condActive)
	{
		IM_active_stdby = IM_ACTIVE_SIDE;
	}
	else
	{
		IM_active_stdby = IM_STDBY_SIED;
	}
	// Set translib callback functions
	memset( &IM_trans_event, 0, sizeof(TransEventT));
	IM_trans_event.TransNotifyPacketArrived    = IMTCPMgr::onReceived;
	IM_trans_event.TransNotifyNewConnection    = IMTCPMgr::onNewConnNotifyCB;
	IM_trans_event.TransNotifyConnected        = IMTCPMgr::onConnectNotifyCB;
	IM_trans_event.TransNotifyConnectionClosed = IMTCPMgr::onCloseNotifyCB;
	IM_trans_event.TransNotifyConnectError     = IMTCPMgr::onConnectErrorNotifyCB;
	IM_trans_event.TransNotifyListenError      = IMTCPMgr::onListenErrorNotifyCB;
	IM_trans_event.TransNotifyTimeout          = IMTCPMgr::onTimeoutNotifyCB;
	IM_trans_event.TransNotifySocketError      = IMTCPMgr::onSocketError;
	IM_trans_event.TransNotifyUDPPacketArrived = NULL;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//flags need to check again
	// Set socket options
	memset( &IM_trans_sock_opts, 0, sizeof( TransSockOpts));
	IM_trans_sock_opts.flags =
		TRANS_FLAG_LOCAL_PORT |
        TRANS_FLAG_NON_BLOCKING |
        TRANS_FLAG_SP_TPKT |
        TRANS_FLAG_SP_TPKT_V1 |
        TRANS_USE_PAYLOAD_BUF_FOR_SNDTPKT |
        TRANS_FLAG_BULK_READ |
        //TRANS_FLAG_SAFE_ARRIVED |
        TRANS_FLAG_REUSEADDR |
        TRANS_FLAG_LOWER_LOG |
		//The TransNotifySocketError() callback is used insteadof transClose().
		TRANS_FLAG_NOCLOSE_ON_ERROR | 
        TRANS_FLAG_TCP;

	// Configure TransLib                                                                                                                                                   
    TransConfT cfg;
    memset( &cfg, 0, sizeof( TransConfT));
    cfg.numWorkerThreads = 1;
    cfg.numSocksPerThread = IM_MULTI_DESTINATION_CONN_NUM_MAX;
    cfg.numTimersPerThread = IM_MULTI_DESTINATION_CONN_NUM_MAX;
    cfg.recvBufSize = IM_TRANS_RECV_BUFFER_SIZE;
    cfg.logFunction = IMTcp_log;
    cfg.flags = 0;

    // Call TransLib function                                                                                                                                               
    int transResult = TransInit( &cfg);

    if( transResult != TRANS_SUCCESS){
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////
	   //log needed
        return IM_FAIL;
    }
	return IM_SUCCESS;
}
//start listening a well-known port
// active side need to listen to both fixed and floating IP
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int IMTCPMgr::start()
{
	//start listen local internal fixed IP
	int result = TransListen( &serv_llc_fixed,
        	0,
        	NULL,
        	&local_intfixed,
        	&IM_trans_event,
        	&IM_trans_sock_opts);

	if(TRANS_SUCCESS != result)
	{
		////////////////////////////////////////////////////////////////////////////////////////
		ASRT_RPT(ASBAD_DATA, 0,
                        "TransListen()  Failed with error %d for ip:0x%x",
                        ret, ipaddr, OSEP_PORTIPTCP(local_addr)) ;
		return IM_FAIL;

	}// if(TRANS_SUCCESS != ret)
	//start listen local internal floating IP if this is active side
	if(IM_active_stdby)
	{
		result = TransListen( &local_intfloat,
				0,
				NULL,
				&local_intfixed,
				&IM_trans_event,
				&IM_trans_sock_opts);

		if(TRANS_SUCCESS != result)
		{
			////////////////////////////////////////////////////////////////////////////////////////
			ASRT_RPT(ASBAD_DATA, 0,
							"TransListen()  Failed with error %d for ip:0x%x",
							ret, ipaddr, OSEP_PORTIPTCP(local_addr)) ;
			return IM_FAIL;

		}// if(TRANS_SUCCESS != ret)
	}
	return IM_SUCCESS;


}

int IMTCPMgr::send(IMPeerId *peerid, void *message, int length)
{
	

}
//start connection
int IMTCPMgr::connect(IMPeerId *peeridp, short num )
{
	short i;
	for(i = 0; i <num; i++)
	{

	}

	
}

int IMTCPMgr::close(IMPeerId *peeridp)
{

}

void IMTCPMgr::IMTcp_log( char *module, int level, char *format, ...)
{

}
void IMTCPMgr::IMBldLocalAddr(OSENDPOINT* local_addr, OSIPPORT local_port)
{
	int ret;
	unsigned long   ipaddr = 0;
	OSEP_INIT(local_addr);
	OSEP_MKIPTCP(local_addr, local_port);//INADDR_ANY
	OSIPADDRSETTYPE(OSEP_IPADDR(*local_addr), OSIPV4);
	ipcfg_getMyInternalFixedServiceIP((in_addr_t *)&ipaddr);
	ipaddr = htonl(ipaddr);
	OSIPADDRSETRAW((OSEP_IPADDR(*local_addr)), &ipaddr,AF_RAWIPV4SIZE);


}
/*
need to include T_ipcfg_service_ips.h
#define IPCFG_SERVICE_IPS_tid   ("ipcfg_service_ips")
#define IPCFG_SERVICE_IPS_tid_lsv       ("SCHLSV:ipcfg_service_ips:3551561432")
#define IPCFG_SERVICE_IPS_str   ("ipcfg_service_ips")
#define IPCFG_SERVICE_IPS_sz    (148)
*/
// load ip info
void IMTCPMgr::IMGetIPInfo()
{
	DB_TABLEID tid = IPCFG_SERVICE_IPS_tid;
	char *tver = IPCFG_SERVICE_IPS_tid_lsv;
	RET_VAL db_ret;
	DB_TRANID tranid;
	union {
		char tupBuff[IPCFG_SERVICE_IPS_sz];
		IPCFG_SERVICE_IPS serviceips;
	} dataBuff;
	db_ret = ipcfg_getDBtranidPtr(&tranid, DB_NOLOCKRD);
	if (db_ret != GLSUCCESS)
	{
		return IMS_FAIL; 
	}
	  
	for (DMCMNDTAG fn=DBRDFST; 
			(db_ret = _DB_xrdfnxt(tranid, tid, tver, dataBuff.tupBuff, "", fn)) == GLSUCCESS;
			fn=DBRDNXT)
	{
		OSTAKE_BRK();
		switch ( dataBuff.serviceips.pool_type)
		{
			case CFED_PT:
			case DFED_PT:
			case AMC_PT:
			case IMS_PT:
				break;
			default:
				continue;
	 
		}
		// Only interested in internal IPs for now
		if (dataBuff.serviceips.subnet_type != INTERNAL_ST) continue;
		// only interested in floating IPs for now
		if (dataBuff.serviceips.assignment_type == FLOATING_AT) 
		{
			// just use member 0 to avoid adding IPs more than once
			if (dataBuff.serviceips.member_instance != 0) continue;
			// convert DM_IPADDRESS to OSIPADDR
			OSIPADDR temp_osip;
			OSDMIP2OSIP(&dataBuff.serviceips.service_ip,&temp_osip);
			// only insert floating member id
			IMip_mapping::insert_ip_mapping(dataBuff.serviceips.pool_type, dataBuff.serviceips.pool_id, 
									dataBuff.serviceips.member_instance, temp_osip.ipaddr[0], IM_INSERT2FLOAT_TABLE);
		}
		else
		{
			// convert DM_IPADDRESS to OSIPADDR
			OSIPADDR temp_osip;
			OSDMIP2OSIP(&dataBuff.serviceips.service_ip,&temp_osip);
			// only insert floating member id
			IMip_mapping::insert_ip_mapping(dataBuff.serviceips.pool_type, dataBuff.serviceips.pool_id, 
									dataBuff.serviceips.member_instance, temp_osip.ipaddr[0], IM_INSERT2FIX_TABLE);
		}
	}
						
	db_ret = ipcfg_endDBtranid(DBABORT);
	if (db_ret != GLSUCCESS)// && db_ret != DBEOR)
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//"Failed to end Transaction %d, db_ret=%d\n", tranid, db_ret
		return IMS_FAIL;
	}
	return IMS_SUCCESS;
}

void IMTCPMgr::insert_ip_mapping(char pooltype, char poolid, char poolmember, in_addr_t peerip, IM_INSERT2TABLE table)
{
	switch( pooltype)
	{
		//AMC->0, IMS->1, CFED->2, DFED->3
		int pooltype_idx;
		case CFED_PT:
			pooltype_idx = 2;
			break;
		case DFED_PT:
			pooltype_idx = 3;
			break;
		case AMC_PT:
			pooltype_idx = 0;
			break;
		case IMS_PT:
			pooltype_idx = 1;
			break;
		default:
				////////////////////////////////////////////////////
		///should not run to here
		return;
	}
	if( table == IM_INSERT2FIX_TABLE)
	{
		conmgrtbl_member[pooltype_idx][poolid][poolmember] = peerip;
	}
	else
	{
		conmgrtbl_floater[pooltype_idx][poolid][poolmember] = peerip;
	}
}