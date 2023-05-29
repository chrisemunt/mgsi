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


#ifndef MGSIMQ_H
#define MGSIMQ_H

#if defined(_WIN32)              /* Windows */
#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
#define _CRT_SECURE_NO_DEPRECATE    1
#define _CRT_NONSTDC_NO_DEPRECATE   1
#endif
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32

/* Windows: General */

#include <tchar.h>
#include <windows.h>
#define _INC_WINDOWS
#include <winsock.h>
#include <direct.h>
#include <process.h>
#include <winsvc.h>
#include <process.h>
#include <signal.h>
#include <conio.h>
#include <fcntl.h>
#include <io.h>

#else

#define WINAPI

#endif

#if defined(MGSIMQ_LIB)

#define MAJORVERSION             1
#define MINORVERSION             6
#define BUILDNUMBER              37
#define PATCHNUMBER              0
#define PRODUCTVERSIONSTR        "1.6.37.0"
#define FILEVERSIONSTR           PRODUCTVERSIONSTR
#define MG_VERSION               "1.6.37"
#define MG_VERSION_DISPLAY       "1.6"
#define MG_VERSION_CREATED       "3 May 2007"

#endif

#define MG_LISTEN_OPEN           1
#define MG_LISTEN_GET            2
#define MG_LISTEN_PUT            3
#define MG_LISTEN_CLOSE          4

#define MG_Q_MGR_NAME_LENGTH     128
#define MG_Q_NAME_LENGTH         128

#define MG_MSG_ID_LENGTH         128
#define MG_CORREL_ID_LENGTH      128
#define MG_GROUP_ID_LENGTH       128

#if defined(MGSIMQ_EXPORTS) ||  defined(MGSIMQC_EXPORTS)
#if defined(MGSIMQ_USESTATICLIB) || defined(MGSIMQC_USESTATICLIB)
#define MGSIMQ_API 
#else
#ifdef _WIN32
#define MGSIMQ_API __declspec(dllexport)
#else
#define MGSIMQ_API
#endif
#endif	/* #ifdef MGSIMQ_USESTATICLIB */
#else	/* #ifdef MGSIMQ_EXPORTS */
#if defined(MGSIMQ_USESTATICLIB) || defined(MGSIMQC_USESTATICLIB)
#define MGSIMQ_API 
#else
#ifdef _WIN32
#define MGSIMQ_API __declspec(dllimport)
#else
#define MGSIMQ_API
#endif
#endif	/* #ifdef MGSIMQ_USESTATICLIB */
#endif	/* #ifdef MGSIMQ_EXPORTS */



typedef struct tagMGMQ {

   int c_code;
   int r_code;

   int open_c_code;
   int open_r_code;

   int put_c_code;
   int put_r_code;

   int get_c_code;
   int get_r_code;

   int close_c_code;
   int close_r_code;

   char s_name[64];
   char q_name[MG_Q_NAME_LENGTH];
   char qm_name[MG_Q_MGR_NAME_LENGTH];
   char tqm_name[MG_Q_MGR_NAME_LENGTH];

   char dq_name[MG_Q_NAME_LENGTH];
   char dqm_name[MG_Q_MGR_NAME_LENGTH];

   char rqm_name[MG_Q_MGR_NAME_LENGTH];
   char rq_name[MG_Q_NAME_LENGTH];

   char msg_id[MG_MSG_ID_LENGTH];
   char correl_id[MG_CORREL_ID_LENGTH];
   char group_id[MG_GROUP_ID_LENGTH];
   long msg_seq_no;
   long offset;

   char connx_options[256];
   char open_options[256];
   char close_options[256];
   char put_options[256];
   char get_options[256];
   char get_match_options[256];
   char begin_options[256];

   char error[128];
   char info[128];

   char key[1024];

   unsigned char *s_buffer;
   unsigned long s_buffer_len;
   unsigned long s_buffer_size;

   unsigned char *r_buffer;
   unsigned long r_buffer_len;
   unsigned long r_buffer_size;
   unsigned long r_buffer_offs;

   unsigned long max_array_size;

   short r_type;

   unsigned long timeout;
   int status;
   int service_no;
   short run_status;
   char server[64];
   char routine[64];
   unsigned long activity;
   unsigned long ws_session_magic;

   void *   (* mg_malloc) (unsigned long size, char * trace);
   int      (* mg_free)   (void * p_mem, char * trace);

   void * p_mqsrv;
   short thread;
   short restart;
} MGMQ, *LPMGMQ;


typedef struct tagMGMQOPT {
   char     options[1024];
   char *   opt[256];
} MGMQOPT, *LPMGMQOPT;


#define MG_MQ_API

typedef int          (MG_MQ_API * LPFN_MG_MQ_VERSION)     (char * version);
typedef int          (MG_MQ_API * LPFN_MG_MQ_CONNX)       (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_DISC)        (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_BEGIN)       (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_BACK)        (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_CMIT)        (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_PUT)         (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_GET)         (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_REQ)         (MGMQ * p_mgmq);
typedef int          (MG_MQ_API * LPFN_MG_MQ_LISTEN)      (MGMQ * p_mgmq, int mode);


MGSIMQ_API int mgmq_version(char * version);
MGSIMQ_API int mgmq_connx(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_disc(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_begin(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_back(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_cmit(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_put(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_get(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_req(MGMQ * p_mgmq);
MGSIMQ_API int mgmq_listen(MGMQ * p_mgmq, int mode);


int mgmq_encodeID(char * encstring, int encstring_len, char *bytestring, int bytestring_len);
int mgmq_decodeID(char * encstring, int encstring_len, char *bytestring, int bytestring_len);
int mgmq_unpack_options(LPMGMQOPT p_mqopt);

#endif /* #ifndef MGSIMQ_H */

