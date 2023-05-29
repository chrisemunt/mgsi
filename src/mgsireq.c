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
#include "mgsireq.h"
#include "mgsidso.h"
#include "mgsiadm.h"
#include "mgsidba.h"

static char *mg_file_types[] = {"htm", "html", "shtml", "txt", "asc", "text",
                                "txt", "jpg", "jpeg", "jpe", "gif", "class",
                                "asp", "ocx", "wav", "ram",
                                "doc", "rtf", "rtx", "sgml" "sgm",
                                "xml", "mpeg", "mpg", "mpe",
                                "qt", "mov", "avi", "movie", NULL};

static char *mg_cgi_vars[] = {"AUTH_PASSWORD", "AUTH_TYPE", "REMOTE_ADDR",
                              "REMOTE_HOST", "REMOTE_USER",
                              "HTTP_USER_AGENT", "SERVER_PROTOCOL",
                              "SERVER_SOFTWARE", "SERVER_NAME",
                              "GATEWAY_INTERFACE", "SERVER_PORT",
                              "REMOTE_IDENT", "HTTP_ACCEPT", "HTTP_REFERER",
                              "PATH_TRANSLATED", "CONTENT_TYPE",
                              "HTTP_AUTHORIZATION", "HTTP_COOKIE",
                              "REQUEST_METHOD", "HTTP_SOAPACTION", NULL};


int mg_php(MGREQ *p_request, int mode)
{
   short t_mode, eod, lpn, offs, term_sent, recv_called, mem_alloc, m_ibm_mq, m_php, m_go, m_jsp, m_aspx, m_python, m_ruby, m_perl, m_apache, m_cgi;
   int closedown, attempt_no;
   int n, chndle, shndle, read_stat, errorcode, timeout, no_retry;
   int context;
   unsigned long len, max, actual_size, tbuffer_size, rsize, clen, rlen, req_no;
   char m_server_name[64], uci[256], cli_ip_address[32];
   char buffer[256], obuffer[256], m_client_name[32];
   char cmnd;
   char *p, *pz, *p_content;
   int size, hlen, hlen1, tbuffer_offset, tbuffer_actual_offset, tbuffer_offset_base;
   unsigned char *ibuffer;
   unsigned char *tbuffer;
   char header[256];

   MEMOBJ debug, *p_debug;

   int phase;

   phase = 0;
   clen = 0;
   rlen = 0;
   req_no = 0;
   timeout = 0;
   no_retry = 0;
   *m_client_name = '\0';

   m_ibm_mq = 0, m_php = 0, m_go = 0, m_jsp = 0, m_aspx = 0, m_python = 0, m_ruby = 0, m_perl = 0, m_apache = 0, m_cgi = 0;

   term_sent = 0;
   mem_alloc = 0;
   tbuffer = NULL;

   p_debug = &debug;

#ifdef _WIN32
__try {
#endif

   mg_buf_init(p_debug, 4096, 2048);
   mg_buf_cpy_ex(p_debug, "", 0);
 
   *cli_ip_address = '\0';

   p_request->request_source = MG_RS_HTTP;

   chndle = -1;
   shndle = -1;

   offs = 0;

   if (mode) {
      tbuffer_offset_base = p_request->roffset;
      tbuffer_offset = p_request->roffset;

      tbuffer_size = 8192;
      tbuffer = mg_malloc(sizeof(char) * (tbuffer_size + 32), "mg_php:1");

      ibuffer = (unsigned char *) p_request->request + tbuffer_offset;

      mem_alloc = 1;
   }
   else {
      tbuffer_offset_base = 128;
      tbuffer_offset = tbuffer_offset_base;

      tbuffer_size = MG_CACHE_BUFFER;
/* CM0052 test
      tbuffer_size = 1000000;
*/
      tbuffer = mg_malloc(sizeof(char) * (tbuffer_size + 32), "mg_php:2");

      ibuffer = tbuffer + tbuffer_offset;
      strcpy((char *) ibuffer, p_request->request_heading);

      mem_alloc = 1;
   }

mg_php_next_request:

   term_sent = 0;
   req_no ++;

   len = (int) strlen((char *) ibuffer);

   offs = 0;
   eod = 0;
   lpn = 0;
   max = 0;
   hlen = 0;
   hlen1 = 0;

   p_request->stream = 0;
   errorcode = 0;
   strcpy(header, "");

   mg_buf_cpy_ex(p_debug, "", 0);

   if (mode) {
      p = strstr((char *) ibuffer, "\n");
      eod = 1;
      errorcode = len;
   }
   else {
      p = strstr((char *) ibuffer, "\n");
      if (!p) {

         p_request->stream = 1;
         max = tbuffer_size - tbuffer_offset;

         for (;;) {

            size = MGNET_RECV(p_request->cli_sockfd, ibuffer + len, max - len, 0);
            errorcode = size;

            if (errorcode < 1) {
               break;
            }

            len += size;
            ibuffer[len] = '\0';

            p = strstr((char *) ibuffer, "\n");
            if (p) {
               break;
            }
            if (len == max)
               break;

         }
      }
   }
   ibuffer[len] = '\0';

   if (errorcode < 0) {
      goto mg_php_exit2;
   }

   if (errorcode == 0) {
      goto mg_php_exit2;
   }

   if (!p) {
      mg_log_event(ibuffer, "Invalid Request");
      goto mg_php_exit2;
   }

   if (!strncmp(ibuffer, "^A^", 3) || !strncmp(ibuffer, "^S^", 3)) { /* Are you there */
      int len, total;
      unsigned char ssize[32];

      strcpy(buffer, "GP0000000000");
      
      strcpy(header, "00000sc\n");
      len = mg_encode_size(ssize, 12, MG_CHUNK_SIZE_BASE);
      strncpy((char *) (header + (5 - len)), ssize, len);
      strcpy(header + MG_RECV_HEAD, buffer);
      total = MG_RECV_HEAD + 12;

      len = MGNET_SEND(p_request->cli_sockfd, header, total, 0);

      goto mg_php_exit1;

   }

   *p = '\0';
   p_content = (p + 1);
   hlen = (int) strlen((char *) ibuffer) + 1;
   *p = '\n';
   rlen = len - hlen;

   clen = mg_decode_size(ibuffer + (hlen - 6), 5, MG_CHUNK_SIZE_BASE);

   if ((clen + tbuffer_offset + 32) > tbuffer_size) { /* CM0052 */
      unsigned long temp_size;
      unsigned char * temp;

      temp = tbuffer;
      temp_size = tbuffer_size;

      tbuffer_size = clen + tbuffer_offset + 32; /* CM0052 */
      tbuffer = (unsigned char *) mg_malloc(sizeof(char) * (tbuffer_size + 32), "mg_php:2A");

      memcpy((void *) tbuffer, (void *) temp, (size_t) temp_size); 
      ibuffer = tbuffer + tbuffer_offset;
      max = tbuffer_size - tbuffer_offset;

      p = strstr((char *) ibuffer, "\n");
      p_content = (p + 1);

      free((void *) temp);
   }


   if (len > clen) {
      rlen = clen;
   }

   if (rlen < clen) { /* CM0052 && max > (len + 1)) { */
      int lenx;
      unsigned long get, got;

      get = clen - rlen;
/* CM0052
      if ((max - len) > (get - 1))
         get = max - 1;
*/
      got = 0;
      for (;;) {

         if ((get - got) > 8192)
            lenx = 8192;
         else
            lenx = (get - got);

         size = MGNET_RECV(p_request->cli_sockfd, ibuffer + (len + got), lenx, 0);

         errorcode = size;

         if (errorcode < 1)
            break;

         got += size;
         ibuffer[len + got] = '\0';

         if (got == get) {
            break;
         }
      }

      if (errorcode < 0) {
         goto mg_php_exit2;
      }

      if (errorcode == 0) {
         goto mg_php_exit2;
      }
      len += got;
      rlen += got;
   }

   if (rlen == clen) {
      eod = 1;
   }

/*
   PHPp^P^##0###1.6.37#0^S^00026\x0a\x018^MGCust\x0111\x0210Chris Munt 
   PHPp^P^##0###1.6.37#0^S^00026\x0a\x0a\x018^MGCust\x0111\x0210Chris Munt 
   PHPp^P^LOCAL#USER##0###1.6.37#0^S^00026\x0a\x0a\x018^MGCust\x0111\x0210Chris Munt 
*/

   if (ibuffer[3] == 'z') {
      m_php = 1;
      strcpy(m_client_name, "m_php");
   }
   else if (ibuffer[3] == 'g') {
      m_go = 1;
      strcpy(m_client_name, "m_go");
   }
   else if (ibuffer[3] == 'j') {
      m_jsp = 1;
      strcpy(m_client_name, "m_jsp");
   }
   else if (ibuffer[3] == 'a') {
      m_aspx = 1;
      strcpy(m_client_name, "m_aspx");
   }
   else if (ibuffer[3] == 'p') {
      m_python = 1;
      strcpy(m_client_name, "m_python");
   }
   else if (ibuffer[3] == 'r') {
      m_ruby = 1;
      strcpy(m_client_name, "m_ruby");
   }
   else if (ibuffer[3] == 'l') {
      m_perl = 1;
      strcpy(m_client_name, "m_perl");
   }
   else if (ibuffer[3] == 'h') {
      m_apache = 1;
      strcpy(m_client_name, "m_apache");
   }
   else if (ibuffer[3] == 'c') {
      m_cgi = 1;
      strcpy(m_client_name, "m_cgi");
   }
   else if (ibuffer[3] == 'w') {
      m_ibm_mq = 1;
      strcpy(m_client_name, "m_ibm_mq");
   }
   else {
      m_php = 1;
      strcpy(m_client_name, "m_client");
   }

#if 1
   if (m_php) {
      p = (char *) ibuffer;
      for (n = 0; ; n ++) {
         pz = strstr(p, "#");
         if (pz)
            *pz = '\0';
         if (n == 3)
            timeout = (int) strtol(p, NULL, 10);
         else if (n == 4)
            no_retry = (int) strtol(p, NULL, 10);
         if (pz)
            *pz = '#';
         else
            break;
         p = (pz + 1);
      } 
   }
#endif

   strcpy(m_server_name, core_data.default_service);
   lpn = 0;
   strcpy(buffer, "");
   if (hlen) {

      strncpy(header, (char *) ibuffer, hlen);
      header[hlen] = '\0';
      if (hlen > 1 && header[hlen - 1] == '\n')
         header[hlen - 1] = '\0';
         
      ibuffer = (unsigned char *) p_content;
      tbuffer_offset += hlen;

      for (n = hlen - 1; n >= 0; n --) {
         if (header[n] == '^') {
            cmnd = header[n - 1];
            break;
         }
      }

      strcpy(buffer, header + (7 + 0));
      p = strstr(buffer, "#");
      if (p)
         *p = '\0';
      if (strlen(buffer)) {
         strcpy(m_server_name, buffer);
         lpn = 1;
      }

      if (clen && rlen) {
         short hbyref, htype;
         int hsize, hl, lpn;

         hl = mg_decode_item_header((unsigned char *) ibuffer, &hsize, &hbyref, &htype);
         if (hsize < 128) {
            strncpy(buffer, ibuffer + hl, hsize);
            buffer[hsize] = '\0';
            lpn = 0;
            if (cmnd == 'x' || cmnd == 'y') {
               char obuffer[64];
               lpn = mg_get_config("SYSTEM_INDEX", buffer, "", obuffer, 64, core_data.mg_ini); 
            }
            else {

               if (strstr(buffer, "^") || strstr(buffer, "$") || strstr(buffer, "#") || strstr(buffer, ".") || strstr(buffer, "&") || strstr(buffer, MG_AK))
                  lpn = 0;
               else
                  lpn = 1;
            }
            if (lpn) {
               strcpy(m_server_name, buffer);

               for (n = 0; n < hsize; n ++) {
                  *(ibuffer + hl + n) = '^';
               }
            }
         }
      }
   }

   strcpy(uci, "");
   closedown = 0;
   context = MG_OS_POOL_CONNECTION;
   phase = 1;

   attempt_no = 0;

mg_php_next_attempt:

   if ((errorcode = mg_get_connection(&chndle, &shndle, (char *) m_server_name, timeout, no_retry, context)) < 1) {

      T_SPRINTF(obuffer, "%s MGateway SIG:ERROR: can't connect to the Server: Error Code# %d", MG_RECV_HEAD_ERROR, errorcode);
      mg_insert_error_size(obuffer);

      if (core_data.log_errors) {
         char title[64];

         T_SPRINTF(title, "%s Error", m_client_name);
         mg_log_event(obuffer + MG_RECV_HEAD, title);
      }

      if (mode)
         strcpy(p_request->response, obuffer);
      else
         MGNET_SEND(p_request->cli_sockfd, obuffer, (int) strlen(obuffer), 0);

      goto mg_php_exit1;

   }

   phase = 2;


#if defined(MG_BDB)
   if (mg_connections[chndle]->p_server->dbtype == MG_DBTYPE_BDB) {
      mg_bdb(p_request, mode);
      term_sent = 1;
      goto mg_phpExit0;
   }
#endif

   phase = 3;

/*
   before
   PHPh^P^LOCAL#USER#0###2.0.43#0^W^000Y7
   after
   PHPh^P^LOCAL#USERUSER#0###2.0.43#0^W^000Y7 
*/


   strcpy(buffer, header + 7);
   strcpy(header + 7, m_server_name);
   p = strstr(buffer, "#");
   if (*(p + 1) == '#') {
      strcat(header, "#");
      strcat(header, mg_servers[shndle]->uci);
      p ++;
   }
   strcat(header, p);
   strcat(header, "\n");

/*
   p = strstr(header, "^P^");
   if (p) {
      p += 3;
      strcpy(buffer, p);
      *p = '\0';
      mg_piece(obuffer, buffer, "#", 7, 999);
      sprintf(buffer, "%s#%s#%d####%s", m_server_name, mg_servers[shndle]->uci, offs, obuffer);
      strcat(header, buffer);
   }
*/

   hlen1 = (int) strlen(header);

   if (hlen1 < tbuffer_offset) {
      ibuffer -= hlen1;
      tbuffer_offset -= hlen1;
      len = hlen1 + rlen;
      strncpy((char *) ibuffer, header, hlen1);
      errorcode = 1;
   }
   else {
      errorcode = mg_server_write_buffer(chndle, header, hlen1, tbuffer_offset);
      if (core_data.verbose) {
         mg_buf_cat_ex(p_debug, header, hlen1);
      }
   }
   
   size = 0;
   recv_called = 0;

   if (errorcode > 0) {

      max = tbuffer_size - tbuffer_offset;

      for (;;) {

         errorcode = mg_server_write_buffer(chndle, (char *) ibuffer, len, tbuffer_offset);

         if (core_data.verbose) {
            mg_buf_cat_ex(p_debug, ibuffer, len);
         }
         if (errorcode == 0)
            break;

         if (eod)
            break;

         len = 0;
         for (;;) {
            size = MGNET_RECV(p_request->cli_sockfd, (char *) ibuffer + len, max - len, 0);
            if (size < 1)
               break;
            recv_called = 1;

            len += size;
            rlen += size;
            ibuffer[len] = '\0';


            if (rlen == clen) {
               eod = 1;
               break;
            }
/*
            if (ibuffer[len - 1] == deod[0]) {
               eod = 1;
               break;
            }
*/
            if (len == max)
               break;
         }
         if (size < 1)
            break;

      }

   }

   if (size < 0) {

      /* Client went away */

      mg_release_connection(chndle, MG_CS_CLOSE);

      goto mg_php_exit2;

   }

   if (errorcode == 0) {

      /* Can't send request to M */

      mg_tcp_close_connection(chndle, 1);

      if (!recv_called && attempt_no < 5) {
         attempt_no ++;
         goto mg_php_next_attempt;
      }

      T_SPRINTF(obuffer, "%sMGateway SIG:ERROR: can't dispatch request: %s Not Responding", MG_RECV_HEAD_ERROR, MG_DB_SERVER_TITLE);
      mg_insert_error_size(obuffer);

      if (core_data.log_errors) {
         char title[64];

         T_SPRINTF(title, "%s Error", m_client_name);
         mg_log_event(obuffer + MG_RECV_HEAD, title);
      }

      if (mode)
         strcpy(p_request->response, obuffer);
      else
         MGNET_SEND(p_request->cli_sockfd, obuffer, (int) strlen(obuffer), 0);

   }
   else {

      phase = 31;

      if (core_data.verbose) {
         char title[128];
/*
         sprintf(title, "MGSI successfully SENT the following message to the SERVER (%d Bytes)", rlen);
         mg_log_buffer((char *) ibuffer, rlen, title);
*/
         sprintf(title, "MGateway SIG successfully SENT the following message to the SERVER (%ld Bytes)", p_debug->curr_size);
         mg_log_buffer((char *) p_debug->p_buffer, p_debug->curr_size, title);
      }

      phase = 311;

      mg_connections[chndle]->activity ++;

      phase = 312;
      errorcode = 1;
      phase = 313;

      if (mode) {
         strcpy(p_request->response, "");
      }

      phase = 32; 
      rsize = 0;
      t_mode = 0;

      mg_buf_cpy_ex(p_debug, "", 0);

      tbuffer_offset = 0;
      tbuffer_actual_offset = tbuffer_offset;

      while ((read_stat = mg_server_read(chndle, (char *) tbuffer + tbuffer_offset, tbuffer_size - tbuffer_offset, &actual_size, timeout, 0)) == MG_READ_OK) {

         if (actual_size == 0)
            continue;

         tbuffer_actual_offset = tbuffer_offset;
         t_mode = mg_connections[chndle]->t_mode;
         rsize += actual_size;

         if (core_data.verbose) {
            char title[128];
            sprintf(title, "MGateway SIG successfully RECEIVED the following data from the SERVER (%ld Bytes)", actual_size);
            mg_log_buffer((char *) tbuffer + tbuffer_offset, actual_size, title);
         }

         phase = 33;

         if (mode) {

            if (rsize < p_request->rsize) {
               strcat(p_request->response, (char *) tbuffer + tbuffer_offset);
            }
            else {
               char *temp;
               temp = p_request->response;
               p_request->response = mg_malloc(sizeof(char) * (p_request->rsize + MG_CACHE_BUFFER), "mg_php:3");
               p_request->rsize += MG_CACHE_BUFFER;
               strcpy(p_request->response, temp);
               strcat(p_request->response, (char *) tbuffer + tbuffer_offset);

               mg_free((void *) temp, "mg_php:1");
            }
         }
         else {

            if (t_mode == 1) {
               unsigned long data_size;
               char ssize[32], header[32];

               if (tbuffer_offset == 0)
                  actual_size -= MG_RECV_HEAD;

               tbuffer_offset = MG_RECV_HEAD;

               strcpy(header, "00000sc\n");
               data_size = actual_size;
               mg_encode_size((unsigned char *) ssize, actual_size, MG_CHUNK_SIZE_BASE);
               len = (unsigned long) strlen(ssize);
               strncpy((char *) (header + (5 - len)), ssize, len);

               tbuffer_actual_offset = 0;

               strncpy((char *) (tbuffer + tbuffer_actual_offset), header, MG_RECV_HEAD);
               actual_size += MG_RECV_HEAD;
               tbuffer[tbuffer_actual_offset + actual_size] = '\0';
               phase = 331;
            }

            len = 0;

            for (;;) {
               n = MGNET_SEND(p_request->cli_sockfd, (char *) tbuffer + (tbuffer_actual_offset + len), actual_size - len, 0);

               if (n < 0) {
                  errorcode = 0;

                  if (core_data.verbose) {
                     char title[128];
                     sprintf(title, "MGateway SIG could not SEND the following data to the CLIENT (%ld Bytes)", actual_size);
                     mg_log_buffer((char *) tbuffer + tbuffer_actual_offset, actual_size, title);
                  }
                  break;
               }
               else {
/*
                  if (core_data.verbose) {
                     char title[128];
                     sprintf(title, "MGateway SIG successfully SENT the following data to the CLIENT (%d Bytes)", n);
                     mg_log_buffer((char *) tbuffer + (tbuffer_actual_offset + len), n, title);
                  }
*/
                  len += n;
                  if (len == actual_size)
                     break;
               }
            }
            if (n < 0) {
               break;
            }
            else {
               if (core_data.verbose) {
                  mg_buf_cat_ex(p_debug, tbuffer + tbuffer_actual_offset, actual_size);
               }
            }
         }
      }

      if (t_mode == 1 && errorcode == 1) {
         strcpy(tbuffer, "00000sc\n");
         actual_size = 8;
         len = 0;

         n = MGNET_SEND(p_request->cli_sockfd, tbuffer + len, actual_size - len, 0);
         if (n < 0)
            errorcode = 0;

         if (core_data.verbose) {
            mg_buf_cat_ex(p_debug, tbuffer, actual_size);
         }
      }

      phase = 34;

      /* Client went away */

      if (errorcode == 0) { /* client must have snapped connection shut */
         mg_release_connection(chndle, MG_CS_CLOSE);
         goto mg_php_exit2;
      }
      else if (read_stat == MG_READ_ERROR || read_stat == MG_READ_TIMEOUT) {
         T_SPRINTF(obuffer, "%sMGateway SIG:ERROR: Read Error: errorcode=%d; read_stat=%d; data=%s", MG_RECV_HEAD_ERROR, errorcode, read_stat, (char *) tbuffer);
         mg_insert_error_size(obuffer);

         if (core_data.log_errors) {
            char title[64];

            T_SPRINTF(title, "%s Error", m_client_name);
            mg_log_event(obuffer + MG_RECV_HEAD, title);
         }

         if (mode)
            strcat(p_request->response, obuffer);
         else {
            MGNET_SEND(p_request->cli_sockfd, obuffer, (int) strlen(obuffer), 0);

            if (core_data.verbose) {
               mg_buf_cat_ex(p_debug, obuffer, (int) strlen(obuffer));
            }
         }

      }
      else {
         if (core_data.verbose) {
            char title[128];
            sprintf(title, "MGateway SIG successfully SENT the following data to the CLIENT (%ld Bytes)", p_debug->curr_size);
            mg_log_buffer((char *) p_debug->p_buffer, p_debug->curr_size, title);
         }
      }
      phase = 35;
   }

#if defined(MG_BDB)
mg_php_exit0:
#endif

   phase = 4;

   context = MG_CS_RELEASE_TO_POOL;

   mg_release_connection(chndle, context);

   if (m_php)
      core_data.php_activity ++;
   else if (m_go)
      core_data.go_activity ++;
   else if (m_jsp)
      core_data.jsp_activity ++;
   else if (m_aspx)
      core_data.aspx_activity ++;
   else if (m_python)
      core_data.python_activity ++;
   else if (m_ruby)
      core_data.ruby_activity ++;
   else if (m_perl)
      core_data.perl_activity ++;
   else if (m_apache) {
      core_data.apache_activity ++;
      core_data.http_activity ++;
   }
   else if (m_cgi) {
      core_data.cgi_activity ++;
      core_data.http_activity ++;
   }


mg_php_exit1:

   if (!mode) {
      chndle = -1;
      shndle = -1;

      tbuffer_offset = tbuffer_offset_base;
      ibuffer = tbuffer + tbuffer_offset;
      strcpy((char *) ibuffer, "");
      goto mg_php_next_request;
   }

mg_php_exit2:

   if (mem_alloc && tbuffer) {
      mg_free((void *) tbuffer, "mg_php:2");
      tbuffer = NULL;
   }

   mg_buf_free(p_debug);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {

      if (mem_alloc && tbuffer) {
         mg_free((void *) tbuffer, "mg_php:3");
         tbuffer = NULL;
      }

      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_php: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");

   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

  
}


int mg_php_direct(char *request, int smode, char **response, unsigned long *size, int *offset, int rmode)
{
   int result;
   MGREQ ecb;
   LPMGREQ p_request;

   result = 0;

#ifdef _WIN32
__try {
#endif

   p_request = &ecb;

   p_request->request_source = MG_RS_PHP;

   p_request->request = request;
   p_request->response = *response;
   p_request->rsize = *size;
   p_request->roffset = *offset;

   mg_php(p_request, 1);

   *size = p_request->rsize;
   *response = p_request->response;

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_php_direct: %x", code);
      mg_log_event(buffer, "Error Condition");

   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}

#endif

}


/*
int mg_encode_size(unsigned char *esize, int size, short base);
int mg_decode_size(unsigned char *esize, int len, short base);
int mg_encode_item_header(unsigned char * head, int size, short byref, short type);
int mg_decode_item_header(unsigned char * head, int * size, short * byref, short * type);

#define MG_TX_DATA           0
#define MG_TX_AKEY           1
#define MG_TX_AREC           2
#define MG_TX_EOD            3
#define MG_TX_AREC_FORMATTED 9
*/

int mg_php_decode_message(unsigned char * buffer, char cmnd)
{
   short byref, type;
   int clen, rlen, size, hlen;
   unsigned char head[8];
   unsigned char *p;
   char header[32], buf[256];
#ifdef _WIN32
__try {
#endif

   p = buffer;
   *head = '\0';

   strncpy(header, p, 9);
   header[8] = '\0';
   p += 8;

   clen = (int) strtol(header, NULL, 10);

   sprintf(buf, "Diagnostic: Header: clen=%d; cmnd=%c", clen, cmnd);
   mg_log_buffer(header, 8, buf);

   if (header[5] != 'c' || header[6] != 'c')
      return 1;

   rlen = 0;
   for (;;) {
      hlen = mg_decode_item_header(p, &size, &byref, &type);
      p += hlen;
      rlen += hlen;

      sprintf(buf, "Diagnostic: Head: clen=%d; rlen=%d; size=%d; byref=%d; type=%d;", clen, rlen, size, byref, type);
      mg_log_buffer(p, size, buf);

      if (type == MG_TX_AREC) {
         for (;;) {
            hlen = mg_decode_item_header(p, &size, &byref, &type);
            p += hlen;
            rlen += hlen;

            sprintf(buf, "Diagnostic: Head ARRAY: clen=%d; rlen=%d; size=%d; byref=%d; type=%d;", clen, rlen, size, byref, type);
            mg_log_buffer(p, size, buf);

            if (type == MG_TX_EOD) {
               break;
            }
            p += size;
            rlen += size;  
         }
      }
      else {
         p += size;
         rlen += size;
      }

      if (rlen >= clen)
         break;
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_php_decode_message: %x", code);
      mg_log_event(buffer, "Error Condition");

   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}

#endif
}


int mg_httpd(MGREQ *p_request)
{
   short request_alloc;
   int n, n1, errorcode, len, ln, clen, ok, size, ptr;
   char cbuffer[4096], ibuffer[4096], buffer1[4096], buffer2[4096];
   char script_name[128], wwwroot[128], default_file[64];
   LPNVLIST p_cgi;
   MEMOBJ request;

   request_alloc = 0;

#ifdef _WIN32
__try {
#endif

   p_request->request_source = MG_RS_HTTP;

   p_request->p_key_list = NULL;
   p_request->p_cgi_list = NULL;

   core_data.http_activity ++;
   core_data.sm_activity ++;

   strcpy(script_name, "");

   strcpy(wwwroot, "/mgsi/wwwroot");

   strcpy(default_file, "default.htm");

   strcpy(p_request->path_translated, "");
   strcpy(p_request->file_type, "");

   mg_buf_init(&request, 4096, 2048);
   request_alloc = 1;

   sprintf(ibuffer, "SERVER_PROTOCOL=HTTP/1.1\rGATEWAY_INTERFACE=CGI/1.1\rREMOTE_ADDR=%s\rREMOTE_HOST=%s\rSERVER_PORT=%d\r", p_request->cli_addr, p_request->cli_addr, p_request->tcp_port);
   mg_buf_cpy(&request, ibuffer);

   strcpy(ibuffer, p_request->request_heading);
   len = (int) strlen(ibuffer);

   ln = 0;
   clen = 0;

   for (;;) {
      size = MGNET_RECV(p_request->cli_sockfd, cbuffer, 1, 0);
      errorcode = size;

      if (errorcode < 1) {
         mg_log_event("Cannot read HTTP request", "Read Error");
         break;
      }

      ptr = 0;

      if (cbuffer[ptr] == '\x0a') {
         ibuffer[len] = '\0';
         ln ++;

         if (len == 0)
            break;

         if (ln == 1) {
            mg_piece(buffer1, ibuffer, " ", 1, 1);
            sprintf(buffer2, "REQUEST_METHOD=%s\r", buffer1);
            mg_buf_cat(&request, buffer2);
            mg_piece(ibuffer, ibuffer, " ", 2, 2);
            mg_piece(buffer1, ibuffer, "?", 1, 1);
            sprintf(buffer2, "SCRIPT_NAME=%s\r",buffer1);

            strcpy(script_name, buffer1);

            if (!strlen(script_name) || !strcmp(script_name, "/"))
               sprintf(script_name, "/%s", default_file);

            mg_buf_cat(&request, buffer2);
            mg_piece(buffer1, ibuffer, "?", 2, 999);
            if (strlen(buffer1)) {
               sprintf(buffer2, "QUERY_STRING=%s\r", buffer1);
               mg_buf_cat(&request, buffer2);
            }
         }
         else {
            mg_piece(buffer1, ibuffer, ": ", 1, 1);
            mg_piece(buffer2, ibuffer, ": ", 2, 999);
            mg_ucase(buffer1);

            for (n1 = 0; buffer1[n1] != '\0'; n1 ++ ) {
               if (buffer1[n1] == '-')
                  buffer1[n1] = '_';
            }
            if (!strcmp(buffer1, "HOST")) {
               mg_piece(buffer2, buffer2, ":", 1, 1);
               sprintf(ibuffer, "SERVER_NAME=%s\r", buffer2);
               mg_buf_cat(&request, ibuffer);
            }
            else if (!strcmp(buffer1, "CONTENT_LENGTH"))
               clen = (int) strtol(buffer2, NULL, 10);
            else if (!strcmp(buffer1, "USER_AGENT")) {
               sprintf(ibuffer, "HTTP_USER_AGENT=%s\r", buffer2);
               mg_buf_cat(&request, ibuffer);
            }
            else if (!strcmp(buffer1, "ACCEPT")) {
               sprintf(ibuffer, "HTTP_ACCEPT=%s\r", buffer2);
               mg_buf_cat(&request, ibuffer);
            }
            else if (!strcmp(buffer1, "COOKIE")) {
               sprintf(ibuffer, "HTTP_COOKIE=%s\r", buffer2);
               mg_buf_cat(&request, ibuffer);
            }
            else if (!strcmp(buffer1, "REFERER")) {
               sprintf(ibuffer, "HTTP_REFERER=%s\r", buffer2);
               mg_buf_cat(&request, ibuffer);
            }
            else {
               sprintf(ibuffer, "%s=%s\r", buffer1, buffer2);
               mg_buf_cat(&request, ibuffer);
            }
         }
         len = 0;
      }
      else if (cbuffer[ptr] != '\x0d') {
         ibuffer[len] = cbuffer[ptr];
         if (len < 4090)
            len ++;
     }
   }

   sprintf(ibuffer, "CONTENT_LENGTH=%d\r", clen);
   mg_buf_cat(&request, ibuffer);

#ifdef _WIN32
   n = _getdrive();
   sprintf(p_request->path_translated, "%c:%s%s", n + 'A' - 1, wwwroot, script_name);
   for (n = 0; p_request->path_translated[n] != '\0'; n ++) {
      if (p_request->path_translated[n] == '/')
         p_request->path_translated[n] = '\\';
   }
#else
   sprintf(p_request->path_translated, "%s%s", wwwroot, script_name);

#endif

   sprintf(ibuffer, "PATH_TRANSLATED=%s\r", p_request->path_translated);
   mg_buf_cat(&request, ibuffer);
   len = (int) strlen(p_request->path_translated);
   ok = 0;
   for (n = len; n > 0; n --) {
      if (p_request->path_translated[n] == '.') {
         ok = 1;
         break;
      }
   }
   if (ok) {
      for (n ++, n1 = 0;n < len && p_request->path_translated[n] != '\0' ; n ++)
         p_request->file_type[n1 ++] = p_request->path_translated[n];
      p_request->file_type[n1] = '\0';
      mg_lcase(p_request->file_type);
   }
   else
      strcpy(p_request->file_type, "");

   mg_buf_cat(&request, "SERVER_SOFTWARE=MG-Service-Integration-Gateway/");
   mg_buf_cat(&request, MG_VERSION);
   mg_buf_cat(&request, "\r");

   p_request->iBuffer = request.p_buffer;
   p_request->iBuffer_size = (int) strlen(p_request->iBuffer);

   mg_http(p_request);

   while (p_request->p_cgi_list) {
      p_cgi = p_request->p_cgi_list;
      p_request->p_cgi_list = p_request->p_cgi_list->p_next;
      mg_free((void *) p_cgi, "mg_httpd:1");
   }

   mg_buf_free(&request);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   if (request_alloc)
      mg_buf_free(&request);

   MGNET_CLOSESOCKET(p_request->cli_sockfd);

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_httpd: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 1;
}
#endif

}


int mg_http(MGREQ *p_request)
{
   short ok, must_use_sm, real_file;
   char m_server_name[32], m_server_name_req[32], cli_ip_address[64];
   char keyname[64], uci[256], method[64];
   char obuffer[256], closedown_service[32], form_id[16];
   char uni_buffer[256] = "\0";
   char token[256];
   char cbuffer[32000];
   unsigned char buffer[1024];
   char *p_wk_buffer, *p_wk_buffer1, *p_keyvalue, *p_databuffer, *p_wk_databuffer, *p_general;
   int nx, key_no, con_stat, attempt_no;
   int len, chndle, shndle, closedown, errorcode;
   int header_inc, no_keys;
   int context, failover, sn, sn_base, althndle, smform;
   unsigned int n = 0, n1 = 0;
   unsigned long header_size, line_no, total_max_size, clen;
   unsigned long databuffer_size, databuffer_actual_size = 0;
   HNVLIST p_key_list, p_key;
   SMDATA smdata;
   LPSMDATA p_smdata;
   MEMOBJ cgievar, trans_buffer;
   LPMEMOBJ p_cgievar, p_trans_buffer;
   FILE *fp;

#ifdef _WIN32
__try {
#endif

   must_use_sm = 0;

   p_request->cli_gone = 0;
   p_request->key_no = 0;
   p_request->clen = 0;
   p_request->rlen = 0;
   p_request->stream = 0;
   p_request->s_timeout = 0;
   p_cgievar = &cgievar;
   p_trans_buffer = &trans_buffer;
   p_wk_buffer = NULL;
   p_wk_buffer1 = NULL;
   p_general = NULL;

   p_key_list = NULL;

   p_smdata = &smdata;
   p_smdata->service_status = 1;
   p_smdata->s_buffer = NULL;
   p_smdata->button[0] = '\0';
   p_smdata->config_section[0] = '\0';
   p_smdata->new_dbserver[0] = '\0';
   p_smdata->mq_service[0] = '\0';
   p_smdata->new_mq_service[0] = '\0';
   p_smdata->pw1[0] = '\0'; /* CM0051 */
   p_smdata->pw2[0] = '\0';

   chndle = -1;
   shndle = -1;

   con_stat = 0;
   errorcode = 0;
   closedown = 0;
   no_keys = 0;
   key_no = 0;
   context = 0;
   *m_server_name = '\0';
   *m_server_name_req = '\0';
   *uci = '\0';
   *form_id = '\0';
   *cli_ip_address = '\0';
   *token = '\0';
   *obuffer = '\0';

   strcpy(method, p_request->request_heading);
   for (n = 0; method[n]; n ++) {
      if (method[n] == ' ') {
         method[n] = '\0';
         break;
      }
   }
   mg_ucase(method);

   if (!strcmp(method, "EXTCWSMQ")) {
      mg_client_write_string(p_request, "MGateway Service Integration Gateway/1.1\r\nIBM IBM MQ LoopBack");
      goto mg_http_exit2;

   }

   real_file = 0;
   if (strlen(p_request->file_type)) {
      for (n = 0; mg_file_types[n]; n ++) {
         if (!strcmp(mg_file_types[n], p_request->file_type)) {
            real_file = 1;
            break;
         }
      }
   }
   if (real_file) {
      fp = fopen(p_request->path_translated, "rb");
      if (!fp) {
         mg_client_write_string(p_request, "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nConnection: close\r\n\r\n<html>\r\n<head><title>Error</title></head>\r\n<body>\r\n<h2>MGateway SIG: Object does not exist</h2>\r\n</body>\r\n</html>\r\n");
         goto mg_http_exit2;
      }

      while ((n = (int) fread(buffer, sizeof(unsigned char), 1000, fp))) {
         mg_client_write_buffer(p_request, buffer, n);
      }

      fclose(fp);
      goto mg_http_exit2;
   }

   mg_buf_init(p_cgievar, 512, 512);

   p_request->multipart = 0;
   p_request->urlencoded = 1;

   p_key_list = mg_get_key_list(p_request);

   if (p_request->cli_gone) {
      mg_buf_free(p_cgievar);
      goto mg_http_exit2;
   }

   if (mg_get_server_variable(p_request, "SCRIPT_NAME",p_cgievar)) {
      T_STRCPY(core_data.mg_lib_path, p_cgievar->p_buffer);
   }

   mg_get_server_variable(p_request, "CONTENT_LENGTH", p_cgievar);
   clen = strtol(p_cgievar->p_buffer, NULL, 10);

   /* No keys: SIG Management */

   if (!p_key_list && !clen && p_request->urlencoded) {

      mg_get_server_variable(p_request, "REQUEST_METHOD", p_cgievar);
      if (strcmp(p_cgievar->p_buffer, "POST")) {

         strcpy(form_id, "0");
         n = mg_sysman(p_request, form_id, m_server_name, p_smdata);

         mg_buf_free(p_cgievar);

         goto mg_http_exit2;

      }
   }

   mg_get_server_variable(p_request, "HTTP_REFERER", p_cgievar);
   mg_get_server_variable(p_request, "HTTP_COOKIE", p_cgievar);

   total_max_size = clen;
   total_max_size += mg_get_server_variable(p_request, "QUERY_STRING", p_cgievar);

   total_max_size += (4 * 20) + 1024 + p_cgievar->size;

   if (total_max_size < 8192)
      total_max_size = 8192;


   /* Multi-part MIME */

   if (total_max_size > 8000)
      total_max_size = 8000;

   if (!mg_buf_init(p_trans_buffer, total_max_size, 1024)) {
      n = mg_return_message(p_request, 1, "SIG Management", "Not enough memory to service the request", m_server_name);
      mg_free_key_list(p_request, (HNVLIST) p_key_list);
      goto mg_http_exit1;
   }

   mg_buf_cpy(p_trans_buffer, "^H^0^0\r");

   p_key = p_key_list;

   for (key_no = 0; key_no < p_request->key_no; key_no ++) {
      char * lpszKeyName;
      unsigned long dwLength;

      lpszKeyName = p_request->p_nvpair[key_no].name;
      dwLength = (unsigned long) strlen(p_request->p_nvpair[key_no].value);
      if (dwLength)
         p_keyvalue = p_request->p_nvpair[key_no].value;
      else {
         buffer[0] = '\0';
         p_keyvalue = buffer;
      }

      no_keys ++;

      if (T_STRNCMP(lpszKeyName, "MG", 3) == 0)
         must_use_sm = 1;

      if (T_STRLEN(lpszKeyName) > MG_RVAR_PREFIX_LEN && T_STRNCMP(lpszKeyName, MG_RVAR_PREFIX, MG_RVAR_PREFIX_LEN) == 0) {
         if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "DBS") == 0) {
            T_STRNCPY(m_server_name, p_keyvalue, 30);
            m_server_name[30] = '\0';
            continue;
         }
         else if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "PW1") == 0) { /* CM0051 */
            strncpy(p_smdata->pw1, p_keyvalue, 60);
            p_smdata->pw1[60] = '\0';
         }
         else if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "PW2") == 0) { /* CM0051 */
            strncpy(p_smdata->pw2, p_keyvalue, 60);
            p_smdata->pw2[60] = '\0';
         }
         else if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "CDN") == 0) {
            T_STRCPY(closedown_service, p_keyvalue);
            closedown = 1;
         }
         else if (T_STRNCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYS", 3) == 0) {
            if (T_STRLEN(lpszKeyName) == (MG_RVAR_PREFIX_LEN + 3))
               T_STRCPY(form_id, p_keyvalue);
            else {
               if (T_STRNCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSb", 4) == 0 && T_STRLEN(p_keyvalue) > 0)
                  T_STRCPY(p_smdata->button, lpszKeyName);
               if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSconfigSection") == 0)
                  T_STRCPY(p_smdata->config_section, p_keyvalue);
               if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSnewDBS") == 0)
                  T_STRCPY(p_smdata->new_dbserver, p_keyvalue);
               if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSTestPage") == 0) {
                  if (mg_sysman_access(p_request) != 1)
                     continue;
               }
               if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSMQService") == 0) {
                  T_STRCAT(p_smdata->mq_service, p_keyvalue);
               }
               if (T_STRCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSnewMQService") == 0) {
                  T_STRCAT(p_smdata->new_mq_service, p_keyvalue);
               }

               if (T_STRNCMP(lpszKeyName + MG_RVAR_PREFIX_LEN, "SYSset", 6) == 0) {

                  T_STRCPY(keyname, (lpszKeyName + (MG_RVAR_PREFIX_LEN + 6)));

                  if (T_STRLEN(keyname) > 0) {
                     if (T_STRCMP(keyname, "Username") == 0) { /* CM0051 */
                        strncpy(p_smdata->username, p_keyvalue, 60);
                        p_smdata->username[60] = '\0';
                     }
                     if (T_STRCMP(keyname, MG_P_SERVICE_STATUS) == 0 || T_STRCMP(keyname, "Application_Status") == 0) {
                        if (T_STRCMP(p_keyvalue, "Disabled") == 0)
                           p_smdata->service_status = 0;
                     }
                     else {
                        if (p_smdata->s_buffer == NULL) {
                           p_wk_buffer = p_trans_buffer->p_buffer;
                           p_smdata->s_buffer = p_wk_buffer;
                        }
                        for (n = 0; keyname[n] != '\0'; n ++)
                           *p_wk_buffer ++ = keyname[n];
                        *p_wk_buffer ++ = '=';
                        for (n = 0; p_keyvalue[n] != '\0'; n ++)
                           *p_wk_buffer ++ = p_keyvalue[n];
                        *p_wk_buffer ++ = '\0';
                        *p_wk_buffer = '\0';
                        continue;
                     }
                  }               
               }
            }
         }
      }

      mg_sysman_trans_control_chars((char *) p_keyvalue, dwLength);

      if (p_smdata->s_buffer == NULL) {

         mg_add_record(p_trans_buffer, "2", (char *) lpszKeyName, p_keyvalue, dwLength);
      }
   }

   mg_free_key_list (p_request, p_key_list);

   /* SIG Management */

   if (must_use_sm || T_STRLEN(form_id) > 0) {
      if (closedown == 1)
         T_STRCPY(m_server_name, closedown_service);
      n = mg_sysman(p_request, form_id, m_server_name, p_smdata);
      goto mg_http_exit1;

   }

   /* Get CGI Ennvironment variables */

   for (n = 0; mg_cgi_vars[n]; n ++) {

      len = mg_get_server_variable(p_request, mg_cgi_vars[n], p_cgievar);

      T_SPRINTF(buffer, "%d", n);

      mg_add_record(p_trans_buffer, "1", buffer, p_cgievar->p_buffer, len);

      if (n == 2) {
         T_STRCPY(cli_ip_address, p_cgievar->p_buffer);
      }
   }

   /* Load-Balancing and Fail-Over */

   failover = 0;
   sn_base = 0;
   sn = 0;
   if (!strlen(m_server_name)) {
      if (shndle >= 0 && shndle < MG_MAX_SERVERS && mg_servers[shndle])
         strcpy(m_server_name, mg_servers[shndle]->m_server_name);
      else
         strcpy(m_server_name, core_data.default_service);
   }
   strcpy(m_server_name_req, m_server_name);
   althndle = mg_get_alt_servers(m_server_name);

mg_failover:

   if (althndle >= 0 && mg_alt_servers[althndle]->alt_status != 0) {

      /* Failover requested after actual failure */

      if (failover) {
         sn = mg_next_alt_server(althndle, sn);
         if (sn == sn_base) {

            strcpy(m_server_name, "");
         }
         else {

            strcpy(m_server_name, mg_alt_servers[althndle]->alt[sn]->server);

            chndle = -1;
            shndle = -1;

            goto mg_failover_retry;
         }

      }

      /* Load Balancing */

      else if (shndle == -1 && mg_alt_servers[althndle]->alt_status == 1) {
         sn = mg_alt_servers[althndle]->alt_ptr;
         if (mg_alt_servers[althndle]->alt[sn] && mg_alt_servers[althndle]->alt[sn]->status == 1) {
            strcpy(m_server_name, mg_alt_servers[althndle]->alt[sn]->server);
         }
         else {
            strcpy(m_server_name, mg_alt_servers[althndle]->default_server);
            sn = 0;
         }
         sn_base = sn;
         mg_alt_servers[althndle]->alt_ptr = mg_next_alt_server(althndle, sn);
         failover = 1;
      }

      /* Fail-Over */

      else if (shndle == -1 && mg_alt_servers[althndle]->alt_status == 2) {
         strcpy(m_server_name, mg_alt_servers[althndle]->default_server);
         failover = 1;
      }
      else {
         strcpy(m_server_name, mg_alt_servers[althndle]->default_server);
         if (!strlen(m_server_name))
            strcpy(m_server_name, core_data.default_service);
         failover = 1;
      }
   }

   if (!strlen(m_server_name)) {
      mg_return_message(p_request, 0, "SIG Management", "Server is currently unavailable", m_server_name);
      goto mg_http_exit1;

   }

   if (closedown == 1) {
      if (mg_sysman_access(p_request) == 1) {
         n = mg_close_connections((char *) closedown_service, 0);
         n = mg_return_message(p_request, 0, "SIG Management", "All Specified Connections are now Closed", "SYSTEM_INDEX");
      }
      goto mg_http_exit1;
   }

   /* Connect */

   attempt_no = 0;

mg_failover_retry:

   nx = 1;
   ok = 1;
   context = MG_OS_POOL_CONNECTION;

   if ((errorcode = mg_get_connection(&chndle, &shndle, (char *) m_server_name, core_data.server_timeout, 0, context)) < 1) {

      if (failover) {
         goto mg_failover;
      }

      if (errorcode == -8 || errorcode == -12)
         mg_return_message(p_request, 0, "SIG Management", "All connections to the Server are busy: Please try later", m_server_name);
      else if (errorcode == -9)
         mg_return_message(p_request, 0, "SIG Management", "Server cannot be identified", m_server_name);
      else if (errorcode == -301)
         mg_return_message(p_request, 0, "SIG Management", "Server is currently disabled", m_server_name);
      else
         mg_return_message(p_request, 0, "SIG Management", "Server is currently unavailable", m_server_name);
      goto mg_http_exit1;
   }
   if (chndle < 0 || shndle < 0) {
      mg_return_message(p_request, 0, "SIG Management", "Server is currently unavailable", m_server_name);
      goto mg_http_exit1;
   }
   p_request->s_timeout = mg_servers[shndle]->server_timeout;

   if (!attempt_no) {

      T_SPRINTF(buffer, "%d", shndle);

      mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "CHD", buffer, 0);

      if (strcmp(m_server_name, m_server_name_req)) {
         mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "LPNreq", m_server_name_req, 0);
      }
   }

   databuffer_size = 4090;

   p_databuffer = p_trans_buffer->p_buffer;

   header_inc = 0;
   header_size = 0;
   line_no = 0;

   /* Send request to M/Cache */

   con_stat = MG_READ_EOF;

   if (!attempt_no) {

      mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "LIB", core_data.mg_lib_path, 0);
      mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "LPN", mg_connections[chndle]->p_server->m_server_name, 0);
      mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "UCI", mg_connections[chndle]->p_server->uci, 0);

      if (mg_connections[chndle]->p_server->idle_timeout) {
         T_SPRINTF(buffer, "%d", mg_connections[chndle]->p_server->idle_timeout);
         mg_add_record(p_trans_buffer, "2", MG_RVAR_PREFIX "CTO", buffer, 0);
      }
   }

   if (mg_server_write(p_request, chndle, p_trans_buffer) == 0) {

      mg_tcp_close_connection(chndle, MG_CS_CLOSE);

      if (p_request->cli_gone) {
   	   con_stat = MG_SEND_ERROR;
	      goto mg_endofrequest;
      }

      if (attempt_no < 3) {
         attempt_no ++;
         goto mg_failover_retry;
      }

      p_databuffer = p_trans_buffer->p_buffer;
      T_SPRINTF(p_databuffer, "%s Not Responding", MG_DB_SERVER_TITLE);
      databuffer_actual_size = (int) T_STRLEN(p_databuffer);
	   con_stat = MG_SEND_ERROR;
	   goto mg_endofrequest;
   }

   mg_connections[chndle]->activity ++;

   p_databuffer = p_trans_buffer->p_buffer;
   databuffer_actual_size = 0;

   smform = 1;
   strcpy(cbuffer, "");

   while ((con_stat = mg_server_read(chndle, p_databuffer, databuffer_size, &databuffer_actual_size, p_request->s_timeout, 0)) == MG_READ_OK) {
      line_no ++;

      p_wk_databuffer = p_databuffer;
      p_wk_buffer = p_databuffer;

      if (smform) {
         strcat(cbuffer, p_databuffer);
         continue;
      }

      if (line_no == 1) {
         if (!header_inc) {
            T_STRNCPY(buffer, p_wk_buffer, 10);
            buffer[9] = '\0';
            mg_ucase(buffer);
            if (!T_STRSTR(buffer, "HTTP/")) {
               mg_sysman_default_header(p_request);
            }
         }
      }

      if (!p_request->cli_gone) {

         if (!mg_client_write_buffer(p_request, p_wk_buffer, databuffer_actual_size)) {

            /* Query interrupted */

            n = mg_release_connection(chndle, MG_CS_CLOSE);

            goto mg_http_exit1;
         }
      }
   }

   if (smform) {

      char *p, *p1;

      p = strstr(cbuffer, "<h2>");
      if (p) {
         p1 = strstr(p, "</body>");
         if (p1) {
            *p1 = '\0';

            mg_sysman_top(p_request, 0, 0, "Test Connection - Result", "", core_data.mg_lib_path, core_data.mg_lib_path);
            mg_client_write_string(p_request, p);
            mg_sysman_tail(p_request, 0);

         }
         else
            mg_client_write_string(p_request, buffer);
      }
      else
         mg_client_write_string(p_request, buffer);


   }

mg_endofrequest:

   if (!p_request->cli_gone) {

      /* Error conditions */

      if (con_stat == MG_READ_ERROR || con_stat == MG_READ_TIMEOUT || con_stat == MG_SEND_ERROR) {
         T_SPRINTF(buffer, "%s reported the following error:", MG_DB_SERVER_TITLE); 
         mg_log_event(p_databuffer, buffer);

         if (!line_no) {
            n = mg_return_message(p_request, 1, buffer, p_databuffer, m_server_name);
         }
         else {
            mg_client_write_string(p_request, "<P><P><B><FONT COLOR=\"RED\">");
            mg_client_write_string(p_request, buffer);
            mg_client_write_string(p_request, "</FONT></B><P>\r\n");
            mg_client_write_string(p_request, p_databuffer);
         }

      }
      else if (line_no == 0)
         n = mg_return_message(p_request, 1, "", "The Server-side program did not return any data", m_server_name);
   }

   /*
      Close/release the  the database System. This is mandatory.
      All Connections successfully opened MUST be closed down (or released) by
      a call to the following function.
   */

   context = MG_CS_RELEASE_TO_POOL;

   n = mg_release_connection(chndle, context);

mg_http_exit1:

   mg_buf_free(p_cgievar);
   mg_buf_free(p_trans_buffer);

mg_http_exit2:

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_http: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 1;
}
#endif

}


/* Read data from client */

int mg_client_read(MGREQ *p_request, void *buffer, int size)
{
   int result, n, ichar, no_read;
   unsigned long postlen;
   unsigned char s_buffer[32];
   unsigned char *p_buffer;


#ifdef _WIN32
__try {
#endif

   result = 0;

   n = 0;
   no_read = 0;
   postlen = 0;
   ichar = 1;
   *s_buffer = '\0';
   p_buffer = (unsigned char *) buffer;

   result = MGNET_RECV(p_request->cli_sockfd, p_buffer, size, 0);
   if (result > 0) {
      p_request->rlen += result;

      if (p_request->rlen == p_request->clen) {

         fd_set rset, eset;
         struct timeval tval;
         char junk[8];

         FD_ZERO(&rset);
         FD_ZERO(&eset);
         FD_SET(p_request->cli_sockfd, &rset);
         FD_SET(p_request->cli_sockfd, &eset);

         tval.tv_sec = 0;
         tval.tv_usec = 0;

         for (;;) {
            n = MGNET_SELECT((int) p_request->cli_sockfd + 1, &rset, NULL, &eset, &tval);

            if (n == 0) {
               break;
            }

            if (n < 0 || !MGNET_FD_ISSET(p_request->cli_sockfd, &rset)) {
               break;
            }

            n = MGNET_RECV(p_request->cli_sockfd, junk, 1, 0);
         }
      }
   }
   else {
      p_request->cli_gone = 1;
   }

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_client_read: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Return data to client */

int mg_client_write_string(MGREQ *p_request, char *buffer)
{
   return mg_client_write_buffer(p_request, (void *) buffer, (int) strlen(buffer));
}


int mg_client_write_buffer(MGREQ *p_request, void *buffer, int size)
{
   int result;


#ifdef _WIN32
__try {
#endif

   result = 1;

   if (MGNET_SEND(p_request->cli_sockfd, buffer, size, 0) < 0)
      result = 0;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_client_write_buffer: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_get_server_variable(MGREQ *p_request, char *VariableName, LPMEMOBJ p_cgievar)
{
   unsigned long n, cgievar_size, incr_size;
   char uni_buffer[64];
   char ansi_buffer[256];
   char *p_data;
   LPNVLIST p_cgi;

#ifdef _WIN32
__try {
#endif

   n = 0;
   cgievar_size = 0;
   p_data = NULL;
   *uni_buffer = '\0';
   *ansi_buffer = '\0';
   mg_buf_cpy(p_cgievar, "");
   incr_size = p_cgievar->incr_size;

   p_cgi = p_request->p_cgi_list;
    while (p_cgi) {
      if (!T_STRCMP(p_cgi->name, VariableName)) {
         mg_buf_cpy(p_cgievar, p_cgi->value);
         break;
      }
      p_cgi = p_cgi->p_next;
   }

   return p_cgievar->curr_size;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_get_server_variable: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_add_server_variable(MGREQ *p_request, char *VariableName, LPMEMOBJ p_cgievar, LPMEMOBJ p_trans_buffer)
{
   int len;

#ifdef _WIN32
__try {
#endif

   len = mg_get_server_variable(p_request, VariableName, p_cgievar);

   mg_sysman_trans_control_chars(p_cgievar->p_buffer, len);

   mg_add_record(p_trans_buffer, "1", VariableName, p_cgievar->p_buffer, len);

   return 1;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_add_server_variable: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_add_record(LPMEMOBJ p_record, char *type, char *name, char *content, unsigned long content_length)
{

#ifdef _WIN32
__try {
#endif

   mg_buf_cat(p_record, type);
   mg_buf_cat(p_record, (char *) name);
   mg_buf_cat(p_record, "=");
   mg_buf_cat(p_record, content);
	mg_buf_cat(p_record, "\r");


   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_add_record: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


HNVLIST mg_get_key_list(MGREQ *p_request)
{
   char *buffer = NULL;             /* input buffer to work on */
   char *wk_buffer = NULL;          /* input buffer to work on */
   char *method;            /* ptr to method string in pblock */
   char *qstr = NULL;       /* actual query string to work on */
   char *query_string = NULL;       /* query string envrironment variable */
   int nvcount;             /* number of name=value pairs processed */
   int i, cvars;                   /* miscellaneous counter/index */
   int in_name, cn, vn, no_mem, len;
   unsigned long clen, nvlen, n, size, size_mem, rlen;     /* content length */
   unsigned long postlen;           /* number of chars read from net_buf */
   int qlen;                /* Query string length */
   char s_buffer[4096];
   LPNVLIST p_key_list, p_key;
   char *p_general;
   char ctype[256];
   LPNVLIST p_cgi_list, p_cgi, p_cgi1;

#ifdef _WIN32
__try {
#endif

   p_key = NULL;
   p_key_list = NULL;
   p_cgi_list = NULL;
   *ctype = '\0';
   *s_buffer = '\0';
   method = NULL;
   query_string = NULL;
   qstr = NULL;
   postlen = 0;
   clen = 0;
   nvlen = 0;
   rlen = 0;
   in_name = 0;
   vn = 0;
   cn = 0;
   no_mem = 0;
   size_mem = 0;
   cvars = 0;
   buffer = p_request->iBuffer;

   p_request->p_key_list = NULL;
   p_request->p_cgi_list = NULL;

   /*
      If the method is GET, we can get the name=value pairs from the
      query string, but if it's POST, then we need to read in all the
      input from the network and stuff it into a string. If it's neither,
      log an error and exit.
   */


   /* Environment variables */

   for (i = 0; i < p_request->iBuffer_size; i ++) {
      if (buffer[i] == '\r')
         buffer[i] = '\0';
   }
   buffer[p_request->iBuffer_size] = '\0';
   buffer[p_request->iBuffer_size + 1] = '\0';

   for (buffer = p_request->iBuffer; *buffer != '\0'; buffer += len + 1) {
      len = (int) T_STRLEN(buffer);

      if (len > 2) {
         wk_buffer = T_STRSTR(buffer, "=");
         if (wk_buffer) {
            *wk_buffer = '\0';

            p_cgi = mg_malloc(sizeof(NVLIST), "mg_get_key_list:1");

            if (!p_cgi) {
               mg_log_event("No Memory", "ERROR");
               no_mem = 1;
               break;
            }

            if (cn == 0)
               p_cgi_list = p_cgi;
            else
               p_cgi1->p_next = p_cgi;
            cn ++;
            p_cgi->name = buffer;
            p_cgi->value = wk_buffer + 1;
            p_cgi->pBuffer = NULL;
            p_cgi->p_next = NULL;
            p_cgi1 = p_cgi;
         }

         if (!strcmp(buffer, "CONTENT_TYPE")) {
            strcpy(ctype, wk_buffer + 1);
            strcpy(p_request->content_type, ctype);
            p_general = T_STRSTR(ctype, "boundary=");
            if (p_general) {
               T_STRCPY(p_request->boundary, p_general + 9);
               p_request->multipart = 1;
            }
            else {
               if (strlen(ctype) && !strstr(ctype, "urlencoded")) {
                  p_request->urlencoded = 0;
               }
            }

         }
         else if (!strcmp(buffer, "REQUEST_METHOD"))
            method = wk_buffer + 1;
         else if (!strcmp(buffer, "CONTENT_LENGTH"))
            clen = (int) strtol(wk_buffer + 1, NULL, 10);
         else if (!strcmp(buffer, "QUERY_STRING"))
            query_string = wk_buffer + 1;

      }
   }

   if (no_mem) {
      while (p_key_list) {
         p_key = p_key_list;
         p_key_list = p_key->p_next;

         mg_free((void *) p_key, "mg_get_key_list:1");

      }
      while (p_cgi_list) {
         p_cgi = p_cgi_list;
         p_cgi_list = p_cgi->p_next;

         mg_free((void *) p_cgi, "mg_get_key_list:2");

      }

      return NULL;
   }

   p_request->p_cgi_list = p_cgi_list;

   /* Content */

   if (method == NULL) {
      p_key_list = NULL;
      goto mg_get_key_list_exit;
   }

   if (query_string)
      qlen = (int) T_STRLEN(query_string);
   else
      qlen = 0;


   if (T_STRCMP(method, "POST") == 0) {

      p_request->clen = clen;
      p_request->rlen = clen;

      if (clen && p_request->urlencoded) {

         /* allocate space to put the query string after we read it in */
 
         n = 0;
         p_request->rlen = 0;
         rlen = 0;

         size = (clen + 1 + (qlen + 3));
         size_mem = MG_MAXCON;

         p_request->p_qstr = p_request->qstr;
         qstr = p_request->qstr;
         *qstr = '\0';

         strcpy(p_request->prfx, "");

         in_name = 1;
         nvcount = 1;

         cvars = 0;
         if (clen > 0) {
         for (;;) {
            postlen = mg_client_read(p_request, (void *) s_buffer, 1);
            if (postlen != 1) {

               p_key_list = NULL;
               goto mg_get_key_list_exit;
            }

            if (in_name) {
               p_request->prfx[in_name - 1] = s_buffer[0];
               p_request->prfx[in_name] = '\0';
               in_name ++;

               if (in_name == 7 || s_buffer[0] == '=') {

                  if (in_name > MG_RVAR_PREFIX_LEN && !strncmp(p_request->prfx, MG_RVAR_PREFIX, MG_RVAR_PREFIX_LEN))
                     cvars = 1;

                  if (cvars && strncmp(p_request->prfx, MG_RVAR_PREFIX, MG_RVAR_PREFIX_LEN)) {

                     /* Passed the MG/EP reserved variables - can stop now */

                     if (size > MG_MAXCON) {
                        p_request->stream = 1;
                        break;
                     }
                  }

                  if (in_name == 7 && !strncmp(p_request->prfx, MG_RVAR_PREFIX "req", 6)) {

                     /* Event Broker call from the applet - can stop now */

                     if (size > MG_MAXCON) {
                        p_request->stream = 1;
                        break;
                     }
                  }

                  if (nvcount > 1)
                     qstr[rlen ++] = '&';
                  for (n = 0; p_request->prfx[n]; n ++)
                     qstr[rlen ++] = p_request->prfx[n];

                  p_request->prfx[0] = '\0';

                  in_name = 0;

                  if (p_request->rlen == clen)
                     break;

               }
               continue;

            }

            if (s_buffer[0] == '&') {
               in_name = 1;
               nvcount ++;
               continue;
            }

            qstr[rlen ++] = s_buffer[0];

            if (rlen > (size_mem - 30) && size > size_mem) {

               qstr[rlen] = '\0';

               p_request->p_qstr = mg_malloc(sizeof(char) * size, "mg_get_key_list:6");
               qstr = p_request->p_qstr;

               if (!qstr) {
                  mg_log_event("No Memory", "ERROR");
                  p_key_list = NULL;
                  goto mg_get_key_list_exit;
               }

               T_STRCPY(qstr, p_request->qstr);
               size_mem = size;

            }

            if (p_request->rlen == clen)
               break;
         }
         }

         qstr[rlen] = '\0';
         clen = rlen;
         nvlen = rlen;

         if (qlen > 0) {
            if (rlen > 0) {
               T_STRCAT(qstr, "&");
               nvlen ++;
            }
            T_STRCAT(qstr, query_string);
            nvlen += qlen;
         }

      }

   }
   else if (T_STRCMP(method, "GET") == 0) {

      /* Make a copy of the query string to work on */

      if (qlen == 0) {
         p_key_list = NULL;
         goto mg_get_key_list_exit;
      }

      size = qlen + 1;

      if (size < MG_MAXCON)
         p_request->p_qstr = p_request->qstr;
      else
         p_request->p_qstr = mg_malloc(sizeof(char) * size, "mg_get_key_list:7");
      qstr = p_request->p_qstr;


      if (!qstr) {
         mg_log_event("No Memory", "ERROR");
         p_key_list = NULL;
         goto mg_get_key_list_exit;

      }

      if (query_string)
         T_STRCPY(qstr, query_string);
      else
         T_STRCPY(qstr, "");

      nvlen = qlen;
   } 
   else {
      p_key_list = NULL;
      goto mg_get_key_list_exit;
   }

   if (!nvlen) {

      p_key_list = NULL;
      goto mg_get_key_list_exit;

   }

   p_request->p_nvpair = p_request->nvpair;

   nvcount = mg_process_query_string(p_request);

   p_key_list = NULL;

   p_request->key_no = nvcount;
   if (nvcount)
      p_key_list = (LPNVLIST) 1;

   for (i = 0; i < nvcount; i ++) {

      if (p_request->p_nvpair[i].name != NULL && p_request->p_nvpair[i].value != NULL) {

         mg_url_decode(p_request->p_nvpair[i].name);
         mg_url_decode(p_request->p_nvpair[i].value);
      }
   }


mg_get_key_list_exit:

   return p_key_list;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_get_key_list: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return NULL;
}
#endif

}


void mg_free_key_list(MGREQ *p_request, HNVLIST hNVList)
{
   char *qstr;
   LPNVLIST pNVList, pNV;

#ifdef _WIN32
__try {
#endif

   qstr = NULL;
   pNVList = NULL;
   pNV = NULL;

   if (p_request->p_qstr && p_request->p_qstr != p_request->qstr) {
      mg_free((void *) p_request->p_qstr, "mg_free_key_list:1");

   if (p_request->p_nvpair && p_request->p_nvpair != p_request->nvpair)
      mg_free((void *) p_request->p_nvpair, "mg_free_key_list:2");

   }

   return;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_free_key_list: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return;
}
#endif
}


/*
   This function takes a string in query string format, and puts the
   names and values of each name=value pair in the specified arrays
   of char *s (which point into the space originally used by the query
   string). name and value must already be allocated. It returns the
   number of pairs thus processed.
*/

int mg_process_query_string(MGREQ *p_request)
{
   int in_name;             /* reading a name? (boolean) */
   int i;                   /* index into qstr */
   int len;                 /* length of query string */
   int vsize, n;
   int nvcount;             /* count of name=value pairs processed */
   char *nstr;              /* pointer to substring under consideration */
   LPNVLIST p_nvlist;

#ifdef _WIN32
__try {
#endif

   /*
      Briefly: we run through the string, looking for = or &. When we
      find one, we set it to \0, and set the name or value element to
      point to the substring preceding it. Then we switch whether we're
      looking at name or value and keep going.
   */

   len = (int) strlen(p_request->p_qstr);
   in_name = 1;
   nvcount = 0;
   nstr = p_request->p_qstr;
   vsize = 0;

   for (i = 0; i < len; i ++) {
      if (in_name) {
         if (p_request->p_qstr[i] == '=') {
            p_request->p_nvpair[nvcount].name = nstr;
            p_request->p_nvpair[nvcount].value = NULL;
            p_request->p_qstr[i] = '\0';
            nstr = p_request->p_qstr + i + 1;
            in_name = 0;
            vsize = 0;
         }
      }
      else {
         if (p_request->p_qstr[i] == '&') {
            p_request->p_nvpair[nvcount].value = nstr;
            p_request->p_qstr[i] = '\0';
            nstr = p_request->p_qstr + i + 1;
            in_name = 1;
            nvcount ++;

            if (nvcount == MG_MAXKEY) {

               p_nvlist = mg_malloc(sizeof(NVLIST) * (len / 4 + 1), "mg_process_query_string:1");
               if (!p_nvlist)
                  return nvcount;
               p_request->p_nvpair = p_nvlist;

               for (n = 0; n < nvcount; n ++) {
                  p_request->p_nvpair[n].name = p_request->nvpair[n].name;
                  p_request->p_nvpair[n].value = p_request->nvpair[n].value;
               }
            }

         }
         else
            vsize ++;
      }
   }
 
   /* since there is no trailing & after the last pair, do this by hand */

   if (in_name == 0) {
      p_request->p_nvpair[nvcount].value = nstr;
      nvcount ++;
   }
   else
      p_request->p_nvpair[nvcount].value = NULL;

   return(nvcount);


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_process_query_string: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif
}


/*
   This function replaces a string with the URL-decoded version of
   that string. Since URL-decoding leaves the string the same length
   or shortens it, this is safe. It returns the new length.
*/

int mg_url_decode(char *target)
{
   char *temp;              /* copy of target */
   char c1, c2;             /* holders for hex digits of escape sequence */
   int len;                 /* length of original string */
   int k;                   /* index for writing to decoded string */
   int j;                   /* index for reading from encoded string */

   len = (int) strlen(target);

#ifdef _WIN32
__try {
#endif

   temp = target;


   /*
      Run down the length of the encoded string, examining each
      character. If it's a +, we write a space to the decoded string.
      If it's a %, we discard it, read in the next two characters,
      convert their hex value to a char, and write that. Anything
      else, we just copy over.
   */

   for (j = 0, k = 0; j < len; j ++, k ++) {
      switch(temp[j]) {
         case '+':
            target[k]=' ';
            break;
         case '%':
            c1 = tolower(temp[++j]);
            if (isdigit(c1)) {
               c1 -= '0';
            }
            else {
               c1 = c1 - 'a' + 10;
            }
            c2 = tolower(temp[++ j]);
            if (isdigit(c2)) {
               c2 -= '0';
            }
            else {
               c2 = c2 - 'a' + 10;
            }
            target[k] = c1 * 16 + c2;
            break;
         default:
            target[k] = temp[j];
            break;
      }
   }
   target[k] = '\0';

   return (k);

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_url_decode: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_wsmq(MGREQ *p_request)
{
   int errorcode, eod, n, ok, mode, idx, idx_free;
   unsigned long len, max, size;
   char fun[32], lenstr[32];

   int ln, clen;
   char buffer[8192], m_job_no[32];
   char *p, *p1, *p2, *pc;
   MEMOBJ request;
   MGMQ mgmq, *p_mgmq;


#ifdef _WIN32
__try {
#endif

   mode = 0;

mg_wsmq_next_request:

   p_mgmq = &mgmq;

   p_request->request_source = MG_RS_MQ;

   p_mgmq->mg_malloc = mg_malloc;
   p_mgmq->mg_free = mg_free;

   strcpy(p_mgmq->q_name, "");

   strcpy(p_mgmq->qm_name, "");

   strcpy(p_mgmq->rqm_name, "");
   strcpy(p_mgmq->rq_name, "");

   strcpy(p_mgmq->tqm_name, "");
   strcpy(p_mgmq->dq_name, "");

   strcpy(p_mgmq->msg_id, "");
   strcpy(p_mgmq->correl_id, "");
   strcpy(p_mgmq->group_id, "");
   p_mgmq->msg_seq_no = 0;
   p_mgmq->offset = 0;

   strcpy(p_mgmq->error, "");
   strcpy(p_mgmq->info, "");

   strcpy(p_mgmq->connx_options, "");
   strcpy(p_mgmq->open_options, "");
   strcpy(p_mgmq->close_options, "");
   strcpy(p_mgmq->put_options, "");
   strcpy(p_mgmq->get_options, "");
   strcpy(p_mgmq->get_match_options, "");
   strcpy(p_mgmq->begin_options, "");

   strcpy(p_mgmq->key, "");

   p_mgmq->s_buffer_size = 0;
   p_mgmq->s_buffer_len = 0;
   p_mgmq->s_buffer = NULL;

   p_mgmq->r_buffer_size = 0;
   p_mgmq->r_buffer_offs = 0;
   p_mgmq->r_buffer_len = 0;
   p_mgmq->r_buffer = NULL;

   p_mgmq->timeout = 0;

   p_mgmq->r_type = 0;

   mg_buf_init(&request, 4096, 2048);

   strcpy(buffer, p_request->request_heading);
   size = (unsigned long) strlen(buffer);

   strcpy(p_request->request_heading, "");

   ln = 0;
   clen = 0;

   max = 4000;
   pc = NULL;
   ok = 0;

   for (;;) {
      errorcode = MGNET_RECV(p_request->cli_sockfd, buffer + size, max - size, 0);

      if (errorcode < 1)
         break;

      size += errorcode;

      buffer[size] = '\0';
      pc = strstr(buffer, "\r\n\r\n");

      if (pc) {
         ok = 1;
         break;
      }

      if (size == max) {
         ok = 1;
         break;
      }
   }

   if (errorcode < 1 || !ok) {
      mg_buf_free(&request);
      goto mg_wsmq_exit2;
   }

   if (mg_sec.wsmq_ip[0]) {

      ok = 0;

      for (n = 0; mg_sec.wsmq_ip[n]; n ++) {
         if (!strcmp(p_request->cli_addr, mg_sec.wsmq_ip[n])) {
            ok = 1;
            break;
         }
      }
      if (!ok) {

         char buf[256];
         sprintf(buf, "MGateway SIG: Security Alert: Illegal access from: %s", p_request->cli_addr);

         p = strstr(buffer, "\r\n\r\n");
         if (p)
            *p = '\0';

         for (n = 0; buffer[n]; n ++) {
            if (buffer[n] == '\r' || buffer[n] == '\n')
               buffer[n] = '|';
         }

         mg_log_event(buffer, buf);

         mg_buf_free(&request);

         goto mg_wsmq_exit2;
      }

   }

   p1 = buffer;

   for (;;) {

      p2 = strstr(p1, "\n");
      if (!p2)
         break;

      *(p2 - 1) = '\0';

      len = (unsigned long) strlen(p1);

      if (len == 0)
         break;

      if (!strncmp(p1, "WSMQ", 4)) {
         strncpy(fun, p1 + 5, 30);
         fun[30] = '\0';
         p = strstr(fun, " ");
         if (p)
            *p = '\0';
      }
      else {
         p = strstr(p1, ":");
         if (p) {
            *p = '\0';
            if (!strcmp(p1, "q_name")) {
               strncpy(p_mgmq->q_name, p + 2, 60);
               p_mgmq->q_name[60] = '\0';
            }
            else if (!strcmp(p1, "qm_name")) {
               strncpy(p_mgmq->qm_name, p + 2, 60);
               p_mgmq->qm_name[60] = '\0';
            }
            else if (!strcmp(p1, "rqm_name")) {
               strncpy(p_mgmq->rqm_name, p + 2, 60);
               p_mgmq->rqm_name[60] = '\0';
            }
            else if (!strcmp(p1, "rq_name")) {
               strncpy(p_mgmq->rq_name, p + 2, 60);
               p_mgmq->rq_name[60] = '\0';
            }
            else if (!strcmp(p1, "dq_name")) {
               strncpy(p_mgmq->dq_name, p + 2, 60);
               p_mgmq->dq_name[60] = '\0';
            }
            else if (!strcmp(p1, "tqm_name")) {
               strncpy(p_mgmq->tqm_name, p + 2, 60);
               p_mgmq->tqm_name[60] = '\0';
            }

            else if (!strcmp(p1, "msg_id")) {
               strncpy(p_mgmq->msg_id, p + 2, 60);
               p_mgmq->msg_id[60] = '\0';
            }
            else if (!strcmp(p1, "correl_id")) {
               strncpy(p_mgmq->correl_id, p + 2, 60);
               p_mgmq->correl_id[60] = '\0';
            }
            else if (!strcmp(p1, "group_id")) {
               strncpy(p_mgmq->group_id, p + 2, 60);
               p_mgmq->group_id[60] = '\0';
            }
            else if (!strcmp(p1, "msg_seq_no")) {
               p_mgmq->msg_seq_no = (int) strtol(p + 2, NULL, 10);
            }
            else if (!strcmp(p1, "offset")) {
               p_mgmq->offset = (int) strtol(p + 2, NULL, 10);
            }
            else if (!strcmp(p1, "connx_options")) {
               strncpy(p_mgmq->connx_options, p + 2, 200);
               p_mgmq->connx_options[200] = '\0';
            }
            else if (!strcmp(p1, "open_options")) {
               strncpy(p_mgmq->open_options, p + 2, 200);
               p_mgmq->open_options[200] = '\0';
            }
            else if (!strcmp(p1, "close_options")) {
               strncpy(p_mgmq->close_options, p + 2, 200);
               p_mgmq->close_options[200] = '\0';
            }
            else if (!strcmp(p1, "put_options")) {
               strncpy(p_mgmq->put_options, p + 2, 200);
               p_mgmq->put_options[200] = '\0';
            }
            else if (!strcmp(p1, "get_options")) {
               strncpy(p_mgmq->get_options, p + 2, 200);
               p_mgmq->get_options[200] = '\0';
            }
            else if (!strcmp(p1, "get_match_options")) {
               strncpy(p_mgmq->get_match_options, p + 2, 200);
               p_mgmq->get_match_options[200] = '\0';
            }
            else if (!strcmp(p1, "begin_options")) {
               strncpy(p_mgmq->begin_options, p + 2, 200);
               p_mgmq->begin_options[200] = '\0';
            }
            else if (!strcmp(p1, "timeout")) {
               p_mgmq->timeout = (int) strtol(p + 2, NULL, 10);
               if (p_mgmq->timeout < 1)
                  p_mgmq->timeout = 0;
            }
            else if (!strcmp(p1, "r_type")) {
               p_mgmq->r_type = (int) strtol(p + 2, NULL, 10);
               if (p_mgmq->r_type < 1)
                  p_mgmq->r_type = 0;
            }
            else if (!strcmp(p1, "key")) {
               strncpy((char *) p_mgmq->key, p + 2, 1000);
            }
            else if (!strcmp(p1, "send")) {
               strncpy((char *) p_mgmq->s_buffer, p + 2, 1000);
               p_mgmq->s_buffer[1000] = '\0';
               p_mgmq->s_buffer_len = (unsigned long) strlen((char *) p_mgmq->s_buffer);
            }
            else if (!strcmp(p1, "keepalive")) {
               mode = 0;

            }
            *p = ':';
         }
      }
      p1 = p2 + 1;
   }

   eod = 0;
   if (buffer[size - 1] == deod[0]) {
      buffer[size - 1] = '\0';
      eod = 1;
   }

   p_mgmq->p_mqsrv = NULL;
   idx = -1;
   idx_free = -1;
   strcpy(m_job_no, "cm");
   for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {
      if (mg_mqctable[n] == NULL) {
         if (idx_free == -1)
            idx_free = n;
         continue;
      }
      if (!strcmp(m_job_no, mg_mqctable[n]->m_job_no)) {
         idx = n;
         p_mgmq->p_mqsrv = mg_mqctable[idx]->p_mqsrv;
         break;
      }
   }


   mg_buf_cpy(&request, pc + 4);

   if (!eod) {
      for (;;) {
         errorcode = MGNET_RECV(p_request->cli_sockfd, buffer, max, 0);

         if (errorcode < 1)
            break;

         len = errorcode;
         if (buffer[len - 1] == deod[0]) {
            buffer[len - 1] = '\0';
            eod = 1;
         }
         else
            buffer[len] = '\0';

         mg_buf_cat(&request, buffer);

         if (eod)
            break;
      }
   }


   n = 1;
   ok = 1;

   if (!ok) {

      sprintf(buffer, "0000000000x: 0\r\nerror: %s\r\n\r\n%s", p_mgmq->error, deod);
      len = ((int) strlen(buffer) - 10);
      sprintf(lenstr, "%010ld", len);
      strncpy(buffer, lenstr, 10);

      mg_wsmq_send(p_request, buffer, (unsigned long) strlen(buffer));

      mg_buf_free(&request);

      goto mg_wsmq_exit2;

   }

   p_mgmq->s_buffer_size = request.size;
   p_mgmq->s_buffer_len = request.curr_size;
   p_mgmq->s_buffer = (unsigned char *) request.p_buffer;

   if (!strcmp(fun, "CONN") || !strcmp(fun, "CONNX")) {
      if (core_data.ws_loaded) {
         mg_mgmq_connx(p_mgmq);
         if (p_mgmq->p_mqsrv) {
            if (idx > -1) {
               mg_mqctable[idx]->p_mqsrv = p_mgmq->p_mqsrv;
            }
            else {
               mg_mqctable[idx_free] = mg_malloc(sizeof(MQC), "mg_wsmq:1");
               strcpy(mg_mqctable[idx_free]->m_job_no, m_job_no);
               mg_mqctable[idx_free]->p_mqsrv = p_mgmq->p_mqsrv;
            }
         }
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "DISC")) {
      if (core_data.ws_loaded) {
         mg_mgmq_disc(p_mgmq);
         if (idx > -1) {
            mg_free((void *) mg_mqctable[idx], "mg_wsmq:1");
         }
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "BEGIN")) {
      if (core_data.ws_loaded) {
         mg_mgmq_begin(p_mgmq);
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "BACK")) {
      if (core_data.ws_loaded) {
         mg_mgmq_back(p_mgmq);
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "CMIT")) {
      if (core_data.ws_loaded) {
         mg_mgmq_cmit(p_mgmq);
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "PUT")) {
      if (core_data.ws_loaded) {
         mg_mgmq_put(p_mgmq);
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "GET")) {
      if (core_data.ws_loaded) {

         p_mgmq->r_buffer_size = 16384;
         p_mgmq->r_buffer_offs = 8192;
         p_mgmq->r_buffer_len = 0;
         p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:2");

         if (p_mgmq->r_buffer)
            mg_mgmq_get(p_mgmq);
         else
            strcpy(p_mgmq->error, "Cannot allocate enough memory to deal with this request");
      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }
   else if (!strcmp(fun, "REQ")) {
      if (core_data.ws_loaded) {
         p_mgmq->r_buffer_size = 16384;
         p_mgmq->r_buffer_offs = 8192;
         p_mgmq->r_buffer_len = 0;
         p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:3");

         if (p_mgmq->r_buffer)
            mg_mgmq_req(p_mgmq);
         else
            strcpy(p_mgmq->error, "Cannot allocate enough memory to deal with this request");

      }
      else {
         strcpy(p_mgmq->error, "MGateway SIG interface library to IBM MQ is not loaded");
      }
   }

#if defined(MG_SSL)
   else if (mg_ssl.ssl && !strncmp(fun, "HMAC-SHA256", 11)) {

      short b64;
      int n;
      int key_len, msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      key_len = (int) strlen((char *) p_mgmq->key);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_HMAC(mg_EVP_sha256(), p_mgmq->key, key_len, p_mgmq->s_buffer, msg_len, mac, &mac_len);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "HMAC-SHA1", 9)) {

      short b64;
      int n;
      int key_len, msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      key_len = (int) strlen(p_mgmq->key);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_HMAC(mg_EVP_sha1(), p_mgmq->key, key_len, p_mgmq->s_buffer, msg_len, mac, &mac_len);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "HMAC-SHA", 8)) {

      short b64;
      int n;
      int key_len, msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      key_len = (int) strlen(p_mgmq->key);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_HMAC(mg_EVP_sha(), p_mgmq->key, key_len, p_mgmq->s_buffer, msg_len, mac, &mac_len);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "HMAC-MD5", 8)) {

      short b64;
      int n;
      int key_len, msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      key_len = (int) strlen(p_mgmq->key);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_HMAC(mg_EVP_md5(), p_mgmq->key, key_len, p_mgmq->s_buffer, msg_len, mac, &mac_len);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "SHA256", 6)) {

      short b64;
      int n;
      size_t msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = strlen(p_mgmq->s_buffer);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_SHA256((const unsigned char *) p_mgmq->s_buffer, (unsigned long) msg_len, mac);
      mac_len = strlen((char *) mac);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, (int) mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = (unsigned long) mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "SHA1", 4)) {

      short b64;
      int n;
      size_t msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = strlen(p_mgmq->s_buffer);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_SHA1((const unsigned char *) p_mgmq->s_buffer, (int) msg_len, mac);
      mac_len = strlen((char *) mac);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, (int) mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = (unsigned long) mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "SHA", 3)) {

      short b64;
      int n;
      size_t msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_SHA((const unsigned char *) p_mgmq->s_buffer, (int) msg_len, mac);
      mac_len = strlen((char *) mac);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, (int) mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = (unsigned long) mac_len;
      }
   }
   else if (mg_ssl.ssl && !strncmp(fun, "MD5", 3)) {

      short b64;
      int n;
      size_t msg_len, mac_len;
      unsigned char mac[CACHE_BUFFER];
      char base64[CACHE_BUFFER];

      if (strstr(fun, "B64"))
         b64 = 1;
      else
         b64 = 0;

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);
      mac_len = 0;

      memset(mac, 0, CACHE_BUFFER - 1);
      mg_MD5((const unsigned char *) p_mgmq->s_buffer, (int) msg_len, mac);
      mac_len = strlen((char *) mac);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      if (b64) {
         n = mg_b64_encode((char *) mac, base64, (int) mac_len, 0);
         base64[n] = '\0';
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
         p_mgmq->r_buffer_len = n;
      }
      else {
         memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) mac, mac_len);
         p_mgmq->r_buffer_len = (unsigned long) mac_len;
      }
   }
   else if (!strncmp(fun, "B64", 3)) {

      int n;
      size_t msg_len;
      char base64[CACHE_BUFFER];

      msg_len = strlen(p_mgmq->s_buffer);

      memset(base64, 0, CACHE_BUFFER - 1);
      n = mg_b64_encode((char *) p_mgmq->s_buffer, base64, (int) msg_len, 0);
      base64[n] = '\0';

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) base64, n);
      p_mgmq->r_buffer_len = n;

   }
   else if (!strncmp(fun, "D-B64", 5)) {

      int n;
      size_t msg_len;
      char d_base64[CACHE_BUFFER];

      msg_len = (int) strlen((char *) p_mgmq->s_buffer);

      memset(d_base64, 0, CACHE_BUFFER - 1);
      n = mg_b64_decode((char *) p_mgmq->s_buffer, d_base64, (int) msg_len);
      d_base64[n] = '\0';

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) d_base64, n);
      p_mgmq->r_buffer_len = n;

   }
#endif /* #if defined(MG_SSL) */

   else if (!strncmp(fun, "TIME", 4)) {

      int n;
      double time;
      char timestr[256];

      time = mg_get_time(timestr);

      n = (int) strlen(timestr);

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      memcpy((void *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), (void *) timestr, n);
      p_mgmq->r_buffer_len = n;

   }
   else if (!strcmp(fun, "TEST")) {

      int n;
      time_t now;
      char timestr[128];

      now = time(NULL);
      strcpy(timestr, T_CTIME(&now));

      for (n = 0; timestr[n] != '\0'; n ++) {
         if ((unsigned int) timestr[n] < 32) {
            timestr[n] = '\0';
            break;
         }
      }

      p_mgmq->r_buffer_size = 16384;
      p_mgmq->r_buffer_offs = 8192;
      p_mgmq->r_buffer_len = 0;
      p_mgmq->r_buffer = mg_malloc(sizeof(char) * (p_mgmq->r_buffer_size), "mg_wsmq:4");

      sprintf((char *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs), "MGateway SIG : IBM MQ loop back message: time: %s", timestr);

      p_mgmq->r_buffer_len = (unsigned long) strlen((char *) (p_mgmq->r_buffer + p_mgmq->r_buffer_offs));
   }
   else {
      sprintf(p_mgmq->error, "MGateway SIG : Unrecognized request : %s", fun);
   }

   if (core_data.log_errors && strlen(p_mgmq->error)) {
      mg_log_event(p_mgmq->error, "MGateway SIG : m_ibm_mq error condition");
   }

   sprintf(buffer, "0000000000x: 0\r\nerror: %s\r\ninfo: %s\r\nrecv_len: %lu\r\nr_code: %d\r\nr_type: %d\r\nrqm_name: %s\r\nrq_name: %s\r\nmsg_id: %s\r\ncorrel_id: %s\r\ngroup_id: %s\r\nmsg_seq_no: %ld\r\noffset: %ld\r\n\r\n", 
                                    p_mgmq->error, p_mgmq->info, p_mgmq->r_buffer_len,  p_mgmq->r_code ? p_mgmq->r_code : p_mgmq->open_r_code ? p_mgmq->open_r_code : p_mgmq->get_r_code, p_mgmq->r_type, p_mgmq->rqm_name, p_mgmq->rq_name, p_mgmq->msg_id, p_mgmq->correl_id, p_mgmq->group_id, p_mgmq->msg_seq_no, p_mgmq->offset);

   size = (int) strlen(buffer) + p_mgmq->r_buffer_len;
   len = (size - 10);
   sprintf(lenstr, "%010ld", len);
   strncpy(buffer, lenstr, 10);
   len = (int) strlen(buffer);

   if (p_mgmq->r_buffer && p_mgmq->r_buffer_len > 0) {

      if (len < p_mgmq->r_buffer_offs) {
         strncpy((char *) (p_mgmq->r_buffer + (p_mgmq->r_buffer_offs - len)), buffer, len);
         mg_wsmq_send(p_request, (char *) (p_mgmq->r_buffer + (p_mgmq->r_buffer_offs - len)), size);
      }
      else {
         mg_wsmq_send(p_request, buffer, len);
         mg_wsmq_send(p_request, (char *) (p_mgmq->r_buffer + (p_mgmq->r_buffer_offs - len)), p_mgmq->r_buffer_len);
      }

   }
   else {
      strcpy(buffer + len, deod);
      mg_wsmq_send(p_request, buffer, size);
   }


   if (p_mgmq->r_buffer) {

      mg_free((void *) p_mgmq->r_buffer, "mg_wsmq:2");
      p_mgmq->r_buffer = NULL;
      p_mgmq->r_buffer_size = 0;
      p_mgmq->r_buffer_offs = 0;
      p_mgmq->r_buffer_len = 0;
   }

   core_data.ws_activity ++;

   mg_buf_free(&request);
   mode = 0;

   if (!mode) {
      goto mg_wsmq_next_request;
   }

mg_wsmq_exit2:

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_wsmq: %x", code);
      mg_log_event(buffer, "Error Condition");

      mg_buf_free(&request);

   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

  
}


int mg_wsmq_send(MGREQ *p_request, char *buffer, unsigned long size)
{
   int n;
   unsigned long sent;

#ifdef _WIN32
__try {
#endif

   if (size < 1)
      return 0;

   sent = 0;
   for (;;) {
      n = MGNET_SEND(p_request->cli_sockfd, buffer + sent, size - sent, 0);

      if (n < 1)
         break;

      sent += n;
      if (sent == size) {
         n = sent;
         break;
      }
   }

   return n;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_wsmq_send: %x", code);
      mg_log_event(buffer, "Error Condition");

   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


#if defined(MG_BDB)
int mg_bdb(MGREQ *p_request, int mode)
{
   short t_mode, eod, lpn, offs, term_sent, recv_called, mem_alloc, m_ibm_mq, m_php, m_go, m_jsp, m_aspx, m_python, m_ruby, m_perl, m_apache, m_cgi;
   int closedown, attempt_no;
   int n, chndle, shndle, read_stat, errorcode, timeout, no_retry;
   int context;
   unsigned long len, max, actual_size, tbuffer_size, rsize, clen, rlen, req_no;
   char m_server_name[64], uci[256], cli_ip_address[32];
   char buffer[256], obuffer[256], m_client_name[32];
   char cmnd;
   char *p, *pz, *p_content;
   int size, hlen, hlen1, tbuffer_offset, tbuffer_actual_offset, tbuffer_offset_base;
   unsigned char *ibuffer;
   unsigned char *tbuffer;
   char header[256];
   MEMOBJ debug, *p_debug;
   int ret;
   char cdata[32100];
   int maxk;
   char * keys[32];
   DB *pdb;
   DBC *pdbc;
   DBT key, data;

   pdb = (DB *) mg_connections[chndle]->p_server->pdb;
   pdbc = (DBC *) mg_connections[chndle]->p_server->pdbc;

   memset(&key, 0, sizeof(DBT));
   memset(&data, 0, sizeof(DBT));

   mg_connections[chndle]->activity ++;

   p = strstr(ibuffer, deod);
   if (p)
      *p = '\0';

   p = ibuffer;
   maxk = 0;
   while ((p = strstr(p, drec))) {
      *p = '\0';
      p += 3;
      keys[maxk ++] = p;
   }

   if (cmnd == 'S') {
     key.data = keys[1];
     key.size = (u_int32_t) strlen(keys[1]) + 1;

     data.data = keys[2];
     data.size = (u_int32_t) strlen(keys[2]) + 1;

      ret = pdb->put(pdb, 0, &key, &data, 0);

      if (ret != 0) {
         p = db_strerror(ret);
         sprintf(obuffer, "%sBDB Database Error (set) : %d (%s) %s", MG_RECV_HEAD_ERROR, ret, p ? p : "Unknown error", deod);
         mg_insert_error_size(obuffer);

		   mg_log_buffer(obuffer + MG_RECV_HEAD, strlen(obuffer) - MG_RECV_HEAD, "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);

      }
      else {
         sprintf(obuffer, "%s", deod);
		   mg_log_buffer(obuffer, strlen(obuffer), "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
   }
   else if (cmnd == 'G') {
      key.data = keys[1];
      key.size = (u_int32_t) strlen(keys[1]) + 1;
      data.flags = DB_DBT_MALLOC;
      ret = pdb->get(pdb, 0, &key, &data, 0);
      if (ret != 0) {
         p = db_strerror(ret);
         sprintf(obuffer, "%sBDB Database Error (get) : %d (%s) %s", MG_RECV_HEAD_ERROR, ret, p ? p : "Unknown error", deod);
         mg_insert_error_size(obuffer);
		   mg_log_buffer(obuffer + MG_RECV_HEAD, strlen(obuffer) - MG_RECV_HEAD, "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
      else {
         strncpy(obuffer, data.data, data.size);
         obuffer[data.size] = '\0';
         strcat(obuffer, deod);
		   mg_log_buffer(obuffer, strlen(obuffer), "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
   }
   else if (cmnd == 'K') {
      key.data = keys[1];
      key.size = (u_int32_t) strlen(keys[1]) + 1;
      data.flags = DB_DBT_MALLOC;
      ret = pdb->del(pdb, 0, &key, 0);
      if (ret != 0) {
         p = db_strerror(ret);
         sprintf(obuffer, "%sBDB Database Error (kill) : %d (%s) %s", MG_RECV_HEAD_ERROR, ret, p ? p : "Unknown error", deod);
         mg_insert_error_size(obuffer);
		   mg_log_buffer(obuffer + MG_RECV_HEAD, strlen(obuffer) - MG_RECV_HEAD, "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
      else {
         sprintf(obuffer, "%s", deod);
		   mg_log_buffer(obuffer, strlen(obuffer), "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
   }
   else if (cmnd == 'O') {
      key.data = keys[1];
      key.size = (u_int32_t) strlen(keys[1]) + 1;
      data.flags = DB_DBT_MALLOC;

      if (!pdbc) {
         pdb->cursor(pdb,0, &pdbc, 0);
         mg_connections[chndle]->p_server->pdbc = (void *) pdbc;
      }
      strcpy(cdata, keys[1]);

      if (strlen(keys[1]) == 0)
         ret = pdbc->c_get(pdbc, &key, &data, DB_FIRST);
      else {
         ret = pdbc->c_get(pdbc, &key, &data, DB_NEXT | DB_SET_RANGE);

         sprintf(obuffer, "BDB Database ************** %d %s %d %s", key.size, key.data, strlen(cdata), cdata);
		   mg_log_buffer(obuffer, strlen(obuffer), "cmcmcm: BDB output");

         if (ret == 0 && key.size > 0 && strlen(cdata) == (key.size - 1) && !strncmp(key.data, cdata, key.size)) {
            ret = pdbc->c_get(pdbc, &key, &data, DB_NEXT);
         }
      }

      if (ret == DB_NOTFOUND) {
         strcpy(obuffer, deod);
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
      else if (ret != 0) {
         p = db_strerror(ret);
         sprintf(obuffer, "%sBDB Database Error (order) : %d (%s) %s", MG_RECV_HEAD_ERROR, ret, p ? p : "Unknown error", deod);
         mg_insert_error_size(obuffer);
		   mg_log_buffer(obuffer + MG_RECV_HEAD, strlen(obuffer) - MG_RECV_HEAD, "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
      else {
         strncpy(obuffer, key.data, key.size);
         obuffer[key.size] = '\0';
         strcat(obuffer, deod);
         mg_log_buffer(obuffer, strlen(obuffer), "cmcmcm: BDB output");
         MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
      }
   }
   else {
      sprintf(obuffer, "%sBDB Database Error : Unsupported function %s", MG_RECV_HEAD_ERROR, deod);
      mg_insert_error_size(obuffer);
      mg_log_buffer(obuffer + MG_RECV_HEAD, strlen(obuffer) - MG_RECV_HEAD, "cmcmcm: BDB output");
      MGNET_SEND(p_request->cli_sockfd, obuffer, strlen(obuffer), 0);
   }

   return 0;
}
#endif


