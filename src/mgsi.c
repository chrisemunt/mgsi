/*
   ----------------------------------------------------------------------------
   | mgsi                                                                     |
   | Description: Service Integration Gateway                                 |
   | Author:      Chris Munt cmunt@mgateway.com                               |
   |                         chris.e.munt@gmail.com                           |
   | Copyright (c) 2002-2020 M/Gateway Developments Ltd,                      |
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

/*

Development Diary (in brief)
============================

CM0001: Build 1.0.0: 15 November 2002
   Introduce support for PHP - initial m_php implementation.

CM0002: Build 1.0.26:
   Implement support for multi-dimensional arrays in m_php.

CM0003: Build 1.1.27:
   Port m_php to UNIX.

CM0004: Build 1.2.28:
   Burn the documentation into the management forms.

CM0005: Build 1.3.37:
   Move away from frame-based management forms.

CM0006: Build 1.4.39:
   Introduce support for Cache NLS system.

CM0007: Build 1.5.41:
   Introduce support for WebSphere MQ - m_websphere_mq.

CM0008: Build 1.5.49:
   Improve support for DSO management for m_websphere_mq.

CM0009: Build 1.5.50:
   Add the ‘no activity timeout’ parameter.

CM0010: Build 1.5.51:
   Introduce improved license management.
   Release m_websphere_mq.

CM0011: Build 1.5.52:
   Port to MAC OS X (PPC).

CM0012: Build 1.5.53:
   Redo documentation and burn into MGSI.

CM0013: Build 1.5.56:
   Respond to Quest issues:
   - Ref: 20050316-01
   - Ref: 20050315-02

CM0014: Build 1.5.58:
   Respond to Quest issues:
   - Ref: 20050324-01 - sigsegv on starting queue listeners on Linux.

CM0015: Build 1.5.60:
   Respond to Quest issues:
   - Ref: 20050315-02 - spurious MQ 2042 errors for replies.

CM0016: Build 1.5.61:
   Respond to Quest issues:
   - Ref: 20050317-01 - fix for multiple MQ listener services.

CM0017: Build 1.6.64:
   Introduce support for JSP - m_jsp

CM0018: Build 1.6.65:
   Burn new M/Gateway logo into the management forms.

CM0019: Build 1.6.66:
   Fix a bug in the license checker.

CM0020: Build 1.6.67: 30 September 2005
   Introduce keepalive connectivity.

CM0021: Build 1.6.68: 30 October 2005

CM0022: Build 1.6.69: 17 February 2006

CM0023: Build 1.6.70: 7 March 2006
   Introduce support for ASP.NET - m_aspx 

CM0024: Build 1.6.71: 17 August 2006
   Fix a fault that could result in large arrays becoming prematurely terminated and/or corrupted when transmitted between the Gateway and M/Cache.
   Fix a fault in the m_php module that could result in multi-dimensional arrays containing a mixture of string and numeric keys (or subscripts) becoming corrupted when transmitted between the PHP environment and M/Cache.

CM0025: Build 1.6.72: 27 September 2006
   Fix a spurious ‘no licence error’ that could occur in busy m_php installations.

CM0026: Build 1.6.73: 3 May 2007
   Tidy source code for export.
 
CM0027: Build 1.6.74: 17 May 2007
   Add 'Internal HTTP Service Status' parameter to the 'security' section to allow users to disable the internal HTTP server.

CM0028: Build 1.7.75: 27 May 2007
   Add support for Berkeley Database (BDB).

CM0029: Build 1.7.76: 11 April 2008
   Fix a buffer overrun caused by oversize HTTP query strings being posted directly at MGSI.

CM0030: Build 1.8.77: 1 May 2008
   Introduce m_python.

CM0031: Build 1.9.78: 5 May 2008
   Introduce m_ruby.

CM0032: Build 1.9.79: 16 May 2008
   Redesign management forms.

CM0033: Build 2.0.80: 21 June 2008
   Introduce a new transmission protocol to support binary data.
   - Ref: CDS.

CM0034: Build 2.0.81: 5 July 2008
   Remove static linkage to the BDB library.
   - Ref: CDS.

CM0035: Build 2.0.82: 11 August 2008
   Introduce the management parameters for load balancing and failover.
   - Ref: National Systems.

CM0036: Build 2.0.83: 10 September 2008 (related to CM0033)
   Fix a problem with the transmission of large result strings (> 10000 Bytes).
   - Ref: CDS.

CM0037: Build 2.0.84: 17 September 2008 (related to CM0033)
   Add (up to) base-62 chunk size encoding and apply to the main request/response chunk headers.
   - Ref: CDS.

CM0038: Build 2.0.85: 25 September 2008 (related to CM0033)
   Fix a crash in clients (e.g. PHP) caused by MGSI not being able to connect to Cache.
   Make sure errors are properly formatted: nnnnnce\n

CM0039: Build 2.0.86: 10 October 2008 (related to CM0033)
   Miscellaneous bug fixes.

CM0040: Build 2.0.87: 30 October 2008
   Bind to OpenSSL.
   Add HMAC and B64 functions.

CM0041: Build 2.0.88: 3 November 2008
   Fix an issue with passing the server name through the first argument of the m_* functions.
   Fix problem with passing large responses through m_apache.
   Add ZTS() function

CM0042: Build 2.0.89: 19 November 2008
   Deal with first-time connection failures.
   - Add back-off to (1 second pause) to the connect loop (and Attempt no = 1 to 7 (instead of 3)).
   - Initialise variables (integers) in mg_tcp_read()

CM0043: Build 2.0.90: 25 November 2008
   Miscellaneous bug fixes.

CM0044: Build 2.0.91: 19 December 2008
   ZMGSI revision 4
   Miscellaneous bug fixes.
   Add SHA256 functions.

CM0045: Build 2.0.92: 12 February 2009
   Miscellaneous bug fixes.

CM0046: Build 2.0.93: 17 February 2009
   Miscellaneous bug fixes.

CM0047: Build 2.0.94: 19 February 2009
   Miscellaneous bug fixes.

CM0048: Build 2.0.95: 1 July 2009
   [x]inetd support.

CM0049: Build 2.0.96: 20 February 2010
   Better backoff for new Cache connections.

CM0050: Build 2.1.97: 12 August 2013
   Add username/password (prototype).

CM0051: Build 2.1.98: 19 August 2013
   Add username/password.

CM0052: Build 2.2.99: 7 August 2014
   Increase accepted data volumes on the input side to 1MB (up from 32K).

CM0053: Build 2.2.100: 18 September 2014
   Miscellaneous bug fixes.

CM0054: Build 2.3.101: 11 February 2020
   Update Windows network infrastructure to winsock v2.2.
   Update network functionality to be IPv6 compliant.
   Update embedded documentation page.
   Remove old licensing code.
   Miscellaneous bug fixes.

CM0055: Build 3.1.102: 3 March 2020
   Release as an Open Source project.

*/


#include "mgsisys.h"
#include "mgsi.h"
#include "mgsiutl.h"
#include "mgsireq.h"
#include "mgsidba.h"
#include "mgsicfg.h"
#include "mgsidso.h"
#include "mgsitls.h"
#include "mgsiadm.h"


#ifdef _WIN32
typedef BOOL (WINAPI * PCHANGESERVICECONFIG2) (SC_HANDLE hService,  unsigned long dwInfoLevel, void *lpInfo); 
#endif

#if !defined(_WIN32)
static pthread_key_t    tsdata_key;
static pthread_once_t   tsdata_once = PTHREAD_ONCE_INIT;
#endif

LPMGSERVER              mg_servers[MG_MAX_SERVERS];
LPMGALTSERVER           mg_alt_servers[MG_MAX_SERVERS];
LPMGCONNECTION          mg_connections[MG_MAX_CONNECTIONS];
LPMQC                   mg_mqctable[MG_MAX_MQ_CONNECTIONS];
LPMGMQ                  mg_mqstable[MG_MAX_CONNECTIONS];
MGSISEC                 mg_sec;


#if defined(_WIN32)
#if defined(MG_MGWSI)
static char             mg_winservice_name[]    = "Mgwsi";
#else
static char             mg_winservice_name[]    = "mgsi";
#endif
LPCTSTR                 mg_winservice_desc      = "Service Integration Gateway";
LPCTSTR                 mg_winservice_desc_ex   = "M/Gateway Development's Service Integration Gateway.  Mediates between software components providing services to applications.";
SERVICE_STATUS          mg_winservice_status; 
SERVICE_STATUS_HANDLE   mg_winservice_status_handle;
#endif

#if !defined(_WIN32)
static pid_t            sig_child[MG_MAX_CHILD];
#endif

COREDATA                core_data   = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0"};

extern int errno;


int                  main                          (int argc, char *argv[]);
int                  main_ex                       (int argc, char *argv[]);

#if defined(_WIN32)
void                 mg_winservice_start           (unsigned long argc, char **argv);
unsigned long        mg_winservice_init            (unsigned long argc, char **argv, unsigned long *specific_error);
VOID                 mg_winservice_ctrl            (unsigned long opcode);
#endif

int                  mg_main_ctrl                  (LPMGMAIN p_mg_main);
MG_THR_TYPE          mg_server_child               (void *arg);
int                  mg_config_files               (void);
void                 mg_startup                    (void);
void                 mg_closedown                  (void);
void                 mg_startup_message            (char *message, int message_size);
int                  mg_initialize                 (void);
int                  mg_uninitialize               (void);
MG_THR_TYPE          mg_daemon                     (void *p_parameters);
MG_THR_TYPE          mg_wsmq_server                (void *arg);

static void          mg_once                       (void);
static void          mg_destructor                 (void *ptr);
static void          mg_goingdown                  (void);
static void          mg_goingdown_signal           (int sig);
static void          mg_parent_goingdown_signal    (int sig);
static void          mg_error_trap                 (int sig);
static void          mg_parent_error_trap          (int sig);

double               mg_get_os_version             (void);


int main(int argc, char *argv[])
{
#if defined(_WIN32)
   char buffer[256];
   SERVICE_TABLE_ENTRY DispatchTable[] = {
      {
         mg_winservice_name,
         (LPSERVICE_MAIN_FUNCTION) mg_winservice_start
      },
      {
         NULL,
         NULL
      }
   };

   if (argc > 1) {
      main_ex(argc, argv);
      return 0;
   }

   mg_config_files();

   if (!StartServiceCtrlDispatcher(DispatchTable)) {
      T_SPRINTF(buffer, "StartServiceCtrlDispatcher Error = %u", GetLastError());
      mg_log_event(buffer, "Service Integration Gateway: Windows Service");
   }

   mg_log_event("Service Stopped", "Service Integration Gateway: Windows Service");

   return 0;
#else
   return main_ex(argc, argv);
#endif
}


#if defined(_WIN32)

void mg_winservice_start(unsigned long argc, char **argv)
{ 
   unsigned long status; 
   unsigned long specificError; 
   MGMAIN mg_main;

   mg_winservice_status.dwServiceType        = SERVICE_WIN32; 
   mg_winservice_status.dwCurrentState       = SERVICE_START_PENDING; 
   mg_winservice_status.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE; 
   mg_winservice_status.dwWin32ExitCode      = 0; 
   mg_winservice_status.dwServiceSpecificExitCode = 0; 
   mg_winservice_status.dwCheckPoint         = 0; 
   mg_winservice_status.dwWaitHint           = 0; 
 
   mg_winservice_status_handle = RegisterServiceCtrlHandler(mg_winservice_name, (LPHANDLER_FUNCTION) mg_winservice_ctrl);
 
   if (mg_winservice_status_handle == (SERVICE_STATUS_HANDLE) 0) {
      mg_log_event("RegisterServiceCtrlHandler Failed", "Service Integration Gateway Windows Service");
      return;
   } 
 
   /* Initialization code goes here. */

   status = mg_winservice_init(argc, argv, &specificError);
 
   /* Handle error condition */

   if (status != NO_ERROR) {
      mg_winservice_status.dwCurrentState       = SERVICE_STOPPED; 
      mg_winservice_status.dwCheckPoint         = 0; 
      mg_winservice_status.dwWaitHint           = 0; 
      mg_winservice_status.dwWin32ExitCode      = status; 
      mg_winservice_status.dwServiceSpecificExitCode = specificError; 
 
      SetServiceStatus(mg_winservice_status_handle, &mg_winservice_status);

      return;

   }
 
   /* Initialization complete - report running status. */

   mg_winservice_status.dwCurrentState       = SERVICE_RUNNING; 
   mg_winservice_status.dwCheckPoint         = 0; 
   mg_winservice_status.dwWaitHint           = 0; 
 
   if (!SetServiceStatus (mg_winservice_status_handle, &mg_winservice_status)) {
      status = GetLastError();
      mg_log_event("SetServiceStatus Error", "Service Integration Gateway Windows Service");
   }
 
   /* This is where the service does its work. */

   mg_main.silent = 0;
   mg_main.help = 0;
   strcpy(mg_main.binary, mg_winservice_name);
   strcpy(mg_main.tcp_port, "");

   mg_main_ctrl(&mg_main);

   return;
}
 

/* Stub initialization function. */

unsigned long mg_winservice_init(unsigned long   argc, char * *argv, unsigned long *specificError)
{ 
   argv;
   argc;
   specificError;

   return(0);
} 


VOID mg_winservice_ctrl(unsigned long Opcode)
{ 
   unsigned long status; 
 
   switch (Opcode) { 
      case SERVICE_CONTROL_PAUSE:

         /* Do whatever it takes to pause here. */

         mg_winservice_status.dwCurrentState = SERVICE_PAUSED; 
         break; 
 
      case SERVICE_CONTROL_CONTINUE: 

         /* Do whatever it takes to continue here. */

         mg_winservice_status.dwCurrentState = SERVICE_RUNNING; 
         break; 
 
      case SERVICE_CONTROL_STOP: 

         /* Do whatever it takes to stop here. */

         mg_log_event("Stopping Service", "Service Integration Gateway Windows Service");
         mg_closedown();
         MGNET_CLOSESOCKET(core_data.server_sockfd);
         MGNET_WSACLEANUP();

         mg_winservice_status.dwWin32ExitCode = 0; 
         mg_winservice_status.dwCurrentState  = SERVICE_STOPPED; 
         mg_winservice_status.dwCheckPoint    = 0; 
         mg_winservice_status.dwWaitHint      = 0; 
 
         if (!SetServiceStatus(mg_winservice_status_handle, &mg_winservice_status)) { 
            status = GetLastError(); 
            mg_log_event("SetServiceStatus Error", "Service Integration Gateway Windows Service"); 
         } 
 
         return; 
 
      case SERVICE_CONTROL_INTERROGATE:

         /* Fall through to send current status. */

         break; 
 
      default:

            mg_log_event("Unrecognized Option Code", "Service Integration Gateway Windows Service"); 
   } 
 
   /* Send current status. */
 
   if (!SetServiceStatus(mg_winservice_status_handle, &mg_winservice_status)) { 
      status = GetLastError(); 
      mg_log_event("SetServiceStatus Error ", "Service Integration Gateway Windows Service");
   }
 
   return; 
} 

#endif /* #if defined(_WIN32) */


int main_ex(int argc, char *argv[])
{
   int n, len, service, startup, closedown, start, resets, thread_test, no_children_specified, status;
   char ibuffer[256], obuffer[256], error_message[128];
   char *p;
   MGMAIN mg_main;
   MGCONF *pconf;
   THRCTRL thread_control[MAX_THREADS];
   THRDATA thr_data[MAX_THREADS];
   FILE *fp;
#if defined(_WIN32)
   int pid, childpid;
   double win_version;
   char buf[16];
   SERVICE_STATUS ssStatus, ssStatus1; 
   unsigned long dwOldCheckPoint; 
   unsigned long dwStartTickCount;
   unsigned long dwWaitTime;
   unsigned long dwStatus;
   SC_HANDLE schSCManager;
   SC_HANDLE schService;

   char mg_winservice_path_name[256];
   LPCTSTR lpMachineName = NULL;   /* computer name */
   LPCTSTR lpDatabaseName = NULL;  /* SCM database name */
#else
   pid_t pid, childpid;
   uid_t uid;
   gid_t gid;
   struct passwd *pwd;
   struct group *grp;
#endif

   start = 0;
   status = 0;
   closedown = 0;
   service = 1;
   startup = 1;
   resets = 0;
   pid = 0;
   childpid = 0;
   no_children_specified = 0;
   thread_test = 0;
   *error_message = '\0';
   *ibuffer = '\0';
   *obuffer = '\0';

   pconf = NULL;
   fp = NULL;

   mg_main.silent = 0;
   mg_main.help = 0;
   mg_main.ipv4 = 0;
   mg_main.ipv6 = 0;
   strcpy(mg_main.tcp_port, "");
   strcpy(mg_main.binary, argv[0]);

   for (n = 0; n < argc; n ++) {

      if (T_STRLEN(argv[n]) > 200)
         continue;

      strncpy(obuffer, argv[n], 250);
      obuffer[250] = '\0';
      strncpy(ibuffer, argv[n], 250);
      ibuffer[250] = '\0';
      mg_lcase(obuffer);

      if (!n) {
         strcpy(ibuffer, argv[n]);
      }
      else if (isdigit(obuffer[0])) {
         strcpy(mg_main.tcp_port, obuffer);
      }
      else if (pconf) {
         p = strstr(argv[n], "=");
         ibuffer[0] = '\0';
         if (p) {
            p ++;
            if (*p == '\"') {
               p ++;
               strncpy(ibuffer, p, 250);
               ibuffer[250] = '\0';
               len = (int) strlen(ibuffer);
               if (len > 2 && ibuffer[len - 1] == '\"') {
                  len --;
                  ibuffer[len] = '\0';
               }
            }
            else {
               strncpy(ibuffer, p, 250);
               ibuffer[250] = '\0';
            }
         }
         if (!strncmp(obuffer, "file=", 5)) {
            strcpy(pconf->file, ibuffer);
         }
         else if (!strncmp(obuffer, "section=", 8)) {
            strcpy(pconf->section, ibuffer);
         }
         else if (!strncmp(obuffer, "name=", 5)) {
            strcpy(pconf->name, ibuffer);
         }
         else if (!strncmp(obuffer, "value=", 6)) {
            strcpy(pconf->value, ibuffer);
         }
      }
      else if (strlen(obuffer) > 1) {
         if (!strncmp(obuffer, "-p", 2))
            strcpy(mg_main.tcp_port, obuffer + 2);
         else if (!strncmp(obuffer, "-p", 2) && isdigit(obuffer[2]))
            T_STRCPY(mg_main.tcp_port, obuffer + 2);
#if !defined(_WIN32)
         else if (!strncmp(obuffer, "-m=", 3)) {
            T_STRNCPY(core_data.sig_manager, obuffer + 3, 3);
            core_data.sig_manager[3] = '\0';
            mg_lcase(core_data.sig_manager);
         }
         else if (!strncmp(obuffer, "-c=", 3)) {
            no_children_specified = 1;
            core_data.no_children = (int) strtol(obuffer + 3, NULL, 10);
            if (core_data.no_children < 0)
               core_data.no_children = 0;
            if (core_data.no_children > MG_MAX_CHILD)
               core_data.no_children = MG_MAX_CHILD;
         }
#endif
         else if (!strncmp(obuffer, "-stop", 5))
            closedown = 1;
         else if (!strncmp(obuffer, "stop", 4))
            closedown = 1;
         else if (!strncmp(obuffer, "-pause", 6))
            closedown = 2;
         else if (!strncmp(obuffer, "pause", 5))
            closedown = 2;
         else if (!strncmp(obuffer, "-cont", 5))
            closedown = 3;
         else if (!strncmp(obuffer, "cont", 4))
            closedown = 3;
         else if (!strncmp(obuffer, "-restart", 5))
            closedown = 1;
         else if (!strncmp(obuffer, "restart", 4))
            closedown = 1;
         else if (!strncmp(obuffer, "-start", 6))
            start = 1;
         else if (!strncmp(obuffer, "start", 5))
            start = 1;
#if defined(MG_IPV6)
         else if (!strncmp(obuffer, "-ipv4", 5) || !strncmp(obuffer, "ipv4", 4))
            mg_main.ipv4 = 1;
         else if (!strncmp(obuffer, "-ipv6", 5) || !strncmp(obuffer, "ipv6", 4))
            mg_main.ipv6 = 1;
#endif
         else if (!strncmp(obuffer, "-s", 2))
            mg_main.silent = 1;
         else if (!strncmp(obuffer, "-v", 2)) {
            service = 0;
            mg_main.service = 0;
         }
         else if (!strncmp(obuffer, "-h", 2) || !strncmp(obuffer, "-?", 2))
            mg_main.help = 1;
         else if (!strncmp(obuffer, "-tt", 3)) {
            if (obuffer[3] == '=')
               thread_test = (int) strtol(obuffer + 4, NULL, 10);
            if (thread_test != 2)
               thread_test = 1;
         }
         else if (!strncmp(obuffer, "-uc", 3)) {
            pconf = mg_malloc(sizeof(MGCONF), "config");
            if (pconf) {
               pconf->file[0] = '\0';
               pconf->section[0] = '\0';
               pconf->name[0] = '\0';
               pconf->value[0] = '\0';
            }
         }
         else {
            T_SPRINTF(error_message, "Unrecognized option: %s", obuffer);
            break;
         }
      }
      else {
         T_SPRINTF(error_message, "Unrecognized option: %s", obuffer);
         break;
      }
   }

   if (T_STRLEN(error_message)) {
      printf("\nError: %s\n", error_message);
      exit(0);
   }

   mg_main.service = service;

   if (pconf) {
      char encoded_pwd[8192];

      if (!pconf->file[0]) {
         strcpy(pconf->file, MG_INI_FILE);
      }
      if (!pconf->section[0]) {
         return 1;
      }
/*
      printf("file:%s\r\n", pconf->file);
      printf("section:%s\r\n", pconf->section);
      printf("name:%s\r\n", pconf->name);
      printf("value:%s\r\n", pconf->value);
*/
      strcpy(ibuffer, pconf->name);
      mg_lcase((char *) ibuffer);
      if (!strcmp(ibuffer, "password")) {
         mg_encode_password((char *) encoded_pwd, (char *) pconf->value, 2);
         mg_set_config(pconf->section, pconf->name, (char *) encoded_pwd, pconf->file);
      }
      else {
         mg_set_config(pconf->section, pconf->name, (char *) pconf->value, pconf->file);
      }

      mg_free(pconf, "config");
      pconf = NULL;
      return 1;
   }

#if !defined(_WIN32)
   uid = getuid();
   gid = getgid();
   if ((pwd = getpwuid(uid)) != NULL) {
      T_STRNCPY(core_data.sig_usr,  pwd->pw_name, 30);
      core_data.sig_usr[30] = '\0';
   }
   if ((grp = getgrgid(gid)) != NULL) {
      T_STRNCPY(core_data.sig_grp, grp->gr_name, 30);
      core_data.sig_grp[30] = '\0';
   }
/*
   printf("\nService Integration Gateway: Process : usr=%s; grp=%s.\n", core_data.sig_usr, core_data.sig_grp);
*/
#endif


/* Set default for number of child processes */

#if !defined(_WIN32)
#if defined(OSF1)
   if (core_data.no_children < 1)
      core_data.no_children = 1;
#endif
#endif

#if !defined(_WIN32)
   if (!no_children_specified)
      core_data.no_children = 1;
#endif

   /* TODO - set up infrastructure for operating over multiple processes */
   core_data.no_children = 0; /* override */

   if (thread_test) {
      int cycle;
      long nt, max;

      printf("\n\nService Integration Gateway thread test running (%d) ...\nThis test takes a few minutes - please wait ...\n", thread_test);

      mg_startup(); /* Service Integration Gateway Thread Test */

      n = 0;

      max = 32768;

#if defined(PTHREAD_THREADS_MAX)
      max = PTHREAD_THREADS_MAX;
#elif defined(_POSIX_THREAD_THREADS_MAX)
      max = _POSIX_THREAD_THREADS_MAX;
#endif

      T_STRCPY(obuffer, "");

      if (thread_test == 2)
         mg_mutex_lock(core_data.p_memlockTH);

      for (nt = 0; nt < max; nt ++) {

         cycle = 0;

         thr_data[cycle].thread_test = thread_test;

         n = mg_thread_create((LPTHRCTRL) &(thread_control[cycle]), (MG_THR_START_ROUTINE) mg_server_child, (void *) &(thr_data[cycle]));

         if (n != 0) {

            mg_mutex_unlock(core_data.p_memlockTH);

#if defined(PTHREAD_THREADS_MAX)
            T_SPRINTF(obuffer, "Maximum number of threads created = %ld; Last creation error = %d; \nPTHREAD_THREADS_MAX = %d", nt, n, PTHREAD_THREADS_MAX);
#elif defined(_POSIX_THREAD_THREADS_MAX)
            T_SPRINTF(obuffer, "Maximum number of threads created = %ld; Last creation error = %d; \n_POSIX_THREAD_THREADS_MAX = %d", nt, n, _POSIX_THREAD_THREADS_MAX);
#else
            T_SPRINTF(obuffer, "Maximum number of threads created = %ld; Last creation error = %d", nt, n);
#endif

            break;
         }

      }

      mg_pause(60000);

      mg_closedown();

      if (n == 0) {
#if defined(PTHREAD_THREADS_MAX)
         T_SPRINTF(obuffer, "Maximum number of threads successfully created = %ld; \nPTHREAD_THREADS_MAX = %d", nt, PTHREAD_THREADS_MAX);
#elif defined(_POSIX_THREAD_THREADS_MAX)
         T_SPRINTF(obuffer, "Maximum number of threads successfully created = %ld; \n_POSIX_THREAD_THREADS_MAX = %d", nt, _POSIX_THREAD_THREADS_MAX);
#else
         T_SPRINTF(obuffer, "Maximum number of threads successfully created = %ld", nt);
#endif
      }

      printf("\nThread test complete\n%s\n\n", obuffer);

      exit(0);
   }

#if !defined(_WIN32)

   if (closedown) {

      fp = fopen(MG_PID_FILE, "r");
      if (!fp) {
         printf("\nService Integration Gateway: Cannot find the process ID of the Gateway (%s).\n", MG_PID_FILE);
         return 1;
      }

      fgets(ibuffer, 255, fp);
      fclose(fp);

      pid = (unsigned int) strtol(ibuffer, NULL, 10);

      if (!pid) {
         printf("\nService Integration Gateway: Cannot identify the process ID of the Gateway (%s).\n", MG_PID_FILE);
         return 1;
      }

      if (closedown == 1) {

         n = kill(pid, SIGTERM);

         if (n < 0) {
            if (errno == EINVAL)
               T_SPRINTF(obuffer, "\nService Integration Gateway: Process (%d) not closed down\n(ERROR: Bad Argument).\n", pid);
            else if (errno == EPERM)
               T_SPRINTF(obuffer, "\nService Integration Gateway: Process (%d) not closed down\n(ERROR: Insufficient Permissions).\n", pid);
            else if (errno == EPERM)
               T_SPRINTF(obuffer, "\nService Integration Gateway: Process (%d) not closed down\n(ERROR: Process Does Not Exist).\n", pid);
            else
               T_SPRINTF(obuffer, "\nService Integration Gateway: Process (%d) not closed down\n(ERROR: Unknown).\n", pid);

            T_SPRINTF(ibuffer, "%u", pid);
            mg_set_config("SYSTEM", "Close_Down", ibuffer, core_data.mg_ini);
            status = 0;
            printf("\nService Integration Gateway: Process (%d): Sending close down signal.\nPlease wait ...", pid);
            fflush(stdout);
            for (n = 0; n < 20; n ++) {
               mg_pause(1000);
               printf("..");
               fflush(stdout);
               ibuffer[0] = '\0';
               mg_get_config("SYSTEM", "Close_Down", "", ibuffer, 32, core_data.mg_ini);
               if (!strcmp(ibuffer, "0")) {
                  status = 1;
                  break;
               }
            }
            if (status) {
               printf("\n\nService Integration Gateway: Process (%d) closed down.\n", pid);
               unlink(MG_PID_FILE);
            }
            else {
               printf("\n\n%s\nUnable to signal the process.\n", obuffer);
            }
         }
         else {
            printf("\nService Integration Gateway: Process (%d) closed down.\n", pid);
            unlink(MG_PID_FILE);
         }
      }
      else if (closedown == 2) {
         n = kill(pid, SIGSTOP);
         if (n < 0)
            printf("\nService Integration Gateway: Process (%d): ERROR: Cannot pause the process\n(Check permissions and that the process exists - errno=%d).\n", pid, errno);
         else
            printf("\nService Integration Gateway: Process (%d) signalled to pause.\n", pid);
      }
      else if (closedown == 3) {
         n = kill(pid, SIGCONT);
         if (n < 0)
            printf("\nService Integration Gateway: Process (%d): ERROR: Cannot continue the process\n(Check permissions and that the process exists - errno=%d).\n", pid, errno);
         else
            printf("\nService Integration Gateway: Process (%d) signalled to continue.\n", pid);
      }

      return 0;
   }

#endif


#ifdef _WIN32

   win_version = mg_get_os_version();

   if (service && win_version >= MG_WINVER_NT) {

      n = 0;
      if (_getcwd(mg_winservice_path_name, 200)) {
         n = (int) strlen(mg_winservice_path_name);
         if (n) {
            if (mg_winservice_path_name[n - 1] != '\\')
               strcat(mg_winservice_path_name, "\\");
#if defined(MG_MGWSI)
            strcat(mg_winservice_path_name, "mgwsisv.exe");
#else
            strcat(mg_winservice_path_name, "mgsi.exe");
#endif
         }
      }
      if (!n) {
#if defined(MG_MGWSI)
         strcpy(mg_winservice_path_name, "c:\\mgwsi\\mgwsisv.exe");
#else
         strcpy(mg_winservice_path_name, "c:\\mgsi\\mgsi.exe");
#endif
      }
      if (!mg_main.silent) {
         printf("\nM/Gateway Developments Ltd - Service Integration Gateway");
         sprintf(obuffer, "\n\n   Version %s (Created: %s)", MG_VERSION, MG_VERSION_CREATED);

         printf(obuffer);
         printf("\n\nService Management\n\n");
      }

      schSCManager = OpenSCManager(lpMachineName, lpDatabaseName, SC_MANAGER_ALL_ACCESS);
      if (schSCManager == NULL) {
         unsigned long error_code;
         char error_message[512];

         printf("Unable to Open Service Manager...\r\n");

         error_code = mg_get_last_error(1);
         *error_message = '\0';
         mg_get_error_message(error_code, error_message, 511, 0);
         printf("%d : %s\r\nMachineName=%s; DatabaseName=%s;\r\nWinServiceName=%s; WinServicePathName=%s;\r\nWinServiceDesc=%s\r\n", error_code, error_message, lpMachineName, lpDatabaseName, mg_winservice_name, mg_winservice_path_name, mg_winservice_desc);
         return 1;
      }

      schService = OpenService(schSCManager, mg_winservice_name, SERVICE_ALL_ACCESS);
 
      if (schService == NULL) {

         printf("Attempting to Create Service ...\r\n");

         schService = CreateService( 
                     schSCManager,              /* SCManager database */
                     mg_winservice_name,            /* name of service */
                     mg_winservice_desc,            /* service name to display */
                     SERVICE_ALL_ACCESS,        /* desired access */
                     SERVICE_WIN32_OWN_PROCESS, /* service type */
                     SERVICE_DEMAND_START,      /* start type */
                     SERVICE_ERROR_NORMAL,      /* error control type */
                     mg_winservice_path_name,        /* service's binary */
                     NULL,                      /* no load ordering group */
                     NULL,                      /* no tag identifier */
                     NULL,                      /* no dependencies */
                     NULL,                      /* LocalSystem account */
                     NULL);                     /* no password */
 
         if (schService == NULL) {
            unsigned long error_code;
            char error_message[512];

            printf("Unable to Create Service ...\r\n");

            error_code = mg_get_last_error(1);
            *error_message = '\0';
            mg_get_error_message(error_code, error_message, 511, 0);
            printf("%d : %s\r\nMachineName=%s; DatabaseName=%s;\r\nWinServiceName=%s; WinServicePathName=%s;\r\nWinServiceDesc=%s\r\n", error_code, error_message, lpMachineName, lpDatabaseName, mg_winservice_name, mg_winservice_path_name, mg_winservice_desc);
            return 1;
         }
         else {

            PCHANGESERVICECONFIG2 p_ChangeServiceConfig2;
            SERVICE_DESCRIPTION sdBuf;
            HMODULE h_module;

            printf("Service Created ...\r\n");

            sdBuf.lpDescription = (LPTSTR) mg_winservice_desc_ex;
            h_module = GetModuleHandle("Advapi32.dll");
            if (h_module) {
               p_ChangeServiceConfig2 = (PCHANGESERVICECONFIG2) GetProcAddress(h_module, "ChangeServiceConfig2A");

               if (p_ChangeServiceConfig2) {
                  if (!p_ChangeServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, &sdBuf)) {

                  }
               }
            }
         }
      }

      else {

         startup = 0;

         if (!QueryServiceStatus(schService, &ssStatus)) {
            printf("Cannot Determine the Service Status ...\r\n");
         }

         buf[1] = '\0';

         if (ssStatus.dwCurrentState == SERVICE_RUNNING) {

            printf("Service is Running.  [1] Stop, or [2] Stop and Remove from Service DataBase : ");
            buf[0] = (char) getch();
            printf(buf);
            printf("\r\n\r\n");

            if (buf[0] == '1') {
               if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus1))
                  printf("Service Stopped.\r\n");
               else
                  printf("Cannot Stop Service.\r\n");
            }
            else if (buf[0] == '2') {
               if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus1)) {
                  printf("Service Stopped.\r\n");
                  if (DeleteService(schService))
                     printf("Service Removed.\r\n");
                  else
                     printf("Cannot Remove Service.\r\n");
               }
               else
                  printf("Cannot Stop Service.\r\n");
            }
            else
               printf("\r\nNo Action Taken.\r\n");
         }

         else if (ssStatus.dwCurrentState == SERVICE_PAUSED) {

            printf("Service is Paused.  [1] Stop, or [2] Stop and Remove from Service DataBase,\r\n or [3] Continue : ");
            buf[0] = (char) getch();
            printf(buf);
            printf("\r\n\r\n");

            if (buf[0] == '1') {
               if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus1))
                  printf("Service Stopped.\r\n");
               else
                  printf("Cannot Stop Service.\r\n");
            }
            else if (buf[0] == '2') {
               if (ControlService(schService, SERVICE_CONTROL_STOP, &ssStatus1)) {
                  printf("Service Stopped.\r\n");
                  if (DeleteService(schService))
                     printf("Service Removed.\r\n");
                  else
                     printf("Cannot Remove Service.\r\n");
               }
               else
                  printf("Cannot Stop Service.\r\n");
            }
            else if (buf[0] == '3') {
               if (ControlService(schService, SERVICE_CONTROL_CONTINUE, &ssStatus1))
                  printf("Service Continued.\r\n");
               else
                  printf("Cannot Continue Service.\r\n");

            }
            else
               printf("\r\nNo Action Taken.\r\n");
         }

         else if (ssStatus.dwCurrentState == SERVICE_STOPPED) {

            printf("Service is Stopped.  [1] Start, or [2] Remove from Service DataBase : ");
            buf[0] = (char) getch();
            printf(buf);
            printf("\r\n\r\n");

            if (buf[0] == '1')
               startup = 1;
            else if (buf[0] == '2') {
               if (DeleteService(schService))
                  printf("Service Removed.\r\n");
           
               else
                  printf("Cannot Remove Service.\r\n");
            }
            else
               printf("\r\nNo Action Taken\r\n");
         }

         else
            printf("Service is in an Unrecognised State.\r\n");

         if (!startup) {
            CloseServiceHandle(schService);

            return 1;
         }
      }

      printf("Attempting to Start Service ...\r\n");

      if (!StartService(schService, 0, NULL)) {
         unsigned long error_code;
         char error_message[512];

         printf("Cannot Start Service.\r\n");

         error_code = mg_get_last_error(1);
         *error_message = '\0';
         mg_get_error_message(error_code, error_message, 511, 0);
         printf("%d : %s\r\nMachineName=%s; DatabaseName=%s;\r\nWinServiceName=%s; WinServicePathName=%s;\r\nWinServiceDesc=%s\r\n", error_code, error_message, lpMachineName, lpDatabaseName, mg_winservice_name, mg_winservice_path_name, mg_winservice_desc);

         CloseServiceHandle(schService);

         return 1;
      }
      else {
         printf("Waiting for Service to Respond ...\r\n");
      }
 
      /* Check the status until the service is no longer start pending. */
 
      if (!QueryServiceStatus(schService, &ssStatus)) {
         printf("Cannot Determine the Service Status ...\r\n");
      }
 
      /* Save the tick count and initial checkpoint. */

      dwStartTickCount = GetTickCount();
      dwOldCheckPoint = ssStatus.dwCheckPoint;

      while (ssStatus.dwCurrentState == SERVICE_START_PENDING) {

         /*
            Do not wait longer than the wait hint. A good interval is 
            one tenth the wait hint, but no less than 1 second and no 
            more than 10 seconds. 
         */

         dwWaitTime = ssStatus.dwWaitHint / 10;

         if (dwWaitTime < 1000)
            dwWaitTime = 1000;
         else if ( dwWaitTime > 10000)
            dwWaitTime = 10000;

         Sleep(dwWaitTime );

         /* Check the status again. */
 
         if (!QueryServiceStatus(schService, &ssStatus))
            break; 
 
         if (ssStatus.dwCheckPoint > dwOldCheckPoint) {

            /* The service is making progress. */

            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
         }
         else {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {

               /* No progress made within the wait hint */

               break;
            }
         }
      } 

      if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
         printf("Service Successfully Started.\r\n");
         dwStatus = NO_ERROR;
      }
      else {
         printf("\nService Failed to Start ...\r\n");
         printf("  Current State: %d\n", ssStatus.dwCurrentState);
         printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
         printf("  Service Specific Exit Code: %d\n", ssStatus.dwServiceSpecificExitCode);
         printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
         printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
         dwStatus = GetLastError();
      } 
 
      CloseServiceHandle(schService);

      return dwStatus;
   }

#endif

   mg_main_ctrl(&mg_main);

   return 1;
}


int mg_main_ctrl(LPMGMAIN p_mg_main)
{
   short ipv6_client, sock_err;
   int n, n1, tcp_port, attempt_no, errorno, cycle, reset_no, resets, no_children, child_index;
   unsigned long e_count;
   const int on = 1;
   const int off = 0;
   char ibuffer[256], obuffer[512], buffer[256];
   struct sockaddr_in cli_addr, serv_addr, *p_serv_addr;
   unsigned long activity;
   socklen_mg clilen;
   SOCKET sockfd, sockfd_ipv6, newsockfd;
   THRCTRL thread_control[MAX_THREADS];
   THRDATA thr_data[MAX_THREADS];
#if defined(MG_IPV6)
   const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
   /* struct sockaddr_storage cli_addr_ipv6; */
   struct sockaddr_in6 cli_addr_ipv6, serv_addr_ipv6, *p_serv_addr_ipv6;
   fd_set cset0, cset;
   struct addrinfo hints, *res;
   char port_str[32];
#endif
   FILE *fp;
#if !defined(_WIN32)
   pid_t pid, childpid;
   struct sigaction action;
#else
   int pid, childpid;
#endif

#ifdef _WIN32
__try {
#endif

   n = 0, n1 = 0;
   activity = 0;
   attempt_no = 0;
   resets = 0;
   sock_err = 0;
   no_children = 0;
   child_index = 0;
   *ibuffer = '\0';
   *obuffer = '\0';
   *buffer = '\0';

   core_data.ipv6 = 0;
   p_serv_addr = &serv_addr;
#if defined(MG_IPV6)
   core_data.ipv6 = 1;
   p_serv_addr_ipv6 = &serv_addr_ipv6;
#endif

   core_data.max_connections = MG_MAX_CONNECTIONS;
   core_data.max_servers = MG_MAX_SERVERS;
   core_data.process_count = 0;
   core_data.server_sockfd = -1;
   core_data.server_sockfd_ipv6 = -1;

   core_data.no_children = 0;
   core_data.process_count = 0;

   if (!p_mg_main->silent) {

#if _WIN32
      sprintf(obuffer, "M/Gateway Developments Ltd. - Service Integration Gateway: Version %s", MG_VERSION_DISPLAY);
      SetConsoleTitle(obuffer);
#endif

      printf("\nM/Gateway Developments Ltd. - Service Integration Gateway");
      sprintf(obuffer, "\n\n   Version %s (Created: %s)", MG_VERSION, MG_VERSION_CREATED);
      printf("%s", obuffer);

      printf(" (Multi-Threaded)\n");

   }

   if (!strlen(p_mg_main->tcp_port))
      T_STRCPY(p_mg_main->tcp_port, MG_TCP_PORT);

   if (!strlen(p_mg_main->tcp_port) || p_mg_main->help) {
      if (!p_mg_main->silent) {
#if defined(MG_IPV6)
/*
         printf("\nUsage: %s [-s] [-v] [-stop] [-pause] [-cont] [-m=s] [-c=n] [-ipv4] [-ipv6] [-tt] [-h or -?] [tcp_port_number]\n", p_mg_main->binary);
*/
         printf("\nUsage: %s [-s] [-v] [-stop] [-ipv4] [-ipv6] [-h or -?] [tcp_port_number]\n", p_mg_main->binary);

#else
/*
         printf("\nUsage: %s [-s] [-v] [-stop] [-pause] [-cont] [-m=s] [-c=n] [-tt] [-h or -?] [tcp_port_number]\n", p_mg_main->binary);
*/
         printf("\nUsage: %s [-s] [-v] [-stop] [-h or -?] [tcp_port_number]\n", p_mg_main->binary);
#endif
         printf("\n   [-s]         Silent mode: suppress start-up messages");
         printf("\n   [-stop]      Stop the Service Integration Gateway");
/*
         printf("\n   [-pause]     Pause the Service Integration Gateway");
         printf("\n   [-cont]      Continue the Service Integration Gateway");
         printf("\n                (after a previous 'pause' command)");
#if !defined(_WIN32)
         printf("\n   [-m=s]       Define the user(s) permitted to manage this service");
         printf("\n                where 's' is:");
         printf("\n                   'u' for the current user (the default),");
         printf("\n                   'g' for the current group,");
         printf("\n                   'o' for others,");
         printf("\n                   'a' for everyone (m=ugo),");
         printf("\n                Example: -m=ug (for current user and group)");
#endif
         printf("\n   [-c=n]       Run as a multithreaded/multiprocess hybrid where 'n' is");
         printf("\n                the number of child processes required");
*/
#if defined(MG_IPV6)
         printf("\n   [-ipv4]      Listen exclusively as an IPv4 service");
         printf("\n   [-ipv6]      Listen exclusively as an IPv6 service");
#endif
         printf("\n   [-v]         Run interactively as a single process");
         printf("\n                (Don't run as a Windows Service or UNIX Daemon Process)");
/*
         printf("\n   [-tt]        Thread test - Determine the maximum number of threads");
         printf("\n                that can be started per process");
*/
         printf("\n   [-h or -?]   Help\n");
      }
      else {
         mg_log_event("TCP Port not specified or help requested in 'silent' mode", "error");
      }
      exit(0);
   }

   tcp_port = (int) strtol(p_mg_main->tcp_port, NULL, 10);

   core_data.os_version = mg_get_os_version();

#if defined(MG_IPV6)
   core_data.ipv6 = 1;
   core_data.ipv6_srv = MG_IPVX_DUAL_STACK; /* Default */
#if defined(_WIN32)
   if (core_data.os_version < MG_WINVER_VISTA)
      core_data.ipv6_srv = MG_IPVX_TWO_SOCKETS;
   if (core_data.os_version < MG_WINVER_XP) {
      core_data.ipv6 = 0;
      core_data.ipv6_srv = MG_IPV4_ONLY;
   }
#endif
#else
   core_data.ipv6 = 0;
   core_data.ipv6_srv = MG_IPV4_ONLY;
#endif

   /* Configuration files */

   mg_config_files();

#if !defined(_WIN32)
   if (!mg_set_config("SYSTEM", "Ip_Address", MG_IP_ADDRESS, core_data.mg_sys_ini)) {

      if (!p_mg_main->silent) {
         printf("\n   ERROR: Unable to write configuration files");
         printf("\n      *** Service Integration Gateway not started ***");
      }
      exit(0);

   }
#endif

   if (p_mg_main->ipv4) {
      core_data.ipv6_srv = MG_IPV4_ONLY;
      core_data.ipv6 = 0;
   }
#if defined(MG_IPV6)
   else if (p_mg_main->ipv6) {
      core_data.ipv6_srv = MG_IPV6_ONLY;
   }
#endif

   if (!p_mg_main->silent) {
      printf("\n   Started and Listening on TCP Port %d\n", tcp_port);
      printf("\n      Any further information (or errors) will be recorded");
#if defined(MG_MGWSI)
      printf("\n      in the Service Integration Gateway's Event Log (mgwsi.log)\n");
#else
      printf("\n      in the Service Integration Gateway's Event Log (mgsi.log)\n");
#endif
#if !defined(_WIN32)
      sprintf(obuffer, "Configuration written in: %s", core_data.config_path);
      if (!p_mg_main->silent)
         printf("\n   %s\n", obuffer);
#endif

#ifdef _WIN32
      printf("\n   Type 'Control-C' to close the Service Integration Gateway down\n");
#else
      printf("\n   Use the following command to close the Service Integration Gateway down:\n");
#if defined(MG_MGWSI)
      printf("\n      ./mgwsi -stop");
#else
      printf("\n      ./mgsi -stop");
#endif
      printf("\n   or");
      printf("\n      kill -TERM `cat mgsi.pid`\n");
#endif
   }


#if !defined(_WIN32)

   if (p_mg_main->service == 0) {
      action.sa_flags = 0;
      action.sa_handler = mg_goingdown_signal;
      sigemptyset(&(action.sa_mask));
      sigaction(SIGINT, &action, NULL);

      core_data.no_children = 0;
   }

   else {

      if (fork() != 0) {
         exit(0);
      }

#if defined(AIX) || defined(AIX5)
      setsid();         /* become session leader */
      signal(SIGHUP, SIG_IGN);
      if (fork() != 0) {
         exit(0);       /* First child terminates */
      }
      umask(0);         /* clear our file mode creation mask */
#elif defined(OSF1)
      _Esetpgrp();
#elif defined(FREEBSD) || defined(MACOSX)
      setpgrp(0, 0);
#else
      setpgrp();
#endif
   }

   pid = getpid();
   childpid = pid;
   atexit(mg_goingdown);

   action.sa_flags = 0;
   action.sa_handler = mg_goingdown_signal;
   sigemptyset(&(action.sa_mask));
   sigaction(SIGTERM, &action, NULL);


#if defined(AIX) || defined(AIX5)
   action.sa_handler = SIG_IGN;
   sigaction(SIGHUP, &action, NULL);      /* 1 Hangup (POSIX).  */
#else
   action.sa_handler = mg_error_trap;
   sigaction(SIGHUP, &action, NULL);      /* 1 Hangup (POSIX).  */
#endif

#if 0
   action.sa_handler = mg_error_trap;
   sigaction(SIGINT, &action, NULL);      /* 2 Interrupt (ANSI).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGQUIT, &action, NULL);     /* 3 Quit (POSIX).  */
#endif
   action.sa_handler = mg_error_trap;
   sigaction(SIGILL, &action, NULL);      /* 4 Illegal instruction (ANSI).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGTRAP, &action, NULL);     /* 5 Trace trap (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGABRT, &action, NULL);     /* 6 Abort (ANSI).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGIOT, &action, NULL);      /* 6 IOT trap (4.2 BSD).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGBUS, &action, NULL);      /* 7 BUS error (4.2 BSD).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGFPE, &action, NULL);      /*	8 Floating-point exception (ANSI).  */
   action.sa_handler = mg_error_trap;
#if 0
   action.sa_handler = mg_error_trap;
   sigaction(SIGKILL, &action, NULL);     /* 9 Kill, unblockable (POSIX).  */
   action.sa_handler = SIG_IGN;
   sigaction(SIGUSR1, SIG_IGN);           /* 10 User-defined signal 1 (POSIX).  */
#endif
   action.sa_handler = mg_goingdown_signal;
   sigaction(SIGSEGV, &action, NULL);     /* 11 Segmentation violation (ANSI).  */
#if 0
   action.sa_handler = SIG_IGN;
   sigaction(SIGUSR2, SIG_IGN);           /* 12 User-defined signal 2 (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGALRM, &action, NULL);     /* 14 Alarm clock (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGTERM, &action, NULL);     /* 15 Termination (ANSI).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGSTKFLT, &action, NULL);   /* 16 ??? */
#endif

#if 0
#if !defined(FREEBSD) && !defined(MACOSX)
   action.sa_handler = SIG_DFL;
   sigaction(SIGCLD, SIG_DFL);            /* Same as SIGCHLD (System V).  */
#endif
   action.sa_handler = SIG_DFL;
   sigaction(SIGCHLD, SIG_DFL);           /* 17 Child status has changed (POSIX).  */
#endif
   action.sa_handler = mg_error_trap;
   sigaction(SIGCONT, &action, NULL);     /* 18 Continue (POSIX).  */
#if 0
   action.sa_handler = mg_error_trap;
   sigaction(SIGSTOP, &action, NULL);     /* 19 Stop, unblockable (POSIX).  */
#endif
   action.sa_handler = mg_error_trap;
   sigaction(SIGTSTP, &action, NULL);     /* 20 Keyboard stop (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGTTIN, &action, NULL);     /* 21 Background read from tty (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGTTOU, &action, NULL);     /* 22 Background write to tty (POSIX).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGURG, &action, NULL);      /* 23 Urgent condition on socket (4.2 BSD).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGXCPU, &action, NULL);     /* 24 CPU limit exceeded (4.2 BSD).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGXFSZ, &action, NULL);     /* 25 File size limit exceeded (4.2 BSD).  */
#if 0
   action.sa_handler = mg_error_trap;
   sigaction(SIGVTALRM, &action, NULL);   /* 26 Virtual alarm clock (4.2 BSD).  */
   action.sa_handler = mg_error_trap;
   sigaction(SIGPROF, &action, NULL);     /* 27 Profiling alarm clock (4.2 BSD).  */
#endif
   action.sa_handler = mg_error_trap;
   sigaction(SIGWINCH, &action, NULL);    /* 28 Window size change (4.3 BSD, Sun).  */

#if !defined(FREEBSD) && !defined(MACOSX)
   action.sa_handler = mg_error_trap;
   sigaction(SIGPOLL, &action, NULL);     /* SIGIO Pollable event occurred (System V).  */
#endif
   action.sa_handler = mg_error_trap;
   sigaction(SIGIO, &action, NULL);       /* 29 I/O now possible (4.2 BSD).  */

#if !defined(FREEBSD) && !defined(MACOSX)
   action.sa_handler = mg_error_trap;
   sigaction(SIGPWR, &action, NULL);      /* 30 Power failure restart (System V).  */
#endif

#else

   pid = _getpid();
   childpid = pid;
   atexit(mg_goingdown);
   signal(SIGINT, mg_goingdown_signal);

#endif

   /* Attempt to write a suitable ini file for our clients */

   mg_set_config("SYSTEM", "Ip_Address", MG_IP_ADDRESS, core_data.mg_sys_ini);
   mg_set_config("SYSTEM", "TCP_Port", p_mg_main->tcp_port, core_data.mg_sys_ini);

   core_data.sig_port = tcp_port;

   reset_no = 0;

#if defined(_WIN32)
   mg_sock.sock = 0;
   mg_sock.load_attempted = 0;
   mg_load_winsock(1);
   if (core_data.wsastartup != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "Service Integration Gateway: Startup Error");
      return 0;
   }
#endif

#if defined(MG_SSL_SERVER)
   if (core_data.no_children == 0 || child_index == 0) {
      mg_tls_proxy_start();
      mg_tls_server_start();
   }
#endif /* #if defined(MG_SSL_SERVER) */

startup_reset:

   sockfd_ipv6 = -1;
   sockfd = -1;
   sock_err = 0;


#if defined(MG_IPV6)

   /* First, load up address structs with getaddrinfo(): */

   if (core_data.ipv6_srv == MG_IPVX_DUAL_STACK || core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS || core_data.ipv6_srv == MG_IPV6_ONLY || core_data.ipv6_srv == MG_IPV4_OVER_IPV6) {

      T_SPRINTF(port_str, "%d", tcp_port);
      memset(&hints, 0, sizeof hints);
      if (core_data.ipv6_srv == MG_IPVX_DUAL_STACK)
         hints.ai_family = AF_UNSPEC;  /* Use IPv4 or IPv6 */
      else if (core_data.ipv6_srv == MG_IPV4_OVER_IPV6)
         hints.ai_family = AF_INET;  /* Use IPv4*/
      else
         hints.ai_family = AF_INET6;  /* Use IPv6 */
      hints.ai_protocol = IPPROTO_TCP;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_flags = AI_PASSIVE;     /* Fill in IP */
      n = MGNET_GETADDRINFO(NULL, port_str, &hints, &res);

      if (n != 0) {
         char message[256];

         errorno = (int) mg_get_last_error(0);
         mg_get_error_message(errorno, message, 250, 0);
         T_SPRINTF(obuffer, "Cannot load the TCP server data using the getaddrinfo() function: Error Code: %d (%s)", errorno, message);
         mg_log_event(obuffer, "Service Integration Gateway: Startup Error");

         mg_log_event("Attempting to use the IPv4 stack instead", "Service Integration Gateway: Startup Error");
         core_data.ipv6_srv = MG_IPV4_ONLY;
         sock_err = 0;
      }
      else {
         p_serv_addr_ipv6 = (struct sockaddr_in6 *) res->ai_addr;

         sockfd_ipv6 = MGNET_SOCKET(res->ai_family, res->ai_socktype, res->ai_protocol);
         if (SOCK_ERROR(sockfd_ipv6)) {
#if defined(_WIN32)
            if (core_data.os_version < MG_WINVER_VISTA) {
               mg_log_event("Cannot create an IPv6 service:  Attempting to use the IPv4 stack instead", "Service Integration Gateway: Startup Error");
               core_data.ipv6_srv = MG_IPV4_ONLY;
               MGNET_FREEADDRINFO(res);
               p_serv_addr_ipv6 = &serv_addr_ipv6;
            }
            else
               sock_err = 1;
#else
            sock_err = 1;
#endif
         }
      }
   }

   if ((core_data.ipv6_srv == MG_IPV4_ONLY || core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS) && !sock_err) {
      sockfd = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);
      if (SOCK_ERROR(sockfd))
         sock_err = 1;
   }

#else
   sockfd = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);
   if (SOCK_ERROR(sockfd))
      sock_err = 1;
#endif

   if (sock_err) {
      char message[256];

      errorno = (int) mg_get_last_error(0);
      mg_get_error_message(errorno, message, 250, 0);
      T_SPRINTF(obuffer, "Cannot open stream socket: Error Code: %d (%s)", errorno, message);
      mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
#ifdef _WIN32
      return 0;
#else  /* UNIX */
      exit(0);
#endif
   }

#if defined(MG_IPV6)
#if !defined(AIX) && !defined(AIX5) && !defined(SOLARIS)
   if (core_data.ipv6_srv == MG_IPVX_DUAL_STACK)
      n = MGNET_SETSOCKOPT(sockfd_ipv6, IPPROTO_IPV6, IPV6_V6ONLY, (void *) &off, sizeof(off));
#endif
#endif

   if (sockfd_ipv6 != -1) {
      core_data.server_sockfd_ipv6 = sockfd_ipv6;
      n = MGNET_SETSOCKOPT(sockfd_ipv6, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
   }

   if (sockfd != -1) {
      core_data.server_sockfd = sockfd;
      n = MGNET_SETSOCKOPT(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
   }

#if !defined(_WIN32)
startup_try_again:
#endif

   sock_err = 0;

#if defined(MG_IPV6)
   if (core_data.ipv6_srv == MG_IPVX_DUAL_STACK || core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS || core_data.ipv6_srv == MG_IPV6_ONLY || core_data.ipv6_srv == MG_IPV4_OVER_IPV6) {
      n = MGNET_BIND(sockfd_ipv6, res->ai_addr, (int) res->ai_addrlen);
      if (SOCK_ERROR(n)) {
#if !defined(_WIN32)
         if (errno == EINVAL) {
            mg_log_event("Unable to bind to the IPv6 stack: Attempting to use the IPv4 stack instead", "Service Integration Gateway: Startup Error");
            core_data.ipv6_srv = MG_IPV4_ONLY;
            goto startup_reset;
         }
         else {
            sock_err = 1;
         }
#else
         sock_err = 1;
#endif
      }
   }

   if ((core_data.ipv6_srv == MG_IPV4_ONLY || core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS) && !sock_err) {
      memset(&serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = MGNET_HTONL(INADDR_ANY);
      serv_addr.sin_port = MGNET_HTONS((unsigned short) tcp_port);

      n = MGNET_BIND(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
      if (SOCK_ERROR(n))
         sock_err = 1;
   }

#else
#if !defined(_WIN32)
   bzero((char *) &serv_addr, sizeof(serv_addr));
#endif

   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = MGNET_HTONL(INADDR_ANY);
   serv_addr.sin_port = MGNET_HTONS((unsigned short) tcp_port);

   n = MGNET_BIND(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
   if (SOCK_ERROR(n))
      sock_err = 1;
#endif


   if (sock_err) {
      char message[256];

#if !defined(_WIN32)
      if (attempt_no < 11) {
         attempt_no ++;
/*
mg_log_event("try again", "cmcmcm");
*/
         sleep(1);
         goto startup_try_again;
      }
#endif

      errorno = (int) mg_get_last_error(0);
      mg_get_error_message(errorno, message, 250, 0);

      T_SPRINTF(obuffer, "Cannot bind to local address on TCP port %d: Error Code %d (%s)", tcp_port, errorno, message);
      mg_log_event(obuffer, "Service Integration Gateway: Startup Error");

#ifdef _WIN32
     if (sockfd_ipv6 != -1)
         MGNET_CLOSESOCKET(sockfd_ipv6);
      if (sockfd != -1)
         MGNET_CLOSESOCKET(sockfd);
      return 0;
#else  /* UNIX */
      if (sockfd_ipv6 != -1)
         close(sockfd_ipv6);
      if (sockfd != -1)
         close(sockfd);
      exit(0);
#endif

   }
   else {
      char config[255];

      mg_startup_message(config, 255);
      T_SPRINTF(obuffer, "Service listening on TCP port %d:%s (%s)", tcp_port, core_data.ipv6_srv == MG_IPVX_DUAL_STACK ? "IPv4/IPv6 Dual Stack" : core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS ? "IPv4/IPv6 Dual Sockets" : core_data.ipv6_srv == MG_IPV6_ONLY ? "IPv6" : "IPv4", config);
      mg_log_event(obuffer, "Service Integration Gateway: Information");
   }

   if (sockfd_ipv6 != -1) {
      n = MGNET_LISTEN(sockfd_ipv6, SOMAXCONN);
      if (SOCK_ERROR(n))
         sock_err = 1;
   }

   if (sockfd != -1 && !sock_err) {
      n = MGNET_LISTEN(sockfd, SOMAXCONN);
      if (SOCK_ERROR(n))
         sock_err = 1;
   }

   if (sock_err) {
      char message[256];

      errorno = (int) mg_get_last_error(0);
      mg_get_error_message(errorno, message, 250, 0);

      T_SPRINTF(obuffer, "Listen Error: Error Code %d (%s)", errorno, message);
      mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
 
#ifdef _WIN32
      if (sockfd_ipv6 != -1)
         MGNET_CLOSESOCKET(sockfd_ipv6);
      if (sockfd != -1)
         MGNET_CLOSESOCKET(sockfd);
      return 0;
#else  /* UNIX */
      if (sockfd_ipv6 != -1)
         close(sockfd_ipv6);
      if (sockfd != -1)
         close(sockfd);
      exit(0);
#endif

   }

   /* Server up and running.  Now write process id in .pid file */

   fp = fopen(core_data.mg_pid,"w");
   if (fp) {
      int flags, ok;

      flags = 0;
      ok= 0;

      T_SPRINTF(obuffer, "%d", pid);

      fputs(obuffer, fp);
      fclose(fp);

#if !defined(_WIN32)

      ok = 0;
      flags = 0;

      flags = (S_IRUSR | S_IRGRP | S_IROTH);
      if (strstr(core_data.sig_manager, "u")) {
         flags |= (S_IWUSR);
         ok = 1;
      }
      if (strstr(core_data.sig_manager, "g")) {
         flags |= (S_IWGRP);
         ok = 1;
      }
      if (strstr(core_data.sig_manager, "o")) {
         flags |= (S_IWOTH);
         ok = 1;
      }
      if (strstr(core_data.sig_manager, "a")) {
         flags |= (S_IWUSR | S_IWGRP | S_IWOTH);
         ok = 1;
      }
      if (!ok) {
         flags = (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
      }

      n = chmod(core_data.mg_pid, flags);
      if (n == -1) {
         T_SPRINTF(obuffer, "Unable to modify permissions on the Process ID File: %s (errno=%d)", core_data.mg_pid, errno);
         mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
      }

      n = chmod(core_data.mg_ini, flags);
      if (n == -1) {
         T_SPRINTF(obuffer, "Unable to modify permissions on the Configuration File: %s (errno=%d)", core_data.mg_ini, errno);
         mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
      }
#endif
   }
   else {
      T_SPRINTF(obuffer, "Unable to Write the Process ID File: %s", core_data.mg_pid);
      mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
 
#ifdef _WIN32
      MGNET_CLOSESOCKET(sockfd);
/*
      MGNET_WSACLEANUP();
*/
      return 0;
#else  /* UNIX */
      close(sockfd);
      exit(0);
#endif

   }

   core_data.parent_pid = mg_current_process_id();
   core_data.parent_process = 1;

   core_data.server_port = tcp_port;

   /* TODO - implement multi-process infrastructure */
/*
   mg_get_config_files();
   mg_semaphore_destroy(NULL, core_data.sem_name_cx, 1);
   mg_shm_destroy(1);
   mg_shm_init();
   core_data.p_semlockCX = &(core_data.p_coreSHM->semlockCX);
   mg_semaphore_create(&(core_data.p_semlockCX), core_data.sem_name_cx, 1, MG_SEMTYPE_DEFAULT, 0);
*/

   core_data.parent_process = 0;

   if (p_mg_main->service == 0) { /* run interactively */
      core_data.no_children = 0;
   }

#if !defined(_WIN32)

   if (core_data.no_children == 0) { /* TODO - complete multi-process infrastructure */
      goto one_process;
   }

   for (n = 0; n < MG_MAX_CHILD; n ++) {
      sig_child[n] = 0;
   }

   no_children = core_data.no_children;

   resets = 0;

   T_SPRINTF(buffer, "Service Integration Gateway Initialization - Number of child processes requested: %d", core_data.no_children);
   mg_log_event(buffer, "Service Integration Gateway Controller");

startup_restart:

   core_data.control = 0;

   for (n = 0; n < no_children; n ++) {

      child_index = n;

      childpid = fork();

      /* error */

      if (childpid < 0) {

         T_SPRINTF(buffer, "Creation of a child process failed (fork function failed) - error=%d", errno);
         mg_log_event(buffer, "Service Integration Gateway Controller");

         break;
      }

      /* child */

      if (childpid == 0) {
         break;
      }

      /* parent */

      for (n1 = 0; n1 < MG_MAX_CHILD; n1 ++) {
         if (sig_child[n1] == 0) {
            sig_child[n1] = childpid;
            break;   
         }
      }

      T_SPRINTF(buffer, "Child process created - index=%d; pid=%d", n1, childpid);
      mg_log_event(buffer, "Service Integration Gateway Controller");

   }

   /* If not the child */

   if (childpid != 0) {

      int status, exstat, exsig, termsig, stopsig;
      char buffer[256];

      if (childpid < 0)
         exit(0);

      action.sa_flags = 0;
      sigemptyset(&(action.sa_mask));

      action.sa_handler = mg_parent_goingdown_signal;
      sigaction(SIGTERM, &action, NULL);
      action.sa_handler = mg_parent_goingdown_signal;
      sigaction(SIGINT, &action, NULL);

      action.sa_handler = SIG_IGN;
      sigaction(SIGHUP, &action, NULL);         /* 1 Hangup (POSIX).  */

      action.sa_handler = SIG_IGN;
      sigaction(SIGQUIT, &action, NULL);        /* 3 Quit (POSIX).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGILL, &action, NULL);         /* 4 Illegal instruction (ANSI).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGTRAP, &action, NULL);        /* 5 Trace trap (POSIX).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGABRT, &action, NULL);        /* 6 Abort (ANSI).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGIOT, &action, NULL);         /* 6 IOT trap (4.2 BSD).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGBUS, &action, NULL);         /* 7 BUS error (4.2 BSD).  */
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGFPE, &action, NULL);         /* 8 Floating-point exception (ANSI).  */

      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGKILL, &action, NULL);        /* 9 Kill, unblockable (POSIX).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGUSR1, &action, NULL);        /* 10 User-defined signal 1 (POSIX).  */

      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGSEGV, &action, NULL);        /* 11 Segmentation violation (ANSI).  */

      action.sa_handler = SIG_IGN;
      sigaction(SIGUSR2, &action, NULL);        /* 12 User-defined signal 2 (POSIX).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGALRM, &action, NULL);        /* 14 Alarm clock (POSIX).  */

#if defined(SIGSTKFLT)
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGSTKFLT, &action, NULL);      /* 16 ??? */
#endif

#if !defined(FREEBSD) && !defined(MACOSX)
      action.sa_handler = SIG_DFL;
      sigaction(SIGCLD, &action, NULL);         /* Same as SIGCHLD (System V).  */
#endif
#if defined(SIGCHLD)
      action.sa_handler = SIG_DFL;
      sigaction(SIGCHLD, &action, NULL);
#endif

      action.sa_handler = SIG_IGN;
      sigaction(SIGCONT, &action, NULL);        /* 18 Continue (POSIX).  */

      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGSTOP, &action, NULL);        /* 19 Stop, unblockable (POSIX).  */
#if 0
      action.sa_handler = mg_parent_error_trap;
      sigaction(SIGTSTP, &action, NULL);        /* 20 Keyboard stop (POSIX).  */
#endif
      action.sa_handler = SIG_IGN;
      sigaction(SIGTTIN, &action, NULL);        /* 21 Background read from tty (POSIX).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGTTOU, &action, NULL);        /* 22 Background write to tty (POSIX).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGURG, &action, NULL);         /* 23 Urgent condition on socket (4.2 BSD).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGXCPU, &action, NULL);        /* 24 CPU limit exceeded (4.2 BSD).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGXFSZ, &action, NULL);        /* 25 File size limit exceeded (4.2 BSD).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGVTALRM, &action, NULL);      /* 26 Virtual alarm clock (4.2 BSD).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGPROF, &action, NULL);        /* 27 Profiling alarm clock (4.2 BSD).  */
      action.sa_handler = SIG_IGN;
      sigaction(SIGWINCH, &action, NULL);       /* 28 Window size change (4.3 BSD, Sun).  */

#if !defined(FREEBSD) && !defined(MACOSX)
      action.sa_handler = SIG_IGN;
      sigaction(SIGPOLL, &action, NULL);        /* SIGIO Pollable event occurred (System V).  */
#endif
      action.sa_handler = SIG_IGN;
      sigaction(SIGIO, &action, NULL);          /* 29 I/O now possible (4.2 BSD).  */

#if !defined(FREEBSD) && !defined(MACOSX)
      action.sa_handler = SIG_IGN;
      sigaction(SIGPWR, &action, NULL);         /* 30 Power failure restart (System V).  */
#endif

      core_data.control = 1;

      for (;;) {

         status = 0, exstat = 0, exsig = 0, termsig = 0, stopsig = 0;

         childpid = wait(&status);

         if (childpid > 0 ) {

            for (n = 0; n < MG_MAX_CHILD; n ++) {
               if (sig_child[n] == childpid) {
                  sig_child[n] = 0;
                  break;   
               }
            }

            if (WIFEXITED(status)) {
               exstat = WEXITSTATUS(status);
            }
            else if (WIFSIGNALED(status)) {
               termsig = WTERMSIG(status);
               exsig = termsig;
            }
            else if (WIFSTOPPED(status)) {
               stopsig = WSTOPSIG(status);
               exsig = stopsig;
            }

            T_SPRINTF(buffer, "Child process (%d) exit status: %d; Termination Signal: %d; Stop Signal: %d", childpid, exstat, exsig, stopsig);
            mg_log_event(buffer, "Service Integration Gateway Controller");

            if (exstat == 7 || exsig == SIGABRT || exsig == SIGSEGV) {
               resets ++;
               break;
            }
            else {
/*
               mg_log_evant("Child closing Down", "Service Integration Gateway Controller");
*/
               continue;
            }

         }
         else {
/*
            mg_log_event("Closing Down - all child processes are closed", "Service Integration Gateway Controller");
*/
            break;

         }
      }

      if (resets > 0) {
         mg_log_event("Restart a child process after a fatal error condition", "Service Integration Gateway Controller");

         no_children = 1;
         resets = 0;

         goto startup_restart;
      }

      mg_log_event("Closing Down - Exit", "Service Integration Gateway Controller");

      close(sockfd);
/*
      mg_semaphore_destroy(core_data.p_semlockCX, core_data.sem_name_cx, 1);
      mg_shm_destroy(2);
*/
      exit(0);

   }

   /* Child process */

   core_data.control = 0;

#if !defined(_WIN32)
   action.sa_flags = 0;
   sigemptyset(&(action.sa_mask));

   action.sa_handler = mg_goingdown_signal;
   sigaction(SIGTERM, &action, NULL);

   action.sa_handler = mg_goingdown_signal;
   sigaction(SIGINT, &action, NULL);
#else
   signal(SIGTERM, mg_goingdown_signal);
   signal(SIGINT, mg_goingdown_signal);
#endif

#if defined(OSF1)
   _Esetpgrp();
#elif defined(FREEBSD)
   setpgrp(0, 0);
#elif defined(MACOSX) && __APPLE_CC__ <= 5300
   setpgrp(0, 0);
#else
   setpgrp();
#endif

   pid = getpid();
   childpid = pid;

one_process:

#endif /* #if !defined(_WIN32) */

   mg_startup(); /* SIG child process */
   mg_initialize();

#if defined(MG_SSL_SERVER)
   if (core_data.no_children == 0 || child_index == 0) {
      mg_tls_proxy_start();
      mg_tls_server_start();
   }
#endif /* #if defined(MG_SSL_SERVER) */

   cycle = 0;
   e_count = 0;

#if defined(MG_IPV6)
   FD_ZERO(&cset0);
   if (core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS) {
      FD_SET(sockfd_ipv6, &cset0);
      FD_SET(sockfd, &cset0);
   }
   cset = cset0;
#endif


   for (;;) {
#if defined(MG_IPV6)
      if (core_data.ipv6_srv == MG_IPVX_TWO_SOCKETS) {

         /* Wait until there is a client connection */
         /* for either the IPv4 or IPv6 listening socket. */ 
         cset = cset0;
         if (MGNET_SELECT(FD_SETSIZE, &cset, NULL, NULL, NULL) < 0) {
            errorno = (int) mg_get_last_error(0);

            e_count ++;

            if (e_count == 1) {
               char message[256];
               mg_get_error_message(errorno, message, 250, 0);
               T_SPRINTF(obuffer, "Select Error (Connection acceptance loop in twin IPv4/IPv6 socket mode): Error Code %d (%s)", errorno, message);
               mg_log_event(obuffer, "Service Integration Gateway: Client-side Connection Error");
            }
            if (e_count > 1000)
               break;
            continue;
         }

         if (MGNET_FD_ISSET(sockfd_ipv6, &cset)) {
#if !defined(_WIN32)
            bzero((char *) &cli_addr_ipv6, sizeof(cli_addr_ipv6));
#endif
            ipv6_client = 1;
            clilen = sizeof(cli_addr_ipv6);
            newsockfd = MGNET_ACCEPT(sockfd_ipv6, (struct sockaddr *) &cli_addr_ipv6, &clilen);
         }
         else if (MGNET_FD_ISSET(sockfd, &cset)) {
#if !defined(_WIN32)
            bzero((char *) &cli_addr, sizeof(cli_addr));
#endif
            ipv6_client = 0;
            clilen = sizeof(cli_addr);
            newsockfd = MGNET_ACCEPT(sockfd, (struct sockaddr *) &cli_addr, &clilen);
         }
         else
            continue;
      }
      else if (core_data.ipv6_srv == MG_IPV4_ONLY) {
#if !defined(_WIN32)
         bzero((char *) &cli_addr, sizeof(cli_addr));
#endif
         ipv6_client = 0;
         clilen = sizeof(cli_addr);
         newsockfd = MGNET_ACCEPT(sockfd, (struct sockaddr *) &cli_addr, &clilen);
      }
      else {
#if !defined(_WIN32)
         bzero((char *) &cli_addr_ipv6, sizeof(cli_addr_ipv6));
#endif
         ipv6_client = 1;
         clilen = sizeof(cli_addr_ipv6);
         newsockfd = MGNET_ACCEPT(sockfd_ipv6, (struct sockaddr *) &cli_addr_ipv6, &clilen);
      }
#else
#if !defined(_WIN32)
      bzero((char *) &cli_addr, sizeof(cli_addr));
#endif
      ipv6_client = 0;
      clilen = sizeof(cli_addr);
      newsockfd = MGNET_ACCEPT(sockfd, (struct sockaddr *) &cli_addr, &clilen);
#endif


/*
MGNET_SHUTDOWN(newsockfd, 2);
MGNET_CLOSESOCKET(newsockfd);
continue;
*/

      if (SOCK_ERROR(newsockfd)) {

         errorno = (int) mg_get_last_error(0);

         e_count ++;

#ifdef _WIN32
         if (errorno == WSAEINTR) {
            reset_no = 999;
            break;
         }
#endif

         if (e_count == 1) {
            char message[256];
            mg_get_error_message(errorno, message, 250, 0);
            T_SPRINTF(obuffer, "Accept Error: Error Code %d (%s)", errorno, message);
            mg_log_event(obuffer, "Service Integration Gateway: Client-side Connection Error");
         }

         if (errorno == 104) {
            if (e_count > 100000)
               break;
         }
         else {
            if (e_count > 1000)
               break;
         }
         continue;
      }

      reset_no = 0;
      e_count = 0;

      if (cycle >= MAX_THREADS)
         cycle = 0;


      if (thr_data[cycle].queued) {
#ifdef _WIN32
         MGNET_CLOSESOCKET(thr_data[cycle].newsockfd);
#else  /* UNIX */
         close(thr_data[cycle].newsockfd);
#endif

         mg_log_event("Thread dropped from queue", "Service Integration Gateway: Error");
      }

      thr_data[cycle].tcp_port = tcp_port;

      if (ipv6_client) {
#if defined(MG_IPV6)
#if defined(_WIN32)
         DWORD len;
         char ip[256];
         char *p1, *p2;

         len = 250;
         *ip = '\0';
         n = MGNET_WSAADDRESSTOSTRING((LPSOCKADDR) p_serv_addr_ipv6, sizeof(serv_addr_ipv6), NULL, ip, &len);
         p1 = strstr(ip, "[");
         if (p1)
            p1 ++;
         else
            p1 = ip;
         p2 = strstr(p1, "]");
         if (p2)
            *p2 = '\0';
         T_STRCPY(thr_data[cycle].srv_addr, p1);
         len = 250;
         *ip = '\0';
         n = MGNET_WSAADDRESSTOSTRING((LPSOCKADDR) &cli_addr_ipv6, clilen, NULL, ip, &len);
         p1 = strstr(ip, "[");
         if (p1)
            p1 ++;
         else
            p1 = ip;
         p2 = strstr(p1, "]");
         if (p2)
            *p2 = '\0';
         T_STRCPY(thr_data[cycle].cli_addr, p1);
         thr_data[cycle].cli_port = cli_addr_ipv6.sin6_port;
#else
         unsigned long len;
         char ip[256];

         len = 250;
         *ip = '\0';
         MGNET_INET_NTOP(AF_INET6, p_serv_addr_ipv6, ip, len);
         T_STRCPY(thr_data[cycle].srv_addr, ip);
         len = 250;
         *ip = '\0';
         MGNET_INET_NTOP(AF_INET6, &cli_addr_ipv6, ip, len);
         T_STRCPY(thr_data[cycle].cli_addr, ip);
         thr_data[cycle].cli_port = cli_addr_ipv6.sin6_port;
#endif
#endif
      }
      else {
      if (MGNET_INET_NTOA(serv_addr.sin_addr))
         T_STRCPY(thr_data[cycle].srv_addr, MGNET_INET_NTOA(serv_addr.sin_addr));
      else
         T_STRCPY(thr_data[cycle].srv_addr, "");

      if (MGNET_INET_NTOA(cli_addr.sin_addr))
         T_STRCPY(thr_data[cycle].cli_addr, MGNET_INET_NTOA(cli_addr.sin_addr));
      else
         T_STRCPY(thr_data[cycle].cli_addr, "");

      thr_data[cycle].cli_port = cli_addr.sin_port;
      }

      thr_data[cycle].sockfd = sockfd;
      thr_data[cycle].newsockfd = newsockfd;

      //thr_data[cycle].ipc = 0;
      thr_data[cycle].use_ssl = 0;
      //thr_data[cycle].ipc_type = MG_IPC_TYPE_NONE;

      activity ++;
      thr_data[cycle].activity = activity;
      thr_data[cycle].queued = 0;

      core_data.sig_request_no ++;
      thr_data[cycle].sig_request_no = core_data.sig_request_no;

      /* Concurrent Server */

      n = mg_thread_create((LPTHRCTRL) &thread_control[cycle], (MG_THR_START_ROUTINE) mg_server_child, (void *) &thr_data[cycle]);

      if (n < 0) {
         sprintf(obuffer, "Thread creation error %d", errno);
         mg_log_event(obuffer, "Service Integration Gateway: Startup Error");
      }

      cycle ++;

      continue;

   }

#ifdef _WIN32
   MGNET_CLOSESOCKET(newsockfd);
   MGNET_CLOSESOCKET(sockfd);
/*
   MGNET_WSACLEANUP();
*/
#else
   close(newsockfd);
   close(sockfd);
#endif

#if defined(MG_IPV6)
   MGNET_FREEADDRINFO(res);
#endif


#ifdef _WIN32
   if (errorno == WSAENOTSOCK) {
      return 0;
   }
#endif

   reset_no ++;
   if (reset_no < 10) {

      if (reset_no > 1)
         mg_pause(reset_no * 1000);

      T_SPRINTF(obuffer, "Accept error: %d; Errors received: %ld; SIG Reset ", errorno, e_count);
      mg_log_event(obuffer, "Service Integration Gateway: Web-Server-Side Connection Error");

      goto startup_reset;
   }

   mg_log_event("Connection Closed", "Service Integration Gateway: Information");

#ifdef _WIN32
   return 0;
#else
   exit(0);
#endif

   return 1;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_main_ctrl: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


MG_THR_TYPE mg_server_child(void *arg)
{
   short phase;
   int result, n, len, total;
   SOCKET sockfd, newsockfd;
   char header[256], buffer[16];
   char *iBuffer;
   MGREQ ecb;
   LPMGREQ p_request;
   LPNVLIST p_cgi;

#if !defined(_WIN32)
   LPTSDATA p_tsdata;
#endif

   phase = 0;
   result = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   if (arg != NULL) {
      if (((LPTHRDATA) arg)->thread_test) {

         if (((LPTHRDATA) arg)->thread_test == 1)
            mg_pause(30000);
         else {
            mg_mutex_lock(core_data.p_memlockTH);
            mg_mutex_unlock(core_data.p_memlockTH);
         }

         mg_thread_exit();
         return MG_THR_RETURN;
      }
   }

   result = 0;
   p_request = &ecb;
   sockfd = ((LPTHRDATA) arg)->sockfd;
   newsockfd = ((LPTHRDATA) arg)->newsockfd;
   p_request->tcp_port = ((LPTHRDATA) arg)->tcp_port;
   strcpy(p_request->cli_addr, ((LPTHRDATA) arg)->cli_addr);
   strcpy(p_request->srv_addr, ((LPTHRDATA) arg)->srv_addr);
   p_request->cli_sockfd = newsockfd;

   p_request->use_ssl = 0;

#ifdef MG_SSL
   p_request->ctx = ((LPTHRDATA) arg)->ctx;
#endif /* #ifdef MG_SSL */

#if !defined(_WIN32)

   pthread_once(&tsdata_once, mg_once);

   if ((p_tsdata = pthread_getspecific(tsdata_key)) == NULL) {
      p_tsdata = (LPTSDATA) malloc(sizeof(TSDATA));
      if (!p_tsdata) {
         mg_thread_exit();
         return MG_THR_RETURN;
      }
      pthread_setspecific(tsdata_key, p_tsdata);
      p_tsdata->sockfd = p_request->cli_sockfd;
      p_tsdata->socket_closed = 0;
    
   }
   
   signal(SIGPIPE, SIG_IGN); /* 13 Broken pipe (POSIX).  */

#if defined(AIX) || defined(AIX5)
   signal(SIGHUP, SIG_IGN);      /* 1 Hangup (POSIX).  */
#else
   signal(SIGHUP, mg_error_trap); /* 1 Hangup (POSIX).  */
#endif
   signal(SIGILL, mg_error_trap); /* 4 Illegal instruction (ANSI).  */
   signal(SIGTRAP, mg_error_trap); /* 5 Trace trap (POSIX).  */
   signal(SIGABRT, mg_error_trap); /* 6 Abort (ANSI).  */
   signal(SIGIOT, mg_error_trap); /* 6 IOT trap (4.2 BSD).  */
   signal(SIGBUS, mg_error_trap); /* 7 BUS error (4.2 BSD).  */
   signal(SIGFPE, mg_error_trap); /*	8 Floating-point exception (ANSI).  */
   signal(SIGSEGV, mg_error_trap); /* 11 Segmentation violation (ANSI).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGCLD, mg_error_trap); /* Same as SIGCHLD (System V).  */
#endif
   signal(SIGCHLD, mg_error_trap); /* 17 Child status has changed (POSIX).  */
   signal(SIGCONT, mg_error_trap); /* 18 Continue (POSIX).  */
   signal(SIGTSTP, mg_error_trap); /* 20 Keyboard stop (POSIX).  */
   signal(SIGTTIN, mg_error_trap); /* 21 Background read from tty (POSIX).  */
   signal(SIGTTOU, mg_error_trap); /* 22 Background write to tty (POSIX).  */
   signal(SIGURG, mg_error_trap); /* 23 Urgent condition on socket (4.2 BSD).  */
   signal(SIGXCPU, mg_error_trap); /* 24 CPU limit exceeded (4.2 BSD).  */
   signal(SIGXFSZ, mg_error_trap); /* 25 File size limit exceeded (4.2 BSD).  */
   signal(SIGWINCH, mg_error_trap); /* 28 Window size change (4.3 BSD, Sun).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPOLL, mg_error_trap); /* SIGIO Pollable event occurred (System V).  */
#endif
   signal(SIGIO, mg_error_trap); /* 29 I/O now possible (4.2 BSD).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPWR, mg_error_trap); /* 30 Power failure restart (System V).  */
#endif

#endif

   p_request->request_source = MG_RS_HTTP;

#ifdef MG_SSL
   if (p_request->use_ssl) {
      result = mg_tls_server_session(p_request);
      if (!result)
        goto mg_server_childExit2;
   }
#endif /* #ifdef MG_SSL */

mg_next_request:

   len = 0;
   for (;;) {
      n = MGNET_RECV(newsockfd, header + len, 10 - len, 0);
      if (n < 1)
         break;
      len += n;
      if (len == 10)
         break;
   }

   if (len != 10) {
      if (len > 0) {
         char buffer[256];
         sprintf(buffer, "Read Error: Cannot read first 10 Bytes of request (%d Bytes Read)", len);
         mg_log_buffer((unsigned char *) header, len, buffer);
      }
      goto mg_server_childExit3;
   }

   header[10] = '\0';

   strncpy(buffer, header, 8);
   buffer[8] = '\0';

   mg_ucase(buffer);

   if (!strncmp(buffer, "^A^", 3) || !strncmp(buffer, "^S^", 3)) { /* Are you there */
      int n, total;
      unsigned char ssize[32];

      for (;;) {
         n = MGNET_RECV(newsockfd, header + len, 1, 0);
         if (n < 1)
            break;
         len += n;
         if (header[len - 1] == '\n')
            break;
      }
      header[len] = '\0';
      strcpy(buffer, "GS0000000000");
      strcpy(header, "00000sc\n");
      n = mg_encode_size(ssize, 12, MG_CHUNK_SIZE_BASE);
      strncpy((char *) (header + (5 - n)), ssize, n);
      strcpy(header + MG_RECV_HEAD, buffer);
      total = MG_RECV_HEAD + 12;

      n = MGNET_SEND(newsockfd, header, total, 0);

      goto mg_next_request;

   }

   if (!strncmp(buffer, "PHP", 3)) {

      strcpy(p_request->request_heading, header);

      p_request->request_source = MG_RS_PHP;
      mg_php(p_request, 0);

      goto mg_server_childExit3;

   }

   if (!strncmp(buffer, "WSMQ", 4)) {

      strcpy(p_request->request_heading, header);

      p_request->request_source = MG_RS_MQ;
      mg_wsmq(p_request);

      goto mg_server_childExit3;

   }

   if (!strncmp(buffer, "GET", 3) || !strncmp(buffer, "POST", 4)) {

      if (core_data.http_server) {
         strcpy(p_request->request_heading, header);

         p_request->request_source = MG_RS_HTTP;

         mg_httpd(p_request);
      }

      goto mg_server_childExit3;
   }


   core_data.sm_activity ++;

   len = (int) strtol(header, NULL, 10);
   if (!len)
      goto mg_server_childExit3;

   iBuffer = (char *) mg_malloc((len + 32) * sizeof(char), "mg_server_child:1");

   if (!iBuffer) {
      mg_log_event("No Memory", "ERROR");

      goto mg_server_childExit3;

   }

   /* Read the data from the (CGI) client */

   total = 0;
   for (;;) {
      n = MGNET_RECV(newsockfd, iBuffer + total, len - total, 0);
      if (n < 0)
         break;
      total += n;
      if (total == len) {
         iBuffer[len] = '\0';
         break;
      }
   }

   p_request->iBuffer = iBuffer;
   p_request->iBuffer_size = len;
   p_request->p_cgi_list = NULL;

   p_request->iBuffer[len] = '\0';

   mg_http(p_request);

   mg_free((void *) iBuffer, "mg_server_child:1");
   iBuffer = NULL;

   while (p_request->p_cgi_list) {
      p_cgi = p_request->p_cgi_list;
      p_request->p_cgi_list = p_request->p_cgi_list->p_next;
      mg_free((void *) p_cgi, "mg_server_child:2");

   }

   phase = 5;

#ifdef MG_SSL
mg_server_childExit2:
   if (p_request->use_ssl) {
      if (p_request->ssl)
         mg_SSL_free (p_request->ssl);
   }
#endif /* #ifdef MG_SSL */

#if !defined(_WIN32)
   p_tsdata->socket_closed = 1;
#endif

mg_server_childExit3:

#ifdef _WIN32
#ifdef SD_BOTH
   MGNET_SHUTDOWN(newsockfd, SD_BOTH);
#else
   MGNET_SHUTDOWN(newsockfd, 2);
#endif
#else
#ifdef SHUT_RDWR
   shutdown(newsockfd, SHUT_RDWR);
#else
   shutdown(newsockfd, 2);
#endif
#endif

#if defined(_WIN32)
   MGNET_CLOSESOCKET(newsockfd);
#else
   close(newsockfd);
#endif

   mg_thread_exit();
   return MG_THR_RETURN;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_server_child: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return MG_THR_RETURN;
}
#endif

}


int mg_config_files(void)
{
   int n;
   char ibuffer[256];

   core_data.os_version = mg_get_os_version();

 #if defined(_WIN32)

   GetModuleFileName(NULL, ibuffer, 128);

   for (n = (int) strlen(ibuffer) - 1; n >= 0; n --) {
      if (ibuffer[n] == '/' || ibuffer[n] == '\\') {
         ibuffer[n + 1] = '\0';
         break;
      }
   }

   strcpy(core_data.config_path, ibuffer);

   strcpy(core_data.mg_ini, ibuffer);
   strcat(core_data.mg_ini, MG_INI_FILE);

   strcpy(core_data.mg_sec, ibuffer);
   strcat(core_data.mg_sec, MG_SEC_FILE);

   strcpy(core_data.mg_log, ibuffer);
   T_STRCAT(core_data.mg_log, MG_LOG_FILE);

   strcpy(core_data.mg_sys_ini, ibuffer);
   T_STRCAT(core_data.mg_sys_ini, MG_SYS_INI_FILE);

   strcpy(core_data.mg_pid, ibuffer);
   T_STRCAT(core_data.mg_pid, MG_PID_FILE);

   strcpy(core_data.root_dir, ibuffer);
   strcpy(ibuffer, core_data.mg_ini);


#else

   T_STRCPY(core_data.config_path, MG_PATH_1);
   T_SPRINTF(core_data.mg_ini, "./%s", MG_INI_FILE);
   T_SPRINTF(core_data.mg_sec, "./%s", MG_SEC_FILE);
   T_SPRINTF(core_data.mg_log, "./%s", MG_LOG_FILE);
   T_SPRINTF(core_data.mg_sys_ini, "./%s", MG_SYS_INI_FILE);
   T_SPRINTF(core_data.mg_pid, "./%s", MG_PID_FILE);

#endif

   return 1;
}

void mg_startup(void)
{
   int n = 0, i = 0, j = 0, diagnostics;
   THRCTRL thread_control;

#ifdef _WIN32
__try {
#endif

   core_data.os_version = mg_get_os_version();

   thread_control.stack_size = 0;

   T_STRCPY(core_data.mg_title, MG_TITLE);

   mg_mutex_create((MUTOBJ **) &(core_data.p_memlockPA), NULL);
   mg_mutex_create((MUTOBJ **) &(core_data.p_memlockSA), NULL);

#ifdef _WIN32
   mg_mutex_create((MUTOBJ **) &(core_data.p_memlockTH), NULL);
#else
   mg_mutex_create((MUTOBJ **) &(core_data.p_memlockFA), NULL);
   mg_mutex_create((MUTOBJ **) &(core_data.p_memlockLG), NULL);
#endif


   mg_mutex_create((MUTOBJ **) &(core_data.p_sem_gethost), NULL);

   diagnostics = 0;
   core_data.diagnostics = 0;
   core_data.session_no = 0;

   core_data.ws_loaded = 0;

   if (diagnostics == 1)
      core_data.diagnostics = 1;

   T_STRCPY(core_data.mg_version, MG_VERSION);


   for (i = 0; i < MG_MAX_SERVERS; i ++)
      mg_servers[i] = NULL;
   for (i = 0; i < MG_MAX_SERVERS; i ++)
      mg_alt_servers[i] = NULL;
   for (i = 0; i < MG_MAX_CONNECTIONS; i ++) {
      mg_connections[i] = NULL;
#ifdef _WIN32
      core_data.thread_handles[i] = NULL;
#endif
   }

   for (i = 0; i < MG_MAX_MQ_CONNECTIONS; i ++)
      mg_mqctable[i] = NULL;

   for (i = 0; i < MG_MAX_CONNECTIONS; i ++)
      mg_mqstable[i] = NULL;

#ifdef _WIN32
   core_data.daemon_thread.hThread = NULL;
#endif
   core_data.daemon_thread.thread_id = 0;

   mg_upgrade();

   mg_global_parameters();

   mg_site_parameters();

   n = mg_thread_create((LPTHRCTRL) &thread_control, (MG_THR_START_ROUTINE) mg_daemon, (void *) NULL);
#if defined(_WIN32)
   core_data.daemon_thread.hThread = thread_control.hThread;
   core_data.daemon_thread.thread_id = thread_control.thread_id;
#endif

   return;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_startup: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return;
}
#endif

}


void mg_closedown(void)
{
   int n = 0, sn;


#ifdef _WIN32
__try {
#endif

   n = mg_close_connections("*", 2);

   mg_uninitialize();

#ifdef _WIN32
   if (core_data.daemon_thread.hThread) {
      TerminateThread(core_data.daemon_thread.hThread, 0);
   }
#endif

   for (n = 0; n < MG_MAX_SERVERS; n ++) {
      if (mg_alt_servers[n] == NULL)
         break;

      for (sn = 0; sn < MG_MAX_ALT; sn ++) {
         if (mg_alt_servers[n]->alt[sn]) {
            mg_free((void *) mg_alt_servers[n]->alt[sn], "mg_closedown:1");
            mg_alt_servers[n]->alt[sn] = NULL;
         }
      }

      mg_free((void *) mg_alt_servers[n], "mg_closedown:2");

      mg_alt_servers[n] = NULL; /* Safety */
   }

   for (n = 0; n < MG_MAX_SERVERS; n ++) {
      if (mg_servers[n] == NULL)
         break;

      mg_free((void *) mg_servers[n], "mg_closedown:3");

      mg_servers[n] = NULL; /* Safety */
   }


   mg_mutex_destroy((MUTOBJ *) core_data.p_sem_gethost);
   core_data.p_sem_gethost = NULL;

   mg_mutex_destroy((MUTOBJ *) core_data.p_memlockPA);
   core_data.p_memlockPA = NULL;

   mg_mutex_destroy((MUTOBJ *) core_data.p_memlockSA);
   core_data.p_memlockSA = NULL;

#ifdef _WIN32
   mg_mutex_destroy((MUTOBJ *) core_data.p_memlockTH);
   core_data.p_memlockTH = NULL;
#else
   mg_mutex_destroy((MUTOBJ *) core_data.p_memlockFA);
   core_data.p_memlockFA = NULL;

   mg_mutex_destroy((MUTOBJ *) core_data.p_memlockLG);
   core_data.p_memlockLG = NULL;
#endif

   return;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_closedown: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return;
}
#endif

}


void mg_startup_message(char *message, int message_size)
{
   T_SPRINTF(message, "Maximum Connections=%d; Maximum Servers=%d; Nagle_Algorithm=%s;",
               core_data.max_connections,
               core_data.max_servers,
               core_data.nagle_algorithm == 1 ? "Enabled" : "Disabled");
   return;
}


int mg_initialize(void)
{
   int n;

#ifdef _WIN32
__try {
#endif

#ifdef MG_SSL
   mg_ssl.ssl = 0;
   mg_ssl.load_attempted = 0;
#endif

   n = mg_load_bdb(0);
   n = mg_load_mq(0);

   n = mg_load_openssl(0);
   n = mg_load_crypt32(0);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_initialize: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif
}


int mg_uninitialize(void)
{
   short phase;

   phase = 0;

#ifdef _WIN32
__try {
#endif

#ifdef MG_MQ_USE_DSO

   if (mg_mqlib.mqlib) {
      mg_dso_unload((MGDSO *) &(mg_mqlib.mgdso));
   }

#endif

#ifdef MG_SSL
#ifdef MG_SSL_USE_DSO
   phase = 1;
   if (mg_ssl.ssl) {
      mg_dso_unload((MGDSO *) &(mg_ssl.mgdso_libeay));
      mg_dso_unload((MGDSO *) &(mg_ssl.mgdso));
   }
   phase = 2;
#endif
#endif

#ifdef MG_CRYPT32
#ifdef MG_CRYPT32_USE_DSO
   phase = 1;
   if (mg_crypt32.crypt32) {
      mg_dso_unload((MGDSO *) &(mg_crypt32.mgdso));
   }
   phase = 2;
#endif
#endif

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_uninitialize: %x:%u", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


MG_THR_TYPE mg_daemon(void *p_parameters)
{
   int result, n, cycle, chndle, shndle;
   unsigned long exit_code, count;
   char obuffer[256];
   time_t time_now;

   result = 1;

#ifdef _WIN32
__try {
#endif

   count = 0;
   cycle = 0;
   chndle = -1;
   shndle = -1;

   *obuffer = '\0';

   for (;;) {

      mg_pause(1000);

      count ++;

      time_now = time(NULL);

      if (!(count % 180)) {
         mg_check_connections();
      }

#ifdef _WIN32

      if (!(count % 7)) {
         for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {
            if (core_data.thread_handles[n]) {
               if (GetExitCodeThread(core_data.thread_handles[n], &exit_code)) {
                  if (exit_code != STILL_ACTIVE) {
                     CloseHandle(core_data.thread_handles[n]);
                     core_data.thread_handles[n] = NULL;
                  }
               }
            }
         }
      }

#endif

   }

   return MG_THR_RETURN;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_daemon: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return MG_THR_RETURN;
}
#endif

}


int mg_wsmq_init(int mode)
{
   int n, len, cycle, status, free_slot, found, sn, no_config, shndle;
   unsigned long max_array_size;
   char *pBuffer, *pKey;
   char names[2048];
   char buffer[1092];
   char service[32], s_name[64], qm_name[64], q_name[64], server[64], routine[64], stat[32];
   THRCTRL thread_control[256];
   THRDATA thr_data[256];

#ifdef _WIN32
__try {
#endif

   if (!core_data.ws_loaded)
      return 1;

   *names = '\0';
   pBuffer = names;
   pKey = NULL;

   core_data.ws_session_magic ++;
   no_config = 0;

   mg_get_config("WSMQS_INDEX", NULL, "", pBuffer, 1024, core_data.mg_ini);

   for (pKey = pBuffer, sn = 1; *pKey != '\0'; pKey += T_STRLEN(pKey) + 1, sn ++) {
   
      if (!T_STRLEN(pKey))
         continue;

      strcpy(s_name, pKey);

      T_SPRINTF(service, "WSMQS:%s", pKey);
      len = mg_get_config(service, "Queue_Manager_Name", "", qm_name, 63, core_data.mg_ini);

      len = mg_get_config(service, "Queue_Name", "", q_name, 63, core_data.mg_ini);

      len = mg_get_config(service, "Server", "", server, 63, core_data.mg_ini);

      len = mg_get_config(service, "Server_Routine", "", routine, 63, core_data.mg_ini);

      len = mg_get_config(service, "Maximum_Array_Size", "", buffer, 16, core_data.mg_ini);
      if (len)
         max_array_size = strtol(buffer, NULL, 10);
      else {
         mg_set_config(service, "Maximum_Array_Size", "2048", core_data.mg_ini);
         max_array_size = 2048;
      }

      len = mg_get_config("WSMQS_INDEX", pKey, "", stat, 16, core_data.mg_ini);
      if (!strcmp(stat, "Disabled"))
         status = 0;
      else
         status = 1;

      if (strlen(qm_name) && strlen(q_name) && strlen(server) && strlen(routine)) {

         found = 0;
         no_config = 0;
         free_slot = -1;

         for (n = 0; n < (int) (core_data.ws_session_max + 10); n ++) {

            if (!mg_mqstable[n]) {
               if (free_slot == -1)
                  free_slot = n;
               continue;
            }

            if (!strcmp(mg_mqstable[n]->s_name, s_name)) {

               if (!mg_mqstable[n]->status && status) {

                  mg_mqstable[n]->run_status = 1;
               }

               strcpy(mg_mqstable[n]->s_name, s_name);

               if (strcmp(mg_mqstable[n]->qm_name, qm_name) || strcmp(mg_mqstable[n]->q_name, q_name)) {
                  strcpy(mg_mqstable[n]->qm_name, qm_name);
                  strcpy(mg_mqstable[n]->q_name, q_name);
                  mg_mqstable[n]->restart = 1;
               }

               strcpy(mg_mqstable[n]->server, server);
               strcpy(mg_mqstable[n]->routine, routine);
               mg_mqstable[n]->max_array_size = max_array_size;
               mg_mqstable[n]->status = status;

               mg_mqstable[n]->ws_session_magic = core_data.ws_session_magic;
               mg_mqstable[n]->service_no = sn;
               found = 1;
               break;
            }
         }
         if (!found) {
            if (free_slot > -1) {

               mg_mqstable[free_slot] = (LPMGMQ) mg_malloc(sizeof(MGMQ), "mg_wsmq_init:1");

               mg_mqstable[free_slot]->restart = 0;

               if (free_slot > (int) core_data.ws_session_max)
                  core_data.ws_session_max = free_slot;

               strcpy(mg_mqstable[free_slot]->s_name, s_name);

               strcpy(mg_mqstable[free_slot]->qm_name, qm_name);
               strcpy(mg_mqstable[free_slot]->q_name, q_name);

               strcpy(mg_mqstable[free_slot]->server, server);
               strcpy(mg_mqstable[free_slot]->routine, routine);
               mg_mqstable[free_slot]->max_array_size = max_array_size;
               mg_mqstable[free_slot]->status = status;
               mg_mqstable[free_slot]->activity = 0;
               mg_mqstable[free_slot]->ws_session_magic = core_data.ws_session_magic;
               mg_mqstable[free_slot]->service_no = sn;

               if (status)
                  mg_mqstable[free_slot]->run_status = 1;
               else
                  mg_mqstable[free_slot]->run_status = 0;

            }
         }
      }

      no_config ++;
   }

   cycle = 0;

   for (shndle = 0; shndle < (int) (core_data.ws_session_max + 10); shndle ++) {

      if (!mg_mqstable[shndle])
         continue;

      if (mg_mqstable[shndle]->run_status != 1)
         continue;

      if (cycle > 200)
         cycle = 0;

      thr_data[cycle].p_mgmq = mg_mqstable[shndle];

      /* Concurrent Server */

      n = mg_thread_create((LPTHRCTRL) &thread_control[cycle], (MG_THR_START_ROUTINE) mg_wsmq_server, (void *) &(thr_data[cycle]));


      if (n < 0) {
         sprintf(buffer, "Thread creation error %d", errno);
         mg_log_event(buffer, "Service Integration Gateway: Startup Error");

#if !defined(_WIN32)
         exit(0);
#else
         return 0;
#endif

      }

      cycle ++;

      continue;

   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_wsmq_init: %x", code);
      mg_log_event(buffer, "Error Condition");


   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

  
}


MG_THR_TYPE mg_wsmq_server(void *arg)
{
   int phase, ok, tbuffer_offset;
   int result, cycle, n, restart, last_reason, xxx;
   char buffer[1024];
   char *p_tbuffer_base, *p_tbuffer, *p, *p1;
   unsigned long tbuffer_len;
   LPMGMQ p_mgmq;

#if !defined(_WIN32)
   LPTSDATA p_tsdata;
#endif

   phase = 0;
   result = 0;
   last_reason = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   p_mgmq = ((LPTHRDATA) arg)->p_mgmq;


#if !defined(_WIN32)

   pthread_once(&tsdata_once, mg_once);

   if ((p_tsdata = pthread_getspecific(tsdata_key)) == NULL) {
      p_tsdata = (LPTSDATA) malloc(sizeof(TSDATA));
      if (!p_tsdata) {
         mg_thread_exit();
         return MG_THR_RETURN;
      }
      pthread_setspecific(tsdata_key, p_tsdata);
      p_tsdata->socket_closed = 0;
    
   }

   signal(SIGPIPE, SIG_IGN); /* 13 Broken pipe (POSIX).  */

#if defined(AIX) || defined(AIX5)
   signal(SIGHUP, SIG_IGN);      /* 1 Hangup (POSIX).  */
#else
   signal(SIGHUP, mg_error_trap); /* 1 Hangup (POSIX).  */
#endif
   signal(SIGILL, mg_error_trap); /* 4 Illegal instruction (ANSI).  */
   signal(SIGTRAP, mg_error_trap); /* 5 Trace trap (POSIX).  */
   signal(SIGABRT, mg_error_trap); /* 6 Abort (ANSI).  */
   signal(SIGIOT, mg_error_trap); /* 6 IOT trap (4.2 BSD).  */
   signal(SIGBUS, mg_error_trap); /* 7 BUS error (4.2 BSD).  */
   signal(SIGFPE, mg_error_trap); /*	8 Floating-point exception (ANSI).  */
   signal(SIGSEGV, mg_error_trap); /* 11 Segmentation violation (ANSI).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGCLD, mg_error_trap); /* Same as SIGCHLD (System V).  */
#endif
   signal(SIGCHLD, mg_error_trap); /* 17 Child status has changed (POSIX).  */
   signal(SIGCONT, mg_error_trap); /* 18 Continue (POSIX).  */
   signal(SIGTSTP, mg_error_trap); /* 20 Keyboard stop (POSIX).  */
   signal(SIGTTIN, mg_error_trap); /* 21 Background read from tty (POSIX).  */
   signal(SIGTTOU, mg_error_trap); /* 22 Background write to tty (POSIX).  */
   signal(SIGURG, mg_error_trap); /* 23 Urgent condition on socket (4.2 BSD).  */
   signal(SIGXCPU, mg_error_trap); /* 24 CPU limit exceeded (4.2 BSD).  */
   signal(SIGXFSZ, mg_error_trap); /* 25 File size limit exceeded (4.2 BSD).  */
   signal(SIGWINCH, mg_error_trap); /* 28 Window size change (4.3 BSD, Sun).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPOLL, mg_error_trap); /* SIGIO Pollable event occurred (System V).  */
#endif
   signal(SIGIO, mg_error_trap); /* 29 I/O now possible (4.2 BSD).  */
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPWR, mg_error_trap); /* 30 Power failure restart (System V).  */
#endif

#endif


   p_mgmq->run_status = 2;

   n = 1;
   ok = 1;

   if (!ok) {

      mg_log_event(p_mgmq->error, "IBM MQ Server : Error Condition on starting a listening service");

      mg_thread_exit();
      return MG_THR_RETURN;

   }


mg_wsmq_server_restart:

   restart = 0;

   for (;;) {

      if (p_mgmq->status == 0) {

         T_SPRINTF(buffer, "Closing listening service for queue '%s' (Queue Manager: '%s')", p_mgmq->q_name, p_mgmq->qm_name);
         mg_log_event(buffer, "IBM MQ Server");

         break;
      }

      mg_mgmq_listen(p_mgmq, MG_LISTEN_OPEN);

      if (p_mgmq->open_c_code != 0) { /* !MQCC_OK */

         if (mg_mqstable[n]->restart == 1) {
            mg_mqstable[n]->restart = 0;
            continue;
         }

         if (p_mgmq->open_r_code != last_reason) {
            T_SPRINTF(buffer, "Queue '%s' (Queue Manager: '%s'); Completion code: %d; Reason code: %d", p_mgmq->q_name, p_mgmq->qm_name, p_mgmq->open_c_code, p_mgmq->open_r_code);
            mg_log_event(buffer, "IBM MQ Server : Error Condition on Connecting to a Queue manager or Opening a Queue");
         }

         last_reason = p_mgmq->open_r_code;

         mg_pause(60000);

      }
      else {
         last_reason = 0;
         break;
      }
   }

   if (p_mgmq->status == 0) {
      mg_thread_exit();
      return MG_THR_RETURN;
   }


   T_SPRINTF(buffer, "Listening for messages on queue '%s' (Queue Manager: '%s')", p_mgmq->q_name, p_mgmq->qm_name);
   mg_log_event(buffer, "IBM MQ Server");

   n = 0;
   cycle = 0;
   xxx = 0;

   for (;;) {

      if (p_mgmq->status == 0) {

         T_SPRINTF(buffer, "Closing listening service for queue '%s' (Queue Manager: '%s')", p_mgmq->q_name, p_mgmq->qm_name);
         mg_log_event(buffer, "IBM MQ Server");

         break;
      }

      p_mgmq->r_buffer_size = 4096;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * p_mgmq->r_buffer_size, "mg_wsmq_server:1");

      p_mgmq->s_buffer_size = 0;
      p_mgmq->s_buffer = NULL;

      p_mgmq->timeout = 10;

      mg_mgmq_listen(p_mgmq, MG_LISTEN_GET);

      if (p_mgmq->get_c_code != 0) { /* !MQCC_OK */

         if (p_mgmq->get_r_code == 2033) { /* MQRC_NO_MSG_AVAILABLE */
            mg_free((void *) p_mgmq->r_buffer, "mg_wsmq_server:1");
            p_mgmq->r_buffer = NULL;
            p_mgmq->r_buffer_size = 0;

            if (mg_mqstable[n]->restart == 1) {
               mg_mqstable[n]->restart = 0;
               restart = 1;
               break;
            }

            continue;
         }

         if (p_mgmq->get_r_code != last_reason) {
            T_SPRINTF(buffer, "Queue '%s' (Queue Manager: '%s'); Completion code: %d; Reason code: %d", p_mgmq->q_name, p_mgmq->qm_name, p_mgmq->get_c_code, p_mgmq->get_r_code);
            mg_log_event(buffer, "IBM MQ Server : Error Condition while WAITING for a message");
         }

         last_reason = p_mgmq->get_r_code;

         mg_pause(60000);

         restart = 1;

         mg_free((void *) p_mgmq->r_buffer, "mg_wsmq_server:2");
         p_mgmq->r_buffer = NULL;
         p_mgmq->r_buffer_size = 0;

         break;
      }
      else {
         last_reason = 0;
      }

      p_mgmq->activity ++;

      tbuffer_offset = 128;
      tbuffer_len = p_mgmq->r_buffer_len + 8192 + tbuffer_offset;
      p_tbuffer_base = (char *) mg_malloc(sizeof(char) * (tbuffer_len + 32), "mg_wsmq_server:2");

      p_tbuffer = p_tbuffer_base + tbuffer_offset;

      strcpy(p_tbuffer, "");

      strcpy(p_tbuffer, "PHPw^P^");
      strcat(p_tbuffer, p_mgmq->server);

      if (p_mgmq->r_buffer_len < p_mgmq->max_array_size)
         strcat(p_tbuffer, "##0####0");
      else
         strcat(p_tbuffer, "##0####1");

      strcat(p_tbuffer, "^X");

      strcat(p_tbuffer, drec);
      strcat(p_tbuffer, "00");
      strcat(p_tbuffer, "WSMQSRV^%ZMGSIS");

      strcat(p_tbuffer, drec);
      strcat(p_tbuffer, "11");
      strcat(p_tbuffer, "routine");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->routine);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "max_array_size");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%lu", p_mgmq->max_array_size);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "qm_name");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->qm_name);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "q_name");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->q_name);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "rqm_name");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->rqm_name);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "rq_name");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->rq_name);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "r_type");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%d", p_mgmq->r_type);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "r_code");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%d", p_mgmq->get_r_code);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "info");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->info);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "error");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->error);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "msg_id");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->msg_id);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "correl_id");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->correl_id);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "group_id");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, p_mgmq->group_id);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "msg_seq_no");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%ld", p_mgmq->msg_seq_no);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "offset");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%ld", p_mgmq->offset);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "recv_len");
      strcat(p_tbuffer, dak);
      sprintf(buffer, "%lu", p_mgmq->r_buffer_len);
      strcat(p_tbuffer, buffer);
      strcat(p_tbuffer, dar);

      strcat(p_tbuffer, "recv");
      strcat(p_tbuffer, dak);
      strcat(p_tbuffer, (char *) p_mgmq->r_buffer);
      strcat(p_tbuffer, dar);
      strcat(p_tbuffer, deod);

      mg_free((void *) p_mgmq->r_buffer, "mg_wsmq_server:3");
      p_mgmq->r_buffer = NULL;
      p_mgmq->r_buffer_size = 0;
      p_mgmq->r_buffer_len = 0;

      result = mg_php_direct(p_tbuffer_base, 0, &p_tbuffer_base, (unsigned long *) &tbuffer_len, (int *) &tbuffer_offset, 1);

      p_tbuffer = p_tbuffer_base;
      phase = 6;

      phase = 7;

      phase = 8;

      if (p_tbuffer[0] == MG_RECV_HEAD_ERROR[0]) {

         p_tbuffer[0] = ':';
         p_mgmq->s_buffer_len = (unsigned long) strlen(p_tbuffer);
         p_mgmq->s_buffer_size = tbuffer_len;
         p_mgmq->s_buffer = (unsigned char *) p_tbuffer;

      }
      else {
         p = strstr(p_tbuffer, "send\x04");
         if (p) {
            p1 = strstr(p, dar);
            if (p1)
               *p1 = '\0';

            p_mgmq->s_buffer_len = (int) strlen(p + 5);
            p_mgmq->s_buffer_size = tbuffer_len;
            p_mgmq->s_buffer = (unsigned char *) (p + 5);

         }
         else {
            p_mgmq->s_buffer = (unsigned char *) p_tbuffer;
            strcpy((char *) p_mgmq->s_buffer, "");
            p_mgmq->s_buffer_len = 0;
            p_mgmq->s_buffer_size = tbuffer_len;
         }
      }

      phase = 9;

      mg_mgmq_listen(p_mgmq, MG_LISTEN_PUT);

      phase = 11;

      mg_free((void *) p_tbuffer_base, "mg_wsmq_server:4");
      p_tbuffer = NULL;
      p_tbuffer_base = NULL;

      p_mgmq->s_buffer = NULL;
      p_mgmq->s_buffer_len = 0;
      p_mgmq->s_buffer_size = 0;

      phase = 12;

      if (strlen(p_mgmq->error)) {

         mg_log_event(p_mgmq->error, "IBM MQ Server: Error replying to a message");

      }

      if (strlen(p_mgmq->info)) {

         mg_log_event(p_mgmq->info, "IBM MQ Server: Information associated with replying to a message");

      }

      phase = 13;

   }


   if (restart) {
      mg_pause(60000);
      goto mg_wsmq_server_restart;
   }

   mg_thread_exit();
   return MG_THR_RETURN;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_wsmq_server: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return MG_THR_RETURN;
}
#endif

}


static void mg_once(void)
{

#if !defined(_WIN32)
   pthread_key_create(&tsdata_key, mg_destructor);
#endif
   return;

}



static void mg_destructor(void *ptr)
{

#if !defined(_WIN32)

   LPTSDATA p_tsdata;

   p_tsdata = (LPTSDATA) ptr;
   if (!p_tsdata)
      return;

   if (!p_tsdata->socket_closed)
      close(p_tsdata->sockfd);

   free((void *) ptr);

#endif

   return;
}



static void mg_goingdown(void)
{

   if (core_data.server_sockfd != -1 || core_data.server_sockfd_ipv6 != -1) {
#ifdef _WIN32
      if (core_data.server_sockfd_ipv6 != -1)
         MGNET_CLOSESOCKET(core_data.server_sockfd_ipv6);
      if (core_data.server_sockfd != -1)
         MGNET_CLOSESOCKET(core_data.server_sockfd);

      if (core_data.wsastartup == 0)
         MGNET_WSACLEANUP();
/*
      mg_dso_unload((MGHLIB) mg_sock.h_sock);
*/
#else
      if (core_data.server_sockfd_ipv6 != -1)
         close(core_data.server_sockfd_ipv6);
      if (core_data.server_sockfd != -1)
         close(core_data.server_sockfd);
#endif
      core_data.server_sockfd_ipv6 = -1;
      core_data.server_sockfd = -1;
   }

   mg_closedown();

   return;
}


static void mg_goingdown_signal(int sig)
{
#ifdef _WIN32
   printf("\nCaught 'SIGINT' (Control-C) - The Service Integration Gateway is closing down ...\n");
#else
   mg_log_event("Caught 'SIGTERM' - The Service Integration Gateway is closing down", "CloseDown");
#endif

  if (core_data.server_sockfd != -1 || core_data.server_sockfd_ipv6 != -1) {

#ifdef _WIN32
      if (core_data.server_sockfd_ipv6 != -1)
         MGNET_CLOSESOCKET(core_data.server_sockfd_ipv6);
      if (core_data.server_sockfd != -1)
         MGNET_CLOSESOCKET(core_data.server_sockfd);

      if (core_data.wsastartup == 0)
         MGNET_WSACLEANUP();
/*
      mg_dso_unload((MGHLIB) mg_sock.h_sock);
*/
#else

#if !defined(AIX) && !defined(AIX5)
      if (core_data.server_sockfd_ipv6 != -1) {
         close(core_data.server_sockfd_ipv6);
      }
      if (core_data.server_sockfd != -1) {
         close(core_data.server_sockfd);
      }
#endif
#endif
      core_data.server_sockfd_ipv6 = -1;
      core_data.server_sockfd = -1;
   }

#ifdef _WIN32
   printf("\nService Integration Gateway is down\n");
#endif

   if (core_data.no_children == 0)
      exit(7);
   else
      exit(0);

}


static void mg_parent_goingdown_signal(int sig)
{
   int n;

   n = 0;

#if defined(_WIN32)
   printf("\nCaught 'SIGINT' (Control-C) - Service Integration Gateway is closing down ...\n");
#else /* UNIX */
   mg_log_event("Caught 'SIGTERM' - Service Integration Gateway is closing down - signal the child process(es)", "CloseDown");
#endif

#if !defined(_WIN32)
   for (n = 0; n < MG_MAX_CHILD; n ++) {
      if (sig_child[n] > 0) {
         kill(sig_child[n], SIGTERM);
      }
   }

   mg_set_config("SYSTEM", "Close_Down", "0", core_data.mg_ini);

#endif

   return;

}


static void mg_error_trap(int sig)
{
   char buffer[256];

   sprintf(buffer, "Signal Received: %d", sig);

   core_data.error_count ++;
   if (core_data.error_count < 100)
      mg_log_event(buffer, "Operating System Event");

   mg_thread_exit();

   return;
}


static void mg_parent_error_trap(int sig)
{
   char buffer[128];

   T_SPRINTF(buffer, "Signal Received: %d", sig);
   mg_log_event(buffer, "Operating System Event");

#if defined(LINUX) || defined(MACOSX)
   if (sig == SIGSEGV) {
      int i, len;
      void *ptrace[128];
      char **func_names;
      char *line;
      char buffer[8192];
      size_t count;

      count = backtrace(ptrace, 128);

      func_names = backtrace_symbols(ptrace, count);

      len = 0;
      *buffer = '\0';
      for (i = 0; i < count; i++) {
         line = func_names[i];
         len += strlen(func_names[i]) + 2;
         if (len < 8192) {
            T_STRCAT(buffer, func_names[i]);
            T_STRCAT(buffer, "\r\n");
         }
      }
      mg_log_event(buffer, "Backtrace (SIGSEGV) : mg_parent_error_trap");

      free(func_names);

      exit(0);
   }
#endif

   return;
}


double mg_get_os_version(void)
{
   double result;

#if defined(_WIN32)

#if defined(_MSC_VER) && (_MSC_VER >= 1900) && (NTDDI_VERSION >= NTDDI_WINXP)

   if (IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0)) {
      result = MG_WINVER_10;
   }
   else if (IsWindows10OrGreater())
      result = MG_WINVER_10;
   else if (IsWindows8OrGreater())
      result = MG_WINVER_8;
   else if (IsWindows7OrGreater())
      result = MG_WINVER_7;
   else if (IsWindowsVistaOrGreater())
      result = MG_WINVER_VISTA;
   else if (IsWindowsXPOrGreater())
      result = MG_WINVER_XP;
   else
      result = MG_WINVER_NT;

#else

   double version;
   unsigned long dwVersion, dwBuild, dwWindowsMajorVersion, dwWindowsMinorVersion;

   dwVersion = GetVersion();
 
   /* Get the Windows version. */

   dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
   dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));

   version = dwWindowsMajorVersion + ((double) (dwWindowsMinorVersion * .1));
/*
{
   char buffer[256];
   sprintf(buffer, "GetVersion(%f): dwVersion=%d/%x; dwWindowsMajorVersion=%d/%x;dwWindowsMinorVersion=%d/%x", version, dwVersion, dwVersion, dwWindowsMajorVersion, dwWindowsMajorVersion, dwWindowsMinorVersion, dwWindowsMinorVersion);
   mg_log_event(buffer, "Windows GetVersion()");
}
*/

   if (version > MG_WINDOWS_NT) {
      result = version;
   }
   else {

      /* Get the build number. */

      if (dwVersion < 0x80000000) {

         /* Windows NT/2000, Whistler */

         dwBuild = (DWORD)(HIWORD(dwVersion));
         result = MG_WINDOWS_NT;
      }
      else if (dwWindowsMajorVersion < 4) {

         /* Win32s */

         dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000);
         result = MG_WINDOWS_32;
      }
      else {

         /* Windows 95/98/Me */

         dwBuild =  0;
         result = MG_WINDOWS_95;
      }
   }
#endif

#else

   result = 0;

#endif

   return result;
}

