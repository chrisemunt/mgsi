/*
   ----------------------------------------------------------------------------
   | mgsi                                                                     |
   | Description: Service Integration Gateway                                 |
   | Author:      Chris Munt cmunt@mgateway.com                               |
   |                         chris.e.munt@gmail.com                           |
   | Copyright (c) 2002-2023 MGateway Ltd                                     |
   | Surrey UK.                                                               |
   | All rights reserved.                                                     |
   |                                                                          |
   | http://www.mgateway.com                                                  |
   |                                                                          |
   | Licensed under the Apache License, Version 2.0 (the "License"); you may  |
   | not use this file except in compliance with the License.                 |
   | You may obtain a copy of the License at                                  |
   |                                                                          |
   | http://www.apache.org/licenses/LICENSE-2.0                               |
   |                                                                          |
   | Unless required by applicable law or agreed to in writing, software      |
   | distributed under the License is distributed on an "AS IS" BASIS,        |
   | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
   | See the License for the specific language governing permissions and      |
   | limitations under the License.                                           |
   |                                                                          |
   ----------------------------------------------------------------------------
*/

#ifndef MGSI_H
#define MGSI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <sys/timeb.h>
#endif

#if !defined(_WIN32)

/* UNIX: General */

#include <sys/errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#if !defined(HPUX)
#include <sys/select.h>
#endif

#include <sys/time.h>

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/* UNIX: DSO Management */
#include <dlfcn.h>

/* UNIX: Thread Management */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>

/* UNIX: Critical Sections */

#include <semaphore.h>

#include <pwd.h>
#include <grp.h>
#if defined(LINUX) || defined(MACOSX)
#include <execinfo.h>
#endif
#include <iconv.h>
#include <sys/mman.h>

#else /* #if !defined(_WIN32) */


/* Windows: General */

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#include <ws2tcpip.h>
#include <direct.h>
#include <process.h>
#include <wincrypt.h>

#include <winsvc.h>
#include <process.h>
#include <signal.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && (NTDDI_VERSION >= NTDDI_WINXP)
#include <VersionHelpers.h>
#if !defined(_WIN32_WINNT_WIN10)
#define _WIN32_WINNT_WIN10       0x0a00
#endif
#endif

#endif /* #if !defined(_WIN32) */


#ifdef MG_SSL
#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#if defined(MG_BDB)
#include <db.h>
#endif

/* Interface to IBM MQ library */
#include "mgsimq.h"


#define MG_CURRENT_YEAR         "2020"

#define MG_MAX_CONNECTIONS       8192
#define MG_MAX_MQ_CONNECTIONS    8192

#define MG_MAX_SERVERS           128
#define MG_MAX_ALT               32

#define MG_MAX_CHILD             32

#define MG_RS_HTTP               1
#define MG_RS_PHP                2
#define MG_RS_MQ                 3

#define MG_CACHE_BUFFER          32768

#define MG_IP_ADDRESS            "127.0.0.1"
#define MG_TCP_PORT              "7040"

#if defined(MG_MGWSI)
#define MG_RVAR_PREFIX           "mgwsi"
#define MG_RVAR_PREFIX_LEN       5
#define MG_LOG_FILE              "mgwsi.log"
#define MG_INI_FILE              "mgwsi.ini"
#define MG_SEC_FILE              "mgwsisec.ini"
#define MG_PID_FILE              "mgwsi.pid"
#define MG_SYS_INI_FILE          "mgwsid.ini"
#define MG_PATH_1                "/mgwsi"
#else
#define MG_RVAR_PREFIX           "mgsi"
#define MG_RVAR_PREFIX_LEN       4
#define MG_LOG_FILE              "mgsi.log"
#define MG_INI_FILE              "mgsi.ini"
#define MG_SEC_FILE              "mgsisec.ini"
#define MG_PID_FILE              "mgsi.pid"
#define MG_SYS_INI_FILE          "mgsid.ini"
#define MG_PATH_1                "/mgsi"
#endif

#define MG_READ_NOCON            -1
#define MG_READ_EOF              0
#define MG_READ_OK               1
#define MG_READ_ERROR            2
#define MG_READ_TIMEOUT          -777
#define MG_SEND_OK               11
#define MG_SEND_ERROR            12

#define MG_WINVER_10             10.0
#define MG_WINVER_2012           6.3
#define MG_WINVER_8              6.2
#define MG_WINVER_7	            6.1
#define MG_WINVER_2008_R2        6.1
#define MG_WINVER_2008	         6.0
#define MG_WINVER_VISTA	         6.0
#define MG_WINVER_2003_R2	      5.2
#define MG_WINVER_2003_R2	      5.2
#define MG_WINVER_XP             5.1
#define MG_WINVER_2000           5.0
#define MG_WINVER_NT             4.0
#define MG_WINVER_95             3.0
#define MG_WINVER_32             2.0

#define MG_IPV4_ONLY             0
#define MG_IPV6_ONLY             1
#define MG_IPVX_DUAL_STACK       2
#define MG_IPVX_TWO_SOCKETS      3
#define MG_IPV4_OVER_IPV6        4

#define MG_OS_POOL_CONNECTION    0
#define MG_OS_CHECK_CONNECTIONS  1

#define MG_CS_RELEASE_TO_POOL    0
#define MG_CS_CLOSE              1
#define MG_CS_FORCE_DOWN         2

#define MG_STMEM                 8300
#define MG_MAXKEY                256
#define MG_MAXCON                8192

#define MAX_THREADS              1024

#define MG_TX_DATA               0
#define MG_TX_AKEY               1
#define MG_TX_AREC               2
#define MG_TX_EOD                3
#define MG_TX_AREC_FORMATTED     9

#define MG_RECV_HEAD             8
#define MG_RECV_HEAD_ERROR       "00000ce\n"
#define MG_CHUNK_SIZE_BASE       62


#if 1

/* Lower panel */
#define MG_SM_CELL_BGCOLOR       "#dce1f2"
/* Upper panel */
#define MG_SM_HCELL_BGCOLOR      "#dce1f2"
#define MG_SM_HCELL_COLOR        "#000000"
#define MG_SM_EVHEAD_COLOR       "#FF0000" /* RED */
#define MG_SM_EVTITLE_COLOR      "#0000FF" /* BLUE */
#define MG_SM_EVMESSAGE_COLOR    "#000000"

#else

/* Lower panel */
#define MG_SM_CELL_BGCOLOR       "#b3ecff"
/* Upper panel */
#define MG_SM_HCELL_BGCOLOR      "#80dfff"
#define MG_SM_HCELL_COLOR        "#000000"
#define MG_SM_EVHEAD_COLOR       "#000000"
#define MG_SM_EVTITLE_COLOR      "#007399"
#define MG_SM_EVMESSAGE_COLOR    "#007399"

#endif

/*
#ccf2ff
#b3ecff
#99e6ff
#80dfff
#66d9ff
#4dd2ff
#33ccff
*/

/* Parameter names - mgsi.ini */

#define MG_T_ENABLED                      "Enabled"
#define MG_T_DISABLED                     "Disabled"
#define MG_T_NOACTION                     "No Action"

#define MG_P_M_RESPONSE_TIMEOUT           "Server_Response_Timeout"
#define MG_P_QUEUED_REQUEST_TIMEOUT       "Queued_Request_Timeout"
#define MG_P_NO_ACTIVITY_TIMEOUT          "No_Activity_Timeout"
#define MG_P_DEFAULT_SERVICE              "Default_Service"
#define MG_P_ENV_PARAMETERS               "Env_Parameters"
#define MG_P_SYSTEM_MANAGER               "System_Manager"

#define MG_P_SSLS_SERVICE_STATUS          "SSLS_Service_Status"
#define MG_P_SSLS_TCP_PORT                "SSLS_TCP_Port"
#define MG_P_SSLS_CERTIFICATE_FILE        "SSLS_Certificate_File"
#define MG_P_SSLS_CERTIFICATE_KEY_FILE    "SSLS_Certificate_Key_File"
#define MG_P_SSLS_CA_CERTIFICATE_FILE     "SSLS_CA_Certificate_File"
#define MG_P_SSLS_CA_CERTIFICATE_PATH     "SSLS_CA_Certificate_Path"
#define MG_P_SSLS_VERIFY_CLIENT           "SSLS_Verify_Client"
#define MG_P_SSLS_VERIFY_DEPTH            "SSLS_Verify_Depth"

#define MG_P_SSLP_SERVICE_STATUS          "SSLP_Service_Status"
#define MG_P_SSLP_TCP_PORT                "SSLP_TCP_Port"

#define MG_P_SERVICE_STATUS               "Service_Status"
#define MG_P_IP_ADDRESS                   "Ip_Address"
#define MG_P_USERNAME                     "Username"
#define MG_P_PASSWORD                     "Password"
#define MG_P_BASE_TCP_PORT                "Base_TCP_Port"
#define MG_P_DEFAULT_USER_UCI             "Default_User_NameSpace"
#define MG_P_MINIMUM_SERVER_SESSIONS      "Minimum_Server_Sessions"
#define MG_P_MAXIMUM_SERVER_SESSIONS      "Maximum_Server_Sessions"
#define MG_P_NLS_TRANSLATION              "NLS_Translation"

/* Default parameter values */

#define MG_D_QUEUED_REQUEST_TIMEOUT       ""
#define MG_D_NO_ACTIVITY_TIMEOUT          ""
#define MG_D_DEFAULT_SERVICE              "LOCAL"
#define MG_D_ENV_PARAMETERS               ""
#define MG_D_SYSTEM_MANAGER               ""
#define MG_D_SERVICE_STATUS               "Enabled"
#define MG_D_IP_ADDRESS                   "127.0.0.1"
#define MG_D_BASE_TCP_PORT                "7041"
#define MG_D_DEFAULT_USER_UCI             "USER"
#define MG_D_NLS_TRANSLATION              ""
#define MG_D_M_RESPONSE_TIMEOUT           "20"
#define MG_D_MAXIMUM_ARRAY_SIZE           ""

#define MG_D_SERVER_TIMEOUT               20
#define MG_D_IDLE_TIMEOUT                 0
#define MG_D_QUEUE_TIMEOUT                0

#define MG_TITLE                          "Service Integration Gateway"

#define MG_DB_SERVER_TITLE                "DB Server"
#define MG_DB_SERVER_SPEC                 "a DB Server"
#define MG_DB_SERVER_TYPE                 ""


#if !defined(_WIN32)

#define SPRINTF         sprintf
#define T_SPRINTF       sprintf
#define T_STRLEN        strlen
#define T_STRCPY        strcpy
#define T_STRNCPY       strncpy
#define T_STRCMP        strcmp
#define T_STRNCMP       strncmp
#define T_STRSTR        strstr
#define T_STRCAT        strcat
#define T_STRTOL        strtol
#define T_FOPEN         fopen
#define T_FPUTS         fputs
#define T_FGETS         fgets
#define T_FCLOSE        fclose
#define T_CTIME         ctime

#define lstrlen         strlen
#define lstrcpy         strcpy
#define lstrcmpi        strcmp
#define lstrcpyn        strncpy

#define FAR
#define PASCAL
#define WINAPI

#define INVALID_HANDLE_VALUE 0

typedef int             HANDLE;
typedef char            * HINSTANCE;
typedef int             WSADATA;
typedef int             SOCKET;
typedef struct sockaddr SOCKADDR;
typedef struct sockaddr * LPSOCKADDR;
typedef struct hostent  HOSTENT;
typedef struct hostent  * LPHOSTENT;
typedef struct servent  SERVENT;
typedef struct servent  * LPSERVENT;

#ifdef MG_BS_GEN_PTR
typedef const void      * xLPSOCKADDR;
typedef void            * xLPIOCTL;
typedef const void      * xLPSENDBUF;
typedef void            * xLPRECVBUF;
#else
typedef LPSOCKADDR      xLPSOCKADDR;
typedef char            * xLPIOCTL;
typedef const char      * xLPSENDBUF;
typedef char            * xLPRECVBUF;
#endif /* #ifdef MG_BS_GEN_PTR */

#if defined(OSF1) || defined(SOLARIS) || defined(HPUX) || defined(HPUX10) || defined(HPUX11)
typedef int             socklen_mg;
#elif defined(LINUX) || defined(AIX) || defined(AIX5) || defined(MACOSX)
typedef socklen_t       socklen_mg;
#else
typedef size_t          socklen_mg;
#endif

typedef void            * MGHLIB;
typedef void            * MGPROC;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifndef INADDR_NONE
#define INADDR_NONE     -1
#endif

#define SOCK_ERROR(n)   (n < 0)
#define INVALID_SOCK(n) (n < 0)

/* Allow error 2 on read/write */

#define NOT_BLOCKING(n) (n != EWOULDBLOCK && n != 2)

#else /* Microsoft */

#define SPRINTF         sprintf
#define T_SPRINTF       _stprintf
#define T_STRLEN        _tcslen
#define T_STRCPY        _tcscpy
#define T_STRNCPY       _tcsncpy
#define T_STRCMP        _tcscmp
#define T_STRNCMP       _tcsncmp
#define T_STRSTR        _tcsstr
#define T_STRCAT        _tcscat
#define T_STRTOL        _tcstol
#define T_FOPEN         _tfopen
#define T_FPUTS         _fputts
#define T_FGETS         _fgetts
#define T_FCLOSE        fclose
#define T_CTIME         _tctime

typedef LPSOCKADDR      xLPSOCKADDR;
typedef u_long FAR      * xLPIOCTL;
typedef const char FAR  * xLPSENDBUF;
typedef char FAR        * xLPRECVBUF;
typedef int             socklen_mg;

typedef HINSTANCE       MGHLIB;
typedef FARPROC         MGPROC;

#define SOCK_ERROR(n)   (n == SOCKET_ERROR)
#define INVALID_SOCK(n) (n == INVALID_SOCKET)
#define NOT_BLOCKING(n) (n != WSAEWOULDBLOCK)

#endif /* #if !defined(_WIN32) */


/* Generic data types */

typedef struct tagMUTOBJ {
#if _WIN32
   HANDLE      hMutex;
#else
   pthread_mutex_t   memlock;
#endif
} MUTOBJ, FAR * LPMUTOBJ;


typedef struct tagSEMOBJ {
   int               stack;
   short             type;
   short             shm_res;
   char              sem_name[256];
#if defined(_WIN32)
   HANDLE            hSemaphore;

#else
   sem_t             *sd;
   int               semid;
   sem_t             memlock;
#endif /* #if defined(_WIN32) */

} SEMOBJ, FAR * LPSEMOBJ;


typedef struct tagDALT {
   int      status;
   char     server[64];
} DALT, FAR * LPDALT;


typedef struct tagMGALTSERVER {
   int      alt_status;
   int      alt_ptr;
   LPDALT   alt[MG_MAX_ALT];
   char     default_server[32];
} MGALTSERVER, FAR * LPMGALTSERVER;

#define MG_DBTYPE_M           0
#define MG_DBTYPE_BDB         1
#define MG_DBTYPE_MYSQL       2

typedef struct tagMGSERVER {
   short    dbtype;
   int      status;
   int      server_timeout;
   int      idle_timeout;
   int      log_errors;
   int      verbose;
   int      tcp_port;
   int      max_server_connections;
   int      version;
   char     m_server_name[32];
   char     uci[256];
   char     ip_address[64];
   char     database_type[16];
   char     database_version[16];
   char     default_user_uci[256];
   char     nls_translation[64];
   char     file_name[256];
   char     username[256];
   char     password[256];
   char     password_trans[256];
   void *   pdb;
   void *   pdbc;
} MGSERVER, FAR * LPMGSERVER;

#define DBC_ALLOC(a) 1

typedef struct tagMGCONNECTION {
   LPMGSERVER     p_server;
   SOCKET         cli_socket;
   WSADATA        wsadata;
   int            shndle;
   int            cycle_no;
   int            status;
   int            close;
   char           m_job_no[32];
   char           uci[256];
   short          eod;
   unsigned long  clen;
   unsigned long  rlen;
   int            tcp_port;
   int            binary;
   int            in_use;
   unsigned long  activity;
   time_t         t_prev;
   short          t_mode;
   char           b_term[8];
   int            b_term_len;
   unsigned char  buffer[128];
   int            buffer_len;
} MGCONNECTION, FAR * LPMGCONNECTION;


typedef struct tagMQC {
   char     m_job_no[32];
   void *   p_mqsrv;
} MQC, FAR * LPMQC;


typedef struct tagMEMOBJ {
   unsigned long  size;
   unsigned long  curr_size;
   unsigned long  incr_size;
   char *         p_buffer;
   char           buffer[MG_STMEM];
} MEMOBJ, FAR * LPMEMOBJ;


typedef struct tagNVLIST {
   char     *name;
   char     *value;
   char     *pBuffer;
   struct   tagNVLIST *p_next;
} NVLIST, *LPNVLIST;

typedef void * HNVLIST;

typedef struct tagMGREQ {
   int            request_source;
   SOCKET         cli_sockfd;
   int            cli_port;
   int            cli_gone;
   int            tcp_port;
   int            multipart;
   int            urlencoded;
   int            s_timeout;
   int            iBuffer_size;
   LPNVLIST       p_key_list;
   LPNVLIST       p_cgi_list;
   char *         iBuffer;
   char           cli_addr[64];
   char           srv_addr[64];
   char           request_heading[64];  
   char           path_translated[128];
   char           file_type[8];
   char           boundary[128];
   char           content_type[64];

   unsigned long  clen;
   unsigned long  rlen;
   int            key_no;
   int            stream;
   short          sysvars[16];

   NVLIST         nvpair[MG_MAXKEY];
   NVLIST *       p_nvpair;

   char           qstr[MG_MAXCON];
   char *         p_qstr;
   char           prfx[64];

   char *         request;
   char *         response;
   unsigned long  rsize;
   int            roffset;

   short          use_ssl;

#ifdef MG_SSL
   SSL_CTX *      ctx;
   SSL *          ssl;
#endif

} MGREQ, *LPMGREQ;

typedef unsigned long (WINAPI *PTHREAD_START_ROUTINE) (void *lpThreadParameter);

/* Thread Management */

#ifdef _WIN32

#define MG_THR_TYPE    unsigned long WINAPI
#define MG_THR_RETURN  result
typedef LPTHREAD_START_ROUTINE MG_THR_START_ROUTINE;

#else

#define MG_THR_TYPE    void *
#define MG_THR_RETURN  NULL
typedef void  *(*MG_THR_START_ROUTINE) (void *arg);

#endif


typedef struct tagTSDATA {
   int      socket_closed;
   SOCKET   sockfd;
} TSDATA, FAR * LPTSDATA;


typedef struct tagTHRCTRL {
#ifdef _WIN32
   unsigned long  thread_id;
   unsigned long  stack_size;
   HANDLE         hThread;
#else
   pthread_t thread_id;
   int stack_size;
#endif
} THRCTRL, FAR * LPTHRCTRL;


typedef struct tagTHRDATA {
   SOCKET         sockfd;
   SOCKET         newsockfd;
   short          queued;
   int            tcp_port;
   unsigned long  activity;
   int            cli_port;
   short          request_type;
   char           cli_addr[32];
   char           srv_addr[32];
   LPMGMQ         p_mgmq;
   short          thread_test;
   short          use_ssl;
   unsigned long  sig_request_no;
#ifdef MG_SSL
   SSL_CTX *      ctx;
#endif
} THRDATA, *LPTHRDATA;


typedef struct tagMGSISEC {
   int      req_no;
   char     wsmq_ipb[4096];
   char *   wsmq_ip[256];
} MGSISEC, FAR * LPMGSISEC;


typedef struct tagMGREC {
   int      kfirst;
   int      klast;

   int      rkey_size;
   char *   rkey;
   char *   rkeys[256];

   int      ckey_size;
   char *   ckey;
   char *   ckeys[256];

   int      data_size;
   char *   data;

} MGREC, FAR * LPMGREC;

#ifdef _WIN32
typedef struct _tagMGMSSD {
   PSID pEveryoneSID;
   PSID pAdminSID;
   PACL pACL;
   PSECURITY_DESCRIPTOR pSD;
   /* SECURITY_ATTRIBUTES sa; */
   PSECURITY_ATTRIBUTES pSA;
} MGMSSD, *LPMGMSSD;
#endif


typedef struct tagCORESHM {
   int shm;
   int size;
   int data;
   char shm_name[256];
} CORESHM, FAR *LPCORESHM;


typedef struct tagCOREDATA {
   short          http_server;
   short          no_dpapi;
   short          nagle_algorithm;
   short          ipv6;
   short          ipv6_srv;
   int            parent_process;
   int            parent_pid;
   int            control;
   int            process_count;
   int            error_count;
   int            diagnostics;
   int            log_errors;
   int            verbose;
   int            sig_port;
   int            server_port;
   int            max_connections;
   int            max_servers;
   int            no_children;
   int            server_timeout;
   int            idle_timeout;
   int            queue_timeout;
   int            ssls_status;
   int            ssls_port;
   int            ssls_closedown;
   int            ssls_verify_depth;
   int            ssls_verify_client;
   int            ssp_status;
   int            ssp_port;
   int            ssp_closedown;
   int            winsock;
   double         os_version;
   SOCKET         server_sockfd;
   SOCKET         server_sockfd_ipv6;
   unsigned long  session_no;
   unsigned long  session_max;
   unsigned long  php_activity;
   unsigned long  go_activity;
   unsigned long  jsp_activity;
   unsigned long  aspx_activity;
   unsigned long  python_activity;
   unsigned long  ruby_activity;
   unsigned long  perl_activity;
   unsigned long  apache_activity;
   unsigned long  cgi_activity;
   unsigned long  ws_loaded;
   unsigned long  ws_activity;
   unsigned long  ws_session_max;
   unsigned long  ws_session_magic;
   unsigned long  http_activity;
   unsigned long  sm_activity;
   unsigned long  sig_request_no;
   LPMUTOBJ       p_sem_gethost;
   LPMUTOBJ       p_memlockSA;
   LPMUTOBJ       p_memlockPA;
   LPMUTOBJ       p_memlockTH;
   LPMUTOBJ       p_memlockFA;
   LPMUTOBJ       p_memlockLG;
   LPCORESHM      p_core_shm;
   HINSTANCE      dll_hinst;
   char           mg_version[32];
   char           sig_manager[4];
   char           system_manager[256];
   char           server_ip_address[64];
   char           default_service[64];
   char           mg_lib_path[64];
   char           config_path[64];
   char           root_dir[64];
   char           mg_ini[64];
   char           mg_sec[64];
   char           mg_log[64];
   char           mg_log_default[128];
   char           mg_sys_ini[64];
   char           mg_pid[64];
   char           mg_title[32];
   char           ip_address[256];
   char           ssls_cert_file[128];
   char           ssls_cert_key_file[128];
   char           ssls_ca_file[128];
   char           ssls_ca_path[128];
   char           sig_usr[32];
   char           sig_grp[32];

   THRCTRL  daemon_thread;
#ifdef _WIN32
   HANDLE         h_core_shm;
   int            wsastartup;
   WSADATA        wsadata;
   HANDLE         thread_handles[MG_MAX_CONNECTIONS];
#endif
} COREDATA, FAR *LPCOREDATA;


typedef struct _tagMGMAIN {
   short ipv4;
   short ipv6;
   short service;
   int   silent;
   int   help;
   char  binary[32];
   char  tcp_port[32];
} MGMAIN, *LPMGMAIN;


typedef struct _tagMGCONF {
   char file[256];
   char section[256];
   char name[256];
   char value[256];
} MGCONF, *LPMGCONF;


typedef struct _tagSMDATA {
   int   service_status;
   char *s_buffer;
   char  button[16];
   char  config_section[32];
   char  new_dbserver[32];
   char  mq_service[32];
   char  new_mq_service[32];
   char  username[256];
   char  pw1[64]; /* CM0051 */
   char  pw2[64];
} SMDATA, *LPSMDATA;


typedef struct _tagSMSERVER {
   int status;
   char server[32];
} SMSERVER, *LPSMSERVER;


/* Externals */

extern LPMGSERVER       mg_servers[];
extern LPMGALTSERVER    mg_alt_servers[];
extern LPMGCONNECTION   mg_connections[];
extern LPMQC            mg_mqctable[];
extern LPMGMQ           mg_mqstable[];
extern MGSISEC          mg_sec;
extern COREDATA         core_data;

#define MG_REC          "\x02"
#define MG_EOD          "\x07"
#define MG_AK           "\x04"
#define MG_AR           "\x05"
#define MG_ERR          "\x06"

static char *drec = "\x02";
static char *deod = "\x07";
static char *dak  = "\x04";
static char *dar  = "\x05";
static char *derr = "\x06";


int                  mg_wsmq_init               (int mode);

#endif /* #ifndef MGSI_H */
