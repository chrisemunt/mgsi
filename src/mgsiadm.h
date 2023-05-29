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

#ifndef MGSIADM_H
#define MGSIADM_H

int            mg_return_message             (MGREQ *p_request, int type,  char *context, char *message, char *m_server_name);
int            mg_sysman                     (MGREQ *p_request, char *form_id, char *m_server_name, LPSMDATA p_smdata);
int            mg_sysman_send_log_line       (MGREQ *p_request, char *buffer, unsigned long size);
int            mg_sysman_top                 (MGREQ *p_request, int type, int target, char *title, char *redirect, char *action, char *escape);
int            mg_sysman_default_header      (MGREQ *p_request);
int            mg_sysman_tail                (MGREQ *p_request, int type);
int            mg_sysman_access              (MGREQ *p_request);
int            mg_sysman_server_list         (MGREQ *p_request, char *nextFormID, char *section, char *defKey, char *defValue, char *config_file, int type);
int            mg_sysman_wsmq_server_list    (MGREQ *p_request, char *nextFormID, char *section, char *defKey, char *defValue, char *config_file, int type);
int            mg_sysman_documentation       (MGREQ *p_request);
int            mg_sysman_url_encode          (char *outstring, char *instring);
int            mg_sysman_form_encode         (char *value);
int            mg_sysman_trans_control_chars (char *buffer, unsigned long dwLength);

#endif