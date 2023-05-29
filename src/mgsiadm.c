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
#include "mgsireq.h"
#include "mgsiutl.h"
#include "mgsicfg.h"
#include "mgsiadm.h"
#include "mgsidba.h"
/* Documentation */
#include "mgsidoc.h"

static char *mgsm_core[] = {MG_P_SYSTEM_MANAGER,
                            NULL};

static char *mgsm_core_text[] = {"Authorised Managers",
                                 NULL};

static char *mgsm_con[] = {MG_P_M_RESPONSE_TIMEOUT, MG_P_QUEUED_REQUEST_TIMEOUT,
                           MG_P_NO_ACTIVITY_TIMEOUT,
                           MG_P_DEFAULT_SERVICE, MG_P_ENV_PARAMETERS,
                           NULL};
static char *mgsm_con_text[] = {"Server Response Timeout",
                                "Timeout for Queued Requests",
                                "Timeout for Inactive Connections",
                                "Default Server",
                                "Event Log Directive",
                                NULL};


static char *mgsm_con_status[] = {"Enabled", "Disabled", NULL};


static char *mgsm_db_con[] = {MG_P_IP_ADDRESS,
                              MG_P_BASE_TCP_PORT,
                              MG_P_DEFAULT_USER_UCI,
                              MG_P_USERNAME,
                              MG_P_PASSWORD,
                              MG_P_PASSWORD,
                              MG_P_MAXIMUM_SERVER_SESSIONS,
                              MG_P_NLS_TRANSLATION,
                              NULL};

static char *mgsm_db_con_text[] = {"IP Address",
                                   "TCP Port (for %ZMGSI)",
                                   "NameSpace",
                                   "User Name",
                                   "Password",
                                   "Password (Confirm)",
                                   "Maximum Number of Connections",
                                   "NLS Translation Table Name",
                                   NULL};

#define MGSI_PW1          4
#define MGSI_PW2          5

static char *mgsm_db_icon[] = {MG_P_M_RESPONSE_TIMEOUT,
                               MG_P_NO_ACTIVITY_TIMEOUT,
                               MG_P_ENV_PARAMETERS,
                               NULL};

static char *mgsm_db_icon_text[] = {"Server Response Timeout",
                                    "Timeout for Inactive Connections",
                                    "Event Log Directive",
                                    NULL};


static char *mgsm_mqs[] = {"Queue_Manager_Name",
                           "Queue_Name",
                           NULL};

static char *mgsm_mqs_text[] = {"Queue Manager Name",
                                "Queue Name",
                                NULL};

static char *mgsm_mqc[] = {"Server_Routine",
                           "Maximum_Array_Size",
                           NULL};

static char *mgsm_mqc_text[] = {"Server Routine",
                                "Maximum Array Size",
                                NULL};

#ifdef MG_SSL

static char *mgsm_ssl[] = {"SSLS_TCP_Port",
                           "SSLS_Certificate_File",
                           "SSLS_Certificate_Key_File",
                           "SSLS_CA_Certificate_File",
                           "SSLS_CA_Certificate_Path",
                           "SSLS_Verify_Client",
                           "SSLS_Verify_Depth",
                           NULL};

static char *mgsm_ssl_text[] = {"Server TCP Port Number",
                               "Server Certificate File",
                               "Server Certificate Key File",
                               "CA Certificate File",
                               "CA Certificate Path",
                               "Verify Client",
                               "Verify Depth",
                               NULL};

static char *mgsm_ssl_help[] = {"The TCP Port the SSL Server will listen on",
                                "The SSL Server Certificate File",
                                "The SSL Server Certificate Key File",
                                "The SSL CA Certificate File",
                                "The SSL CA Certificate Path",
                                "The extent to which the client is to be verified",
                                "The depth in the verification chain to which the client is to be verified (usually 1)",
                                NULL};

static char *mgsm_sslnet[] = {"SSLP_TCP_Port",
                              NULL};

static char *mgsm_sslnet_text[] = {"Proxy Server TCP Port Number",
                                   NULL};

static char *mgsm_sslnet_help[] = {"The TCP Port the SSL Proxy Server will listen on",
                                   NULL};

#endif /* #ifdef MG_SSL */


/* Send error message back to the client */

int mg_return_message(MGREQ *p_request, int type, char *context, char *message, char *m_server_name)
{
   int len;
   char redirect[256];

#ifdef _WIN32
__try {
#endif

   redirect[0] = '\0';
   len = 0;

   if (type == 0 || type == 10)
      mg_sysman_top(p_request, type, 0, "Information", redirect, core_data.mg_lib_path, core_data.mg_lib_path);
   else
      mg_sysman_top(p_request, type, 0, "Error Condition", redirect, core_data.mg_lib_path, core_data.mg_lib_path);
   mg_client_write_string(p_request, "<B>");
   if (T_STRLEN(context)) {
      mg_client_write_string(p_request, context);
   }
   mg_client_write_string(p_request, "</B><P>");
   mg_client_write_string(p_request, message);
   mg_sysman_tail(p_request, 0);

   return TRUE;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_return_message: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return FALSE;
}
#endif

}


int mg_sysman(MGREQ *p_request, char *form_id, char *m_server_name, LPSMDATA p_smdata)
{
   short phase;
   short mq[256];
   int len, lenc, lenmq, n, n1, n2, timeout, shndle, tcp_port, sn, msn;
   char conf_buffer[4096], buffer[2048], buffer1[2048], message[128], old_pwd[64], encoded_pwd[8192];
   char name[64], name1[64], ipa[64];
   char dbt[64], status[8], mjob[32];
   char uci[256], port[16];
   char token[32], config_file[64];
   char *wk_script;
   unsigned long activity;
   FILE *fp;
   SMSERVER servers[32];

   phase = 0;

#ifdef _WIN32
__try {
#endif

   /* Security */

   n2 = 0;
   timeout = 0;

   if (mg_sysman_access(p_request) == 0)
      return 0;

   sn = 0;
   msn = 0;
   message[0] = '\0';
   old_pwd[0] = '\0';
   conf_buffer[0] = '\0';
   encoded_pwd[0] = '\0';

   mg_make_token(buffer, 8, 1);
   T_SPRINTF(token, "%sSYSn=%s", MG_RVAR_PREFIX, buffer);

   mg_trim_string(p_smdata->config_section);
   mg_trim_string(p_smdata->mq_service);

   T_STRCPY(config_file, core_data.mg_ini);

   for (n = 0; n < 32; n ++) {
      T_STRCPY(servers[n].server, "");
      servers[n].status = 0;
   }

   for (n = 0; n < 256; n ++)
      mq[n] = 0;


   /* File modified stuff to mgsi.ini */

   lenc = mg_trim_string(p_smdata->new_dbserver);
   lenmq = mg_trim_string(p_smdata->new_mq_service);

   T_STRCPY(ipa, "");

   if (p_smdata->s_buffer != NULL && T_STRLEN(p_smdata->config_section)) {

      n = 0;
      for (wk_script = p_smdata->s_buffer; *wk_script != '\0'; wk_script += T_STRLEN(wk_script) + 1) {
         n ++;
         T_STRCPY(conf_buffer, wk_script);

         mg_piece(buffer, conf_buffer, "=", 1, 1);
         mg_piece(conf_buffer, conf_buffer, "=", 2, 9999);

         if (!strncmp("MQ_", buffer, 3)) {
            int n, sn;

            for (n = (int) strlen(buffer); n >= 0; n --) {
               if (buffer[n] == '_') {
                  sn = (int) strtol(buffer + n + 1, NULL, 10);
                  break;
               }
            }

            if (sn) {
               if (mq[sn]) {
                  mg_set_config(p_smdata->config_section, (char *) buffer, NULL, config_file);
                  continue;
               }
               if (strstr(buffer, "qm_name") || strstr(buffer, "q_name")) {
                  if (!strlen(conf_buffer)) {
                     mq[sn] = 1;
                     mg_set_config(p_smdata->config_section, (char *) buffer, NULL, config_file);
                     continue;
                  }
               }
            }
         }

         if (!T_STRNCMP(buffer, "Alternative_Server_", 19)) {
            sn = strtol(buffer + 19, NULL, 10);
            if (sn > 0 && sn < 32) {
               if (T_STRSTR(buffer, "Status")) {
                  if (!T_STRCMP(conf_buffer, "Enabled"))
                     servers[sn].status = 1;
                  else
                     servers[sn].status = 0;
               }
               else if (T_STRCMP(conf_buffer, "--- none ---"))
                  T_STRCPY(servers[sn].server, conf_buffer);
            }
         }
         else {
            mg_set_config(p_smdata->config_section, (char *) buffer, conf_buffer, config_file);
            if (lenc)
               mg_set_config(p_smdata->new_dbserver, (char *) buffer, conf_buffer, config_file);
            if (lenmq) {
               T_SPRINTF(buffer1, "WSMQS:%s", p_smdata->new_mq_service);
               mg_set_config(buffer1, (char *) buffer, conf_buffer, config_file);
            }
         }
      }

      /* CM0051 */
/*
      mg_log_event(p_smdata->username, "MGateway SIG Configuration: Username");
      mg_log_event(p_smdata->pw1, "MGateway SIG Configuration: PW1");
      mg_log_event(p_smdata->pw2, "MGateway SIG Configuration: PW2");
*/
      len = mg_get_config((char *) p_smdata->config_section, (char *) MG_P_PASSWORD, "", encoded_pwd, 8190, config_file);
      mg_decode_password((char *) encoded_pwd, (char *) old_pwd, 2);
/*
      mg_log_event(old_pwd, "MGateway SIG Configuration: DECODED PWD");
*/
      if (len && T_STRSTR(p_smdata->pw1, "       ") && T_STRSTR(p_smdata->pw2, "       ")) {
         if (T_STRLEN(p_smdata->username)) {
            T_STRCAT(message, "; Password Unchanged");
         }
         else {
            T_STRCAT(message, "; Password Error: No Username Provided");
         }
      }
      else if (len && !T_STRLEN(p_smdata->pw1) && !T_STRLEN(p_smdata->pw2)) {
         T_STRCAT(message, "; Password Removed");
         mg_set_config(p_smdata->config_section, "Password", (char *) NULL, config_file);
      }
      else if (T_STRLEN(p_smdata->pw1) && T_STRLEN(p_smdata->pw2) && !strcmp(p_smdata->pw1, p_smdata->pw2)) {
         if (T_STRLEN(p_smdata->username)) {
            T_STRCAT(message, "; Password Updated");

            mg_encode_password((char *) encoded_pwd, (char *) p_smdata->pw1, 2);

/*
{
            char hash[8192], pwd[256], buffer[256];
            mg_encode_password((char *) hash, (char *) p_smdata->pw1, 2);
            mg_decode_password((char *) hash, (char *) pwd, 2);
            sprintf(buffer, "original=%s; decoded=%s; cmp=%d;", p_smdata->pw1, pwd, strcmp(p_smdata->pw1, pwd));
            mg_log_event(hash, buffer);
}
*/
            mg_set_config(p_smdata->config_section, "Password", (char *) encoded_pwd, config_file);
         }
         else {
            T_STRCAT(message, "; Password Error: No Username Provided");
         }
      }
      else {
         if (len || T_STRLEN(p_smdata->pw1) || T_STRLEN(p_smdata->pw2)) {
            T_STRCAT(message, "; Password Error");
         }
      }

      for (sn = 1, msn = 0; sn < 32 ; sn ++) {
         if (!T_STRLEN(servers[sn].server))
            continue;
         if (T_STRLEN(ipa) && !T_STRCMP(ipa, servers[sn].server))
            continue;
         for (n = 1, n1 = 0; n < sn; n ++) {
            if (!T_STRLEN(servers[n].server))
               continue;
            if (!T_STRCMP(servers[n].server, servers[sn].server)) {
               n1 = 1;
               break;
            }
         }
         if (n1)
            continue;
         msn ++;
         T_SPRINTF(buffer, "Alternative_Server_%d", msn);
         T_SPRINTF(conf_buffer, "%d~~~~~~%s", servers[sn].status, servers[sn].server);

         mg_set_config(p_smdata->config_section, (char *) buffer, conf_buffer, config_file);
         if (lenc > 0)
            mg_set_config(p_smdata->new_dbserver, (char *) buffer, conf_buffer, config_file);
      }
      for (sn = msn + 1; sn < 32; sn ++) {
         T_STRCPY(servers[sn].server, "");
         T_SPRINTF(buffer, "Alternative_Server_%d", sn);
         mg_set_config(p_smdata->config_section, (char *) buffer, NULL, config_file);
         if (lenc > 0)
            mg_set_config(p_smdata->new_dbserver, (char *) buffer, NULL, config_file);
      }

   }


   /* Respond to buttons pressed */

   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRLEN(p_smdata->button) > 13 && T_STRNCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbClose_", 10) == 0 && T_STRCMP(form_id, "sigms") == 0) {
      n = (int) T_STRTOL(p_smdata->button + (MG_RVAR_PREFIX_LEN + 10), NULL, 10);

      n = mg_release_connection(n, MG_CS_CLOSE);

   }
   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbOK") == 0 && T_STRCMP(form_id, "sigsdbacc") == 0) {

      n = mg_close_connections((char *) m_server_name, 0);

      n = mg_return_message(p_request, 10, "", "All Specified Connections are now Closed", "SYSTEM_INDEX");
      return 1;
   }


   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRNCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbOK", 6) == 0 && (T_STRCMP(form_id, "sigmc") == 0 || T_STRCMP(form_id, "sigsc") == 0 || T_STRCMP(form_id, "sigsdba1") == 0 || T_STRCMP(form_id, "sigmgc") == 0 || T_STRCMP(form_id, "sigmqsc1") == 0)) {

      if (T_STRLEN(p_smdata->config_section) > 0) {
         if (p_smdata->service_status == 1)
            T_STRCPY(buffer, "Enabled");
         else
            T_STRCPY(buffer, "Disabled");

         if (T_STRCMP(form_id, "sigmqsc1") == 0) {
            mg_set_config("WSMQS_INDEX", (char *) p_smdata->mq_service, buffer, config_file);
            mg_wsmq_init(1);
         }
         else
            mg_set_config("SYSTEM_INDEX", (char *) p_smdata->config_section, buffer, config_file);

         mg_global_parameters();

         for (shndle = 0; shndle < MG_MAX_SERVERS; shndle ++) {
            if (mg_servers[shndle] != NULL)
                  mg_system_parameters(shndle);
         }

         /* CM0051 */
         T_STRCPY(buffer, "Configuration Saved");
         T_STRCAT(buffer, message);
         n = mg_return_message(p_request, 10, "SIG Management", buffer, "");
         return 1;
      }
      else {
         n = mg_return_message(p_request, 10, "SIG Management", "Configuration not Saved: Name not provided", "");
         return 1;
      }
   }


   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRNCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbRDV", 10) == 0 && T_STRCMP(form_id, "sigmc") == 0) {
      mg_set_global_defaults();
      T_STRCPY(form_id, "sigmc");
   }
   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbDEL") == 0 && T_STRCMP(form_id, "sigsdba2") == 0 && T_STRLEN(m_server_name) > 0) {
      mg_set_config((char *) m_server_name, NULL, NULL, config_file);
      mg_set_config("SYSTEM_INDEX", (char *) m_server_name, NULL, config_file);
      n = mg_return_message(p_request, 10, "SIG Management", "The Server Configuration has been Deleted", "");
      return 1;
   }
   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbDEL") == 0 && T_STRCMP(form_id, "sigmqsc2") == 0 && T_STRLEN(p_smdata->mq_service) > 0) {

      T_SPRINTF(p_smdata->config_section, "WSMQS:%s", p_smdata->mq_service);
      mg_set_config((char *) p_smdata->config_section, NULL, NULL, config_file);

      mg_set_config("WSMQS_INDEX", (char *) p_smdata->mq_service, NULL, config_file);
      n = mg_return_message(p_request, 10, "SIG Management", "The Service Configuration has been Deleted", "");

      mg_wsmq_init(1);

      return 1;
   }

   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbDEL") == 0 && T_STRCMP(form_id, "sigsdba1") == 0)
      T_STRCPY(form_id, "sigsdba2");
   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbCOPY") == 0 && T_STRCMP(form_id, "sigsdba1") == 0) {
      if (T_STRLEN(p_smdata->new_dbserver) > 0) {
         mg_set_config("SYSTEM_INDEX", (char *) p_smdata->new_dbserver, "Enabled", config_file);
         n = mg_return_message(p_request, 10, "SIG Management", "The Server Configuration has been copied", "");
      }
      else
         n = mg_return_message(p_request, 10, "SIG Management", "The Server Configuration has not been copied: No new name was provided", "");
      return 1;
   }

   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbDEL") == 0 && T_STRCMP(form_id, "sigmqsc1") == 0)
      T_STRCPY(form_id, "sigmqsc2");
   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbCOPY") == 0 && T_STRCMP(form_id, "sigmqsc1") == 0) {
      if (T_STRLEN(p_smdata->new_mq_service) > 0) {
         mg_set_config("WSMQS_INDEX", (char *) p_smdata->new_mq_service, "Disabled", config_file);
         n = mg_return_message(p_request, 10, "SIG Management", "The Service Configuration has been copied", "");
         mg_wsmq_init(1);
      }
      else
         n = mg_return_message(p_request, 10, "SIG Management", "The Service Configuration has not been copied: No new name was provided", "");
      return 1;
   }

   if (T_STRLEN(p_smdata->button) > MG_RVAR_PREFIX_LEN && T_STRCMP((p_smdata->button) + MG_RVAR_PREFIX_LEN, "SYSbClearLog") == 0 && T_STRCMP(form_id, "sigmel") == 0) {
      fp = T_FOPEN(core_data.mg_log, "w");
      if (fp)
         fclose(fp);
      n = mg_return_message(p_request, 10, "SIG Management", "The Event Log has been Cleared", "");
      return 1;
   }

   /* System Manager forms */

   if (T_STRCMP(form_id, "0") == 0 || T_STRCMP(form_id, "menu") == 0) {
      T_STRCPY(form_id, "sigabout");
   }

   if (T_STRCMP(form_id, "sigmc") == 0) {

      mg_site_parameters();

      T_STRCPY(buffer1, "SIG : Configuration");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmc\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYSconfigSection\" VALUE=\"SYSTEM\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");

      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\"><B>Security:</B></TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><FONT SIZE=\"-1\"></FONT></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      len = mg_get_config("SYSTEM", "HTTP_Service_Status", "Enabled", dbt, 32, config_file);
      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">Internal HTTP Service Status: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      for (n = 0; mgsm_con_status[n]; n ++) {
         T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetHTTP_Service_Status\" VALUE=\"%s\"", MG_RVAR_PREFIX, mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
         if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
            mg_client_write_string(p_request, " CHECKED");
         T_SPRINTF(buffer1, "> %s ", mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
      }
      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_core[n]; n ++) {
         len = mg_get_config("SYSTEM", mgsm_core[n], "", buffer, 1024, config_file);

         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\">", MG_SM_CELL_BGCOLOR, mgsm_core_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_core[n], buffer);
         mg_client_write_string(p_request, buffer1);

         mg_client_write_string(p_request, "</TD></TR>\r\n");
      }

#if defined(MG_SSL_SERVER)

      if (mg_ssl.ssl) {

      /* SSL Server */

      mg_client_write_string(p_request, "<TR>\r\n");

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap><B>%s:</B></TD><TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap></TD>\r\n", MG_SM_CELL_BGCOLOR, "SSL Server", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "</TR>\r\n");

      mg_client_write_string(p_request, "<tr>\r\n");

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap>%s:</TD>\r\n", MG_SM_CELL_BGCOLOR, "Service Status");
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      len = mg_get_config("SYSTEM", "SSLS_Service_Status", MG_T_DISABLED, dbt, 31, core_data.mg_ini);

      for (n = 0; mgsm_con_status[n]; n ++) {
         T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSset%s\" VALUE=\"%s\" title=\"%s\"", MG_RVAR_PREFIX, "SSLS_Service_Status", mgsm_con_status[n], "Status of the SSL (HTTPS) Server");
         mg_client_write_string(p_request, buffer1);
         if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
            mg_client_write_string(p_request, " CHECKED");
         T_SPRINTF(buffer1, "> %s ", mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
      }
      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_ssl[n] != NULL; n ++) {
         len = mg_get_config("SYSTEM", mgsm_ssl[n], "", buffer, 1024, core_data.mg_ini);

         mg_client_write_string(p_request, "<TR>\r\n");
         T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s:</TD>\r\n", MG_SM_CELL_BGCOLOR, mgsm_ssl_text[n]);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);

         if (!strcmp(mgsm_ssl[n], "SSLS_Verify_Client") {

            int n, client_verify;

            client_verify = (int) strtol(buffer, NULL, 10);

            T_SPRINTF(buffer1, "<select name=\"%sSYSset%s\" title=\"%s\">\r\n", MG_RVAR_PREFIX, "SSLS_Verify_Client", "The extent to which the client is to be verified");
            mg_client_write_string(p_request, buffer1);

            for (n = 0; n < 10; n ++) {

               if (n == 0)
                  T_SPRINTF(buffer1, "<option value=\"%d\"%s>%s</option>", n, (n == client_verify) ? " selected": "", "Client certificates are not required");
               else if (n == 1)
                  T_SPRINTF(buffer1, "<option value=\"%d\"%s>%s</option>", n, (n == client_verify) ? " selected": "", "Client may present a valid certificate");
               else if (n == 2)
                  T_SPRINTF(buffer1, "<option value=\"%d\"%s>%s</option>", n, (n == client_verify) ? " selected": "", "Client must present a valid certificate");
               else
                  continue;

               mg_client_write_string(p_request, buffer1);


            }
            mg_client_write_string(p_request, "</SELECT>\r\n");
         }
         else {
            T_SPRINTF(buffer1, "<INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\" title=\"%s\">\r\n", MG_RVAR_PREFIX, mgsm_ssl[n], buffer, mgsm_ssl_help[n]);
            mg_client_write_string(p_request, buffer1);
         }

         mg_client_write_string(p_request, "</TD>\r\n");
         mg_client_write_string(p_request, "</TR>\r\n");
      }

      /* SSL Proxy Server */

      mg_client_write_string(p_request, "<TR>\r\n");

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap><B>%s:</B></TD><TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap></TD>\r\n", MG_SM_CELL_BGCOLOR, "SSL Proxy Server", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "</TR>\r\n");

      mg_client_write_string(p_request, "<tr>\r\n");

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\" nowrap>%s:</TD>\r\n", MG_SM_CELL_BGCOLOR, "Service Status");
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      len = mg_get_config("SYSTEM", "SSLP_Service_Status", MG_T_DISABLED, dbt, 31, core_data.mg_ini);

      for (n = 0; mgsm_con_status[n]; n ++) {
         T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSset%s\" VALUE=\"%s\" title=\"%s\"", MG_RVAR_PREFIX, "SSLP_Service_Status", mgsm_con_status[n], "Status of the SSL Proxy Server");
         mg_client_write_string(p_request, buffer1);
         if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
            mg_client_write_string(p_request, " CHECKED");
         T_SPRINTF(buffer1, "> %s ", mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
      }
      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_sslnet[n] != NULL; n ++) {
         len = mg_get_config("SYSTEM", mgsm_sslnet[n], "", buffer, 1024, core_data.mg_ini);

         mg_client_write_string(p_request, "<TR>\r\n");
         T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s:</TD>\r\n", MG_SM_CELL_BGCOLOR, mgsm_sslnet_text[n]);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
         T_SPRINTF(buffer1, "<INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\" title=\"%s\">\r\n", MG_RVAR_PREFIX, mgsm_sslnet[n], buffer, mgsm_sslnet_help[n]);
         mg_client_write_string(p_request, buffer1);
         mg_client_write_string(p_request, "\r\n");

         if (!n && !len) {
            mg_client_write_string(p_request, "<BR>\r\n");
            T_SPRINTF(buffer, "<FONT SIZE=\"-1\"><I>%s: %d</I></FONT></font>\r\n", "The Proxy Service will be provided through the main SIG port", core_data.mg_port);
            mg_client_write_string(p_request, buffer);
         }

         mg_client_write_string(p_request, "</TD></TR>\r\n");
      }

      }

#endif /* #ifdef MG_SSL */


      T_SPRINTF(buffer1, "<P><LI><A HREF=\"%s?%sSYS=sigabou&%s\">Event Log</A>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);

      mg_client_write_string(p_request, "</TABLE>\r\n");


      mg_client_write_string(p_request, "<P>\r\n");
      mg_client_write_string(p_request, "<TABLE>\r\n");
      T_SPRINTF(buffer1, "<TR><TD><INPUT TYPE=SUBMIT NAME=\"%sSYSbOK\" VALUE=\"Save Configuration\"></TD>\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<TD></TD><TD></TD><TD></TD><TD><INPUT TYPE=SUBMIT NAME=\"%sSYSbRDV\" VALUE=\"Restore Default Values\"></TD></TR>\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (T_STRCMP(form_id, "sigms") == 0) {

      int pid;

      mg_sysman_top(p_request, 0, 0, "SIG : Status", "", core_data.mg_lib_path, core_data.mg_lib_path);
      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");

      T_SPRINTF(buffer1, "<TR><TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Connection<BR>Number</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Service<BR>Type</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Server<BR>Name</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">IP<BR>Address</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">TCP<BR>Port</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Server<BR>NameSpace</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Server<BR>Process ID</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Status</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Requests</TH>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Close</TH></TR>\r\n", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
      mg_client_write_string(p_request, buffer1);

      phase = 11;

      int mg_check_connections();

      phase = 12;

      for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {

         phase = 13;

         if (mg_connections[n] != NULL) {
            shndle = mg_connections[n]->shndle;
            tcp_port = mg_connections[n]->tcp_port;
            activity = mg_connections[n]->activity;

            T_STRCPY(m_server_name, "");
            if (n < MG_MAX_CONNECTIONS) {
               if (mg_connections[n]->p_server) {
                  T_STRCPY(m_server_name, mg_connections[n]->p_server->m_server_name);
               }
            }

            phase = 14;

            T_STRCPY(uci, mg_connections[n]->uci);
            T_STRCPY(mjob, mg_connections[n]->m_job_no);

            if (mg_connections[n]->p_server) {

               T_SPRINTF(dbt, "%s (%s)", mg_connections[n]->p_server->database_type, mg_connections[n]->p_server->database_version);

               if (mg_connections[n]->p_server->dbtype == MG_DBTYPE_BDB) {
                  tcp_port = 0;
                  T_STRCPY(ipa, "");
                  T_STRCPY(mjob, "");
                  T_STRCPY(uci, mg_connections[n]->p_server->file_name);
               }
               else {
                  T_STRCPY(ipa, mg_connections[n]->p_server->ip_address);
               }
            }

            if (mg_connections[n]->status == 1)
               T_STRCPY(status, "InUse");
            else
               T_STRCPY(status, "Free");

            phase = 15;

            if (tcp_port) {
               phase = 151;
               T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=blue>%s</font></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">\r\n",
                                                                          MG_SM_CELL_BGCOLOR, n,                             MG_SM_CELL_BGCOLOR, dbt,                    MG_SM_CELL_BGCOLOR, m_server_name,           MG_SM_CELL_BGCOLOR, ipa,            MG_SM_CELL_BGCOLOR, tcp_port,             MG_SM_CELL_BGCOLOR,   uci,              MG_SM_CELL_BGCOLOR, mjob,              MG_SM_CELL_BGCOLOR, status,          MG_SM_CELL_BGCOLOR, activity,            MG_SM_CELL_BGCOLOR);
            }
            else {
               phase = 152;
               T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=blue>%s</font></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">\r\n",
                                                                  MG_SM_CELL_BGCOLOR,    n,                           MG_SM_CELL_BGCOLOR,   dbt,                         MG_SM_CELL_BGCOLOR,      m_server_name,      MG_SM_CELL_BGCOLOR,  ipa,         MG_SM_CELL_BGCOLOR,                    MG_SM_CELL_BGCOLOR,   uci,             MG_SM_CELL_BGCOLOR, mjob,                      MG_SM_CELL_BGCOLOR, status,        MG_SM_CELL_BGCOLOR, activity,                       MG_SM_CELL_BGCOLOR);
            }

            phase = 16;

            mg_client_write_string(p_request, buffer1);

            phase = 17;

            T_SPRINTF(buffer1, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbClose_%d\" VALUE=\"Close\">", MG_RVAR_PREFIX, n);
            mg_client_write_string(p_request, buffer1);
            T_STRCPY(buffer1, "</TD></TR>\r\n");

            phase = 18;

            mg_client_write_string(p_request, buffer1);
         }
      }

      phase = 100;

      getcwd(uci, 50);
      uci[50] = '\0';
      strcpy(uci, core_data.root_dir);
      pid = getpid();

      if (core_data.php_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>PHP</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                         MG_SM_CELL_BGCOLOR,                                            MG_SM_CELL_BGCOLOR,                    MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,                            MG_SM_CELL_BGCOLOR, uci,                 MG_SM_CELL_BGCOLOR, pid,     MG_SM_CELL_BGCOLOR,                         MG_SM_CELL_BGCOLOR,
                                                      core_data.php_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.go_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>Go</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                         MG_SM_CELL_BGCOLOR,                                            MG_SM_CELL_BGCOLOR,                    MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,                            MG_SM_CELL_BGCOLOR, uci,                 MG_SM_CELL_BGCOLOR, pid,     MG_SM_CELL_BGCOLOR,                         MG_SM_CELL_BGCOLOR,
                                                      core_data.go_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.jsp_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>JSP</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                          MG_SM_CELL_BGCOLOR,                                            MG_SM_CELL_BGCOLOR,                   MG_SM_CELL_BGCOLOR,                      MG_SM_CELL_BGCOLOR,                            MG_SM_CELL_BGCOLOR, uci,                 MG_SM_CELL_BGCOLOR,  pid,     MG_SM_CELL_BGCOLOR,                       MG_SM_CELL_BGCOLOR,
                                                      core_data.jsp_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.aspx_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>ASP.NET</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                           MG_SM_CELL_BGCOLOR,                                         MG_SM_CELL_BGCOLOR,                      MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,                           MG_SM_CELL_BGCOLOR, uci,                 MG_SM_CELL_BGCOLOR, pid,       MG_SM_CELL_BGCOLOR,                        MG_SM_CELL_BGCOLOR,
                                                      core_data.aspx_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.python_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>Python</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                           MG_SM_CELL_BGCOLOR,                                           MG_SM_CELL_BGCOLOR,                   MG_SM_CELL_BGCOLOR,                       MG_SM_CELL_BGCOLOR,                           MG_SM_CELL_BGCOLOR, uci,                 MG_SM_CELL_BGCOLOR, pid,        MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,
                                                      core_data.python_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.ruby_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>Ruby</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                           MG_SM_CELL_BGCOLOR,                                           MG_SM_CELL_BGCOLOR,                   MG_SM_CELL_BGCOLOR,                       MG_SM_CELL_BGCOLOR,                          MG_SM_CELL_BGCOLOR, uci,                MG_SM_CELL_BGCOLOR,  pid,        MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,
                                                      core_data.ruby_activity,  MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.ws_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>IBM MQ</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                        MG_SM_CELL_BGCOLOR,                                             MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,                     MG_SM_CELL_BGCOLOR,                          MG_SM_CELL_BGCOLOR, uci,                   MG_SM_CELL_BGCOLOR,  pid,         MG_SM_CELL_BGCOLOR,                    MG_SM_CELL_BGCOLOR,
                                                      core_data.ws_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.http_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>HTTP</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                         MG_SM_CELL_BGCOLOR,                                        MG_SM_CELL_BGCOLOR,                             MG_SM_CELL_BGCOLOR,                 MG_SM_CELL_BGCOLOR,                          MG_SM_CELL_BGCOLOR,uci,                    MG_SM_CELL_BGCOLOR,  pid,           MG_SM_CELL_BGCOLOR,                  MG_SM_CELL_BGCOLOR,
                                                      core_data.http_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      if (core_data.sm_activity) {
         T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=red>Systems<br>Management</font></TD><TD bgcolor=\"%s\"></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">Local</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">7040</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%d</TD><TD bgcolor=\"%s\">Enabled</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD><TD bgcolor=\"%s\"></TD></TR>\r\n",
                                                      MG_SM_CELL_BGCOLOR,                    MG_SM_CELL_BGCOLOR,                                              MG_SM_CELL_BGCOLOR,                        MG_SM_CELL_BGCOLOR,                               MG_SM_CELL_BGCOLOR,                       MG_SM_CELL_BGCOLOR, uci,          MG_SM_CELL_BGCOLOR,  pid,        MG_SM_CELL_BGCOLOR,                             MG_SM_CELL_BGCOLOR,
                                                      core_data.sm_activity, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
      }

      mg_client_write_string(p_request, "</TABLE>\r\n");


      if (core_data.ws_loaded) {

         mg_client_write_string(p_request, "<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\"><P><B>IBM MQ</B></font>");

         mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");


         T_SPRINTF(buffer1, "<TR><TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Service<BR>Name</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Queue<BR>Manager</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Queue</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Server</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Routine</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Status</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         T_SPRINTF(buffer1, "<TH ALIGN=\"CENTER\" bgcolor=\"%s\" valign=\"center\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"%s\">Requests</TH>", MG_SM_HCELL_BGCOLOR, MG_SM_HCELL_COLOR);
         mg_client_write_string(p_request, buffer1);

         for (n = 0; n < (int) (core_data.ws_session_max + 10); n ++) {

            if (!mg_mqstable[n])
               continue;

            T_SPRINTF(buffer1, "<TR><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\"><font color=blue>%s</font></TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%s</TD><TD ALIGN=\"CENTER\" bgcolor=\"%s\">%ld</TD></TR>",
                                                                      MG_SM_CELL_BGCOLOR,  mg_mqstable[n]->s_name, MG_SM_CELL_BGCOLOR,  mg_mqstable[n]->qm_name,                      MG_SM_CELL_BGCOLOR,  mg_mqstable[n]->q_name,    MG_SM_CELL_BGCOLOR,  mg_mqstable[n]->server, MG_SM_CELL_BGCOLOR,  mg_mqstable[n]->routine, MG_SM_CELL_BGCOLOR, mg_mqstable[n]->status ? "Enabled" : "Disabled",   MG_SM_CELL_BGCOLOR,   mg_mqstable[n]->activity);
            mg_client_write_string(p_request, buffer1);
         }

         mg_client_write_string(p_request, "</TABLE>\r\n");

      }


      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigms\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, "<P><INPUT TYPE=SUBMIT NAME=\"SUBMIT\" VALUE=\"Refresh\">\r\n");
      mg_sysman_tail(p_request, 0);
      return 1;
   }

   if (T_STRCMP(form_id, "sigmsd") == 0) {

      mg_check_connections();

      for (n = 0; n < MG_MAX_CONNECTIONS; n ++) {

         if (mg_connections[n] != NULL) {
            shndle = mg_connections[n]->shndle;
            tcp_port = mg_connections[n]->tcp_port;
            activity = mg_connections[n]->activity;

            T_STRCPY(m_server_name, "");
            if (n < MG_MAX_CONNECTIONS) {
               if (mg_connections[n]->p_server) {
                  T_STRCPY(m_server_name, mg_connections[n]->p_server->m_server_name);
               }
            }

            if (mg_connections[n]->p_server) {
               T_SPRINTF(dbt, "%s (%s)", mg_connections[n]->p_server->database_type, mg_connections[n]->p_server->database_version);
               T_STRCPY(ipa, mg_connections[n]->p_server->ip_address);
            }

            T_STRCPY(uci, mg_connections[n]->uci);
            T_STRCPY(mjob, mg_connections[n]->m_job_no);
            if (mg_connections[n]->status == 1)
               T_STRCPY(status, "InUse");
            else
               T_STRCPY(status, "Free");

            T_SPRINTF(buffer1, "%d#%s#%s#%d#%s#%s#%s#%s#%ld\r\n", n, name, ipa, tcp_port, dbt, uci, mjob, status, activity);

            mg_client_write_string(p_request, buffer1);
         }
      }
      mg_client_write_string(p_request, "\r\n");

      return 1;
   }

   if (T_STRCMP(form_id, "sigmel") == 0 || T_STRCMP(form_id, "sigmelr") == 0) {

      short reverse;
      long size, pos;

      mg_sysman_top(p_request, 0, 0, "SIG : Event Log", "", core_data.mg_lib_path, core_data.mg_lib_path);

      fp = T_FOPEN(core_data.mg_log, "rb");
      if (fp != NULL) {

         fseek(fp, 0, SEEK_END);
         size = ftell(fp);
         if (size < 0)
            size = 0;

         fclose(fp);
      }

      if (!T_STRCMP(form_id, "sigmel"))
         reverse = 0;
      else
         reverse = 1;

      T_SPRINTF(buffer, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmel\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer);
      T_SPRINTF(buffer, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbClearLog\" VALUE=\"Clear Log\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer);

      if (reverse) {
         T_SPRINTF(buffer1, " The Event Log is shown in <I>Descending</I> date order <A HREF=\"%s?%sSYS=sigmel&%s\">Click here to Reverse</A>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
         mg_client_write_string(p_request, buffer1);
      }
      else {
         T_SPRINTF(buffer1, " The Event Log is shown in <I>Ascending</I> date order <A HREF=\"%s?%sSYS=sigmelr&%s\">Click here to Reverse</A>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
         mg_client_write_string(p_request, buffer1);
      }

      mg_client_write_string(p_request, "<P>\r\n");

      if (reverse) {

         fp = fopen(core_data.mg_log, "rb");
         if (fp) {

            long ptr, bsize, bend, binc;
            char *p, *p1, *p2, *p3;
            char *buf, *buf1;

            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            if (size < 0)
               size = 0;

            pos = size;
            bsize = MG_CACHE_BUFFER;
            binc = 8192;
            buf = (char *) mg_malloc(sizeof(char) * (bsize + 32), "ReverseLog:1");
            if (buf) {
               ptr = bsize - 10;
               bend = ptr;
               buf[ptr] = '\0';

               for (pos = size - 1; pos >= 0; pos --) {

                  fseek(fp, pos, SEEK_SET);

                  n = fgetc(fp);

                  if (ptr == 0) {
                     bsize += binc;
                     buf1 = (char *) mg_malloc(sizeof(char) * (bsize + 32), "ReverseLog:2");
                     if (!buf1) {
                        break;
                     }
                     strncpy(buf1 + binc, buf, bend);
                     buf1[bend + binc] = '\0';;
                     mg_free((void *) buf, "ReverseLog:1");
                     buf = buf1;
                     buf1 = NULL;
                     ptr = binc;
                     bend += binc;
                  }
                  buf[-- ptr] = (char) n;
                  if (!strncmp(buf + ptr, ">>>", 3)) {
                     p = buf + ptr;

                     p1 = strstr(p, "\n");
                     if (*(p1 - 1) == '\r')
                        *(p1 - 1) = '\0';
                     else
                        *p1 = '\0';
                     p1 ++;
                     p2 = strstr(p1, "\n");
                     if (*(p2 - 1) == '\r')
                        *(p2 - 1) = '\0';
                     else
                        *p2 = '\0';
                     p2 ++;
                     while (*p == '>' || *p == ' ')
                        p ++;
                     while (*p1 == '>' || *p1 == ' ')
                        p1 ++;
                     while (*p2 == '>' || *p2 == ' ')
                        p2 ++;

                     T_SPRINTF(buffer1, "\r\n<FONT COLOR=\"%s\">", MG_SM_EVHEAD_COLOR); /* Header Line */
                     mg_client_write_string(p_request, buffer1);

                     p3 = NULL;
                     if (p3) {
                        *p3 = '\0';
                        mg_client_write_string(p_request, p);
                        mg_client_write_string(p_request, "</FONT>");
                        mg_client_write_string(p_request, p3 + 1);
                     }
                     else {
                        mg_client_write_string(p_request, p);
                        mg_client_write_string(p_request, "\r\n</B></FONT><BR>\r\n");
                     }

                     T_SPRINTF(buffer1, "\r\n<FONT COLOR=\"%s\">", MG_SM_EVTITLE_COLOR); /* Title Line */
                     mg_client_write_string(p_request, buffer1);

                     mg_sysman_send_log_line(p_request, p1, (unsigned long) strlen(p1));

                     mg_client_write_string(p_request, "</FONT>\r\n");
                     mg_client_write_string(p_request, "\r\n<br>");
                     T_SPRINTF(buffer1, "<FONT COLOR=\"%s\">", MG_SM_EVMESSAGE_COLOR); /* Message Line */
                     mg_client_write_string(p_request, buffer1);

                     mg_sysman_send_log_line(p_request, p2, (unsigned long) strlen(p2));
                     mg_client_write_string(p_request, "</FONT>\r\n");
                     mg_client_write_string(p_request, "\r\n<br>");
                     ptr = MG_CACHE_BUFFER - 10;
                     buf[ptr] = '\0';
                  }
               }
               mg_free((void *) buf, "ReverseLog:2");
            }
            fclose(fp);
         }
      }

      if (!reverse) {

         fp = T_FOPEN(core_data.mg_log, "r");
         if (fp != NULL) {
            n = 0;
            while (T_FGETS(buffer, 2040, fp) != NULL) {
               if (n == 1) {
                  T_SPRINTF(buffer1, "<FONT COLOR=\"%s\">", MG_SM_EVTITLE_COLOR); /* Title Line */
                  mg_client_write_string(p_request, buffer1);

                  mg_sysman_send_log_line(p_request, buffer, (unsigned long) strlen(buffer));

                  mg_client_write_string(p_request, "</FONT>");
                  n = 0;
               }
               else if (T_STRSTR(buffer, ">>>") != NULL) {
                  T_SPRINTF(buffer1, "<FONT COLOR=\"%s\">", MG_SM_EVHEAD_COLOR); /* Header Line */
                  mg_client_write_string(p_request, buffer1);

                  strcpy(buffer, buffer + 4);
                  n2 = (int) strlen(buffer);

                  mg_client_write_string(p_request, buffer);

                  if (n2) {

                     char *p;

                     p = NULL;
                     if (p) {
                        *p = '\0';
                        mg_client_write_string(p_request, "</FONT>");
                        mg_client_write_string(p_request, p + 1);
                    }
                     else {
                        mg_client_write_string(p_request, "</FONT><br>");
                     }
                  }
                  n = 1;
               }
               else {
                  mg_client_write_string(p_request, "<br>");
                  T_SPRINTF(buffer1, "<FONT COLOR=\"%s\">", MG_SM_EVMESSAGE_COLOR); /* Message Line */
                  mg_client_write_string(p_request, buffer1);
                  mg_sysman_send_log_line(p_request, buffer, (unsigned long) strlen(buffer));
                  mg_client_write_string(p_request, "</FONT>\r\n");
                  mg_client_write_string(p_request, "<br>");

               }

            }
            T_FCLOSE(fp);
         }
      }
      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (T_STRCMP(form_id, "sigsc") == 0) {

      T_STRCPY(buffer1, "Connection to Servers : Configuration");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigsc\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYSconfigSection\" VALUE=\"SYSTEM\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");
      for (n = 0; mgsm_con[n]; n ++) {
         len = mg_get_config("SYSTEM", mgsm_con[n], "", buffer, 1024, config_file);
         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_con_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_con[n], buffer);
         mg_client_write_string(p_request, buffer1);
      }    

      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_client_write_string(p_request, "<P>\r\n");
      mg_client_write_string(p_request, "<TABLE>\r\n");
      T_SPRINTF(buffer1, "<TR><TD><INPUT TYPE=SUBMIT NAME=\"%sSYSbOK\" VALUE=\"Save Configuration\"></TD>\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<TD></TD><TD></TD><TD></TD><TD><INPUT TYPE=SUBMIT NAME=\"%sSYSbRDV\" VALUE=\"Restore Default Values\"></TD></TR>\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (T_STRCMP(form_id, "sigsdba") == 0 || (T_STRCMP(form_id, "sigsdba1") == 0 && T_STRLEN(m_server_name) == 0)) {
      T_SPRINTF(buffer1, "Connection to Servers : %s Access", MG_DB_SERVER_TITLE);

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigsdba1\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<B>Configured %ss</B>", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);
      mg_sysman_server_list(p_request, "sigsdba1", "SYSTEM", "", "", core_data.mg_ini, 0);
      T_SPRINTF(buffer1, "<P>%s Name: <INPUT TYPE=TEXT NAME=\"%sDBS\" SIZE=\"20\" MAXLENGTH=\"30\">\r\n", MG_DB_SERVER_TITLE, MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);   
      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"SUBMIT\" VALUE=\"Configure %s\">\r\n", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);
      mg_sysman_tail(p_request, 0);
      return 1;
   }
   if (T_STRCMP(form_id, "sigsdba1") == 0) {

      T_SPRINTF(buffer1, "Connection to Servers : %s Access", MG_DB_SERVER_TITLE);

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigsdba1\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer, "<INPUT TYPE=HIDDEN NAME=\"%sDBS\" VALUE=\"%s\">\r\n", MG_RVAR_PREFIX, m_server_name);
      mg_client_write_string(p_request, buffer);
      T_SPRINTF(buffer, "<INPUT TYPE=HIDDEN NAME=\"%sSYSconfigSection\" VALUE=\"%s\">\r\n", MG_RVAR_PREFIX, m_server_name);
      mg_client_write_string(p_request, buffer);

      T_SPRINTF(buffer1, "%s Name: <B>", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, m_server_name);
      mg_client_write_string(p_request, "</B>\r\n");

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");

      len = mg_get_config(m_server_name, MG_P_BASE_TCP_PORT, "T", port, 7, config_file);


      len = mg_get_config("SYSTEM_INDEX", m_server_name, "Enabled", dbt, 32, config_file);
      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">Service Status: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);
     
      for (n = 0; mgsm_con_status[n]; n ++) {
         T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetService_Status\" VALUE=\"%s\"", MG_RVAR_PREFIX, mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
         if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
            mg_client_write_string(p_request, " CHECKED");
         T_SPRINTF(buffer1, "> %s ", mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
      }
      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_db_con[n]; n ++) {
         len = mg_get_config(m_server_name, mgsm_db_con[n], "", buffer, 1024, config_file);
         mg_sysman_form_encode(buffer);
         /* CM0051 */
         if (n == MGSI_PW1) {
            if (len)
               T_STRCPY(buffer, "       ");
            else
               T_STRCPY(buffer, "");
            T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=PASSWORD NAME=\"%sPW1\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_db_con_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, buffer);
/*
            T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=PASSWORD NAME=\"%sPW1\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"> %s <INPUT TYPE=PASSWORD NAME=\"%sPW2\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_db_con_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, buffer, mgsm_db_con_text[n + 1], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, buffer);
*/
         }
         else if (n == MGSI_PW2) {
            if (len)
               T_STRCPY(buffer, "       ");
            else
               T_STRCPY(buffer, "");
            T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=PASSWORD NAME=\"%sPW2\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_db_con_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, buffer);
         }
         else
            T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_db_con_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_db_con[n], buffer);
         mg_client_write_string(p_request, buffer1);
      }

      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\"><B>Optional Parameters:</B></TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><FONT SIZE=\"-1\"><I>(Will be inherited from global configuration if not specified)</I></FONT></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      for (n = 0; mgsm_db_icon[n]; n ++) {
         len = mg_get_config(m_server_name, mgsm_db_icon[n], "", buffer, 1024, config_file);
         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_db_icon_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_db_icon[n], buffer);
         mg_client_write_string(p_request, buffer1);
      }

      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\"><B>Fail-Over and Load-Balancing:</B></TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><FONT SIZE=\"-1\"><I>(Specifying Alternative Servers)</I></FONT></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      len = mg_get_config(m_server_name, "Alternative_Servers", "Disabled", dbt, 32, config_file);

      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">Alternative Servers: </TD>", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetAlternative_Servers\" VALUE=\"LoadBalancing\"", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      if (T_STRCMP(dbt, "LoadBalancing") == 0)
         mg_client_write_string(p_request, " CHECKED");
      mg_client_write_string(p_request,"> Use for Load-Balancing<br>");

      T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetAlternative_Servers\" VALUE=\"FailOver\"", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      if (T_STRCMP(dbt, "FailOver") == 0)
         mg_client_write_string(p_request, " CHECKED");
      mg_client_write_string(p_request,"> Use for Fail-Over only<br>");

      T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetAlternative_Servers\" VALUE=\"Disabled\"", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      if (T_STRCMP(dbt, "Disabled") == 0)
         mg_client_write_string(p_request, " CHECKED");
      mg_client_write_string(p_request,"> Disabled");

      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (sn = 1, msn = 0; ; sn ++) {
         T_SPRINTF(name, "Alternative_Server_%d", sn);
         T_SPRINTF(name1, "Alternative Server %d", sn);
         len = mg_get_config(m_server_name, name, "", buffer, 64, config_file);
         if (buffer[0] == '1')
            T_STRCPY(dbt, "Enabled");
         else
            T_STRCPY(dbt, "Disabled");
         mg_piece(buffer, buffer, "~", 7, 999);
         if (len)
            msn = sn;
         if (sn > msn + 3)
            break;
         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR, name1, MG_SM_CELL_BGCOLOR);
         mg_client_write_string(p_request, buffer1);
         mg_sysman_server_list(p_request, "", "SERVERS", name, buffer, config_file, 22);

         for (n = 0; mgsm_con_status[n]; n ++) {
            T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetAlternative_Server_%d_Status\" VALUE=\"%s\"", MG_RVAR_PREFIX, sn, mgsm_con_status[n]);
            mg_client_write_string(p_request, buffer1);
            if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
               mg_client_write_string(p_request, " CHECKED");
            T_SPRINTF(buffer1, "> %s", mgsm_con_status[n]);
            mg_client_write_string(p_request, buffer1);
         }

         mg_client_write_string(p_request, "</TD></TR>\r\n");
      }

      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_client_write_string(p_request, "<P>\r\n");
      T_SPRINTF(buffer1, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbOK\" VALUE=\"Save Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "<P>New Server Name: \r\n");
      T_SPRINTF(buffer1, "<INPUT TYPE=TEXT NAME=\"%sSYSnewDBS\" SIZE=\"20\" MAXLENGTH=\"20\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbCOPY\" VALUE=\"Copy Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"%sSYSbDEL\" VALUE=\"Delete Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_sysman_tail(p_request, 0);

      return 1;
   }
   if (T_STRCMP(form_id, "sigsdba2") == 0 && T_STRLEN(m_server_name) > 0) {
      T_SPRINTF(buffer1, "Connection to Servers : Remove %s Access", MG_DB_SERVER_TITLE);

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigsdba2\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sDBS\" VALUE=\"%s\">\r\n", MG_RVAR_PREFIX, m_server_name);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "Delete the Configuration for Server: <B>%s</B>\r\n", m_server_name);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, "<P><FONT COLOR=\"RED\"><B>Are You Sure ?</B></FONT>\r\n");
      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"%sSYSbDEL\" VALUE=\"YES : DELETE\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (T_STRCMP(form_id, "sigstc") == 0) {
      T_STRCPY(buffer1, "Connection to Servers : Test Connection");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sdef\" VALUE=\"Default_Server\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYSTestPage\" VALUE=\"Show All Variables\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);


      T_SPRINTF(buffer1, "<P>%s Name: \r\n", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);

      mg_sysman_server_list(p_request, "", "SYSTEM", "Default_Server", "", core_data.mg_ini, 2);

      mg_client_write_string(p_request, "<P><INPUT TYPE=SUBMIT NAME=\"SUBMIT\" VALUE=\"Connect\">\r\n");
      mg_sysman_tail(p_request, 0);
      return 1;
   }

   if (T_STRCMP(form_id, "sigscc") == 0) {

      T_STRCPY(buffer1, "Connection to Servers : Close Connections");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sdef\" VALUE=\"Default_CloseDown_Server\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigscc\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<P>Close All Connections to %s: ", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);
      mg_sysman_server_list(p_request, "", "SYSTEM", "Default_CloseDown_Server", "", core_data.mg_ini, 1);

      T_SPRINTF(buffer1, "<P>(Select * to Close All Connections to All %ss)<BR>\r\n", MG_DB_SERVER_TITLE);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"%sSYSbOK\" VALUE=\"Close Connections(s)\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      mg_sysman_tail(p_request, 0);
      return 1;
   }

   if (T_STRCMP(form_id, "sigmqc") == 0) {

      T_STRCPY(buffer1, "IBM MQ : Configuration");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmqc\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYSconfigSection\" VALUE=\"SYSTEM\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");

#ifdef _WIN32
      if (GetEnvironmentVariable((LPCTSTR) "MQSERVER", buffer, 128) < 1)
         strcpy(buffer, "");
#else
      wk_script = getenv("MQSERVER");
      if (wk_script)
         strcpy(buffer, getenv("MQSERVER"));
      else
         strcpy(buffer, "");
#endif

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD>\r\n", MG_SM_CELL_BGCOLOR, "IBM MQ Environment Variable (MQSERVER)");
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\" ALIGN=\"LEFT\"><B>%s</B></TD>\r\n", MG_SM_CELL_BGCOLOR, buffer);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, "</TR>");

      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_client_write_string(p_request, "<P>\r\n");

      mg_sysman_tail(p_request, 0);
      return 1;
   }

   if (T_STRCMP(form_id, "sigmqsc") == 0 || (T_STRCMP(form_id, "sigmqsc1") == 0 && T_STRLEN(p_smdata->mq_service) == 0)) {
      T_SPRINTF(buffer1, "IBM MQ : Configure Service");

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmqsc1\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<B>Configured Services</B>");
      mg_client_write_string(p_request, buffer1);
      mg_sysman_wsmq_server_list(p_request, "sigmqsc1", "SYSTEM", "", "", core_data.mg_ini, 0);
      T_SPRINTF(buffer1, "<P>Service Name: <INPUT TYPE=TEXT NAME=\"%sMQService\" SIZE=\"20\" MAXLENGTH=\"30\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);   
      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"SUBMIT\" VALUE=\"Configure Service\">\r\n");
      mg_client_write_string(p_request, buffer1);
      mg_sysman_tail(p_request, 0);
      return 1;
   }
   if (T_STRCMP(form_id, "sigmqsc1") == 0) {

      T_SPRINTF(buffer1, "IBM MQ: Configure Service");
      T_SPRINTF(p_smdata->config_section, "WSMQS:%s", p_smdata->mq_service);

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmqsc1\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer, "<INPUT TYPE=HIDDEN NAME=\"%sMQService\" VALUE=\"%s\">\r\n", MG_RVAR_PREFIX, p_smdata->mq_service);
      mg_client_write_string(p_request, buffer);
      T_SPRINTF(buffer, "<INPUT TYPE=HIDDEN NAME=\"%sSYSconfigSection\" VALUE=\"WSMQS:%s\">\r\n", MG_RVAR_PREFIX, p_smdata->mq_service);
      mg_client_write_string(p_request, buffer);

      T_SPRINTF(buffer1, "Service Name: <B>");
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, p_smdata->mq_service);
      mg_client_write_string(p_request, "</B>\r\n");

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");

      len = mg_get_config("WSMQS_INDEX", p_smdata->mq_service, "Enabled", dbt, 32, config_file);
      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">Service Status: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\">\r\n", MG_SM_CELL_BGCOLOR, MG_SM_CELL_BGCOLOR);  
      mg_client_write_string(p_request, buffer1);
    
      for (n = 0; mgsm_con_status[n]; n ++) {
         T_SPRINTF(buffer1, "<INPUT TYPE=RADIO NAME=\"%sSYSsetService_Status\" VALUE=\"%s\"", MG_RVAR_PREFIX, mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
         if (T_STRCMP(dbt, mgsm_con_status[n]) == 0)
            mg_client_write_string(p_request, " CHECKED");
         T_SPRINTF(buffer1, "> %s ", mgsm_con_status[n]);
         mg_client_write_string(p_request, buffer1);
      }
      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_mqs[n]; n ++) {
         len = mg_get_config(p_smdata->config_section, mgsm_mqs[n], "", buffer, 1024, config_file);
         mg_sysman_form_encode(buffer);
         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_mqs_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_mqs[n], buffer);
         mg_client_write_string(p_request, buffer1);
      }

      T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\">", MG_SM_CELL_BGCOLOR, "Server", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      len = mg_get_config(p_smdata->config_section, "Server", "", buffer, 1024, config_file);

      mg_sysman_server_list(p_request, "", "", "Server", buffer, core_data.mg_ini, 24);

      mg_client_write_string(p_request, "</TD></TR>\r\n");

      for (n = 0; mgsm_mqc[n]; n ++) {
         len = mg_get_config(p_smdata->config_section, mgsm_mqc[n], "", buffer, 1024, config_file);
         mg_sysman_form_encode(buffer);
         T_SPRINTF(buffer1, "<TR><TD bgcolor=\"%s\" ALIGN=\"RIGHT\">%s: </TD><TD bgcolor=\"%s\" ALIGN=\"LEFT\"><INPUT TYPE=TEXT NAME=\"%sSYSset%s\" VALUE=\"%s\" SIZE=\"40\" MAXLENGTH=\"2048\"></TD></TR>\r\n", MG_SM_CELL_BGCOLOR, mgsm_mqc_text[n], MG_SM_CELL_BGCOLOR, MG_RVAR_PREFIX, mgsm_mqc[n], buffer);
         mg_client_write_string(p_request, buffer1);
      }


      mg_client_write_string(p_request, "</TABLE>\r\n");

      mg_client_write_string(p_request, "<P>\r\n");
      T_SPRINTF(buffer1, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbOK\" VALUE=\"Save Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "<P>New Service Name: \r\n");
      T_SPRINTF(buffer1, "<INPUT TYPE=TEXT NAME=\"%sSYSnewMQService\" SIZE=\"20\" MAXLENGTH=\"20\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=SUBMIT NAME=\"%sSYSbCOPY\" VALUE=\"Copy Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"%sSYSbDEL\" VALUE=\"Delete Configuration\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_sysman_tail(p_request, 0);

      return 1;
   }
   if (T_STRCMP(form_id, "sigmqsc2") == 0 && T_STRLEN(p_smdata->mq_service) > 0) {
      T_SPRINTF(buffer1, "IBM MQ : Remove Service");
      T_SPRINTF(p_smdata->config_section, "WSMQS:%s", p_smdata->mq_service);

      mg_sysman_top(p_request, 0, 0, buffer1, "", core_data.mg_lib_path, core_data.mg_lib_path);

      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sSYS\" VALUE=\"sigmqsc2\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "<INPUT TYPE=HIDDEN NAME=\"%sMQService\" VALUE=\"%s\">\r\n", MG_RVAR_PREFIX, p_smdata->mq_service);
      mg_client_write_string(p_request, buffer1);
      T_SPRINTF(buffer1, "Delete the Configuration for Service: <B>%s</B>\r\n", p_smdata->mq_service);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, "<P><FONT COLOR=\"RED\"><B>Are You Sure ?</B></FONT>\r\n");
      T_SPRINTF(buffer1, "<P><INPUT TYPE=SUBMIT NAME=\"%sSYSbDEL\" VALUE=\"YES : DELETE\">\r\n", MG_RVAR_PREFIX);
      mg_client_write_string(p_request, buffer1);

      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (!T_STRCMP(form_id, "sigabout")) {

      MEMOBJ cgievar, *p_cgievar;

      p_cgievar = &cgievar;
      mg_buf_init(p_cgievar, 256, 256);

      mg_sysman_top(p_request, 0, 0, "About the SIG", "", core_data.mg_lib_path, core_data.mg_lib_path);

      mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");
      mg_client_write_string(p_request, "<tr>\r\n");

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\" align=left>Version:</td>\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\">%s (Created: %s)</td>\r\n", MG_SM_CELL_BGCOLOR, MG_VERSION, MG_VERSION_CREATED);
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "</tr>\r\n");

      mg_client_write_string(p_request, "<tr>\r\n");

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\" align=left>Description:</td>\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\">%s</td>\r\n", MG_SM_CELL_BGCOLOR, "Mediates between software components providing services to applications.");
      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "</tr>\r\n");

      T_SPRINTF(buffer1, "<tr bgcolor=\"%s\">\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\" align=left>Copyright:</td>\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      T_SPRINTF(buffer1, "<TD bgcolor=\"%s\"><A HREF=\"http://www.mgateway.com\" TARGET=mgateway_window><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">MGateway Ltd.</font></A></TD>\r\n", MG_SM_CELL_BGCOLOR);

      mg_client_write_string(p_request, buffer1);

      mg_client_write_string(p_request, "</tr>\r\n");

      mg_client_write_string(p_request, "<tr>\r\n"); 

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\" align=left>Web Server Name:</td>\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      mg_get_server_variable(p_request, "SERVER_NAME", &cgievar);
      T_SPRINTF(buffer1, "<td bgcolor=\"%s\">%s\r\n", MG_SM_CELL_BGCOLOR, p_cgievar->p_buffer);
      mg_client_write_string(p_request, buffer1);
      mg_client_write_string(p_request, "</td>\r\n");
      mg_client_write_string(p_request, "</tr>\r\n");

      mg_client_write_string(p_request, "<tr>\r\n");

      T_SPRINTF(buffer1, "<td bgcolor=\"%s\" align=left>Web Server Type:</td>\r\n", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

       T_SPRINTF(buffer1, "<td bgcolor=\"%s\">", MG_SM_CELL_BGCOLOR);
      mg_client_write_string(p_request, buffer1);

      mg_get_server_variable(p_request, "SERVER_SOFTWARE", &cgievar);

      mg_client_write_string(p_request, cgievar.p_buffer);
      mg_client_write_string(p_request, "\r\n");
      mg_buf_free(&cgievar);

      mg_client_write_string(p_request, "</td>\r\n");
      mg_client_write_string(p_request, "</tr>\r\n");

      mg_client_write_string(p_request, "</table>\r\n");

      mg_sysman_tail(p_request, 0);

      return 1;
   }

   if (strstr(form_id, "sigdoc")) {

      char *b, *p1, *p2;

      b = (char *) malloc(sizeof(char) * (sizeof(mgsidoc) + 32));
      memcpy((char *) b, mgsidoc, sizeof(mgsidoc));
      b[sizeof(mgsidoc)] = '\0';

      p1 = strstr(b, "<HR>");
      if (p1) {
         p2 = strstr(p1, "Copyright &copy");
         p1 += 4;
         if (p2) {
            *(p2 - 31) = '\0';
         }
         else
            p1 = b;
      }
      else
         p1 = b;

      mg_sysman_default_header(p_request);
/*
      mg_sysman_top(p_request, 0, 0, "Documentation : SI Gateway", "", core_data.mg_lib_path, core_data.mg_lib_path);
*/
      mg_client_write_string(p_request, p1);
      mg_sysman_tail(p_request, 0);

      if (b) {
         free((void *) b);
      }

      return 1;
   }

   n = mg_return_message(p_request, 11, "SIG Management Error", "The operation selected is not available", "");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_sysman_send_log_line(MGREQ *p_request, char *buffer, unsigned long size)
{
   unsigned long n1, n2;
   char buffer1[2048];

#ifdef _WIN32
__try {
#endif

   *buffer1 = '\0';
   for (n1 = 0, n2 = 0; n1 < size; n1 ++) {
      if (buffer[n1] == '<') {
         T_STRCAT(buffer1, "&lt;");
         n2 += 4;
      }
      else if (buffer[n1] == '>') {
        T_STRCAT(buffer1, "&gt;");
        n2 += 4;
      }
      else {
         buffer1[n2 ++] = buffer[n1];
         buffer1[n2] = '\0';
      }
      if (n2 > 2040) {
         mg_client_write_string(p_request, buffer1);
         n2 = 0;
         *buffer1 = '\0';
      }
   }

   if (n2)
      mg_client_write_string(p_request, buffer1);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_send_log_line: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_sysman_top(MGREQ *p_request, int type, int target, char *title, char *redirect, char *action, char *escape)
{
   char buffer[256], token[32];
   MEMOBJ cgievar;
   LPMEMOBJ p_cgievar;

#ifdef _WIN32
__try {
#endif

   p_cgievar = &cgievar;

   mg_make_token(buffer, 8, 1);
   T_SPRINTF(token, "%sSYSn=%s", MG_RVAR_PREFIX, buffer);

   mg_site_parameters();

   if (T_STRLEN(redirect)) {

      mg_buf_init(p_cgievar, 256, 511);

      mg_client_write_string(p_request, "HTTP/1.1 301 REDIRECT\r\n");
      mg_client_write_string(p_request, "Location: ");
      mg_client_write_string(p_request, redirect);

      mg_get_server_variable(p_request, "SCRIPT_NAME", p_cgievar);
      T_SPRINTF(buffer, "?MGLIB=%s", p_cgievar->p_buffer);
      mg_client_write_string(p_request, buffer);
      mg_get_server_variable(p_request, "QUERY_STRING", p_cgievar);
      if (strlen(p_cgievar->p_buffer)) {
         mg_client_write_string(p_request, "&");
         mg_client_write_string(p_request, p_cgievar->p_buffer);
      }
      mg_buf_free(p_cgievar);

      mg_client_write_string(p_request, "\r\n");
      mg_client_write_string(p_request, "Connection: close\r\n");
      mg_client_write_string(p_request, "\r\n");
   }
   else
      mg_sysman_default_header(p_request);

   mg_client_write_string(p_request, "<html>\r\n");
   mg_client_write_string(p_request, "  <head>\r\n");

   T_SPRINTF(buffer, "    <title>%s : Management</title>\r\n", core_data.mg_title);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "  </head>\r\n");
   mg_client_write_string(p_request, "  <body bgcolor=\"#FFFFFF\" bgproperties=\"\" bottommargin=\"12\" leftmargin=\"12\" topmargin=\"12\" rightmargin=\"12\" link=003366>\r\n");

   T_SPRINTF(buffer, "    <form METHOD=POST ACTION=\"%s\">\r\n", action);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "<table cellspacing=\"1\" cellpadding=\"4\" style=\"border-style:solid;border-width:0px\">\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");
   mg_client_write_string(p_request, "<td bgcolor=\"#FFFCF5\" valign=\"top\" style=\"border-style:solid;border-width:1px 1px 0px 1px;border-color:#EBAB3C\">\r\n");
   mg_client_write_string(p_request, "<table border=\"0\" cellspacing=\"2\" cellpadding=\"8\" align=\"center\">\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");
   mg_client_write_string(p_request, "<td valign=\"top\">\r\n");
   mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\">\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");
   mg_client_write_string(p_request, "<td>\r\n");
   mg_client_write_string(p_request, "<font FACE=\"Arial, Helvetica, sansserif\" SIZE=\"4\">\r\n");

   T_SPRINTF(buffer, "<b>Service Integration Gateway (SIG) Version %s</b>\r\n", MG_VERSION_DISPLAY);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "</font>\r\n");
   mg_client_write_string(p_request, "</td>\r\n");
   mg_client_write_string(p_request, "<td align=\"right\">\r\n");
   mg_client_write_string(p_request, "<font FACE=\"Arial, Helvetica, sansserif\" SIZE=\"4\">\r\n");

   mg_client_write_string(p_request, "<b>Management</b>\r\n");

   mg_client_write_string(p_request, "</font>\r\n");
   mg_client_write_string(p_request, "</td>\r\n");
   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "</table>\r\n");
   mg_client_write_string(p_request, "<br />\r\n");
   mg_client_write_string(p_request, "<table width=\"100%\" border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");
   mg_client_write_string(p_request, "<th bgcolor=\"#DCE1F2\" align=\"left\" valign=\"top\">&nbsp;<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">\r\n");
   mg_client_write_string(p_request, "SIG</font>&nbsp;\r\n");
   mg_client_write_string(p_request, "</th>\r\n");
   mg_client_write_string(p_request, "<th bgcolor=\"#DCE1F2\" align=\"left\" valign=\"top\">&nbsp;<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">\r\n");
   mg_client_write_string(p_request, "Connection to Servers</font>&nbsp;\r\n");
   mg_client_write_string(p_request, "</th>\r\n");
   mg_client_write_string(p_request, "<th bgcolor=\"#DCE1F2\" align=\"left\" valign=\"top\">&nbsp;<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">\r\n");
   mg_client_write_string(p_request, "IBM MQ</font>&nbsp;\r\n");
   mg_client_write_string(p_request, "</th>\r\n");
   mg_client_write_string(p_request, "<th bgcolor=\"#DCE1F2\" align=\"left\" valign=\"top\">&nbsp;<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">\r\n");
   mg_client_write_string(p_request, "Help</font>&nbsp;\r\n");
   mg_client_write_string(p_request, "</th>\r\n");
   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigmc&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Configuration</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigsc&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Configuration</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigmqc&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Configuration</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigabout&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">About the SIG</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigms&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Status</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigsdba&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">DB Server Access</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigmqsc&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Configure Service</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigdocd&%s\" target=mgsidoc_window><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Documentation</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigmel&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Event Log</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigstc&%s&\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Test Connection</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;</td>\r\n");
   mg_client_write_string(p_request, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;</td>\r\n");

   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "<tr>\r\n");
   mg_client_write_string(p_request, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;</td>\r\n");

   T_SPRINTF(buffer, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;<a href=\"%s?%sSYS=sigscc&%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">Close Connections</font></a></td>\r\n", core_data.mg_lib_path, MG_RVAR_PREFIX, token);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;</td>\r\n");
   mg_client_write_string(p_request, "<td bgcolor=\"#DCE1F2\" valign=\"top\">&nbsp;</td>\r\n");
   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "</table>\r\n");
   mg_client_write_string(p_request, "<br />\r\n");
   mg_client_write_string(p_request, "<font FACE=\"Arial, Helvetica, sansserif\" size=\"3\">\r\n");
   mg_client_write_string(p_request, "<b> <center>\r\n");
   mg_client_write_string(p_request, title);
   mg_client_write_string(p_request, "</center> </b>\r\n");
   mg_client_write_string(p_request, "</font>\r\n");

   mg_client_write_string(p_request, "<font FACE=\"Arial, Helvetica, sansserif\" size=\"2\">\r\n");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_top: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_sysman_default_header(MGREQ *p_request)
{
   unsigned long dwWritten;
   char str[256];

#ifdef _WIN32
__try {
#endif

   str[0] = '\0';
   dwWritten = 0;

   mg_client_write_string(p_request, "HTTP/1.1 200 OK\r\n");
   mg_client_write_string(p_request, "Content-type: text/html\r\n");
   mg_client_write_string(p_request, "Connection: close\r\n");

   mg_client_write_string(p_request, "\r\n");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_default_header: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_sysman_tail(MGREQ *p_request, int type)
{
   int n, len;
   char buffer[256], word1[32], word2[16];
   char sesh[32];
   char timestr[64], yearstr[8];
   time_t now = 0;
   MEMOBJ cgievar;

#ifdef _WIN32
__try {
#endif

   *sesh = '\0';

   cgievar.size = 0;
   *word1 = '\0';
   *word2 = '\0';

   now = time(NULL);
   T_SPRINTF(timestr, ">>> %s", T_CTIME(&now));
   for (n = 0; timestr[n] != '\0'; n ++) {
      if (timestr[n] < '\x20') {
         timestr[n] = '\0';
         break;
      }
   }
   len = (int) strlen(timestr);
   if (len > 4) {
      strcpy(yearstr, timestr + (len - 4));
      if (strncmp(yearstr, "20", 2))
         strcpy(yearstr, MG_CURRENT_YEAR);
   }
   else
      strcpy(yearstr, MG_CURRENT_YEAR);

   mg_client_write_string(p_request, "</table>\r\n");
   mg_client_write_string(p_request, "</td>\r\n");
   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "<tr style=\"border-color:#000000\" >\r\n");
   mg_client_write_string(p_request, "<td valign=\"top\" bgcolor=\"#000000\" style=\"border-color:#000000\" height=\"10\">\r\n");
   mg_client_write_string(p_request, "<font face=\"Sans-serif\" size=\"1\" color=\"#fffcf5\">\r\n");

   T_SPRINTF(buffer, "&copy; 2004-%s MGateway Ltd All Rights Reserved. &nbsp;&nbsp;&nbsp;&nbsp;\r\n", yearstr);
   mg_client_write_string(p_request, buffer);

   mg_client_write_string(p_request, "<a href=\"http://www.mgateway.com\" style=\"color:#ffffff;\">http://www.mgateway.com</a>\r\n");
   mg_client_write_string(p_request, "</font>\r\n");
   mg_client_write_string(p_request, "</td>\r\n");
   mg_client_write_string(p_request, "</tr>\r\n");
   mg_client_write_string(p_request, "</table>\r\n");
   mg_client_write_string(p_request, "</form>\r\n");
   mg_client_write_string(p_request, "</body>\r\n");
   mg_client_write_string(p_request, "</html>\r\n");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_tail: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_sysman_access(MGREQ *p_request)
{
   int len, n;
   unsigned long j;
   char ansi_remIpAddress[32] = "\0";
   char remIpAddress[32] = "\0", sysman[256] = "\0";
   MEMOBJ cgievar;
   LPMEMOBJ p_cgievar;

#ifdef _WIN32
__try {
#endif

   j = 32;

   p_cgievar = &cgievar;

   mg_buf_init(p_cgievar, 64, 64);

   mg_get_server_variable(p_request, "REMOTE_ADDR", p_cgievar);
   T_STRCPY(remIpAddress, p_cgievar->p_buffer);
   mg_buf_free(p_cgievar);

   len = mg_get_config("SYSTEM", MG_P_SYSTEM_MANAGER, "", sysman, 200, core_data.mg_ini);

   if (T_STRSTR(sysman, "*.*.*.*"))
      return 1;

   if (len == 0)
      return 1;
   if (T_STRSTR(sysman, remIpAddress) != NULL)
      return 1;

   n = mg_return_message(p_request, 1, "SIG Management", "You are not authorised to use this facility", "SYSTEM_INDEX");

   mg_log_event(remIpAddress, "The following unauthorised client attempted to access SI Gateway's SIG Management suite");

   return 0;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_access: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_sysman_server_list(MGREQ *p_request, char *nextFormID, char *section, char *defKey, char *defValue, char *config_file, int type)
{
   int n, cn, defset;
   char *pBuffer, *pKey;
   char name[64], buffer[1024], names[2048], keyEnc[32], defVal[32];

#ifdef _WIN32
__try {
#endif

   defset = 0;
   *defVal = '\0';
   *names = '\0';

   if (type == 22 || type == 24)
      T_STRCPY(defVal, defValue);
   else {
      if (T_STRLEN(defKey))
         mg_get_config(section, defKey, "", defVal, 31, config_file);
   }

   if (type == 1)
      T_SPRINTF(name, "%sCDN", MG_RVAR_PREFIX);
   else if (type == 21 || type == 22 || type == 23 || type == 24)
      T_SPRINTF(name, "%sSYSset%s", MG_RVAR_PREFIX, defKey);
   else
      T_SPRINTF(name, "%sDBS", MG_RVAR_PREFIX);

   pBuffer = names;

   mg_get_config("SYSTEM_INDEX", NULL, "", pBuffer, 1024, core_data.mg_ini);

   if (type != 0) {
      T_SPRINTF(buffer, "<SELECT NAME=\"%s\">\r\n", name);
      mg_client_write_string(p_request, buffer);

      if (type == 22) {
         if (!T_STRLEN(defVal)) {
            mg_client_write_string(p_request, "<OPTION SELECTED>");
         }
         else
            mg_client_write_string(p_request, "<OPTION>");
         mg_client_write_string(p_request, "--- none ---");
         mg_client_write_string(p_request, "\r\n");
      }
   }


   cn = 0;
   if (type == 0)
      mg_client_write_string(p_request, "\r\n<table border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n<TR>\r\n");

   for (pKey = pBuffer, n = 0; *pKey != '\0'; pKey += T_STRLEN(pKey) + 1) {
   
      if (T_STRCMP(pKey, "SYSTEM") == 0)
         continue;

      if (type == 0) {
         mg_sysman_url_encode(keyEnc, pKey);
         T_SPRINTF(buffer, "<TD bgcolor=\"%s\" valign=\"top\">&nbsp;<A HREF=\"%s?%sSYS=%s&%sDBS=%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">%s</font></A>&nbsp;</TD>\r\n", MG_SM_CELL_BGCOLOR, core_data.mg_lib_path, MG_RVAR_PREFIX, nextFormID, MG_RVAR_PREFIX, keyEnc, pKey);
         mg_client_write_string(p_request, buffer);
         cn ++;
         if (cn == 7) {
            mg_client_write_string(p_request, "</TR><TR>\r\n");
            cn = 0;
         }
      }
      if (type != 0) {
         if (!T_STRCMP(defVal, pKey)) {
            mg_client_write_string(p_request, "<OPTION SELECTED>");
            defset = 1;
         }
         else
            mg_client_write_string(p_request, "<OPTION>");
         mg_client_write_string(p_request, pKey);
         mg_client_write_string(p_request, "\r\n");
      }
   }

   if (type == 0)
      mg_client_write_string(p_request, "</TABLE>\r\n");

   if (type == 1) {
      if (defset)
         mg_client_write_string(p_request, "<OPTION>*\r\n");
      else
         mg_client_write_string(p_request, "<OPTION SELECTED>*\r\n");
   }


   if (type != 0)
      mg_client_write_string(p_request, "</SELECT>\r\n");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_server_list: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_sysman_wsmq_server_list(MGREQ *p_request, char *nextFormID, char *section, char *defKey, char *defValue, char *config_file, int type)
{
   int n, cn, defset;
   char *pBuffer, *pKey;
   char name[64], buffer[1024], names[2048], keyEnc[32], defVal[32];

#ifdef _WIN32
__try {
#endif

   defset = 0;
   *defVal = '\0';
   *names = '\0';

   if (type == 22)
      T_STRCPY(defVal, defValue);
   else {
      if (T_STRLEN(defKey))
         mg_get_config(section, defKey, "", defVal, 31, config_file);
   }

   if (type == 1)
      T_SPRINTF(name, "%sCDN", MG_RVAR_PREFIX);
   else if (type == 21 || type == 22 || type == 23)
      T_SPRINTF(name, "%sSYSset%s", MG_RVAR_PREFIX, defKey);
   else
      T_SPRINTF(name, "%sMQService", MG_RVAR_PREFIX);

   pBuffer = names;

   mg_get_config("WSMQS_INDEX", NULL, "", pBuffer, 1024, core_data.mg_ini);

   if (type != 0) {
      T_SPRINTF(buffer, "<SELECT NAME=\"%s\">\r\n", name);
      mg_client_write_string(p_request, buffer);

      if (type == 22) {
         if (!T_STRLEN(defVal)) {
            mg_client_write_string(p_request, "<OPTION SELECTED>");
         }
         else
            mg_client_write_string(p_request, "<OPTION>");
         mg_client_write_string(p_request, "--- none ---");
         mg_client_write_string(p_request, "\r\n");
      }
   }


   cn = 0;
   if (type == 0)
      mg_client_write_string(p_request, "\r\n<table border=\"0\" bgcolor=\"#FFFCF5\" cellspacing=\"2\">\r\n<TR>\r\n");

   for (pKey = pBuffer, n = 0; *pKey != '\0'; pKey += T_STRLEN(pKey) + 1) {
   
      if (T_STRCMP(pKey, "SYSTEM") == 0)
         continue;

      if (type == 0) {
         mg_sysman_url_encode(keyEnc, pKey);
         T_SPRINTF(buffer, "<TD bgcolor=\"%s\" valign=\"top\">&nbsp;<A HREF=\"%s?%sSYS=%s&%sMQService=%s\"><font FACE=\"Arial, Helvetica, sansserif\" size=\"3\" color=\"#000000\">%s</font></A>&nbsp;</TD>\r\n", MG_SM_CELL_BGCOLOR, core_data.mg_lib_path, MG_RVAR_PREFIX, nextFormID, MG_RVAR_PREFIX, keyEnc, pKey);
         mg_client_write_string(p_request, buffer);
         cn ++;
         if (cn == 7) {
            mg_client_write_string(p_request, "</TR><TR>\r\n");
            cn = 0;
         }
      }
      if (type != 0) {
         if (!T_STRCMP(defVal, pKey)) {
            mg_client_write_string(p_request, "<OPTION SELECTED>");
            defset = 1;
         }
         else
            mg_client_write_string(p_request, "<OPTION>");
         mg_client_write_string(p_request, pKey);
         mg_client_write_string(p_request, "\r\n");
      }
   }

   if (type == 0)
      mg_client_write_string(p_request, "</TABLE>\r\n");

   if (type == 1) {
      if (defset)
         mg_client_write_string(p_request, "<OPTION>*\r\n");
      else
         mg_client_write_string(p_request, "<OPTION SELECTED>*\r\n");
   }


   if (type != 0)
      mg_client_write_string(p_request, "</SELECT>\r\n");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_wsmq_server_list: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_sysman_documentation(MGREQ *p_request)
{
   char *b, *p1, *p2;

   b = (char *) malloc(sizeof(char) * (sizeof(mgsidoc) + 32));
   memcpy((char *) b, mgsidoc, sizeof(mgsidoc));
   b[sizeof(mgsidoc)] = '\0';

   p1 = strstr(b, "<HR>");
   if (p1) {
      p2 = strstr(p1, "Copyright &copy");
      p1 += 4;
      if (p2) {
         *(p2 - 31) = '\0';
      }
      else
         p1 = b;
   }
   else
      p1 = b;

   mg_sysman_default_header(p_request);
/*
   mg_sysman_top(p_request, 0, 0, "Documentation : SI Gateway", "", core_data.mg_lib_path, core_data.mg_lib_path);
*/
   mg_client_write_string(p_request, p1);
   mg_sysman_tail(p_request, 0);

   if (b) {
      free((void *) b);
   }

   return 1;
}


/* URL encode on spaces */

int mg_sysman_url_encode(char *outstring, char *instring)
{
   int n;

#ifdef _WIN32
__try {
#endif

   n = 0;
   while (instring[n] != '\0') {
      if (instring[n] == ' ')
         outstring[n] = '+';
      else
         outstring[n] = instring[n];
      n ++;
   }
   outstring[n] = '\0';

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_url_encode: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Encode defaults for form input */

int mg_sysman_form_encode(char *value)
{
   int n, len;
   char *temp;
   char enc[8];

#ifdef _WIN32
__try {
#endif

   len = (int) T_STRLEN(value);

   temp = (char *) mg_malloc(sizeof(char) * (len + 1), "mg_sysman_form_encode:1");

   if (!temp) {
      mg_log_event("No Memory", "ERROR");
      return 0;
   }

   T_STRCPY(temp, value);
   value[0] = '\0';
   for (n = 0; temp[n] != '\0'; n ++) {
      if (temp[n] == '\"')
         T_STRCPY(enc, "&#34;");
      else {
         enc[0] = temp[n];
         enc[1] = '\0';
      }
      T_STRCAT(value, enc);
   }

   mg_free((void *) temp, "mg_sysman_form_encode:1");

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_form_encode: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


/* Translate all control characters likely to interact with M read */

int mg_sysman_trans_control_chars(char *buffer, unsigned long dwLength)
{
   unsigned long line_no;

#ifdef _WIN32
__try {
#endif

   for (line_no = 0; line_no < dwLength; line_no ++) {
      if (buffer[line_no] == '\x0d')
         buffer[line_no] = '\x1f';
      else if (buffer[line_no] == '\x0a')
         buffer[line_no] = '\x1e';
   }


   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_sysman_trans_control_chars: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


