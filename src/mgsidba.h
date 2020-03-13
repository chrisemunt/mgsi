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

#ifndef MGSIDBA_H
#define MGSIDBA_H

int                  mg_release_connection         (int chndle, int force_down);
int                  mg_get_connection             (int *p_chndle, int *p_shndle, char *m_server_name, int timeout, int no_retry, int context);
int                  mg_server_read                (int chndle, void *instring, unsigned long instring_size, unsigned long *p_instring_actual_size, int timeout, int no_close_on_timeout);
int                  mg_server_write               (MGREQ *p_request, int chndle, LPMEMOBJ request_data);
int                  mg_server_write_buffer        (int chndle, char *outstring, unsigned long size, int offset);
int                  mg_dbc_test                   (int chndle, int close_on_error, int mode);
int                  mg_test_connection            (int chndle, char *m_server_name);
int                  mg_check_connections          ();
int                  mg_close_connection_to_host   (int chndle);
int                  mg_close_connections          (char *m_server_name, int context);
int                  mg_get_log_level              (int chndle);
int                  mg_get_server_index           (char *m_server_name, int shndle);
int                  mg_set_server_index           (int *p_shndle, char *m_server_name);

int                  mg_get_alt_servers            (char *m_server_name);
int                  mg_set_alt_servers            (int altndle, char *m_server_name);
int                  mg_next_alt_server            (int althndle, int sn_start);
int                  mg_encode_password            (char *hash, char *password, short type);
int                  mg_decode_password            (char *hash, char *password, short type);

int                  mg_tcp_open_connection        (int *p_chndle, int *p_shndle, char *m_server_name, int timeout, int no_retry, int context);

int                  mg_tcp_open_socket            (int chndle, int shndle, int free_mem_on_error, int timeout, int no_retry, int *pnet_error);
int                  mg_tcp_connect                (SOCKET sockfd, xLPSOCKADDR p_srv_addr, socklen_mg srv_addr_len, int timeout, int no_retry);
int                  mg_tcp_close_connection       (int chndle, int free_mem);
int                  mg_tcp_close_socket           (int chndle);
int                  mg_tcp_write                  (int chndle, void *BufferToSendFrom, unsigned long size);
int                  mg_tcp_read                   (int chndle, void *BufferToReadInto, int maxBufferSize, int m_timeout);

int                  mg_tcp_free_dbc_memory        (int chndle);
int                  mg_tcp_free_sys_memory        (int chndle);

#endif