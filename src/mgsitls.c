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
#include "mgsitls.h"

#ifdef MG_SSL

int mg_tls_proxy_start(void)
{
   int n;
   char buf[256];
   THRCTRL thread_control;

#ifdef _WIN32
__try {
#endif

   if (!mg_ssl.ssl)
      return 0;

   if (!core_data.ssp_status)
      return 0;

   if (!core_data.ssp_port || core_data.ssp_port == core_data.sig_port) {
      T_SPRINTF(buf, "The SSL Proxy will operate through the TCP port of the main MGateway SIG service (%d)", core_data.sig_port);
      mg_log_event(buf, "SSL Proxy Information");
      return 0;
   }


   thread_control.stack_size = 0;

   n = mg_thread_create((LPTHRCTRL) &thread_control, (MG_THR_START_ROUTINE) mg_tls_proxy, (void *) NULL);

   return n;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_proxy_start: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tls_proxy_stop(int tcp_port)
{
   int n;

#ifdef _WIN32
__try {
#endif

   if (!mg_ssl.ssl)
      return 0;

   if (!tcp_port)
      tcp_port = core_data.ssp_port;

   if (!tcp_port)
      return 0;

   if (tcp_port == core_data.sig_port)
      return 0;

   n = mg_tls_service_stop(tcp_port, &(core_data.ssp_closedown));

   return n;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_proxy_stop: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



MG_THR_TYPE mg_tls_proxy(void *arg)
{
   int result, cycle, errorno, e_count, reset_no, attempt_no;
   int n, err;
   SOCKET listen_sd;
   SOCKET sd;
   unsigned short tcp_port;
   struct sockaddr_in sa_serv;
   struct sockaddr_in sa_cli;
   socklen_mg client_len;
   char     buf [4096];

   unsigned long activity;

#ifdef _WIN32
   static WORD VersionRequested     = 0;
   WSADATA wsadata;
#endif

   THRCTRL thread_control[512];
   THRDATA thr_data[512];

   result = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   mg_tls_service_sigset();

   tcp_port = core_data.ssp_port;

   err = 0;
   n = 0;
   cycle = 0;
   errorno = 0;
   e_count = 0;
   reset_no = 0;
   attempt_no = 0;
   activity = 0;


#ifdef _WIN32
/*
   n = core_data.wsastartup;
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Proxy Error");
      return MG_THR_RETURN;
   }
*/

   VersionRequested = 0x101;
   n = MGNET_WSASTARTUP(VersionRequested, &wsadata);
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Proxy Error");
      MGNET_WSACLEANUP();
      return MG_THR_RETURN;
   }

#endif

   /* ----------------------------------------------- */
   /* Prepare TCP socket for receiving connections */

   listen_sd = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);

   if (listen_sd < 0) {
      T_SPRINTF(buf, "SSL start-up error - Socket Error - code %d", 6);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
   }

  
   memset (&sa_serv, '\0', sizeof(sa_serv));
   sa_serv.sin_family      = AF_INET;
   sa_serv.sin_addr.s_addr = INADDR_ANY;
   sa_serv.sin_port        = MGNET_HTONS((unsigned short) tcp_port);          /* Server Port number */
  
   err = MGNET_BIND(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));

   if (SOCK_ERROR(err)) {

/*
#if !defined(_WIN32)
      if (attempt_no < 11) {
         attempt_no ++;
         sleep(1);
         goto startup_try_again;
      }
#endif
*/

      errorno = (int) mg_get_last_error(0);

      T_SPRINTF(buf, "Can't bind to local address on TCP port %d %d", tcp_port, errorno);
      mg_log_event(buf, "SSL Proxy: Startup Error");

#ifdef _WIN32
      MGNET_CLOSESOCKET(listen_sd);
/*
      MGNET_WSACLEANUP();
*/
#else  /* UNIX or VMS */
      close(listen_sd);
#endif
      return MG_THR_RETURN;

   }
   else {
      T_SPRINTF(buf, "SSL Proxy listening on TCP port %d", tcp_port);
      mg_log_event(buf, "SSL Proxy: Information");
   }

	     
   /* Receive a TCP connection. */
	     
   err = listen (listen_sd, 5);

   if (err < 0) {
      T_SPRINTF(buf, "SSL start-up error - Listen Error - code %d", 8);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
   }
  

   for (;;) {

#if !defined(_WIN32)
      bzero((char *) &sa_cli, sizeof(sa_cli));
#endif

      client_len = sizeof(sa_cli);
      sd = accept (listen_sd, (struct sockaddr*) &sa_cli, (socklen_mg *) &client_len);

      if (SOCK_ERROR(sd)) {

         errorno = (int) mg_get_last_error(0);

         e_count ++;

#ifdef _WIN32
         if (errorno == 4) {
            reset_no = 999;
            break;
         }
#endif

         if (e_count == 1) {
            sprintf(buf, "Accept error: %d", errorno);
            mg_log_event(buf, "SSL Service: Web-Server-Side Connection Error");
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

      if (core_data.ssp_closedown) {
         T_SPRINTF(buf, "SSL Proxy Server closed-down on TCP port %d", tcp_port);
         mg_log_event(buf, "SSL Proxy: Information");
         core_data.ssp_closedown = 0;
         break;
      }

      if (cycle > 500)
         cycle = 0;

      thr_data[cycle].tcp_port = tcp_port;
      if (inet_ntoa(sa_serv.sin_addr))
         strcpy(thr_data[cycle].srv_addr, inet_ntoa(sa_serv.sin_addr));
      else
         strcpy(thr_data[cycle].srv_addr, "");
      if (inet_ntoa(sa_cli.sin_addr))
         strcpy(thr_data[cycle].cli_addr, inet_ntoa(sa_cli.sin_addr));
      else
         strcpy(thr_data[cycle].cli_addr, "");
      thr_data[cycle].cli_port = sa_cli.sin_port;
      thr_data[cycle].sockfd = listen_sd;
      thr_data[cycle].newsockfd = sd;

      thr_data[cycle].use_ssl = 0;

      activity ++;
      thr_data[cycle].activity = activity;
      thr_data[cycle].queued = 0;


      /* Concurrent Server */

      n = mg_thread_create((LPTHRCTRL) &(thread_control[cycle]),
                          (MG_THR_START_ROUTINE) mg_tls_proxy_child,
                          (void *) &(thr_data[cycle]));

      if (n != 0) {

         attempt_no ++;

         sprintf(buf, "Thread creation error %d", n);
         mg_log_event(buf, "SSL Service: Error");

      }

      cycle ++;

   }

   /* Clean up. */

#if _WIN32
   MGNET_CLOSESOCKET(listen_sd);
/*
   MGNET_WSACLEANUP();
*/
#else
   close (listen_sd);
#endif

   return MG_THR_RETURN;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_proxy: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return MG_THR_RETURN;

}
#endif

}


MG_THR_TYPE mg_tls_proxy_child(void *arg)
{

   int result;
   SOCKET sockfd, newsockfd;
   char     buf[4096];
   unsigned long activity;

   MGREQ ecb;
   LPMGREQ p_request;

   result = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   mg_tls_service_sigset();

   result = 0;
   p_request = &ecb;

   sockfd = ((LPTHRDATA) arg)->sockfd;
   newsockfd = ((LPTHRDATA) arg)->newsockfd;
   p_request->tcp_port = ((LPTHRDATA) arg)->tcp_port;
   strcpy(p_request->cli_addr, ((LPTHRDATA) arg)->cli_addr);
   p_request->cli_port, ((LPTHRDATA) arg)->cli_port;
   strcpy(p_request->srv_addr, ((LPTHRDATA) arg)->srv_addr);
   activity = ((LPTHRDATA) arg)->activity;

   p_request->cli_sockfd = newsockfd;

   p_request->use_ssl = ((LPTHRDATA) arg)->use_ssl;

   p_request->request_source = MG_RS_HTTP;

/*
   p_request->c_recv_size = 0;
   p_request->c_recv_chunk_size = 0;
   p_request->c_recv_ptr = 0;
   p_request->c_recv_wsptr = 0;
*/
   strcpy(buf, "");
   mg_tls_proxy_request(p_request, buf);


#ifdef _WIN32
   MGNET_CLOSESOCKET(newsockfd);
#else  /* UNIX or VMS */
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
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_proxy_child: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return MG_THR_RETURN;

}
#endif

}


int mg_tls_proxy_request(MGREQ *p_request, char *partial_header)
{
   short physical_ip;
   int result, n, n1, n2, err, len, hlen, clen, tlen, size;
   SOCKET sd;
   int connected;
   unsigned short tcp_port, tcp_port1, tcp_port_def;
   unsigned long inetaddr;
   struct sockaddr_in sa, ca;
   struct hostent *hp;
   struct in_addr **pptr;
   SSL_CTX* ctx;
   SSL*     ssl;
   X509*    server_cert;
   char*    str;
   char     *p_general;
   char     buf[4096], buf1[4096], error_message[256], cif[256], nam[256], sub[256], iss[256];
   char     ip_address[64], ip_address1[64], ansi_ip_address[128];
   unsigned char chr;
   unsigned char *p_request;
   SSL_METHOD *meth;

#ifdef _WIN32
   static WORD VersionRequested     = 0;
   WSADATA wsadata;
#endif

   result = 0;

#ifdef _WIN32
__try {
#endif


   ssl = NULL;
   err = 0;
   n = 0;

   tcp_port_def = 443;

   tcp_port = 0;
   strcpy(ip_address, "");
   tcp_port1 = 0;
   strcpy(ip_address1, "");

   *error_message = '\0';
   *cif = '\0';
   *nam = '\0';
   *sub = '\0';
   *iss = '\0';

   p_general = NULL;
   hlen = 0;
   clen = 0;

   len = (int) T_STRLEN(partial_header);
   strcpy(buf, partial_header);

   for (;;) {

      n = MGNET_RECV(p_request->cli_sockfd, buf + len, 2048, 0);

      if (n < 1)
         break;
      len += n;
      buf[len] = '\0';
      if (p_general = strstr(buf, "\r\n\r\n")) {
         chr = *(p_general + 4);
         *(p_general + 4) = '\0';
         hlen = (int) T_STRLEN(buf);
         strcpy(buf1, buf);
         *(p_general + 4) = chr;

         mg_lcase(buf1);

         for (n1 = 0, n2 = 0; buf1[n1]; n1 ++) {
            if (buf1[n1] == ' ')
               continue;
            buf1[n2 ++] = buf1[n1];
         }
         buf1[n2] = '\0';
         p_general = strstr(buf1, "content-length:");
         if (p_general)
            clen = (int) strtol(p_general + 15, NULL, 10);

         p_general = strstr(buf1, "host:");

         if (p_general) {
            strncpy(ip_address, p_general + 5, 50);
            ip_address[50] = '\0';

            p_general = strstr(ip_address, "\r");
            if (p_general)
               *p_general = '\0';

            p_general = strstr(ip_address, ":");
            if (p_general) {
               n1 = (int) strtol(p_general + 1, NULL, 10);
               *p_general = '\0';
               if (n1)
                  tcp_port = n1;
            }
         }

         chr = buf1[15];
         buf1[15] = '\0';
         p_general = strstr(buf1, "https://");
         buf1[15] = chr;

         if (p_general) {
            strncpy(ip_address1, p_general + 8, 50);
            ip_address1[50] = '\0';

            p_general = strstr(ip_address1, "/");
            if (p_general)
               *p_general = '\0';

            p_general = strstr(ip_address1, ":");
            if (p_general) {
               n1 = (int) strtol(p_general + 1, NULL, 10);
               *p_general = '\0';
               if (n1)
                  tcp_port1 = n1;
            }
         }

         break;
      }

   }

   if (!T_STRLEN(ip_address))
      strcpy(ip_address1, ip_address);

   if (!tcp_port)
      tcp_port = tcp_port1;

   if (!tcp_port)
      tcp_port = tcp_port_def;


{
   char buffer[256];
   T_SPRINTF(buffer, "SSL connection using to server %s on port %d", ip_address, tcp_port);
   mg_log_event(buffer, "SSL Connection");
}


   tlen = hlen + clen;
   size = tlen + 32;
   if (size < CACHE_BUFFER)
      size = CACHE_BUFFER;

   p_request = (unsigned char *) mg_malloc(sizeof(char) * size, NULL);
   if (!p_request)
      goto mg_tls_proxy_requestExit1;

   strncpy((char *) p_request, buf, len);
   p_request[len] = '\0';

   if (clen) {
      tlen = 0;
      for (;;) {

         n = MGNET_RECV(p_request->cli_sockfd, p_request + len, tlen - len, 0);

         if (n < 1)
            break;
         len += n;
         if (len = tlen)
            break;
      }

   }
   p_request[len] = '\0';


   /* SSL */

   mg_SSL_library_init();
   meth = mg_SSLv2_client_method();
   mg_SSL_load_error_strings();
   ctx = mg_SSL_CTX_new (meth);

   if (!ctx) {
      T_SPRINTF(error_message, "SSL error - No Session - code %d", 21);
      goto mg_tls_proxy_requestExit2;

   }
 
#if 0

/* cli cert */

{
   char cert[64], key[64];

/*
   strcpy(cert, "c:/delegate/ksb_cert.pem");
   strcpy(key, "c:/delegate/ksb_priv_key.pem");
*/

   strcpy(cert, "c:/c/cachesp/ssls/cacert.pem");
   strcpy(key, "c:/c/cachesp/ssls/cakey.pem");


   if (mg_SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {

/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL start-up error - code %d", 3);
      mg_log_event(buf, "SSL Error", 0);
      return MG_THR_RETURN;
  }


   if (mg_SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {

/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL start-up error - code %d", 4);
      mg_log_event(buf, "SSL Error", 0);
      return MG_THR_RETURN;
  }

   if (!mg_SSL_CTX_check_private_key(ctx)) {

      T_SPRINTF(buf, "SSL start-up error - Private key does not match the certificate public key - code %d", 5);
      mg_log_event(buf, "SSL Error", 0);
      return MG_THR_RETURN;

  }

}

#endif

#ifdef _WIN32
/*
   n = core_data.wsastartup;
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Client Error", 0);

      goto mg_tls_proxy_requestExit3;
   }
*/

   VersionRequested = 0x101;
   n = MGNET_WSASTARTUP(VersionRequested, &wsadata);
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Client Error");

      goto mg_tls_proxy_requestExit3;
   }

#endif

   inetaddr = MGNET_INET_ADDR(ip_address);
   strcpy(ansi_ip_address, ip_address);

   physical_ip = 0;
   if (isdigit(ansi_ip_address[0])) {
      char *p;

      if (p = strstr(ansi_ip_address, ".")) {
         if (isdigit(*(++ p))) {
            if (p = strstr(p, ".")) {
               if (isdigit(*(++ p))) {
                  if (p = strstr(p, ".")) {
                     if (isdigit(*(++ p))) {
                        physical_ip = 1;
                     }
                  }
               }
            }
         }
      }
   }


   if (inetaddr == INADDR_NONE || !physical_ip) {

      if ((hp = MGNET_GETHOSTBYNAME((const char *) ip_address)) == NULL) {
         n = -2;
         if (core_data.log_errors == 1)
            mg_log_event("Invalid Host Name", "SSL Proxy Error");

         goto mg_tls_proxy_requestExit3;

      }

      pptr = (struct in_addr **) hp->h_addr_list;

      connected = 0;

      for (; *pptr != NULL; pptr ++) {

         sd = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);
         if (INVALID_SOCK(sd)) {
            n = -2;
            if (core_data.log_errors == 1)
               mg_log_event("Invalid Socket", "SSL Proxy Error");

            connected = -1;
            break;
         }

#if !defined(_WIN32)
         bzero((char *) &ca, sizeof(ca));
         bzero((char *) &sa, sizeof(sa));
#endif

         ca.sin_family = AF_INET;
         sa.sin_port = MGNET_HTONS((unsigned short) tcp_port);

         ca.sin_addr.s_addr = MGNET_HTONL(INADDR_ANY);
         ca.sin_port = MGNET_HTONS(0);
         n = MGNET_BIND(sd, (xLPSOCKADDR) &ca, sizeof(ca));
         if (SOCK_ERROR(n)) {
            n = -3;
            if (core_data.log_errors == 1)
               mg_log_event("Cannot bind to Socket", "SSL Proxy Error");


            connected = -1;
            break;

         }

         sa.sin_family = AF_INET;
         sa.sin_port = MGNET_HTONS((unsigned short) tcp_port);

         memcpy(&sa.sin_addr, *pptr, sizeof(struct in_addr));

         n = MGNET_CONNECT(sd, (xLPSOCKADDR) &sa, sizeof(sa));
         if (SOCK_ERROR(n)) {
            char message[256];

            n = (int) mg_get_last_error(0);
            mg_get_error_message(n, message, 250, 0);
            T_SPRINTF(buf, "Cannot Connect to Server (%s:%d): Error Code: %d (%s)", ip_address, tcp_port, n, message);
            n = -5;
            if (core_data.log_errors == 1)
               mg_log_event(buf, "SSL Proxy Error");

#ifdef _WIN32
               MGNET_CLOSESOCKET(sd);
#else  /* UNIX or VMS */
               close(sd);
#endif
            continue;
         }
         else {
            connected = 1;
            break;
         }
      }

      if (connected == -1)
         goto mg_tls_proxy_requestExit3;
      if (connected == -2)
         goto mg_tls_proxy_requestExit4;

      if (!connected) {
         if (core_data.log_errors == 1)
            mg_log_event("Failed to find the Server via a DNS Lookup", "SSL Proxy Error");

         goto mg_tls_proxy_requestExit4;

      }
   }

   else {

      /* ----------------------------------------------- */
      /* Create a socket and connect to server using normal socket calls. */
  
      sd = MGNET_SOCKET(AF_INET, SOCK_STREAM, 0);

      if (sd < 0) {
         T_SPRINTF(buf, "Socket Error - code %d", 8);
         mg_log_event(buf, "SSL Proxy Error");

         goto mg_tls_proxy_requestExit3;

      }


      memset (&sa, '\0', sizeof(sa));
      sa.sin_family      = AF_INET;
      sa.sin_addr.s_addr = MGNET_INET_ADDR (ip_address);   /* Server IP */
      sa.sin_port        = MGNET_HTONS     ((unsigned short) tcp_port);          /* Server Port number */
  
      err = MGNET_CONNECT(sd, (struct sockaddr*) &sa, sizeof(sa));

      if (err < 0) {
         if (core_data.log_errors == 1) {

            T_SPRINTF(buf, "Connection Error - code %d", 8);
            mg_log_event(buf, "SSL Proxy Error");
         }

         goto mg_tls_proxy_requestExit4;
/*
         mg_free(p_request, NULL);
*/

      }
   }


   /* ----------------------------------------------- */
   /* Now we have TCP connection. Start SSL negotiation. */
  
   ssl = mg_SSL_new (ctx);

   if (!ssl) {
      if (core_data.log_errors == 1) {

         T_SPRINTF(buf, "SSL error - No Session - code %d", 2);
         mg_log_event(buf, "SSL Client Error");
      }
      goto mg_tls_proxy_requestExit4;
   }

   mg_SSL_set_fd (ssl, (int) sd);
   err = mg_SSL_connect (ssl);

   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL error - SSL_connect - code %d", 15);
      mg_log_event(buf, "SSL Error");
      goto mg_tls_proxy_requestExit4;
   }

    
   /* Following two steps are optional and not required for
      data exchange to be successful. */
  
   /* Get the cipher - opt */
  
   strcpy(cif, mg_SSL_get_cipher(ssl));

   /* Get server's certificate (note: beware of dynamic allocation) - opt */

#if 1

   server_cert = mg_SSL_get_peer_certificate (ssl);

   if (!server_cert) {
      T_SPRINTF(error_message, "SSL error - No peer certificate - code %d", 3);
      goto mg_tls_proxy_requestExit4;
   }

   str = mg_X509_NAME_oneline (mg_X509_get_subject_name (server_cert),0,0);

   if (!str) {
      T_SPRINTF(error_message, "SSL Client Certificate error - No issuer name - code %d", 11);
      goto mg_tls_proxy_requestExit4;
   }

   strcpy(sub, str);
   strcpy(nam, str);

   mg_CRYPTO_free(str);

   str = mg_X509_NAME_oneline (mg_X509_get_issuer_name  (server_cert),0,0);

   if (!str) {
      T_SPRINTF(error_message, "SSL Client Certificate error - No issuer name - code %d", 11);
      goto mg_tls_proxy_requestExit4;
   }

   strcpy(iss, str);


   mg_CRYPTO_free(str);

   /* We could do all sorts of certificate verification stuff here before
      deallocating the certificate. */

   mg_X509_free (server_cert);

#endif

   /* --------------------------------------------------- */
   /* DATA EXCHANGE - Send a message and receive a reply. */


/*
   strcpy(buf, "GET https://www2.ggn.net/cgi-bin/ssl HTTP/1.1\r\nHost: www2.ggn.net\r\n\r\n");
   err = mg_SSL_write (ssl, buf, T_STRLEN(buf));
*/

   err = mg_SSL_write (ssl, p_request, tlen);

   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/

      T_SPRINTF(error_message, "SSL error - Write - code %d", 15);
      goto mg_tls_proxy_requestExit4;
   }

  
   err = mg_SSL_read (ssl, buf, sizeof(buf) - 1);

   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(error_message, "SSL error - SSL_read - code %d", 17);

      goto mg_tls_proxy_requestExit4;
   }

   buf[err] = '\0';

   n = MGNET_SEND(p_request->cli_sockfd, buf, err, 0);

   mg_SSL_shutdown (ssl);  /* send SSL/TLS close_notify */

   result = 1;

   if (core_data.verbose >= 3) {

      T_SPRINTF(buf, "SSL connection to %s; port %d; Using %s", ip_address,  tcp_port, cif);
      if (T_STRLEN(nam)) {
         char cert[1024];

         T_SPRINTF(cert, ";  Server Certificate: Name=%s; Issuer=%s", nam, iss);
         strcat(buf, cert);
      }
      else
         strcat(buf, ";  Server Certificate Not Supplied");

      mg_log_event(buf, "SSL Proxy Connection Information");

   }

  /* Clean up. */

mg_tls_proxy_requestExit4:

#ifdef _WIN32
   MGNET_CLOSESOCKET(sd);
#else
   close (sd);
#endif

   if (ssl)
      mg_SSL_free (ssl);

mg_tls_proxy_requestExit3:

   mg_SSL_CTX_free (ctx);

#ifdef _WIN32
/*
   MGNET_WSACLEANUP();
*/
#endif

mg_tls_proxy_requestExit2:

   mg_free(p_request, NULL);

mg_tls_proxy_requestExit1:

   if (T_STRLEN(error_message)) {
      result = 0;
      mg_log_event(error_message, "SSL Proxy Error");
   }

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_proxy_request: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return 0;

}
#endif

}


int mg_tls_server_start(void)
{
   int n;
   char buf[256];
   THRCTRL thread_control;
   FILE *fp;

#ifdef _WIN32
__try {
#endif

   if (!mg_ssl.ssl)
      return 0;

   if (!core_data.ssls_status)
      return 0;

   if (!core_data.ssls_port) {
      mg_log_event("The TCP Port for the server is not specified in the configuration", "SSL Server Error");
      return 0;
   }

   if (!T_STRLEN(core_data.ssls_cert_file)) {
      mg_log_event("The Server Certificate file the server is not specified in the configuration", "SSL Server Error");
      return 0;
   }

   if (!T_STRLEN(core_data.ssls_cert_key_file)) {
      mg_log_event("The Server Certificate Key file the server is not specified in the configuration", "SSL Server Error");
      return 0;
   }

   if (core_data.ssls_verify_client == 2 && !T_STRLEN(core_data.ssls_ca_file) && !T_STRLEN(core_data.ssls_ca_path)) {
      mg_log_event("The Server requires a valid client certificate but no CA file or path is specified", "SSL Server Error");
      return 0;
   }

   fp = NULL;
   fp = fopen(core_data.ssls_cert_file, "r");
   if (!fp) {
      T_SPRINTF(buf, "The Server Certificate file (%s) does not exist", core_data.ssls_cert_file);
      mg_log_event(buf, "SSL Server Error");
      return 0;
   }
   fclose(fp);

   fp = NULL;
   fp = fopen(core_data.ssls_cert_key_file, "r");
   if (!fp) {
      T_SPRINTF(buf, "The Server Certificate Key file (%s) does not exist", core_data.ssls_cert_key_file);
      mg_log_event(buf, "SSL Server Error");
      return 0;
   }
   fclose(fp);

   if (T_STRLEN(core_data.ssls_ca_file)) {
      fp = NULL;
      fp = fopen(core_data.ssls_ca_file, "r");
      if (!fp) {
         T_SPRINTF(buf, "The CA file (%s) does not exist", core_data.ssls_ca_file);

         if (core_data.ssls_verify_client == 2) {
            mg_log_event(buf, "SSL Server Error");
            return 0;
         }
         else
            mg_log_event(buf, "SSL Server Warning");
      }
      fclose(fp);
   }

   thread_control.stack_size = 0;

   n = mg_thread_create((LPTHRCTRL) &thread_control, (MG_THR_START_ROUTINE) mg_tls_server, (void *) NULL);

   return n;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_server_start: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_tls_server_stop(int tcp_port)
{
   int n;

#ifdef _WIN32
__try {
#endif

   if (!mg_ssl.ssl)
      return 0;

   if (!tcp_port)
      tcp_port = core_data.ssls_port;

   if (!tcp_port)
      return 0;

   n = mg_tls_service_stop(tcp_port, &(core_data.ssls_closedown));

   return n;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_server_stop: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



MG_THR_TYPE mg_tls_server(void *arg)
{
   int result, cycle, errorno, e_count, reset_no, attempt_no;
   int n, err;
   SOCKET listen_sd;
   SOCKET sd;
   unsigned short tcp_port;
   struct sockaddr_in sa_serv;
   struct sockaddr_in sa_cli;
   socklen_mg client_len;
   SSL_CTX* ctx;
   char     buf[4096];
   char     cert[128], key[128];
   char     *p_ssls_ca_file, *p_ssls_ca_path;

   unsigned long activity;

   SSL_METHOD *meth;

#ifdef _WIN32
   static WORD VersionRequested     = 0;
   WSADATA wsadata;
#endif

   THRCTRL thread_control[512];
   THRDATA thr_data[512];

   result = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   mg_tls_service_sigset();

/*
   tcp_port = 443;
   strcpy(cert, "c:/c/cachesp/ssls/cacert.pem");
   strcpy(key, "c:/c/cachesp/ssls/cakey.pem");
*/


   tcp_port = core_data.ssls_port;
   strcpy(cert, core_data.ssls_cert_file);
   strcpy(key, core_data.ssls_cert_key_file);


   err = 0;
   n = 0;
   cycle = 0;
   errorno = 0;
   e_count = 0;
   reset_no = 0;
   attempt_no = 0;
   activity = 0;
   errorno = 0;

  /* SSL preliminaries. We keep the certificate and key with the context. */

   mg_SSL_load_error_strings();
   mg_SSL_library_init();
   meth = mg_SSLv23_server_method();
   ctx = mg_SSL_CTX_new (meth);
   if (!ctx) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL start-up error - code %d", 2);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
  }
  

   if (mg_SSL_CTX_use_certificate_file(ctx, cert, SSL_FILETYPE_PEM) <= 0) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL start-up error - code %d", 3);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
  }


   if (mg_SSL_CTX_use_PrivateKey_file(ctx, key, SSL_FILETYPE_PEM) <= 0) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL start-up error - code %d", 4);
      mg_log_event(buf, "SSL Error");
  }

   if (!mg_SSL_CTX_check_private_key(ctx)) {

      T_SPRINTF(buf, "SSL start-up error - Private key does not match the certificate public key - code %d", 5);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;

  }

   /* Load the CAs we trust*/ 

   if (T_STRLEN(core_data.ssls_ca_file))
      p_ssls_ca_file = core_data.ssls_ca_file;
   else
      p_ssls_ca_file = NULL;

   if (T_STRLEN(core_data.ssls_ca_path))
      p_ssls_ca_path = core_data.ssls_ca_path;
   else
      p_ssls_ca_path = NULL;

   if (p_ssls_ca_file || p_ssls_ca_path) {
      n = mg_SSL_CTX_load_verify_locations(ctx, p_ssls_ca_file, p_ssls_ca_path);
      if (!n) {
         if (core_data.ssls_verify_client == 2) {
            mg_log_event("Unable to configure verify locations for client authentication.  Details to follow ...", "SSL Server Error");
            mg_tls_log_error();
            return MG_THR_RETURN;
         }
         else {
            mg_log_event("Unable to configure verify locations for client authentication.  Details to follow ...", "SSL Server Warning");
            mg_tls_log_error();
         }
      }
   }

   /*
    * Let the verify_callback catch the verify_depth error so that we get
    * an appropriate error in the logfile.
    */

   mg_SSL_CTX_set_verify_depth(ctx, core_data.ssls_verify_depth + 1);


   /*

   The verification of certificates can be controlled by a set of logically or'ed mode flags: 

      SSL_VERIFY_NONE

         Server mode:   the server will not send a client certificate request to the client, so the client will not send a certificate. 
         Client mode:   if not using an anonymous cipher (by default disabled), the server will send a certificate which will be checked.
                        The result of the certificate verification process can be checked after the TLS/SSL handshake using the 
                        SSL_get_verify_result(3) function. The handshake will be continued regardless of the verification result. 

      SSL_VERIFY_PEER

         Server mode:   the server sends a client certificate request to the client. The certificate returned (if any) is checked.
                        If the verification process fails, the TLS/SSL handshake is immediately terminated with an alert message
                        containing the reason for the verification failure. The behaviour can be controlled by the additional
                        SSL_VERIFY_FAIL_IF_NO_PEER_CERT and SSL_VERIFY_CLIENT_ONCE flags. 
         Client mode:   the server certificate is verified. If the verification process fails, the TLS/SSL handshake is immediately
                        terminated with an alert message containing the reason for the verification failure. If no server certificate
                        is sent, because an anonymous cipher is used, SSL_VERIFY_PEER is ignored. 

      SSL_VERIFY_FAIL_IF_NO_PEER_CERT

         Server mode:   if the client did not return a certificate, the TLS/SSL handshake is immediately terminated with a
                        ``handshake failure'' alert. This flag must be used together with SSL_VERIFY_PEER. 
         Client mode:   ignored 

      SSL_VERIFY_CLIENT_ONCE

         Server mode:   only request a client certificate on the initial TLS/SSL handshake. Do not ask for a client certificate again
                        in case of a renegotiation. This flag must be used together with SSL_VERIFY_PEER. 
         Client mode:   ignored 
*/


   if (core_data.ssls_verify_client == 0)
      mg_SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, mg_tls_verify_callback);
   else if (core_data.ssls_verify_client == 1)
      mg_SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, mg_tls_verify_callback);
   else if (core_data.ssls_verify_client == 2)
      mg_SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, mg_tls_verify_callback);
   else
      mg_SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, mg_tls_verify_callback);


#ifdef _WIN32
/*
   n = core_data.wsastartup;
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Server Error");
      return MG_THR_RETURN;
   }
*/

   VersionRequested = 0x101;
   n = MGNET_WSASTARTUP(VersionRequested, &wsadata);
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Server Error");
      MGNET_WSACLEANUP();
      return MG_THR_RETURN;
   }

#endif

   /* ----------------------------------------------- */
   /* Prepare TCP socket for receiving connections */

   listen_sd = MGNET_SOCKET (AF_INET, SOCK_STREAM, 0);

   if (listen_sd < 0) {
      T_SPRINTF(buf, "SSL start-up error - Socket Error - code %d", 6);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
   }

  
   memset (&sa_serv, '\0', sizeof(sa_serv));
   sa_serv.sin_family      = AF_INET;
   sa_serv.sin_addr.s_addr = INADDR_ANY;
   sa_serv.sin_port        = MGNET_HTONS ((unsigned short) tcp_port);          /* Server Port number */
  
   err = MGNET_BIND(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));

   if (SOCK_ERROR(err)) {

/*
#if !defined(_WIN32)
      if (attempt_no < 11) {
         attempt_no ++;
         sleep(1);
         goto startup_try_again;
      }
#endif
*/

      errorno = (int) mg_get_last_error(0);

      T_SPRINTF(buf, "Can't bind to local address on TCP port %d %d", tcp_port, errorno);
      mg_log_event(buf, "SSL Server: Startup Error");

#ifdef _WIN32
      MGNET_CLOSESOCKET(listen_sd);
/*
      MGNET_WSACLEANUP();
*/
#else  /* UNIX or VMS */
      close(listen_sd);
#endif
      return MG_THR_RETURN;

   }
   else {
      T_SPRINTF(buf, "SSL Server listening on TCP port %d", tcp_port);
      mg_log_event(buf, "SSL Server: Information");
   }

	     
   /* Receive a TCP connection. */
	     
   err = listen (listen_sd, 5);

   if (err < 0) {
      T_SPRINTF(buf, "SSL start-up error - Listen Error - code %d", 8);
      mg_log_event(buf, "SSL Error");
      return MG_THR_RETURN;
   }

   for (;;) {

#if !defined(_WIN32)
      bzero((char *) &sa_cli, sizeof(sa_cli));
#endif

      client_len = sizeof(sa_cli);
      sd = accept (listen_sd, (struct sockaddr*) &sa_cli, (socklen_mg *) &client_len);

      if (SOCK_ERROR(sd)) {

         errorno = (int) mg_get_last_error(0);

         e_count ++;

#ifdef _WIN32
         if (errorno == 4) {
            reset_no = 999;
            break;
         }
#endif

         if (e_count == 1) {
            sprintf(buf, "Accept error: %d", errorno);
            mg_log_event(buf, "SSL Service: Web-Server-Side Connection Error");
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

      if (core_data.ssls_closedown) {
         T_SPRINTF(buf, "SSL Server closed-down on TCP port %d", tcp_port);
         mg_log_event(buf, "SSL Server: Information");
         core_data.ssls_closedown = 0;
         break;
      }

      if (cycle > 500)
         cycle = 0;

      thr_data[cycle].tcp_port = tcp_port;
      if (inet_ntoa(sa_serv.sin_addr))
         strcpy(thr_data[cycle].srv_addr, inet_ntoa(sa_serv.sin_addr));
      else
         strcpy(thr_data[cycle].srv_addr, "");
      if (inet_ntoa(sa_cli.sin_addr))
         strcpy(thr_data[cycle].cli_addr, inet_ntoa(sa_cli.sin_addr));
      else
         strcpy(thr_data[cycle].cli_addr, "");
      thr_data[cycle].cli_port = sa_cli.sin_port;
      thr_data[cycle].sockfd = listen_sd;
      thr_data[cycle].newsockfd = sd;

      thr_data[cycle].ctx = ctx;
      thr_data[cycle].use_ssl = 1;

      activity ++;
      thr_data[cycle].activity = activity;
      thr_data[cycle].queued = 0;

      thr_data[cycle].thread_test = 0;

      /* Concurrent Server */

      n = mg_thread_create((LPTHRCTRL) &(thread_control[cycle]), (MG_THR_START_ROUTINE) mg_server_child, (void *) &(thr_data[cycle]));

      if (n != 0) {

         attempt_no ++;

         sprintf(buf, "Thread creation error %d", n);
         mg_log_event(buf, "SSL Service: Error");

      }

      cycle ++;

   }

#if _WIN32
   MGNET_CLOSESOCKET(listen_sd);
#else
   close (listen_sd);
#endif

  

   /* Clean up. */

#ifdef _WIN32
   MGNET_CLOSESOCKET(sd);
/*
   MGNET_WSACLEANUP();
*/
#else
   close (sd);
#endif

   mg_SSL_CTX_free (ctx);

   return MG_THR_RETURN;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_server: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return MG_THR_RETURN;

}
#endif

}



MG_THR_TYPE mg_tls_server_child(void *arg)
{
   int result;
   int n, err;
   SOCKET sockfd, newsockfd;
   char     buf [4096];
   unsigned long activity;
   MGREQ ecb;
   LPMGREQ p_request;

   result = 0;

#ifdef _WIN32
__try {
#endif

   mg_thread_detach();

   mg_tls_service_sigset();

   result = 0;
   p_request = &ecb;
   sockfd = ((LPTHRDATA) arg)->sockfd;
   newsockfd = ((LPTHRDATA) arg)->newsockfd;
   p_request->tcp_port = ((LPTHRDATA) arg)->tcp_port;
   p_request->cli_port = ((LPTHRDATA) arg)->cli_port;

   strcpy(p_request->cli_addr, ((LPTHRDATA) arg)->cli_addr);
   strcpy(p_request->srv_addr, ((LPTHRDATA) arg)->srv_addr);


   activity = ((LPTHRDATA) arg)->activity;

   p_request->cli_sockfd = newsockfd;

   p_request->ctx = ((LPTHRDATA) arg)->ctx;
   p_request->use_ssl = ((LPTHRDATA) arg)->use_ssl;

   n = 0;

   result = mg_tls_server_session(p_request);
   if (!result)
      goto mg_tls_server_childExit;

   /* DATA EXCHANGE - Receive message and send reply. */

{
   char buffer[256], buf1[256];

   strcpy(buffer, "");

   strcpy(buf, "");
   for (;;) {
   err = mg_SSL_read (p_request->ssl, buf1, 10);
/*
      T_SPRINTF(buffer, "SSL progress - Read - code %d", err);
      mg_log_event(buffer, "SSL Info", 0);
*/
   if (err > -1) {
      buf1[err] = '\0';
      strcat(buf, buf1);
/*
      mg_log_event(buf, "SSL Info", 0);
*/

   }
   if (strstr(buf, "\r\n\r\n"))
break;
   if (err < 1)
      break;
   }

}

/*
   err = mg_SSL_read (ssl, buf, sizeof(buf) - 1);
   buf[err] = '\0';
*/

   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL error - Read - code %d", 15);
      mg_log_event(buf, "SSL Error");
      goto mg_tls_server_childExit;
   }


{
   char buffer[256];
   T_SPRINTF(buffer, "Got %d chars:'%s'", err, buf);
   mg_log_event(buffer, "SSL Connection");
}
{
char buf[4096];

strcpy(buf, "I hear you.");
strcpy(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: Close\r\n\r\n<html>\r\n<head><title>Test Form</title></head>\r\n<h2>Test form from MG SSL Server</h2>\r\n\r\n");

   err = mg_SSL_write(p_request->ssl, buf, (int) T_STRLEN(buf));

strcpy(buf, "I hear you.");
strcpy(buf, "<p><h3>Another buffer load of stuff </h3>\r\n</html>\r\n");

   err = mg_SSL_write(p_request->ssl, buf, (int) T_STRLEN(buf));
}
   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL error - Write - code %d", 15);
      mg_log_event(buf, "SSL Error");
      goto mg_tls_server_childExit;
   }

  /* Clean up. */

mg_tls_server_childExit:

   if (p_request->ssl)
      mg_SSL_free (p_request->ssl);
/*
   mg_SSL_shutdown (ssl);
*/

#ifdef _WIN32
   MGNET_CLOSESOCKET(p_request->cli_sockfd);
#else
   close (p_request->cli_sockfd);
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
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_server_child: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return MG_THR_RETURN;

}
#endif

}



static int mg_tls_verify_callback(int preverify_ok, X509_STORE_CTX *ctx)
{
   char    buf[256], buffer[256];
   X509   *err_cert;
   int     err, depth;
   SSL    *ssl;
   MGSSLV *p_mgsslv;

#ifdef _WIN32
__try {
#endif

   err_cert = mg_X509_STORE_CTX_get_current_cert(ctx);
   err = mg_X509_STORE_CTX_get_error(ctx);
   depth = mg_X509_STORE_CTX_get_error_depth(ctx);

   /*
    * Retrieve the pointer to the SSL of the connection currently treated
    * and the application specific data stored into the SSL object.
    */

   ssl = mg_X509_STORE_CTX_get_ex_data(ctx, mg_SSL_get_ex_data_X509_STORE_CTX_idx());
   p_mgsslv = mg_SSL_get_ex_data(ssl, mgsslv_index);

   mg_X509_NAME_oneline(mg_X509_get_subject_name(err_cert), buf, 256);

   /*
    * Catch a too long certificate chain. The depth limit set using
    * SSL_CTX_set_verify_depth() is by purpose set to "limit+1" so
    * that whenever the "depth>verify_depth" condition is met, we
    * have violated the limit and want to log this error condition.
    * We must do it here, because the CHAIN_TOO_LONG error would not
    * be found explicitly; only errors introduced by cutting off the
    * additional certificates would be logged.
    */

   if (depth > p_mgsslv->verify_depth) {
      preverify_ok = 0;
      err = X509_V_ERR_CERT_CHAIN_TOO_LONG;
      mg_X509_STORE_CTX_set_error(ctx, err);
   }
   if (!preverify_ok) {

      if (p_mgsslv->verbose_mode >= 3) {
         T_SPRINTF(buffer, "SSL Server : Client Verification Error : ErrorNo=%d:%s; Depth=%d", err, mg_X509_verify_cert_error_string(err), depth);
         mg_log_event(buf, buffer);

      }
   }
   else {
      if (p_mgsslv->verbose_mode >= 3) {
         T_SPRINTF(buffer, "SSL Server : Client Verification Information : Depth=%d", depth);
         mg_log_event(buf, buffer);
      }
   }

   /*
    * At this point, err contains the last verification error. We can use
    * it for something special
    */
   if (!preverify_ok && (err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT)) {

      mg_X509_NAME_oneline(mg_X509_get_issuer_name(ctx->current_cert), buf, 256);

      T_SPRINTF(buffer, "Certificate Issuer=%s", (buf) ? buf: "(NULL)");
      mg_log_event(buffer, "SSL Server : Client Verification Error ");
   }

   if (p_mgsslv->always_continue)
      return 1;
   else
      return preverify_ok;


#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_verify_callback: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   mg_thread_exit();
   return 0;

}
#endif

}


int mg_tls_server_session(MGREQ *p_request)
{
   int      result, valid_cert;
   int      err;
   X509*    client_cert;
   char*    str;
   char     buf[4096], error_message[256], cif[256], nam[256], sub[256], iss[256];
   MGSSLV  mgsslv;

   result = 0;

#ifdef _WIN32
__try {
#endif

   valid_cert = 0;

   *error_message = '\0';
   *cif = '\0';
   *nam = '\0';
   *sub = '\0';
   *iss = '\0';

   /* ----------------------------------------------- */
   /* TCP connection is ready. Do server side SSL. */

   p_request->ssl = mg_SSL_new (p_request->ctx);

   if (!p_request->ssl) {
      T_SPRINTF(error_message, "SSL error - No Session - code %d", 21);
      goto mg_tls_server_sessionExit;
   }

   mg_SSL_set_fd (p_request->ssl, (int) p_request->cli_sockfd);


   mgsslv_index = mg_SSL_get_ex_new_index(0, "mgsslv index", NULL, NULL, NULL);


   mgsslv.verbose_mode = core_data.verbose;
   mgsslv.verify_depth = core_data.ssls_verify_depth;
   mgsslv.always_continue = 1;

   mg_SSL_set_ex_data(p_request->ssl, mgsslv_index, &mgsslv);


   err = mg_SSL_accept (p_request->ssl);

   if (err == -1) {
/*
      ERR_print_errors_fp(stderr);
*/
      T_SPRINTF(buf, "SSL error - SSL_accept - code %d", 22);
      goto mg_tls_server_sessionExit;
   }


   /* Get the cipher - opt */

   strcpy(cif, mg_SSL_get_cipher(p_request->ssl));

   /* Get client's certificate (note: beware of dynamic allocation) - opt */

   client_cert = mg_SSL_get_peer_certificate (p_request->ssl);

   valid_cert = 0;

   if (client_cert != NULL) {

      if (mg_SSL_get_verify_result(p_request->ssl) == X509_V_OK) {

         /* The client sent a certificate which verified OK */

         valid_cert = 1;

      }

      str = mg_X509_NAME_oneline (mg_X509_get_subject_name (client_cert), 0, 0);

      if (!str) {
         T_SPRINTF(error_message, "SSL Client Certificate error - No subject name - code %d", 11);
         goto mg_tls_server_sessionExit;
      }
      strcpy(sub, str);
      strcpy(nam, str);

      mg_CRYPTO_free(str);

      str = mg_X509_NAME_oneline (mg_X509_get_issuer_name  (client_cert), 0, 0);

      if (!str) {
         T_SPRINTF(error_message, "SSL Client Certificate error - No issuer name - code %d", 11);
         goto mg_tls_server_sessionExit;
      }

      strcpy(iss, str);

      mg_CRYPTO_free(str);

      /* We could do all sorts of certificate verification stuff here before
         deallocating the certificate. */
    
      mg_X509_free (client_cert);
   }


   result = 1;

mg_tls_server_sessionExit:

   if (T_STRLEN(error_message)) {
      result = 0;
      mg_log_event(error_message, "SSL Server Error");

   }
   else {

      if (valid_cert)
         result = 1;
      else {
         if (core_data.ssls_verify_client == 2)
            result = 0;
         else
            result = 1;
      }

      if (core_data.verbose >= 3) {

         T_SPRINTF(buf, "SSL connection from %s; port %d; Using %s", p_request->cli_addr,  p_request->cli_port, cif);
         if (T_STRLEN(nam)) {
            char cert[1024];

               T_SPRINTF(cert, ";  %s Client Certificate: Name=%s; Issuer=%s",  (valid_cert) ? "VALID": "INVALID", nam, iss);

            strcat(buf, cert);
         }
         else {
            strcat(buf, ";  Client Certificate Not ");
            if (core_data.ssls_verify_client)
               strcat(buf, "Supplied");
            else
               strcat(buf, "Requested");
         }

         mg_log_event(buf, "SSL Server Connection Information");
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
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_server_session: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;

}
#endif

}


int mg_tls_log_error(void)
{

   unsigned long e;
   char *err;
   char buffer[256];

#ifdef _WIN32
__try {
#endif


   while ((e = mg_ERR_get_error())) {

      err = mg_ERR_error_string(e, NULL);

      if (err) {

         T_SPRINTF(buffer, "%s (%lu)", err, e);
         mg_log_event(buffer, "SSL: Error Condition");

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
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_log_error: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_tls_service_stop(int tcp_port, int *stop_flag)
{
   int result, n, err;
   SOCKET sd;
   struct sockaddr_in sa;
   char     ip_address[64];

#ifdef _WIN32
   static WORD VersionRequested     = 0;
   WSADATA wsadata;
#endif

#ifdef _WIN32
__try {
#endif

   result = 0;
   strcpy(ip_address, "127.0.0.1");
   *stop_flag = 1;

#ifdef _WIN32
/*
   n = core_data.wsastartup;
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Server Error");
      goto mg_tls_service_stopExit1;
   }
*/

   VersionRequested = 0x101;
   n = MGNET_WSASTARTUP(VersionRequested, &wsadata);
   if (n != 0) {
      mg_log_event("Microsoft WSAStartup Failed", "SSL Server Error");
      goto mg_tls_service_stopExit1;
   }

#endif

   sd = MGNET_SOCKET (AF_INET, SOCK_STREAM, 0);

   if (sd < 0)
      goto mg_tls_service_stopExit2;


   memset (&sa, '\0', sizeof(sa));
   sa.sin_family      = AF_INET;
   sa.sin_addr.s_addr = MGNET_INET_ADDR (ip_address);   /* Server IP */
   sa.sin_port        = MGNET_HTONS     ((unsigned short) tcp_port);          /* Server Port number */
  
   err = MGNET_CONNECT(sd, (struct sockaddr*) &sa, sizeof(sa));

   if (err < 0)
      goto mg_tls_service_stopExit3;

   result = 1;

mg_tls_service_stopExit3:

#ifdef _WIN32
   MGNET_CLOSESOCKET(sd);
#else
   close(sd);
#endif

mg_tls_service_stopExit2:

/*
#ifdef _WIN32
   MGNET_WSACLEANUP();
#endif
*/

#ifdef _WIN32
mg_tls_service_stopExit1:
#endif

   *stop_flag = 0;

   return result;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_service_stop: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_tls_service_sigset(void)
{


#ifdef _WIN32
__try {
#endif

#if 0

   signal(SIGPIPE, SIG_IGN);

#if defined(AIX) || defined(AIX5)
   signal(SIGHUP, SIG_IGN);      /* 1 Hangup (POSIX).  */
#else
   signal(SIGHUP, mg_error_trap); /* 1 Hangup (POSIX).  */
#endif

#if 0
   signal(SIGINT, mg_error_trap); /* 2 Interrupt (ANSI).  */
   signal(SIGQUIT, mg_error_trap); /* 3 Quit (POSIX).  */
#endif
   signal(SIGILL, mg_error_trap); /* 4 Illegal instruction (ANSI).  */
   signal(SIGTRAP, mg_error_trap); /* 5 Trace trap (POSIX).  */
   signal(SIGABRT, mg_error_trap); /* 6 Abort (ANSI).  */
   signal(SIGIOT, mg_error_trap); /* 6 IOT trap (4.2 BSD).  */
   signal(SIGBUS, mg_error_trap); /* 7 BUS error (4.2 BSD).  */
   signal(SIGFPE, mg_error_trap); /*	8 Floating-point exception (ANSI).  */
#if 0
   signal(SIGKILL, mg_error_trap); /* 9 Kill, unblockable (POSIX).  */
   signal(SIGUSR1, SIG_IGN); /* 10 User-defined signal 1 (POSIX).  */
#endif
   signal(SIGSEGV, mg_error_trap); /* 11 Segmentation violation (ANSI).  */
#if 0
   signal(SIGUSR2, SIG_IGN); /* 12 User-defined signal 2 (POSIX).  */
   signal(SIGALRM, mg_error_trap); /* 14 Alarm clock (POSIX).  */
   signal(SIGTERM, mg_error_trap); /* 15 Termination (ANSI).  */
   signal(SIGSTKFLT, mg_error_trap); /* 16 ??? */
#endif

#if 0
#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGCLD, mg_error_trap); /* Same as SIGCHLD (System V).  */
#endif

   signal(SIGCHLD, mg_error_trap); /* 17 Child status has changed (POSIX).  */
#endif

   signal(SIGCONT, mg_error_trap); /* 18 Continue (POSIX).  */
#if 0
   signal(SIGSTOP, mg_error_trap); /* 19 Stop, unblockable (POSIX).  */
#endif
   signal(SIGTSTP, mg_error_trap); /* 20 Keyboard stop (POSIX).  */
   signal(SIGTTIN, mg_error_trap); /* 21 Background read from tty (POSIX).  */
   signal(SIGTTOU, mg_error_trap); /* 22 Background write to tty (POSIX).  */
   signal(SIGURG, mg_error_trap); /* 23 Urgent condition on socket (4.2 BSD).  */
   signal(SIGXCPU, mg_error_trap); /* 24 CPU limit exceeded (4.2 BSD).  */
   signal(SIGXFSZ, mg_error_trap); /* 25 File size limit exceeded (4.2 BSD).  */
#if 0
   signal(SIGVTALRM, mg_error_trap); /*	26 Virtual alarm clock (4.2 BSD).  */
   signal(SIGPROF, mg_error_trap); /* 27 Profiling alarm clock (4.2 BSD).  */
#endif
   signal(SIGWINCH, mg_error_trap); /* 28 Window size change (4.3 BSD, Sun).  */

#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPOLL, mg_error_trap); /* SIGIO Pollable event occurred (System V).  */
#endif

   signal(SIGIO, mg_error_trap); /* 29 I/O now possible (4.2 BSD).  */

#if !defined(FREEBSD) && !defined(MACOSX)
   signal(SIGPWR, mg_error_trap); /* 30 Power failure restart (System V).  */
#endif

#elif defined(MG_VMS)

   signal(SIGPIPE, SIG_IGN);

#endif


   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_tls_service_sigset: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}

#endif /* #ifdef MG_SSL */

