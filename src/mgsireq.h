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

#ifndef MGSIREQ_H
#define MGSIREQ_H

int                  mg_php                     (MGREQ *p_request, int mode);
int                  mg_php_direct              (char *request, int smode, char **response, unsigned long *size, int *offset, int rmode);
int                  mg_php_decode_message      (unsigned char * buffer, char cmnd);
int                  mg_httpd                   (MGREQ *p_request);
int                  mg_http                    (MGREQ *p_request);
int                  mg_client_read             (MGREQ *p_request, void *buffer, int size);
int                  mg_client_write_string     (MGREQ *p_request, char *buffer);
int                  mg_client_write_buffer     (MGREQ *p_request, void *buffer, int size);
int                  mg_get_server_variable     (MGREQ *p_request, char *VariableName, LPMEMOBJ p_cgievar);
int                  mg_add_server_variable     (MGREQ *p_request, char *VariableName, LPMEMOBJ p_cgievar, LPMEMOBJ p_trans_buffer);
int                  mg_add_record              (LPMEMOBJ p_record, char *type, char *name, char *content, unsigned long content_length);
HNVLIST              mg_get_key_list            (MGREQ *p_request);
void                 mg_free_key_list           (MGREQ *p_request, HNVLIST hNVList);
int                  mg_process_query_string    (MGREQ *p_request);
int                  mg_url_decode              (char *target);
int                  mg_wsmq                    (MGREQ *p_request);
int                  mg_wsmq_send               (MGREQ *p_request, char *buffer, unsigned long size);
#if defined(MG_BDB)
int                  mg_bdb                     (MGREQ *p_request, int mode);
#endif

#endif
