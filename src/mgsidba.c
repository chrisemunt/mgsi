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


#include "mgsisys.h"
#include "mgsi.h"
#include "mgsiutl.h"
#include "mgsicfg.h"
#include "mgsidso.h"
#include "mgsidba.h"

extern int errno;

int mg_get_connection(int *p_chndle, int *p_shndle, char *m_server_name, int timeout, int no_retry, int context)
{
   short phase;
   int result, n;
   time_t time1, time2, time_diff;

   phase = 0;

#ifdef _WIN32
__try {
#endif

   time1 = time(NULL);
   for (n = 1;; n ++) {

      result = mg_tcp_open_connection(p_chndle, p_shndle, (char *) m_server_name, timeout, no_retry, context);

      if (no_retry) {
         break;
      }
      phase = 1;

      if (result > 0)
	      break;
      if (result < -100)
         break;

      time2 = time(NULL);
      time_diff = (time_t) difftime(time2, time1);
      if (time_diff > (time_t) core_data.server_timeout) {
         break;
      }

      mg_pause(500);
   }
   if (result < 1) {
      goto mg_get_connection_exit;
   }
   if (result == 2) {
      goto mg_get_connection_exit;
   }

   phase = 2;

   result = mg_test_connection(*p_chndle, m_server_name);
   phase = 3;
   if (result == -130) { /* CM0051 */
      phase = 40;
      mg_tcp_close_socket(*p_chndle);
      mg_tcp_free_dbc_memory(*p_chndle);
      goto mg_get_connection_exit;
   }
   phase = 11;
	if (result < 1) {
      mg_tcp_close_connection(*p_chndle, 1);
   }

   phase = 12;

mg_get_connection_exit:

   return result;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_get_connection: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_release_connection(int chndle, int context)
{
   int x = 0;

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   mg_connections[chndle]->t_prev = time(NULL);

   if (context == MG_CS_RELEASE_TO_POOL && mg_connections[chndle]->status != 2) {
      mg_connections[chndle]->status = 0;
      mg_connections[chndle]->in_use = 0;
      return 1;
   }

   mg_connections[chndle]->status = 1;

   if (context == MG_CS_FORCE_DOWN) {
      x = mg_tcp_close_connection(chndle, 1);
   }
   else {
/*
      mg_pause(500);
*/
	   x = mg_close_connection_to_host(chndle);
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_release_connection: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_server_read(int chndle, void *instring, unsigned long instring_size, unsigned long *p_instring_actual_size, int timeout, int no_close_on_timeout)
{
   int n, result, tstrt;
   int nread;
   char *p_instring;

#ifdef _WIN32
__try {
#endif

   p_instring = (char *) instring;
   if (timeout == 0)
      timeout =  mg_connections[chndle]->p_server->idle_timeout;

   *p_instring_actual_size = 0;
   nread = 0;
   n = 0;
   result = 0;
   tstrt = 0;

   if (mg_connections[chndle]->eod) {
      result = MG_READ_EOF;
      mg_connections[chndle]->eod = 0;
      return result;
   }

   mg_connections[chndle]->eod = 0;
   nread = 0;

   if (mg_connections[chndle]->buffer_len) {
      strncpy(p_instring, mg_connections[chndle]->buffer, mg_connections[chndle]->buffer_len);
      p_instring[mg_connections[chndle]->buffer_len] = '\0';
      nread = mg_connections[chndle]->buffer_len;
      mg_connections[chndle]->buffer[0] = '\0';
      mg_connections[chndle]->buffer_len = 0;
   }

   if (mg_connections[chndle]->t_mode == 0 && mg_connections[chndle]->clen == 0) {
      short got_header;
      int n;

      got_header = 0;

      for (;;) {

         n = mg_tcp_read(chndle, p_instring + nread, instring_size - nread, timeout * 1000);

         if (n < 0) {
            nread = n;
            break;
         }
         nread += n;
         p_instring[nread] = '\0';

         if (!got_header && nread >= MG_RECV_HEAD) {

            got_header = 1;

            if (!strncmp(p_instring + 5, "sc", 2)) {
               for (n = 0; n < 8; n ++) {
                  if (p_instring[n] == '0')
                     break;
                  mg_connections[chndle]->b_term[n] = p_instring[n];
                  p_instring[n] = '0';
               }
               mg_connections[chndle]->b_term[n] = '\0';
               mg_connections[chndle]->b_term_len = n;

               mg_connections[chndle]->t_mode = 1;
            }
            mg_connections[chndle]->clen = mg_decode_size(p_instring, 5, MG_CHUNK_SIZE_BASE);

         }

         if ((mg_connections[chndle]->t_mode == 0 && got_header) || (mg_connections[chndle]->t_mode == 1 && (nread >= (MG_RECV_HEAD + mg_connections[chndle]->b_term_len)))) {
            break;
         }
      }
   }
   else {

      for (;;) {
         n = mg_tcp_read(chndle, p_instring + nread, instring_size - nread, timeout * 1000);

         if (n < 1) {
            nread = n;
            break;
         }
         nread += n;
         p_instring[nread] = '\0';
         if (mg_connections[chndle]->t_mode == 1 && nread >= mg_connections[chndle]->b_term_len) {
            tstrt = nread - mg_connections[chndle]->b_term_len;
            if (!strcmp(p_instring + tstrt, mg_connections[chndle]->b_term))
               break;
         }
         if (mg_connections[chndle]->t_mode == 0)
            break;

         if (nread >= (int) instring_size)
            break;
      }
   }

   if (nread == MG_READ_TIMEOUT) {

      T_SPRINTF(p_instring, "%s Not Responding", MG_DB_SERVER_TITLE);
      *p_instring_actual_size = (unsigned long) strlen(p_instring);

      if (!no_close_on_timeout) {
         mg_close_connection_to_host(chndle);
      }
      result = MG_READ_TIMEOUT;
   }

   else if (nread < 0) {
      result = MG_READ_ERROR;
   }
   else {

      if (nread > 0) {

         if (mg_connections[chndle]->t_mode == 1) {
            tstrt = nread - mg_connections[chndle]->b_term_len;
            if (tstrt < 0)
               tstrt = 0;

            if (!strcmp(p_instring + tstrt, mg_connections[chndle]->b_term)) {
               nread -= mg_connections[chndle]->b_term_len;
               mg_connections[chndle]->eod = 1;
            }
            if (!mg_connections[chndle]->eod) {

               for (n = tstrt; n < nread; n ++) { /* was n = (tstrt + 1) */

                  if (strchr(mg_connections[chndle]->b_term, (int) p_instring[n])) {

                     strncpy(mg_connections[chndle]->buffer, p_instring + n, nread - n);
                     mg_connections[chndle]->buffer_len = nread - n;
                     p_instring[n] = '\0';
                     nread = n;
                     break;
                  }
               }
            }
         }

         mg_connections[chndle]->rlen += nread;

         if (mg_connections[chndle]->t_mode == 0 && mg_connections[chndle]->rlen >= (mg_connections[chndle]->clen + 8))
            mg_connections[chndle]->eod = 1;
      }
      *p_instring_actual_size = nread;
      p_instring[nread] = '\0';

      result = MG_READ_OK;

   }

   return result;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_server_read: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_server_write(MGREQ *p_request, int chndle, LPMEMOBJ p_request_data)
{
   int errorcondition = 0, result = 0, n = 0, requestlen = 0, headerlen = 0, endstringlen = 0;
   char endstring[16], buffer[8192];

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   if (core_data.diagnostics) {
      T_SPRINTF(buffer, "Diagnostics: net header on channel %d, thread %lu", chndle, mg_current_thread_id());
      mg_log_event(p_request_data->p_buffer, buffer);
   }

   errorcondition = 0;
   mg_connections[chndle]->binary = 0;
   mg_connections[chndle]->rlen = 0;
   mg_connections[chndle]->clen = 0;
   mg_connections[chndle]->t_mode = 0;
   mg_connections[chndle]->buffer_len = 0;

   requestlen = (int) strlen(p_request_data->p_buffer);

   if (requestlen > 0 && strstr(p_request_data->p_buffer, "\n"))
      strcpy(endstring, "");
   else
      strcpy(endstring, "00000\n");



   mg_buf_cat(p_request_data, endstring);
   if (mg_server_write_buffer(chndle, p_request_data->p_buffer, p_request_data->curr_size, 0) == 0)
	   errorcondition = 1;

   if (errorcondition == 1) {
      result = 0;
   }
   else {
      result = 1;
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_server_write: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_server_write_buffer(int chndle, char *outstring, unsigned long size, int offset)
{
   int result, n;
   unsigned long nsent;

int cnt = 0;

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   mg_connections[chndle]->rlen = 0;
   mg_connections[chndle]->clen = 0;
   mg_connections[chndle]->t_mode = 0;
   mg_connections[chndle]->buffer_len = 0;

   result = 1;
   nsent = 0;

   n = mg_tcp_write(chndle, (void *) outstring, size);

   nsent = (unsigned long) n;

   if (n < 0 || nsent < size)
      result = 0;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_server_write_buffer: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_close_connection_to_host(int chndle)
{
   int x = 0, len = 0;
   char buffer[512];

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   mg_connections[chndle]->close = 1;

   if (mg_connections[chndle]->status != 0)
/*
      mg_pause(1000);
*/

   if (mg_connections[chndle]->p_server->dbtype == MG_DBTYPE_M) {
      T_STRCPY(buffer, "^X^");
/*
      T_STRCAT(buffer, deod);
*/
      T_STRCAT(buffer, "\n");

      x = mg_tcp_write(chndle, (void *) buffer, (unsigned long) T_STRLEN(buffer));
   }

	x = mg_tcp_close_connection(chndle, 1);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_close_connection_to_host: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_close_connections(char *m_server_name, int context)
{
   int chndle, n, selective, max_connection_no;

#ifdef _WIN32
__try {
#endif

   if (T_STRLEN(m_server_name) == 0 || T_STRCMP(m_server_name, "*") == 0)
      selective = 0;
   else
      selective = 1;

   max_connection_no = MG_MAX_CONNECTIONS;

   for (chndle = 0; chndle < max_connection_no; chndle ++) {

      if (mg_connections[chndle] != NULL) {

         if (selective == 0 || T_STRCMP(mg_connections[chndle]->p_server->m_server_name, m_server_name) == 0) {

            mg_connections[chndle]->status = 1;
            n = mg_close_connection_to_host(chndle);
         }
      }
   }
   if (selective == 0) {
      for (chndle = 0; chndle < MG_MAX_CONNECTIONS; chndle ++) {
         if (mg_connections[chndle] != NULL) {
            n = mg_tcp_close_connection(chndle, 1);
         }
      }
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_close_connections: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_dbc_test(int chndle, int close_on_error, int mode)
{
   int result, n, errorno;
   char buffer[1024], instring[1024];
   unsigned long buffer_actual_size, size;
   MEMOBJ request;
   fd_set rset, wset, eset;
   struct timeval tval;

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   if (mg_connections[chndle]->p_server->dbtype != MG_DBTYPE_M) {
      return 1;
   }

   if (mode == 1) {

      if (mg_connections[chndle]->in_use || mg_connections[chndle]->activity == 0) {
         return 1;
      }

      result = 1;
      n = 0;
      errorno = 0;
      *buffer = '\0';

      FD_ZERO(&rset);
      FD_ZERO(&wset);
      FD_ZERO(&eset);
      FD_SET(mg_connections[chndle]->cli_socket, &rset);
      FD_SET(mg_connections[chndle]->cli_socket, &wset);
      FD_SET(mg_connections[chndle]->cli_socket, &eset);
      tval.tv_sec = 0;
      tval.tv_usec = 0;

      n = MGNET_SELECT((int) mg_connections[chndle]->cli_socket + 1, &rset, &wset, &eset, &tval);

      if (n < 1) {
         if (mg_servers[mg_connections[chndle]->shndle]->log_errors == 1) {
            mg_log_event("Cannot detect a socket available for writing", "Connection Error");
         }

         result = -1;
      }


      else if (MGNET_FD_ISSET(mg_connections[chndle]->cli_socket, &rset)) {

         n = MGNET_RECV(mg_connections[chndle]->cli_socket, buffer, 1, MSG_PEEK);

         if (n < 1) {
            result = -1;
         }
      }


      if (result == -1) {
         if (close_on_error)
            mg_release_connection(chndle, MG_CS_FORCE_DOWN);
         return -1;

      }

      return result;

   }
   else {

      result = 0;
      buffer_actual_size = 0;
      *instring = '\0';

      mg_buf_init(&request, 64, 64);

      while (mg_tcp_read(chndle, buffer, 128, 0) > 0)
         continue;

      strcpy(buffer, "^A^G0123456789^^^^^\n");

      mg_buf_cpy(&request, buffer);

      if (mg_server_write(NULL, chndle, &request) != 0) {
         T_STRCPY(buffer, "");
         T_STRCPY(instring, "");
         size = 0;

         while (mg_server_read(chndle, (void *) buffer, 511L, &buffer_actual_size, 7, 1) == MG_READ_OK) {
            T_STRCAT(instring, buffer);
            size += buffer_actual_size;
         }

         if (size > 0) {
            result = 1;
         }
      }

      mg_buf_free(&request);

      return result;

   }

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_dbc_test: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_test_connection(int chndle, char *m_server_name)
{
   short phase;
   int result = 0, len, child_port, status, timeout;
   char buffer[512], instring[512], job[32], uci[256];
   char *p, *p1;
   unsigned long buffer_actual_size = 0, size = 0;
   MEMOBJ request;

   phase = 0;

#ifdef _WIN32
__try {
#endif

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   result = 0;
   len = 0;
   child_port = 0;
   status = 0;
   buffer_actual_size = 0;
   timeout = 7;

   uci[0] = '\0';

   mg_buf_init(&request, 256, 256);

   /* CM0051 */
   T_SPRINTF(buffer, "^S^version=%s&timeout=%d&nls=%s&uci=%s&&&&&&&unpw=", core_data.mg_version, mg_connections[chndle]->p_server->idle_timeout, mg_connections[chndle]->p_server->nls_translation, mg_connections[chndle]->p_server->uci);

   mg_buf_cpy(&request, buffer);

   /* CM0051 */
   mg_buf_cat(&request, mg_connections[chndle]->p_server->username);
   mg_buf_cat(&request, "\x01");
#if 1
   mg_buf_cat(&request, mg_connections[chndle]->p_server->password_trans);
#else
   mg_buf_cat(&request, mg_connections[chndle]->p_server->password);
#endif
   mg_buf_cat(&request, "\n");
   request.curr_size = (unsigned long) strlen(request.p_buffer);

   phase = 1;

   if (mg_server_write(NULL, chndle, &request) != 0) {
      T_STRCPY(buffer, "");
	   T_STRCPY(instring, "");
      size = 0;

      phase = 2;

      while ((status = mg_server_read(chndle, (void *) buffer, 511L, &buffer_actual_size, timeout, 0)) == MG_READ_OK) {
         phase = 3;

	      T_STRCAT(instring, buffer);
         size += buffer_actual_size;
      }

      if (status == MG_READ_ERROR) {
         result = -130;
         goto mg_test_connection_exit;
      }

      phase = 4;

      if (!mg_connections[chndle]) {
         goto mg_test_connection_exit;
      }

      p = strstr(instring, "pid=");
      if (p) {
         result = 1;
         p +=4;
         p1 = strstr(p, "&");
         if (p1)
            *p1 = '\0';
         strcpy(job, p);
         strcpy(mg_connections[chndle]->m_job_no, job);
         if (p1)
            *p1 = '&';
      }
      p = strstr(instring, "uci=");
      if (p) {
         p +=4;
         p1 = strstr(p, "&");
         if (p1)
            *p1 = '\0';
         strcpy(uci, p);
         strcpy(mg_connections[chndle]->uci, uci);
         if (p1)
            *p1 = '&';
      }
      p = strstr(instring, "server_type=");
      if (p) {
         p +=12;
         p1 = strstr(p, "&");
         if (p1)
            *p1 = '\0';
         strcpy(mg_connections[chndle]->p_server->database_type, p);
         if (p1)
            *p1 = '&';
      }
      p = strstr(instring, "&server_version=");
      if (p) {
         p +=16;
         p1 = strstr(p, "&");
         if (p1)
            *p1 = '\0';
         strcpy(mg_connections[chndle]->p_server->database_version, p);
         if (p1)
            *p1 = '&';
      }
      p = strstr(instring, "&version=");
      if (p) {
         p +=9;
         p1 = strstr(p, "&");
         if (p1)
            *p1 = '\0';
         strcpy(buffer, p);
         if (p1)
            *p1 = '&';
         mg_connections[chndle]->p_server->version = (int) strtol(buffer, NULL, 10);
      }
   }

mg_test_connection_exit:

   phase = 14;

   mg_buf_free(&request);

   phase = 15;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_test_connection: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_check_connections()
{
   int n;

   mg_mutex_lock((MUTOBJ *) core_data.p_memlockSA);
   for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {
      if (mg_connections[n]) {
         mg_dbc_test(n, 1, 1);
      }
   }
   mg_mutex_unlock((MUTOBJ *) core_data.p_memlockSA);

   return 0;
}


int mg_get_log_level(int chndle)
{

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return (core_data.verbose);

   if (!mg_connections[chndle]->p_server)
      return (core_data.verbose);

   return (mg_connections[chndle]->p_server->verbose);
}



/* Get actual values for server configuration */

int mg_get_server_index(char *m_server_name, int shndle)
{
   int result;

#ifdef _WIN32
__try {
#endif

   result = 0;
   m_server_name[0] = '\0';
   if (shndle >= 0 && shndle < MG_MAX_SERVERS) {
      if (mg_servers[shndle] != NULL) {
         T_STRCPY(m_server_name, mg_servers[shndle]->m_server_name);
         result = 1;
      }
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_get_server_index: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Store actual values for server configuration */

int mg_set_server_index(int *p_shndle, char *m_server_name)
{
   int n, len, nfree, error_code, mem_locked, repair, new_mem, slot_found;
   char buffer[32];
   void *phdsys = NULL;

   mem_locked = 0;

#ifdef _WIN32
__try {
#endif

   error_code = 0;
   nfree = -1;

   if (*p_shndle == -1)
      repair = 0;
   else
      repair = 1;

   slot_found = 1;
   if (repair == 0) {
      slot_found = 0;
      for (n = 0; n < MG_MAX_SERVERS; n ++) {
         if (mg_servers[n] != NULL) {
            if (T_STRCMP(mg_servers[n]->m_server_name, m_server_name) == 0) {
               *p_shndle = n;
               slot_found = 1;
               break;
            }
         }
      }
      if (slot_found) {
         return error_code;
      }
   }


   /* Synchronise the allocation of system block handles */

   mem_locked = mg_mutex_lock((MUTOBJ *) core_data.p_memlockPA);

   if (mem_locked == 1) {
      slot_found = 1;
      if (repair == 0) {
         slot_found = 0;
         for (n = 0; n < MG_MAX_SERVERS; n ++) {
            if (mg_servers[n] == NULL) {
               if (nfree == -1)
                  nfree = n;
            }
            else {
               if (T_STRCMP(mg_servers[n]->m_server_name, m_server_name) == 0) {
                  *p_shndle = n;
                  slot_found = 1;
                  break;
               }
            }
         }
      }
      else {
         nfree = *p_shndle;
      }

      if (repair == 1 || (slot_found == 0 && nfree >= 0)) {

         new_mem = 0;
         if (mg_servers[nfree] == NULL) {
            new_mem = 1;

            len = mg_get_config(m_server_name, MG_P_IP_ADDRESS, "", buffer, 31, core_data.mg_ini);
            if (len == 0) {

               mg_mutex_unlock((MUTOBJ *) core_data.p_memlockPA);

               return -9;
            }


            phdsys = (LPMGSERVER) mg_malloc(sizeof(MGSERVER) + 32, "mg_set_server_index:1");

            if (phdsys == NULL) {
               if (core_data.log_errors == 1)
                  mg_log_event("UNIX is out of memory", "System Control Block Allocation Error");
               error_code = -113;
            }
            else {
               mg_servers[nfree] = phdsys;
            }

         }
         if (error_code == 0) {
            *p_shndle = nfree;
            if (new_mem == 1) {
               T_STRCPY(mg_servers[nfree]->m_server_name, m_server_name);
               mg_servers[nfree]->version = 0;
               mg_system_parameters(nfree);
               T_STRCPY(mg_servers[nfree]->uci, mg_servers[nfree]->default_user_uci);
            }
         }
      }

      mg_mutex_unlock((MUTOBJ *) core_data.p_memlockPA);

   }
   else {
      error_code = -112;
   }

   if (*p_shndle < 0) {
      if (core_data.verbose == 1) {
         T_SPRINTF(buffer, "Serialization Error Code: %d", error_code);
         mg_log_event(buffer, "System Table Management Error");
      }
      error_code = -111;
   }

   return error_code;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {

      if (mem_locked)
         mg_mutex_unlock((MUTOBJ *) core_data.p_memlockPA);

      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_set_server_index: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_get_alt_servers(char *m_server_name)
{
   int n, sn, althndle, nfree, error_code;
   LPMGALTSERVER phdsys;

#ifdef _WIN32
__try {
#endif

   althndle = -1;
   nfree = -1;
   error_code = 0;

   if (!strlen(m_server_name))
      return -1;

   for (n = 0; n < MG_MAX_SERVERS; n ++) {
      if (mg_alt_servers[n]) {
         if (!strcmp(mg_alt_servers[n]->default_server, m_server_name)) {
            althndle = n;
            break;
         }
      }
      else {
         if (nfree == -1)
            nfree = n;
      }
   }

   if (althndle != -1)
      return althndle;

   if (nfree == -1)
      return nfree;

   phdsys = (LPMGALTSERVER) mg_malloc(sizeof(MGALTSERVER) + 32, "mg_get_alt_servers:1");

   if (phdsys == NULL) {
      if (core_data.log_errors == 1)
         mg_log_event("Server is out of memory", "System Alternatives Control Block Allocation Error");
      error_code = -113;
   }
   else {
      mg_alt_servers[nfree] = phdsys;
   }

   if (error_code != 0)
      return error_code;

   althndle = nfree;
   mg_alt_servers[althndle]->alt_ptr = 0;

   for (sn = 0; sn < MG_MAX_ALT; sn ++) {
      mg_alt_servers[althndle]->alt[sn] = NULL;
   }

   mg_set_alt_servers(althndle, m_server_name);

   return althndle;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_get_alt_servers: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_set_alt_servers(int althndle, char *m_server_name)
{
   int sn, len, alt;
   char buffer[256], name[64];

#ifdef _WIN32
__try {
#endif

   T_STRCPY(mg_alt_servers[althndle]->default_server, m_server_name);

   for (sn = 0; sn < MG_MAX_ALT; sn ++) {

      if (mg_alt_servers[althndle]->alt[sn]) {
         mg_free((void *) mg_alt_servers[althndle]->alt[sn], "mg_set_alt_servers:1");
         mg_alt_servers[althndle]->alt[sn] = NULL;
      }
      mg_alt_servers[althndle]->alt[sn] = NULL;
   }

   sn = 0;
   mg_alt_servers[althndle]->alt[sn] = (LPDALT) mg_malloc(sizeof(DALT), "mg_set_alt_servers:1");
   mg_alt_servers[althndle]->alt[sn]->status = 1;
   strcpy(mg_alt_servers[althndle]->alt[sn]->server, mg_alt_servers[althndle]->default_server);

   len = mg_get_config(m_server_name, "Alternative_Servers", "Disabled", buffer, 32, core_data.mg_ini);
   if (!strcmp(buffer, "LoadBalancing"))
      mg_alt_servers[althndle]->alt_status = 1;
   else if (!strcmp(buffer, "FailOver"))
      mg_alt_servers[althndle]->alt_status = 2;
   else
      mg_alt_servers[althndle]->alt_status = 0;

   alt = 0;
   for (sn = 1; sn < MG_MAX_ALT; sn ++) {
      T_SPRINTF(name, "Alternative_Server_%d", sn);
      len = mg_get_config(m_server_name, name, "", buffer, 64, core_data.mg_ini);
      if (!len)
         break;

      mg_alt_servers[althndle]->alt[sn] = (LPDALT) mg_malloc(sizeof(DALT), "mg_set_alt_servers:2");

      if (buffer[0] == '1') {
         mg_alt_servers[althndle]->alt[sn]->status = 1;
         alt = 1;
      }
      else
         mg_alt_servers[althndle]->alt[sn]->status = 0;

      mg_piece(buffer, buffer, "~", 7, 999);

      strcpy(mg_alt_servers[althndle]->alt[sn]->server, buffer);

   }

   if (!alt)
      mg_alt_servers[althndle]->alt_status = 0;

   return althndle;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_set_alt_servers: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_next_alt_server(int althndle, int sn_start)
{
   int sn, sn_next;

   if (althndle < 0 || althndle >= MG_MAX_SERVERS || mg_alt_servers[althndle] == NULL)
      return -1;

   sn_next = -1;
   sn = sn_start + 1;

   for (;;) {
      if (sn >= MG_MAX_ALT || !mg_alt_servers[althndle]->alt[sn]) {
         sn_next = 0;
         break;
      }
      if (mg_alt_servers[althndle]->alt[sn]->status == 1) {
         sn_next = sn;
         break;
      }
      sn ++;
   }

   return sn_next;
}


int mg_encode_password(char *hash, char *password, short type)
{
   short no_dpapi;
   int n;
   long rem, sum;
   unsigned long crc;
   char c1, c2, c3;
   char userinfo[2048], code[256], code1[40], code2[40], code3[40], ascii[8192], n36str[10], buffer[8192], sha[128], b64[128];

#ifdef _WIN32
__try {
#endif

   no_dpapi = 0;
   *sha = '\0';
   *b64 = '\0';

   if (type == 9) {
      T_STRCPY(hash, password);
      return 1;
   }


   if (type == 2) {

#if defined(MG_CRYPT32)

      if (core_data.no_dpapi)
         no_dpapi = core_data.no_dpapi;

      if (!no_dpapi) {

         DATA_BLOB DataIn, DataOut;
         BYTE *pbDataInput;
         unsigned long cbDataInput;
         BOOL truth;

         mg_load_crypt32(1);
         if (!mg_crypt32.crypt32) {
            no_dpapi = 2;
         }
         else {

            if (!strncmp(password, "|2|", 3)) {
               T_STRCPY(hash, password);
               return 1;
            }

            cbDataInput = (unsigned long) T_STRLEN(password) + 1;
            pbDataInput = (BYTE *) password;

            DataIn.pbData = pbDataInput;    
            DataIn.cbData = cbDataInput;

            if (core_data.os_version < MG_WINVER_XP)
               truth = mg_CryptProtectData(&DataIn, L"The MGateway SIG", NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE|CRYPTPROTECT_UI_FORBIDDEN, &DataOut);
            else
               truth = mg_CryptProtectData(&DataIn, NULL, NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE|CRYPTPROTECT_UI_FORBIDDEN, &DataOut);

            if (truth == TRUE) {

               if (DataOut.cbData > 4096) {

                  mg_log_event("Encrypted password too long", "Windows Encryption Error");

                  mg_local_free(DataOut.pbData);

                  T_STRCPY(hash, password);
                  return 1;
               }


               n = mg_b64_encode((char *) DataOut.pbData, ascii, (int) DataOut.cbData, 0);

               ascii[n] = '\0';
               mg_local_free(DataOut.pbData);

               T_STRCPY(hash, "|2|");
               T_STRCPY(hash + 3, ascii);

            }
            else {

               unsigned long error_code;
               char errs[256];

               error_code = GetLastError();
               mg_get_error_message(error_code, errs, 250, 0);

               T_SPRINTF(buffer, "Unable to encrypt password - code: %d - %s", error_code, errs);
               mg_log_event(buffer, "Windows Encryption Error");

               no_dpapi = 3;
            }
         }

         if (!no_dpapi)
            return 1;
      }

#else

      no_dpapi = 1;

#endif

      if (no_dpapi) {

         if (!strncmp(password, "|2|", 3)) {
            T_STRCPY(hash, password);
            return 1;
         }

         n = mg_b64_encode((char *) password, ascii, (int) strlen(password), 0);

         ascii[n] = '\0';

         T_STRCPY(hash, "|2|");
         T_STRCPY(hash + 3, ascii);
         return 1;
      }
   }



   if (type == 0) {
      T_STRCPY(code, "LN3@MQPCKM9DHZ7AES5BT2XFR4YGU6W/V8+J");

      T_STRCPY(code1, "lasiweoritnsuatisjdflsaibaplxhdtudns");
      T_STRCPY(code2, "945t90rgopqwoi8493202jfnvdwosq0w4ijd");
      T_STRCPY(code3, "prui9akfjsdoqw8lsfkcmvnhsfkgcmslskfd");


      T_STRCPY(userinfo, password);

      n36str[1] = '\0';

      T_STRCPY(buffer, userinfo);
      T_STRCAT(buffer, code);
      crc = mg_crc32(buffer, T_STRLEN(buffer));
      sprintf(buffer, "%08lx", crc);
      n36str[0] = buffer[0];
      rem = strtol(n36str, NULL, 16);
      for (n = 0; buffer[n]; n ++) {
         n36str[0] = buffer[n];
         sum = strtol(n36str, NULL, 16) + rem;
         buffer[n] = code1[sum];
      }
      T_STRCPY(ascii, buffer);

      T_STRCPY(buffer, code);
      T_STRCAT(buffer, userinfo);
      T_STRCAT(buffer, ascii);
      crc = mg_crc32(buffer, T_STRLEN(buffer));
      sprintf(buffer, "%08lx", crc);
      n36str[0] = buffer[0];
      rem = strtol(n36str, NULL, 16);
      for (n = 0; buffer[n]; n ++) {
         n36str[0] = buffer[n];
         sum = strtol(n36str, NULL, 16) + rem;
         buffer[n] = code2[sum];
      }
      T_STRCAT(ascii, buffer);

      T_STRCPY(buffer, code);
      T_STRCAT(buffer, ascii);
      T_STRCAT(buffer, userinfo);
      crc = mg_crc32(buffer, T_STRLEN(buffer));
      sprintf(buffer, "%08lx", crc);
      n36str[0] = buffer[0];
      rem = strtol(n36str, NULL, 16);
      for (n = 0; buffer[n]; n ++) {
         n36str[0] = buffer[n];
         sum = strtol(n36str, NULL, 16) + rem;
         buffer[n] = code3[sum];
      }
      T_STRCAT(ascii, buffer);

      for (n = 0; n < 7; n += 2) {
         c1 = ascii[n];
         c2 = ascii[n + 8];
         c3 = ascii[n + 16];

         ascii[n] = c3;
         ascii[n + 8] = c1;
         ascii[n + 16] = c2;

      }
   }
   else {
      T_STRCPY(ascii, password);
   }

#ifdef MG_SHA

   mg_sha1((unsigned char *) sha, (const unsigned char *) ascii, T_STRLEN(ascii));
   sha[20] = '\0';

   n = mg_b64_encode(sha, b64, 20, 0);

   b64[n] = '\0';

   if (type == 0) {
      T_STRCPY(hash, "1");
      T_STRCAT(hash, b64);
      hash[28] = '\0';
   }
   else {
      T_STRCPY(hash, b64);
   }

#else

   if (type == 0) {
      T_STRCPY(hash, "0");
      T_STRCAT(hash, ascii);
   }
   else {
      T_STRCPY(hash, password);
   }

#endif

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_encode_password: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_decode_password(char *hash, char *password, short type)
{
   short no_dpapi;
   int n, len;
   char ascii[8192], buffer[8192], code[256];

#ifdef _WIN32
__try {
#endif

   no_dpapi = 0;
   n = 0;
   len = 0;
   *ascii = '\0';
   *buffer = '\0';
   *code = '\0';

   if (type == 2) {

#if defined(MG_CRYPT32)

      if (core_data.no_dpapi)
         no_dpapi = core_data.no_dpapi;

      if (!no_dpapi) {

         DATA_BLOB DataIn, DataOut;
         BYTE *pbDataInput;
         unsigned long cbDataInput;
         BOOL truth;
         LPWSTR pszDataDescr = NULL;

         mg_load_crypt32(1);
         if (!mg_crypt32.crypt32) {
            no_dpapi = 2;
         }
         else {

            if (strncmp(hash, "|2|", 3)) {
               T_STRCPY(password, hash);
               return 1;
            }

            len = mg_b64_decode((char *) hash + 3, ascii, (int) T_STRLEN(hash) - 3);

            cbDataInput = len;
            pbDataInput = (BYTE *) ascii;

            DataIn.pbData = pbDataInput;
            DataIn.cbData = cbDataInput;

            if (core_data.os_version < MG_WINVER_XP)
               truth = mg_CryptUnprotectData(&DataIn, &pszDataDescr, NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE|CRYPTPROTECT_UI_FORBIDDEN, &DataOut);
            else
               truth = mg_CryptUnprotectData(&DataIn, &pszDataDescr, NULL, NULL, NULL, CRYPTPROTECT_LOCAL_MACHINE|CRYPTPROTECT_UI_FORBIDDEN, &DataOut);

            if (truth == TRUE) {
               T_STRCPY(password, DataOut.pbData);
               mg_local_free(DataOut.pbData);
            }
            else {

               unsigned long error_code;
               char errs[256];

               error_code = GetLastError();
               mg_get_error_message(error_code, errs, 250, 0);

               T_SPRINTF(buffer, "Unable to decrypt password - code: %d - %s", error_code, errs);
               mg_log_event(buffer, "Windows Decryption Error");

               no_dpapi = 3;

            }

         }
         if (!no_dpapi)
            return 1;
      }

#else

      no_dpapi = 1;

#endif

      if (no_dpapi) {

         if (strncmp(hash, "|2|", 3)) {
            T_STRCPY(password, hash);
            return 1;
         }

         len = mg_b64_decode((char *) hash + 3, ascii, (int) T_STRLEN(hash) - 3);
         if (len < 0) {
            no_dpapi = 7;
            len = 0;
         }
         else {
            ascii[len] = '\0';
            T_STRCPY(password, ascii);
         }
      }
   }

   if (no_dpapi) {
      T_STRCPY(password, hash);
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_decode_password: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Open New TCP/IP Connection */

int mg_tcp_open_connection(int *p_chndle, int *p_shndle, char *m_server_name, int timeout, int no_retry, int context)
{
   short phase;
   int n, error_code, mem_locked, sem_locked, net_error;
   int i, restart, slotfound, chndle, first_empty_slot;
   int minSession1, maxSessions1, s_count;
   time_t time_now;
   char message_buffer[256];
   void *phdbc = NULL;

   mem_locked = 0;
   sem_locked = 0;
   phase = 0;

#ifdef _WIN32
__try {
#endif

   chndle = -1;
   *p_chndle = chndle;
   sem_locked = 0;

   /* Handle to system definition block */

   if (*p_shndle > -1) {
      if (mg_get_server_index(m_server_name, *p_shndle) == 0) {
         return -9;
      }
   }

   if (T_STRLEN(m_server_name) == 0) {
      strcpy(m_server_name, core_data.default_service);
      if (T_STRLEN(m_server_name) == 0) {
         return -9;
      }
   }

   if (*p_shndle < 0) {
      error_code = mg_set_server_index(p_shndle, m_server_name);
      if (error_code < 0) {
         return error_code;
      }
   }

   mem_locked = mg_mutex_lock((MUTOBJ *) core_data.p_memlockSA);
   sem_locked = 1;

   phase = 0;
   if (mem_locked == 1 ) {
      phase = 2;
      minSession1 = 0;
      maxSessions1 = core_data.max_connections;
      maxSessions1 = MG_MAX_CONNECTIONS;

      time_now = time(NULL);
      s_count = 0;
      restart = 0;
      slotfound = 0;
      error_code = 0;
      first_empty_slot = -1;

      for (i = minSession1; i < maxSessions1; i ++) {
         if (first_empty_slot < 0 && mg_connections[i] == NULL) {
            first_empty_slot = i;
         }
         if (mg_connections[i] != NULL && mg_connections[i]->shndle == *p_shndle) {
            if (mg_connections[i]->status != 2)
               s_count ++;
            if (mg_connections[i]->status == 0) {

               short ok;

               ok = 1;

               if (mg_connections[i]->t_prev != 0 && mg_connections[i]->p_server && mg_connections[i]->p_server->idle_timeout) {
                  time_t time_now, time_diff;

                  time_now = time(NULL);
                  time_diff = (time_t) difftime(time_now, mg_connections[i]->t_prev);
                  if (time_diff > (mg_connections[i]->p_server->idle_timeout - 2))
                     ok = 0;
               }

               if (ok) {
                  mg_connections[i]->status = 1;
                  chndle = i;
                  slotfound = 1;
                  restart = 1;
                  break;
               }
            }
         }
      }

      phase = 5;

      if (slotfound == 0 && s_count >= mg_servers[*p_shndle]->max_server_connections) {
         if (mg_servers[*p_shndle]->log_errors == 1) {
            T_SPRINTF(message_buffer, "Maximum number of connections running for server %s; s_count=%d; max_server_connections=%d", mg_servers[*p_shndle]->m_server_name, s_count, mg_servers[*p_shndle]->max_server_connections);
            mg_log_event(message_buffer, "Connection Error");
         }
         error_code = -8;
      }
      else {
         if (slotfound == 0 && first_empty_slot > -1) {
            chndle = first_empty_slot;
            slotfound = 1;
         }
         if (slotfound == 0 || chndle < 0) {
            if (mg_servers[*p_shndle]->log_errors == 1) {
               T_SPRINTF(message_buffer, "Maximum number of connections running for server %s; max_connections=%d", mg_servers[*p_shndle]->m_server_name, core_data.max_connections);
               mg_log_event(message_buffer, "Connection Error");
               mg_log_event("Maximum number of connections running", "Connection Error");
            }
            error_code = -8;
         }
      }

      /* Claim the socket */
      phase = 10;
      if (error_code > -1 && chndle > -1) {

         /* Get memory for a brand new connection */

         if (mg_connections[chndle] == NULL) {

            phdbc = (LPMGCONNECTION) mg_malloc(sizeof(MGCONNECTION) + 32, "mg_tcp_open_connection:1");

            if (phdbc == NULL) {
               if (mg_servers[*p_shndle]->log_errors == 1)
                  mg_log_event("Server is out of memory", "Connection Error");
               error_code = -13;
            }
            else {
               mg_connections[chndle] = phdbc;
               mg_connections[chndle]->p_server = NULL;
            }

            if (mg_connections[chndle] != NULL) {
               mg_connections[chndle]->t_prev = 0;
               mg_connections[chndle]->close = 0;
               mg_connections[chndle]->cycle_no = 0;
               mg_connections[chndle]->binary = 0;
               mg_connections[chndle]->m_job_no[0] = '\0';
               mg_connections[chndle]->uci[0] = '\0';
               mg_connections[chndle]->tcp_port = mg_servers[*p_shndle]->tcp_port;

            }
         }
         if (mg_connections[chndle] != NULL) {
            mg_connections[chndle]->status = 1;
            mg_connections[chndle]->p_server = mg_servers[*p_shndle];
            mg_connections[chndle]->shndle = *p_shndle;
            mg_connections[chndle]->in_use = 0;
         }
      }
      if (error_code == 0) {
         mg_connections[chndle]->in_use = 1;
      }
   }
   else {
      error_code = -11;
   }

   phase = 30;
   if (sem_locked) {
      mg_mutex_unlock((MUTOBJ *) core_data.p_memlockSA);
      sem_locked = 0;
   }

   /* End of synchronisation code */

   phase = 40;
   if (error_code < 0) {
      mg_tcp_free_dbc_memory(chndle);
      return error_code;
   }


#if defined(MG_BDB)

   if (strstr(mg_servers[*p_shndle]->m_server_name, "BDB")) {
      int ret = 0;
      u_int32_t open_flags;

      if (!mg_servers[*p_shndle]->pdb) {
         mg_servers[*p_shndle]->dbtype = MG_DBTYPE_BDB;
         strcpy(mg_servers[*p_shndle]->file_name, "c:/mgsi/bin/mgsi.db");

         strcpy(mg_servers[*p_shndle]->database_type, "BDB");

mg_connections[chndle]->activity = 0L;

   ret = db_create((DB **) &(mg_servers[*p_shndle]->pdb), (DB_ENV *) NULL, (u_int32_t) 0);

    if (ret != 0) {

         *p_chndle = chndle;

      sprintf(message_buffer, "Database '%s' db_create failed: %d", mg_servers[*p_shndle]->file_name, ret);
      mg_log_event(message_buffer, "New BDB Connection");
   }

    /* Set the open flags */
    open_flags = DB_CREATE | DB_THREAD;    /* Allow database creation */

    /* Now open the database */
    ret = ((DB *) (mg_servers[*p_shndle]->pdb))->open((DB *) mg_servers[*p_shndle]->pdb,        /* Pointer to the database */
                    NULL,       /* Txn pointer */
                    mg_servers[*p_shndle]->file_name,  /* File name */
                    NULL,       /* Logical db name */
                    DB_BTREE,   /* Database type (using btree) */
                    open_flags, /* Open flags */
                    0);         /* File mode. Using defaults */
    if (ret != 0) {

         *p_chndle = chndle;

      sprintf(message_buffer, "Database '%s' open failed: %d", mg_servers[*p_shndle]->file_name, ret);
      mg_log_event(message_buffer, "New BDB Connection");
    }

      T_SPRINTF(message_buffer, "BDB Server %s; %x %d", mg_servers[*p_shndle]->m_server_name, mg_connections[chndle]->p_server, ret);
      mg_log_event(message_buffer, "New Connection");


      }

         *p_chndle = chndle;


      return 2;
   }
#endif

   if (restart == 1) {
      restart = 0;
      if (mg_dbc_test(chndle, 0, 1) != 0)
         restart = 1;

      if (restart == 0) {
         mg_tcp_close_connection(chndle, 1);
         return -999;
      }
      else {
         *p_chndle = chndle;
         return 2;
      }
   }

   phase = 50;
   if (mg_servers[*p_shndle]->verbose > 0) {
      T_SPRINTF(message_buffer, "Server %s. (%s:%d)", mg_servers[*p_shndle]->m_server_name, mg_servers[*p_shndle]->ip_address, mg_servers[*p_shndle]->tcp_port);
      mg_log_event(message_buffer, "New Connection");
   }

   mg_connections[chndle]->activity = 0L;

   n = mg_tcp_open_socket(chndle, *p_shndle, 1, timeout, no_retry, &net_error);
   phase = 60;
   if (n < 0) {
      return n;
   }

   *p_chndle = chndle;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {

      if (sem_locked)
         mg_mutex_unlock((MUTOBJ *) core_data.p_memlockSA);

      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_open_connection: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tcp_open_socket(int chndle, int shndle, int free_mem_on_error, int timeout, int no_retry, int *pnet_error)
{
   short physical_ip, ipv6, connected, getaddrinfo_ok, phase;
   int n, nc, errorno;
   unsigned long inetaddr;
   unsigned long spin_count;
   char ansi_ip_address[64];
   struct sockaddr_in srv_addr, cli_addr;
   struct hostent *hp;
   struct in_addr **pptr;
   char message_buffer[512];
   LPSERVENT srv_info = NULL;
   LPHOSTENT host_info = NULL;

   phase = 0;

#ifdef _WIN32
__try {
#endif

   *pnet_error = 0;
   connected = 0;
   getaddrinfo_ok = 0;
   spin_count = 0;

   ipv6 = core_data.ipv6;
#if !defined(MG_IPV6)
   ipv6 = 0;
#endif

   *message_buffer = '\0';
   T_STRCPY(ansi_ip_address, (char *) mg_servers[shndle]->ip_address);

   phase = 2;

#ifdef _WIN32

   n = core_data.wsastartup;
   if (n != 0) {
      if (mg_servers[shndle]->log_errors >= 1)
         mg_log_event("Unusable Winsock Library", "DLL Load Error");
      if (free_mem_on_error)
         mg_tcp_free_dbc_memory(chndle);

      return n;
   }

#endif /* #ifdef _WIN32 */

   phase = 3;

#if defined(MG_IPV6)

   if (ipv6) {
      short mode;
      struct addrinfo hints, *res;
      struct addrinfo *ai;
      char port_str[32];
	   int off = 0;
	   int ipv6_v6only = 27;

      res = NULL;
      T_SPRINTF(port_str, "%d", mg_connections[chndle]->tcp_port);
      connected = 0;

      for (mode = 0; mode < 3; mode ++) {

         phase = 31;

         if (res) {
            MGNET_FREEADDRINFO(res);
            res = NULL;
         }

         memset(&hints, 0, sizeof hints);
         hints.ai_family = AF_UNSPEC;     /* Use IPv4 or IPv6 */
         hints.ai_socktype = SOCK_STREAM;
         /* hints.ai_flags = AI_PASSIVE; */
         if (mode == 0)
            hints.ai_flags = AI_NUMERICHOST | AI_CANONNAME;
         else if (mode == 1)
            hints.ai_flags = AI_CANONNAME;
         else if (mode == 2) {
            /* Apparently an error can occur with AF_UNSPEC (See RJW1564) */
            /* This iteration will return IPV6 addresses if any */
            hints.ai_flags = AI_CANONNAME;
            hints.ai_family = AF_INET6;
         }
         else
            break;

         phase = 32;

         n = MGNET_GETADDRINFO(ansi_ip_address, port_str, &hints, &res);
         if (n != 0) {
            continue;
         }

         phase = 321;

         getaddrinfo_ok = 1;
         spin_count = 0;
         for (ai = res; ai != NULL; ai = ai->ai_next) {

            spin_count ++;
            if (spin_count > 10000) {
               mg_log_event("Possible infinite loop encountered while resetting the connection", "Diagnostic: mg_tcp_open_socket(): 1");
            }

            phase = 33;

	         if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6) {
               continue;
            }

	         /* Open a socket with the correct address family for this address. */
	         mg_connections[chndle]->cli_socket = MGNET_SOCKET(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            /* MGNET_BIND(mg_connections[chndle]->cli_socket, ai->ai_addr, (int) (ai->ai_addrlen)); */
            /* MGNET_CONNECT(mg_connections[chndle]->cli_socket, ai->ai_addr, (int) (ai->ai_addrlen)); */

            if (core_data.nagle_algorithm == 0) {

               int flag = 1;
               int result;

               result = MGNET_SETSOCKOPT(mg_connections[chndle]->cli_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &flag, sizeof(int));
               if (result < 0 && mg_servers[shndle]->log_errors >= 1) {
                  mg_log_event("Unable to disable the Nagle Algorithm", "Connection Error");
               }
/*
               else {
                  mg_log_event("Nagle Algorithm disabled", "Connection Information", 0);
               }
*/
            }

            phase = 34;

            n = mg_tcp_connect(mg_connections[chndle]->cli_socket, (xLPSOCKADDR) ai->ai_addr, (socklen_mg) (ai->ai_addrlen), timeout, no_retry);

            phase = 35;

            if (n == -2) {
               *pnet_error = n;
               n = -737;
               continue;
            }
            if (SOCK_ERROR(n)) {
               char message[256];

               errorno = (int) mg_get_last_error(0);
               mg_get_error_message(errorno, message, 250, 0);
               *pnet_error = errorno;
               T_SPRINTF(message_buffer, "Cannot Connect to Server (%s:%d): Error Code: %d (%s)", (char *) mg_servers[shndle]->ip_address, mg_servers[shndle]->tcp_port, errorno, message);
               n = -5;
               if (mg_servers[shndle]->log_errors >= 1)
                  mg_log_event(message_buffer, "Connection Error");

               mg_tcp_close_socket(chndle);
               continue;
            }
            else {
               connected = 1;
               break;
            }
         }
         if (connected)
            break;
      }
      if (res) {
         MGNET_FREEADDRINFO(res);
         res = NULL;
      }
   }
#endif

   phase = 4;

   if (ipv6) {
      if (connected) {
         return 1;
      }
      else {
         if (getaddrinfo_ok) {
            nc = mg_tcp_close_connection(chndle, free_mem_on_error);
            return -5;
         }
         else {
            char message[256];

            errorno = (int) mg_get_last_error(0);
            mg_get_error_message(errorno, message, 250, 0);
            T_SPRINTF(message_buffer, "Cannot identify Server: Error Code: %d (%s)", errorno, message);
            mg_log_event(message_buffer, "Connection Error");
            nc = mg_tcp_close_connection(chndle, free_mem_on_error);
            return -5;
         }
      }
   }

   phase = 5;

   ipv6 = 0;
   inetaddr = MGNET_INET_ADDR(ansi_ip_address);

   physical_ip = 0;
   if (isdigit(ansi_ip_address[0])) {
      char *p;

      if ((p = strstr(ansi_ip_address, "."))) {
         if (isdigit(*(++ p))) {
            if ((p = strstr(p, "."))) {
               if (isdigit(*(++ p))) {
                  if ((p = strstr(p, "."))) {
                     if (isdigit(*(++ p))) {
                        physical_ip = 1;
                     }
                  }
               }
            }
         }
      }
   }

   phase = 6;

   if (inetaddr == INADDR_NONE || !physical_ip) {

      phase = 7;

      if ((hp = MGNET_GETHOSTBYNAME((const char *) ansi_ip_address)) == NULL) {
         n = -2;
         if (mg_servers[shndle]->log_errors >= 1)
            mg_log_event("Invalid Host", "Connection Error");
         if (free_mem_on_error)
            mg_tcp_free_dbc_memory(chndle);

         return n;
      }

      pptr = (struct in_addr **) hp->h_addr_list;
      connected = 0;

      spin_count = 0;

      for (; *pptr != NULL; pptr ++) {

         spin_count ++;
         if (spin_count > 10000) {
            mg_log_event("Possible infinite loop encountered while resetting the connection", "Diagnostic: mg_tcp_open_socket(): 2");
         }

         mg_connections[chndle]->cli_socket = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);
         if (INVALID_SOCK(mg_connections[chndle]->cli_socket)) {
            char message[256];

            n = -2;
            if (mg_servers[shndle]->log_errors >= 1) {

               errorno = (int) mg_get_last_error(0);
               mg_get_error_message(errorno, message, 250, 0);
               T_SPRINTF(message_buffer, "Invalid Socket: Context=1: Error Code: %d (%s)", errorno, message);

               mg_log_event(message_buffer, "Connection Error");
            }

            break;
         }

#if !defined(_WIN32)
         bzero((char *) &cli_addr, sizeof(cli_addr));
         bzero((char *) &srv_addr, sizeof(srv_addr));
#endif

         cli_addr.sin_family = AF_INET;
            srv_addr.sin_port = MGNET_HTONS((unsigned short) mg_connections[chndle]->tcp_port);

         cli_addr.sin_addr.s_addr = MGNET_HTONL(INADDR_ANY);
         cli_addr.sin_port = MGNET_HTONS(0);

         n = MGNET_BIND(mg_connections[chndle]->cli_socket, (xLPSOCKADDR) &cli_addr, sizeof(cli_addr));
         if (SOCK_ERROR(n)) {
            char message[256];

            n = -3;
            if (mg_servers[shndle]->log_errors >= 1) {
               errorno = (int) mg_get_last_error(0);
               mg_get_error_message(errorno, message, 250, 0);
               T_SPRINTF(message_buffer, "Cannot bind to Socket: Error Code: %d (%s)", errorno, message);

               mg_log_event(message_buffer, "Connection Error");
            }

            break;
         }

         if (core_data.nagle_algorithm == 0) {

            int flag = 1;
            int result;

            result = MGNET_SETSOCKOPT(mg_connections[chndle]->cli_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &flag, sizeof(int));
            if (result < 0 && mg_servers[shndle]->log_errors >= 1) {
               mg_log_event("Unable to disable the Nagle Algorithm", "Connection Error");
            }
/*
            else {
               mg_log_event("Nagle Algorithm disabled", "Connection Information", 0);
            }
*/
         }

         srv_addr.sin_family = AF_INET;
         srv_addr.sin_port = MGNET_HTONS((unsigned short) mg_connections[chndle]->tcp_port);

         memcpy(&srv_addr.sin_addr, *pptr, sizeof(struct in_addr));

         n = mg_tcp_connect(mg_connections[chndle]->cli_socket, (xLPSOCKADDR) &srv_addr, sizeof(srv_addr), timeout, no_retry);
         if (n == -2) {
            *pnet_error = n;
            n = -737;
            continue;
         }

         if (SOCK_ERROR(n)) {
            char message[256];

            errorno = (int) mg_get_last_error(0);
            mg_get_error_message(errorno, message, 250, 0);

            *pnet_error = errorno;

            T_SPRINTF(message_buffer, "Cannot Connect to Server (%s:%d): Error Code: %d (%s)", (char *) mg_servers[shndle]->ip_address, mg_servers[shndle]->tcp_port, errorno, message);
            n = -5;
            if (mg_servers[shndle]->log_errors >= 1)
               mg_log_event(message_buffer, "Connection Error");

            mg_tcp_close_socket(chndle);
            continue;
         }
         else {
            connected = 1;
            break;
         }
      }
      if (!connected) {

         if (chndle >= 0 && chndle < MG_MAX_CONNECTIONS && mg_connections[chndle] && DBC_ALLOC(chndle))

            nc = mg_tcp_close_connection(chndle, free_mem_on_error);

         if (mg_servers[shndle]->log_errors >= 1)
            mg_log_event("Failed to find the Server via a DNS Lookup", "Connection Error");

         return n;
      }
   }
   else {

      mg_connections[chndle]->cli_socket = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);
      if (INVALID_SOCK(mg_connections[chndle]->cli_socket)) {
         char message[256];

         n = -2;
         if (mg_servers[shndle]->log_errors >= 1) {
            errorno = (int) mg_get_last_error(0);
            mg_get_error_message(errorno, message, 250, 0);
            T_SPRINTF(message_buffer, "Invalid Socket: Context=2: Error Code: %d (%s)", errorno, message);

            mg_log_event(message_buffer, "Connection Error");
         }

         if (free_mem_on_error)
            mg_tcp_free_dbc_memory(chndle);

         return n;
      }

#if !defined(_WIN32)
      bzero((char *) &cli_addr, sizeof(cli_addr));
      bzero((char *) &srv_addr, sizeof(srv_addr));
#endif

      cli_addr.sin_family = AF_INET;
      cli_addr.sin_addr.s_addr = MGNET_HTONL(INADDR_ANY);
      cli_addr.sin_port = MGNET_HTONS(0);

      n = MGNET_BIND(mg_connections[chndle]->cli_socket, (xLPSOCKADDR) &cli_addr, sizeof(cli_addr));

      if (SOCK_ERROR(n)) {
         char message[256];

         n = -3;
         if (mg_servers[shndle]->log_errors >= 1) {

            errorno = (int) mg_get_last_error(0);
            mg_get_error_message(errorno, message, 250, 0);

            T_SPRINTF(message_buffer, "Cannot bind to Socket: Error Code: %d (%s)", errorno, message);

            mg_log_event(message_buffer, "Connection Error");
         }
         if (chndle >= 0 && chndle < MG_MAX_CONNECTIONS && mg_connections[chndle] && DBC_ALLOC(chndle))
            nc = mg_tcp_close_connection(chndle, free_mem_on_error);

         return n;
      }

      if (core_data.nagle_algorithm == 0) {

         int flag = 1;
         int result;

         result = MGNET_SETSOCKOPT(mg_connections[chndle]->cli_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &flag, sizeof(int));
         if (result < 0 && mg_servers[shndle]->log_errors >= 1) {
            mg_log_event("Unable to disable the Nagle Algorithm", "Connection Error");
         }
/*
         else {
            mg_log_event("Nagle Algorithm disabled", "Connection Information", 0);
         }
*/
      }

      srv_addr.sin_port = MGNET_HTONS((unsigned short) mg_connections[chndle]->tcp_port);
      srv_addr.sin_family = AF_INET;
      srv_addr.sin_addr.s_addr = MGNET_INET_ADDR(ansi_ip_address);

      n = mg_tcp_connect(mg_connections[chndle]->cli_socket, (xLPSOCKADDR) &srv_addr, sizeof(srv_addr), timeout, no_retry);
      if (n == -2) {
         *pnet_error = n;
         n = -737;

         if (chndle >= 0 && chndle < MG_MAX_CONNECTIONS && mg_connections[chndle] && DBC_ALLOC(chndle))

            nc = mg_tcp_close_connection(chndle, free_mem_on_error);

         return n;
      }

      if (SOCK_ERROR(n)) {
         char message[256];

         errorno = (int) mg_get_last_error(0);
         mg_get_error_message(errorno, message, 250, 0);

         *pnet_error = errorno;

         T_SPRINTF(message_buffer, "Cannot Connect to Server (%s:%d): Error Code: %d (%s)", (char *) mg_servers[shndle]->ip_address, mg_servers[shndle]->tcp_port, errorno, message);
         n = -5;
         if (mg_servers[shndle]->log_errors >= 1)
            mg_log_event(message_buffer, "Connection Error");

         if (chndle >= 0 && chndle < MG_MAX_CONNECTIONS && mg_connections[chndle] && DBC_ALLOC(chndle))

            nc = mg_tcp_close_connection(chndle, free_mem_on_error);

         return n;
      }
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_open_socket: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tcp_connect(SOCKET sockfd, xLPSOCKADDR p_srv_addr, socklen_mg srv_addr_len, int timeout, int no_retry)
{
#ifdef _WIN32
   int n;
#else
   int flags, n, error;
   socklen_mg len;
   fd_set rset, wset;
   struct timeval tval;
   time_t time1, time2, time_now, time_diff;

#endif

#ifdef _WIN32
__try {
#endif

#if defined(SOLARIS) && BIT64PLAT
   timeout = 0;
#endif

   /* It seems that BIT64PLAT is set to 0 for 64-bit Solaris:  So, to be safe .... */

#if defined(SOLARIS)
   timeout = 0;
#endif

   if (timeout != 0) {

#if defined(_WIN32)

      n = MGNET_CONNECT(sockfd, (xLPSOCKADDR) p_srv_addr, (socklen_mg) srv_addr_len);

      return n;

#else
      flags = fcntl(sockfd, F_GETFL, 0);
      fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

      time1 = time(NULL);

      error = 0;

      n = MGNET_CONNECT(sockfd, (xLPSOCKADDR) p_srv_addr, (socklen_mg) srv_addr_len);

      if (n < 0) {

         if (errno != EINPROGRESS) {

#if defined(SOLARIS)

            if (errno != 2 && errno != 146) {

               char header[256];

               T_SPRINTF((char *) header, "Solaris: Initial Connection Error errno=%d; EINPROGRESS=%d", errno, EINPROGRESS);
               mg_log_event((char *) header, "Diagnostic");

               return -1;

            }
#else
            return -1;
#endif

         }
      }

      if (n != 0) {

         FD_ZERO(&rset);
         FD_SET(sockfd, &rset);

         wset = rset;
         tval.tv_sec = timeout;
         tval.tv_usec = timeout;

         n = MGNET_SELECT((int) (sockfd + 1), &rset, &wset, NULL, &tval);

         time2 = time(NULL);
         time_diff = (time_t) difftime(time2, time1);

         if (n == 0) {
            close(sockfd);
            errno = ETIMEDOUT;
            return (-2);
         }
         if (MGNET_FD_ISSET(sockfd, &rset) || MGNET_FD_ISSET(sockfd, &wset)) {

            len = sizeof(error);
            if (MGNET_GETSOCKOPT(sockfd, SOL_SOCKET, SO_ERROR, (void *) &error, &len) < 0) {

               char header[256];

               T_SPRINTF((char *) header, "Solaris: Pending Error %d", errno);
               mg_log_event((char *) header, "Diagnostic");

               return (-1);   /* Solaris pending error */
            }
         }
         else {
            ;
         }
      }

      fcntl(sockfd, F_SETFL, flags);      /* Restore file status flags */

      if (error) {
         close(sockfd);
         errno = error;
         return (-1);
      }

      return 1;

#endif

   }
   else {

      n = MGNET_CONNECT(sockfd, (xLPSOCKADDR) p_srv_addr, (socklen_mg) srv_addr_len);

      return n;
   }

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_connect: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Close TCP/IP Connection */

int mg_tcp_close_connection(int chndle, int free_mem)
{
   int n;

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   if (mg_connections[chndle]->p_server->dbtype == MG_DBTYPE_M)
      n = mg_tcp_close_socket(chndle);

#if defined(MG_BDB)
   else if (mg_connections[chndle]->p_server->dbtype == MG_DBTYPE_BDB) {
      int ret;

      if (mg_connections[chndle]->p_server->pdb) {
         ret = ((DB *) (mg_connections[chndle]->p_server->pdb))->close((DB *) mg_connections[chndle]->p_server->pdb, 0);

         if (ret != 0) {
               mg_log_event("Vendor database close failed:", "cmcmcm");
         }
         else
            mg_log_event("DBD closeed", "cmcmcm");

         mg_connections[chndle]->p_server->pdb = NULL;
      }
return 0;
   }
#endif

   if (free_mem)
      mg_tcp_free_dbc_memory(chndle);

   return 0;
}


int mg_tcp_close_socket(int chndle)
{
   int n;

   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   if (mg_connections[chndle]->cli_socket) {
#if defined(_WIN32)
      n = MGNET_CLOSESOCKET(mg_connections[chndle]->cli_socket);
 #else
      n = close(mg_connections[chndle]->cli_socket);
#endif
   }

   return 0;
}


/* Send a buffer to host */

int mg_tcp_write(int chndle, void *datastring, unsigned long size)
{
   int n = 0, errorno = 0, char_sent = 0;
   unsigned long total;
   char errormessage[256];
   char *p_datastring;

#ifdef _WIN32
__try {
#endif

   p_datastring = datastring;

   if (mg_connections[chndle] == NULL || mg_connections[chndle]->status == 0) {
      if (mg_connections[chndle]->p_server->log_errors == 1)
         T_SPRINTF(errormessage, "Socket is Closed on server %s/$Job:%s", mg_connections[chndle]->p_server->m_server_name, mg_connections[chndle]->m_job_no);
      return -1;
   }

   total = 0;
   for (;;) {
      n = MGNET_SEND(mg_connections[chndle]->cli_socket, (xLPSENDBUF) (p_datastring + total), size - total, 0);
      if (SOCK_ERROR(n)) {

#if !defined(_WIN32)
         errorno = errno;
#else
         errorno = MGNET_WSAGETLASTERROR();
#endif

         if (NOT_BLOCKING(errorno) && errorno != 0) {
            if (mg_connections[chndle]->p_server->log_errors == 1) {
               T_SPRINTF(errormessage, "Cannot Write Data on server %s/$Job:%s: Error Code: %d", mg_connections[chndle]->p_server->m_server_name, mg_connections[chndle]->m_job_no, errorno);
               mg_log_event(errormessage, "TCP/IP Write Error");
            }
            char_sent = -1;
            break;
         }
      }
      else {
         total += n;
         if (total == size) {
            break;
         }
      }
   }

   if (char_sent < 0)
      return char_sent;
   else
      return size;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_write: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}

#endif

}


/* Read a buffer from host */

int mg_tcp_read(int chndle, void *datastring, int length, int m_timeout)
{
   int n, errorno, length_read, read_error;
   char errormessage[256];
   char *p_datastring;
   fd_set rset, eset;
   struct timeval tval;

#ifdef _WIN32
__try {
#endif

   p_datastring = datastring;
   read_error = 0;
   n = 0;
   errorno = 0;
   length_read = 0;

   p_datastring[0] = '\0';

   if (mg_connections[chndle] == NULL || mg_connections[chndle]->status == 0) {
      if (mg_connections[chndle]->p_server->log_errors == 1) {
         T_SPRINTF(errormessage, "Socket is Closed on server %s/$Job:%s", mg_connections[chndle]->p_server->m_server_name, mg_connections[chndle]->m_job_no);
         mg_log_event(errormessage, "TCP/IP Read Error");
      }
      return -1;
   }


   if (m_timeout) {
      FD_ZERO(&rset);
      FD_ZERO(&eset);
      FD_SET(mg_connections[chndle]->cli_socket, &rset);
      FD_SET(mg_connections[chndle]->cli_socket, &eset);

      tval.tv_sec = m_timeout / 1000;
      tval.tv_usec = m_timeout % 1000;

      n = MGNET_SELECT((int) mg_connections[chndle]->cli_socket + 1, &rset, NULL, &eset, &tval);

      if (n == 0) {
         if (m_timeout && length && m_timeout > 100 && mg_servers[mg_connections[chndle]->shndle]->log_errors == 1) {
            mg_log_event("Cannot read from server within timeout period", "Connection Error");
         }
         return MG_READ_TIMEOUT;
      }
      if (n < 0 || !MGNET_FD_ISSET(mg_connections[chndle]->cli_socket, &rset)) {
         return -1;
      }
   }

   if (length == 0) {
      return 0;
   }

   n = MGNET_RECV(mg_connections[chndle]->cli_socket, (char FAR *) p_datastring, length, 0);

   if (n == 0) {
      return -1;
   }

   if (SOCK_ERROR(n)) {
#if !defined(_WIN32)
      errorno = errno;
#else
      errorno = MGNET_WSAGETLASTERROR();
#endif
      if (errorno != 0 && NOT_BLOCKING(errorno)) {
         if (mg_connections[chndle]->p_server->log_errors == 1) {
            T_SPRINTF(errormessage, "Socket Error Number %d Reported on port %d (server %s/Job:%s)", errorno, mg_connections[chndle]->tcp_port, mg_connections[chndle]->p_server->m_server_name, mg_connections[chndle]->m_job_no);
            mg_log_event(errormessage, "TCP/IP Read Error");
         }
         read_error = -1;
      }
   }

   if (n > 0) {
      p_datastring[n] = '\0';
      length_read = n;
   }
   else {
      p_datastring[0] = '\0';
      length_read = 0;
   }

   if (read_error < 0) {
      return read_error;
   }
   else {
      return length_read;
   }

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_read: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tcp_free_dbc_memory(int chndle)
{

#ifdef _WIN32
__try {
#endif


   if (chndle < 0 || chndle >= MG_MAX_CONNECTIONS || mg_connections[chndle] == NULL)
      return 0;

   mg_free((void *) mg_connections[chndle], "mg_tcp_free_dbc_memory:1");

   mg_connections[chndle] = NULL;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_free_dbc_memory: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tcp_free_sys_memory(int shndle)
{
   int n;

#ifdef _WIN32
__try {
#endif

   if (shndle < 0 || shndle >= MG_MAX_SERVERS || mg_servers[shndle] == NULL)
      return 0;

   mg_free((void *) mg_servers[shndle], "mg_tcp_free_sys_memory:1");

   for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {
      if (mg_connections[n] != NULL && mg_connections[n]->p_server == mg_servers[shndle])
         mg_connections[n]->p_server = NULL;
   }
   mg_servers[shndle] = NULL;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tcp_free_sys_memory: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}

