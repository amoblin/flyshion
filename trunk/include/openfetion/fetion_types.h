/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef FETION_TYPES_H
#define FETION_TYPES_H


/* inline function to trace program track */

#ifdef FETION_DEBUG_TRACE
#	define DEBUG_FOOTPRINT()	\
		printf("| TRACK |function '%s()' in file: <%s:%d>\n" \
		, __FUNCTION__ , __FILE__ , __LINE__)
#else
#	define DEBUG_FOOTPRINT()
#endif

#define FETION_VERSION "1.5"
#define PROTO_VERSION "4.0.2380"
#define NAVIGATION_URI "nav.fetion.com.cn"
#define LOGIN_TYPE_FETIONNO 1
#define LOGIN_TYPE_MOBILENO 0
#define BOUND_MOBILE_ENABLE 1
#define BOUND_MOBILE_DISABLE 0
/**
 * some other buddylists
 */
typedef enum
{
	BUDDY_LIST_NOT_GROUPED = 0 ,
	BUDDY_LIST_STRANGER =   -1
} BuddyListType;

/**
 * Presence states
 */
typedef enum
{
	P_ONLINE = 		 400 , 
	P_RIGHTBACK = 	 300 ,
	P_AWAY = 		 100 ,
	P_BUSY = 		 600 ,
	P_OUTFORLUNCH =  500 ,
	P_ONTHEPHONE = 	 150 ,
	P_MEETING = 	 850 ,
	P_DONOTDISTURB = 800 ,
	P_HIDDEN = 		 0 ,
	P_NOTAPRESENCE = -1
} StateType;

/**
 * Type used to indicate whether user`s portrait has been changed
 */
typedef enum
{
	IMAGE_NOT_INITIALIZED = -1 ,		/* portrait has not been initialized */
	IMAGE_NOT_CHANGED ,					/* portrait does not change 		 */
	IMAGE_CHANGED						/* portrait has been changed 		 */
} ImageChangedType;

/**
 * Type to indicate user`s service status 
 */
typedef enum
{
	STATUS_NORMAL = 1 ,					/* normal status											 */
	STATUS_OFFLINE ,					/* user offline , deleted you from his list or out of service*/
	STATUS_NOT_AUTHENTICATED ,			/* user has not accept your add buddy request				 */
	STATUS_SMS_ONLINE ,					/* user has not start fetion service						 */
	STATUS_REJECTED ,					/* user rejected your add buddy request,wait for deleting 	 */
	STATUS_SERVICE_CLOSED , 			/* user has closed his fetion service 						 */
	STATUS_NOT_BOUND					/* user doesn`t bound fetion number to a mobile number 		 */
} StatusType;

/**
 * Two-way linked list that can contans any types
 */
typedef struct fxlist
{
	struct fxlist* pre;
	void* data;
	struct fxlist* next;
} FxList;

/**
 * Fetion Connection
 */
typedef struct
{
	int socketfd;						/* socket file descriptor*/
	char local_ipaddress[16];			/* local ip address      */
	int local_port;						/* local port			 */
	char remote_ipaddress[16];			/* remote ip address	 */
	int remote_port;					/* remote port 			 */
	SSL* ssl;							/* SSL handle			 */
	SSL_CTX* ssl_ctx;					/* SSL ctx struct 		 */
} FetionConnection;

/**
 * Sip header that in form of "name: value" such as "AK: ak-value"
 */
typedef struct sipheader
{
	char name[5];						/* sip header namne*/
	char* value;						/* sip header value*/
	struct sipheader* next;				/* next sip header */
} SipHeader;

/**
 * Sip type include some common attributes
 */
typedef struct
{
	int type;							/* sip message type						  */
	char from[20];						/* sender`s fetion no ,in sip it`s "F: "  */
	int sequence;						/* sequence number , in sip it`s "Q: "    */
	int threadCount;					/* listening threads count using this sip */
	char sipuri[48];					/* outer sipuri used when listening       */
	SipHeader* header;					/* some othre header list				  */
	FetionConnection* tcp;				/* fetion connection used to send message */
} FetionSip;

/**
 * Sip message list that parsed from received chunk 
 */
typedef struct sipmsg
{
	char* message;
	struct sipmsg* next;
} SipMsg;

/**
 * Buddy lists information (Two-way linked list)
 */
typedef struct group
{
	struct group* preNode;				/* previous buddy list node */
	char groupname[32];					/* current buddy list name  */
	int groupid;						/* current buddy list Id	*/
	struct group* nextNode;				/* next buddy list node		*/
} Group;

/**
 * Contact lists information (Two-way linked list) 
 */
typedef struct contact
{
	struct contact* preNode;			/* previous contact list node         				*/
	char userId[16];					/* userid used since v4 protocal      				*/
	char sId[16];						/* fetion no					      				*/
	char sipuri[48];					/* sipuri like 'sip:100@fetion.com.cn'				*/
	char localname[32];					/* name set by yourself				  				*/
	char nickname[32];					/* user`s nickname					    			*/
	char impression[384];				/* user`s mood phrase				    			*/
	char mobileno[12];					/* mobile phone number				    			*/
	char devicetype[10];				/* user`s client type , like PC and J2ME,etc		*/
	char portraitCrc[12];				/* a number generated by crc algorithm 				*/
	char birthday[16];					/* user`s bitrhday									*/
	char country[6];					/* user`s country`s simplified form,like CN 		*/
	char province[6];					/* user`s province`s simplified form,like bj		*/
	char city[6];						/* user`s city`s code ,like 10 for beijing			*/
	int identity;						/* whethere to show mobileno to this user   		*/
	int scoreLevel;						/* user`s score level,unused now					*/
	int serviceStatus;					/* service status like offline or closed,etc		*/
	StateType state;					/* state type like online,busy,etc					*/
	int groupid;						/* buddylist id										*/
	int gender;							/* gender 1 for male 2 for female,0 for private		*/
	int imageChanged;					/* whether user`s portrait has changed				*/
	struct contact* nextNode;
} Contact;

/**
 * Verification information used for picture code confirm
 */
typedef struct
{
	char* algorithm;
	char* type;
	char* text;
	char* tips;
	char* code;
	char* guid;
} Verification;

/**
 * User list store in local data file  (One-way linked list)
 */
typedef struct userlist
{
	char no[24];						/* fetion no or mobile no  		*/
	char password[48];					/* password 			   		*/
	int laststate;						/* last state when logining		*/
	int islastuser;						/* is the last logined user		*/
	struct userlist* next;				/* next User node				*/
} UserList;

/**
 * structure used to describe global proxy information
 */
typedef struct
{

	int proxyEnabled;					/* whether http proxy is enable							  */
	char proxyHost[48];					/* proxy host name or ip address						  */
	int proxyPort;					/* port number of proxy server							  */
	char proxyUser[48];					/* username for proxy authentication					  */
	char proxyPass[48];					/* password for proxy authentication 					  */

} Proxy;

/**
 * Configuration information 
 */
typedef struct
{
	char* globalPath;					/* global path,default $(HOME)/.openfetion                */
	char* userPath;						/* user path , directory name by user`s sid in globalPath */
	char* iconPath;						/* path stores user`s friend portraits in user`s path     */	
	char sipcProxyIP[17];				/* sipc proxy server`s ip ,read from configuration.xml    */
	int sipcProxyPort;					/* sipc proxy server`s port , read from configuration.xml */
	char portraitServerName[48];		/* portrait server`s hostname ,read from configuration.xml*/
	char portraitServerPath[32];		/* portrait server`s path , such as /HD_POOL8             */
	int iconSize;						/* portrait`s display size default 25px					  */
	int autoReply;						/* whether auto reply enabled							  */
	int isMute;
	char autoReplyMessage[180];			/* auto reply message content							  */
	int autoPopup;						/* whether auto pupup chat dialog enabled				  */
	int sendMode;						/* press enter to send message or ctrl + enter 			  */
	int closeMode;						/* close button clicked to close window or iconize it	  */
	UserList* userList;					/* user list stored in local data file					  */
	Proxy *proxy;						/* structure stores the global proxy information 		  */
} Config;

/**
 * User`s personal information and some related structs 
 */
typedef struct
{
	char sId[11];						/* fetion number 											*/
	char userId[11];					/* user id													*/
	char mobileno[13];					/* mobile phone number										*/
	char password[42];					/* raw password not hashed									*/
	char sipuri[48];					/* sipuri like 'sip:100@fetion.com.cn'						*/
	char publicIp[17];					/* public ip of current session								*/
	char lastLoginIp[17];				/* public ip of last login									*/
	char lastLoginTime[48];				/* last login time , got after sipc authentication			*/

	char personalVersion[16];			/* the version of personal information						*/
	char contactVersion[16];			/* the version of contact information						*/
	char customConfigVersion[16];		/* the version of custom config string,unused now			*/

	char configServersVersion[16];		/* the version of some related servers such as sipc server	*/
	char configHintsVersion[16];		/* the version of hints										*/

	char nickname[48];					/* nickname of yourself										*/
	char impression[256];				/* mood phrase of yourself									*/
	char portraitCrc[16];				/* a number generated by crc algorithm						*/
	char country[6];					/* the country which your number belongs to					*/
	char province[6];					/* the province which your number belongs to				*/
	char city[6];						/* the city which your number belongs to 					*/
	int gender;							/* the gender of your self									*/

	int smsDayLimit;
	int smsDayCount;
	int smsMonthLimit;
	int smsMonthCount;

	int state;							/* presence state											*/
	int loginType;   					/* using sid or mobileno									*/
	int loginStatus; 					/* login status code 										*/
	int boundToMobile;					/* whether this number is bound to a mobile number  */
	long loginTimes;
	char* ssic;						    /* cookie string read from reply message after ssi login 	*/
	char* customConfig;					/* custom config string used to set personal information	*/
	Verification* verification;			/* a struct used to generate picture code					*/	 
	Contact* contactList;				/* friend list of current user								*/
	Group* groupList;					/* buddylist list of current user							*/
	Config* config;						/* config information										*/
	FetionSip* sip;						/* sip object used to handle sip event						*/
} User;

/**
 * structure used to describe onversation information 
 */
typedef struct
{
	Contact* currentContact;			 /* current friend who you a chating with					   */
	User* currentUser;					 /* current user,ourselves									   */
	FetionSip* currentSip;				 /* sip struct used to send message 
										  * NULL if did not start a chat channel for this conversation */
} Conversation;

/**
 * structure used to describe message information 
 */
typedef struct
{
	char* message;						 /* message content  		*/
	char* sipuri;						 /* sender`s sip uri 		*/
	struct tm sendtime;					 /* message sent time 		*/
} Message;

/**
 * structure used to describe chat history
 */
typedef struct
{
	char name[48];						 /* name of message sender	   */
	char userid[16];					 /* userid of message sender   */
	struct tm sendtime;					 /* message sent time		   */
	char message[550];					 /* message content			   */
	int issend;							 /* message is sent of received*/	
} History;

typedef struct
{
	User* user;
	FILE* file;
} FetionHistory;

#define FILE_RECEIVE 1
#define FILE_SEND 2

#define FILE_ACCEPTED 1
#define FILE_DECLINED 2

typedef struct{

	FetionSip *sip;
	int shareMode;
	int shareState;
	char guid[64];
	char sessionid[64];
	char absolutePath[1024];
	char filename[64];
	char sipuri[64];
	char md5[64];
	long long filesize;
	char preferType[8];
	char innerIp[24];
	int innerUdpPort;
	int innerTcpPort;
	char outerIp[24];
	int outerUdpPort;
	int outerTcpPort;
} Share;

#endif
