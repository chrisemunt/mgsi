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

#ifndef MGSITLS_H
#define MGSITLS_H

#ifdef MG_SSL
int                  mg_tls_proxy_start         (void);
int                  mg_tls_proxy_stop          (int tcp_port);
MG_THR_TYPE          mg_tls_proxy               (void *arg);
MG_THR_TYPE          mg_tls_proxy_child         (void *arg);
int                  mg_tls_proxy_request       (MGREQ *p_request, char *partial_header);

int                  mg_tls_server_start        (void);
int                  mg_tls_server_stop         (int tcp_port);
MG_THR_TYPE          mg_tls_server              (void *arg);
MG_THR_TYPE          mg_tls_server_child        (void *arg);
static int           mg_tls_verify_callback     (int preverify_ok, X509_STORE_CTX *ctx);
int                  mg_tls_server_session      (MGREQ *p_request);
int                  mg_tls_log_error           (void);

int                  mg_tls_service_stop        (int tcp_port, int *stop_flag);
int                  mg_tls_service_sigset      (void);
#endif

#endif