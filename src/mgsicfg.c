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


#include "mgsisys.h"
#include "mgsi.h"
#include "mgsiutl.h"
#include "mgsicfg.h"
#include "mgsidba.h"


int mg_site_parameters()
{
   short phase;
   int n, len;
   char buffer[4096], ip[32];
   char *pKey;

   phase = 0;

#ifdef _WIN32
__try {
#endif

   len = 0;
   *buffer = '\0';

   core_data.max_servers = MG_MAX_SERVERS;
   core_data.max_connections = MG_MAX_CONNECTIONS;

   strcpy(mg_sec.wsmq_ipb, "");
   phase = 1;
   mg_get_config("WSMQ", NULL, "", buffer, 4090, core_data.mg_sec);
   mg_sec.wsmq_ip[0] = NULL;

   phase = 2;

   for (pKey = buffer, n = 0, len = 0; *pKey != '\0'; pKey += T_STRLEN(pKey) + 1) {
      phase = 3;
      if (!T_STRNCMP("Allowed_Client", pKey, 14)) {

         phase = 4;
         mg_get_config("WSMQ", pKey, "", ip, 4090, core_data.mg_sec);

         strcpy(mg_sec.wsmq_ipb + len, ip);
         mg_sec.wsmq_ip[n ++] = (mg_sec.wsmq_ipb + len);
         mg_sec.wsmq_ip[n] = NULL;
         len += ((int) strlen(ip) + 1);
      }
   }

   phase = 9;
   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_site_parameters: %x:%d", code, phase);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_get_config(char *section, char *entry, char *defvalue, char *value, int size, char *file)
{

#if !defined(_WIN32)

   int insect, len, vlen, getall, ptr, n, valuelen;
   char lbuffer[8192], sbuffer[64], fentry[256];
   char *p_general;
   FILE *fp;

   T_STRCPY(value, defvalue);
   valuelen = T_STRLEN(value);
   if (section == NULL || T_STRLEN(section) == 0)
      return valuelen;
   if (entry == NULL || T_STRLEN(entry) == 0)
      getall = 1;
   else
      getall = 0;
   T_STRCPY(sbuffer, "[");
   T_STRCAT(sbuffer, section);
   T_STRCAT(sbuffer, "]");
   insect = 0;
   ptr = 0;

   n = mg_mutex_lock((MUTOBJ *) core_data.p_memlockFA);

   fp = fopen(file, "r");
   if (fp != NULL) {
      while (fgets(lbuffer, 512, fp) != NULL) {
         len = mg_trim_string((char *) lbuffer);
         if (lbuffer[0] == '[' && insect == 1)
            break;

         if (strcmp(lbuffer, sbuffer) == 0) {
            insect = 1;
            continue;
         }

         if (!insect)
            continue;

         p_general = strstr(lbuffer, "=");
         if (!p_general)
            continue;

         *p_general = '\0';
         T_STRCPY(fentry, lbuffer);
         *p_general = '=';

         if (getall == 0 && insect == 1 && strcmp(fentry, (char *) entry) == 0) {
            p_general = strstr(lbuffer, "=");

            if (p_general) {
               vlen = strlen(p_general + 1);
               if (vlen < size) {
                  strcpy(value, p_general + 1);
               }
               else
                  strcpy(value, "");
            }
            else
               strcpy(value, "");

            valuelen = strlen(value);
            break;
         }
         if (getall == 1 && insect == 1 && strlen(fentry) > 0 && fentry[0] != ';') {
            for (n = 0; fentry[n] != '\0'; n ++)
               value[ptr ++] = fentry[n];
            value[ptr ++] = '\0';
         }
      }
      if (getall == 1 && insect == 1) {
         valuelen = ptr;
         value[ptr ++] = '\0';
      }
      fclose(fp);
   }

   n = mg_mutex_unlock((MUTOBJ *) core_data.p_memlockFA);

   return valuelen;

#else

   return GetPrivateProfileString((LPCTSTR) section, (LPCTSTR) entry, (LPCTSTR) defvalue, value, size, (LPCTSTR) file);

#endif

}


int mg_set_config(char *section, char *entry, char *value, char *file)
{

#if !defined(_WIN32)

   int insect, done, n, len, valuelen, ok, remove;
   char lbuffer[8192], sbuffer[64], fentry[64];
   char *p_value, *p_entry;
   char *pbuffer, *pkey;

   FILE *fp;

   if (section == NULL || T_STRLEN(section) == 0)
      return 0;

   if (entry == NULL || T_STRLEN(entry) == 0)
      p_entry = NULL;
   else
      p_entry = entry;

   valuelen = 0;
   if (value == NULL || (valuelen = T_STRLEN(value)) == 0)
      p_value = NULL;
   else
      p_value = value;

   pbuffer = (char *) mg_malloc(sizeof(char) * 16384, "mg_set_config:1");

   if (pbuffer == NULL)
      return 0;

   n = mg_mutex_lock((MUTOBJ *) core_data.p_memlockFA);

   pkey = pbuffer;
   *pkey = '\0';
   insect = 0, done = 0;
   T_STRCPY(sbuffer, "[");
   T_STRCAT(sbuffer, section);
   T_STRCAT(sbuffer, "]");
   remove = 0;
   ok = 1;

   fp = fopen(file, "r");
   if (fp == NULL)
      ok = 0;

   if (ok == 1) {
      for (;;) {

         if (fgets(lbuffer, 512, fp) == NULL)
            break;

         len = mg_trim_string((char *) lbuffer);
         if (len > 0) {
            if (lbuffer[0] == '[') {
               if (remove == 0 && insect == 1 && done == 0 && valuelen > 0) {
                  for (n = 0; entry[n] != '\0'; n ++)
                     *pkey ++ = entry[n];
                  *pkey ++ = '=';
                  for (n = 0; value[n] != '\0'; n ++)
                     *pkey ++ = value[n];
                  *pkey ++ = '\0';
                  done = 1;
               }
               insect = 0;
               remove = 0;
            }
            if (strcmp(lbuffer, sbuffer) == 0) {
               insect = 1;
               if (p_entry == NULL) {
                  remove = 1;
                  done = 1;
               }
            }

            if (remove == 1)
               continue;

            len = mg_piece(fentry, lbuffer, "=", 1, 1);
            if (insect == 1 && T_STRCMP(fentry, entry) == 0) {
               if (valuelen == 0)
                  T_STRCPY(lbuffer, "");
               else {
                  T_STRCPY(lbuffer, fentry);
                  T_STRCAT(lbuffer, "=");
                  T_STRCAT(lbuffer, value);
               }
               done = 1;
            }
            if (strlen(lbuffer) > 0) {
               for (n = 0; lbuffer[n] != '\0'; n ++)
                  *pkey ++ = lbuffer[n];
               *pkey ++ = '\0';
            }
         }
      }

      fclose(fp);

   }
   if (done == 0 && valuelen > 0) {
      if (insect == 0) {
         for (n = 0; sbuffer[n] != '\0'; n ++)
            *pkey ++ = sbuffer[n];
         *pkey ++ = '\0';
      }
      T_STRCPY(lbuffer, entry);
      T_STRCAT(lbuffer, "=");
      T_STRCAT(lbuffer, value);
      for (n = 0; lbuffer[n] != '\0'; n ++)
         *pkey ++ = lbuffer[n];
      *pkey ++ = '\0';
   }
   *pkey ++ = '\0';
   insect = 0;
   pkey = pbuffer; /* Reset pointer to start of buffer */
   ok = 1;

   fp = fopen(file, "w");
   if (fp == NULL)
      ok = 0;

   if (ok == 1) {
      for (pkey = pbuffer; *pkey != '\0'; pkey += strlen(pkey) + 1) {
         T_SPRINTF(lbuffer, "%s\n", pkey);

         if (insect > 0 && lbuffer[0] == '[')
            fputs("\n", fp);
         fputs(lbuffer, fp);

         insect ++;
      }

      fclose(fp);
   }

   n = mg_mutex_unlock((MUTOBJ *) core_data.p_memlockFA);

   mg_free((void *) pbuffer, "mg_set_config:1");
   pbuffer = NULL;

   return ok;

#else

   char *p_entry, *p_value;

   if (section == NULL || T_STRLEN(section) == 0)
      return 0;

   if (entry == NULL || T_STRLEN(entry) == 0)
      p_entry = NULL;
   else
      p_entry = entry;

   if (value == NULL || T_STRLEN(value) == 0)
      p_value = NULL;
   else
      p_value = value;

   return WritePrivateProfileString((LPCTSTR) section, (LPCTSTR) p_entry, (LPCTSTR) p_value, (LPCTSTR) file);

#endif

}


int mg_global_parameters(void)
{
   char strLrt[8], strMrt[8], strQrt[8], buffer[256];
   int i, len, lrt, mrt, qrt;

#ifdef _WIN32
__try {
#endif

   core_data.server_timeout = MG_D_SERVER_TIMEOUT;
   core_data.idle_timeout = MG_D_IDLE_TIMEOUT;
   core_data.queue_timeout = MG_D_QUEUE_TIMEOUT;

   len = mg_get_config("SYSTEM", MG_P_SYSTEM_MANAGER, "", core_data.system_manager, 200, core_data.mg_ini);

   T_STRCPY(strLrt, "5");
   lrt = 5;

   len = mg_get_config("SYSTEM", MG_P_M_RESPONSE_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   if (!len) {
      mg_set_config("SYSTEM", MG_P_M_RESPONSE_TIMEOUT, MG_D_M_RESPONSE_TIMEOUT, core_data.mg_ini);
      len = mg_get_config("SYSTEM", MG_P_M_RESPONSE_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   }
   mrt = (int) T_STRTOL(strMrt, NULL, 10);
   if (mrt >= 5)
      core_data.server_timeout = mrt;

   len = mg_get_config("SYSTEM", MG_P_NO_ACTIVITY_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   if (!len && strlen(MG_D_NO_ACTIVITY_TIMEOUT)) {
      mg_set_config("SYSTEM", MG_P_NO_ACTIVITY_TIMEOUT, MG_D_NO_ACTIVITY_TIMEOUT, core_data.mg_ini);
      len = mg_get_config("SYSTEM", MG_P_NO_ACTIVITY_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   }
   mrt = (int) T_STRTOL(strMrt, NULL, 10);
   if (mrt >= 5 || mrt == 0)
      core_data.idle_timeout = mrt;

   len = mg_get_config("SYSTEM", MG_P_QUEUED_REQUEST_TIMEOUT, "", strQrt, 8, core_data.mg_ini);
   if (!len) {
      mg_set_config("SYSTEM", MG_P_QUEUED_REQUEST_TIMEOUT, MG_D_QUEUED_REQUEST_TIMEOUT, core_data.mg_ini);
      len = mg_get_config("SYSTEM", MG_P_QUEUED_REQUEST_TIMEOUT, "", strQrt, 8, core_data.mg_ini);
   }
   qrt = (int) T_STRTOL(strQrt, NULL, 10);
   if (qrt >= 5)
      core_data.queue_timeout = qrt;

   core_data.log_errors = 0;
   core_data.verbose = 0;
   len = mg_get_config("SYSTEM", MG_P_ENV_PARAMETERS, "", buffer, 16, core_data.mg_ini);

   for (i = 0; buffer[i] != '\0'; i ++) {

      if (buffer[i] == 'e' || buffer[i] == 'E' || buffer[i] == 'x' || buffer[i] == 'X')
         core_data.log_errors = 1;
      if (buffer[i] == 'v' || buffer[i] == 'V' || buffer[i] == 'i' || buffer[i] == 'I') {
         core_data.verbose = 1;
         if (isdigit(buffer[i + 1])) {
            len = (int) strtol(buffer + i + 1, NULL, 10);
            if (len > 1 && len < 8)
               core_data.verbose = len;
         }
      }
   }

   len = mg_get_config("SYSTEM", MG_P_DEFAULT_SERVICE, "", core_data.default_service, 255, core_data.mg_ini);
   if (!len) {
      mg_set_config("SYSTEM", MG_P_DEFAULT_SERVICE, MG_D_DEFAULT_SERVICE, core_data.mg_ini);
      strcpy(core_data.default_service, MG_D_DEFAULT_SERVICE);
   }

   len = mg_get_config("SYSTEM", "HTTP_Service_Status", "", buffer, 32, core_data.mg_ini);
   if (T_STRCMP(buffer, "Disabled") == 0)
      core_data.http_server = 0;
   else
      core_data.http_server = 1;


   len = mg_get_config("SYSTEM", MG_P_SYSTEM_MANAGER, "", buffer, 255, core_data.mg_ini);
   if (!len)
      mg_set_config("SYSTEM", MG_P_SYSTEM_MANAGER, MG_D_SYSTEM_MANAGER, core_data.mg_ini);

#ifdef MG_SSL

   len = mg_get_config("SYSTEM", MG_P_SSLS_SERVICE_STATUS, "", buffer, 255, core_data.mg_ini);
   if (T_STRCMP(buffer, sysEnabled) == 0)
      core_data.ssls_status = 1;
   else
      core_data.ssls_status = 0;

   len = mg_get_config("SYSTEM", MG_P_SSLS_TCP_PORT, "", buffer, 255, core_data.mg_ini);
   core_data.ssls_port = (int) strtol(buffer, NULL, 10);
   len = mg_get_config("SYSTEM", MG_P_SSLS_CERTIFICATE_FILE, "", core_data.ssls_cert_file, 255, core_data.mg_ini);
   len = mg_get_config("SYSTEM", MG_P_SSLS_CERTIFICATE_KEY_FILE, "", core_data.ssls_cert_key_file, 255, core_data.mg_ini);

   len = mg_get_config("SYSTEM", MG_P_SSLS_CA_CERTIFICATE_FILE, "", core_data.ssls_ca_file, 255, core_data.mg_ini);
   len = mg_get_config("SYSTEM", MG_P_SSLS_CA_CERTIFICATE_PATH, "", core_data.ssls_ca_path, 255, core_data.mg_ini);

   len = mg_get_config("SYSTEM", MG_P_SSLS_VERIFY_CLIENT, "", buffer, 255, core_data.mg_ini);
   core_data.ssls_verify_client = (int) strtol(buffer, NULL, 10);
   if (!len || core_data.ssls_verify_client < 0 || core_data.ssls_verify_client > 2) {
      core_data.ssls_verify_client = 0;
      mg_set_config("SYSTEM", MG_P_SSLS_VERIFY_CLIENT, "0", core_data.mg_ini);
   }

   len = mg_get_config("SYSTEM", MG_P_SSLS_VERIFY_DEPTH, "", buffer, 255, core_data.mg_ini);
   core_data.ssls_verify_depth = (int) strtol(buffer, NULL, 10);
   if (!len || core_data.ssls_verify_depth < 0 || core_data.ssls_verify_depth > 20) {
      core_data.ssls_verify_depth = 1;
      mg_set_config("SYSTEM", MG_P_SSLS_VERIFY_DEPTH, "1", core_data.mg_ini);
   }


   len = mg_get_config("SYSTEM", MG_P_SSLP_SERVICE_STATUS, "", buffer, 255, core_data.mg_ini);
   if (T_STRCMP(buffer, sysEnabled) == 0)
      core_data.ssp_status = 1;
   else
      core_data.ssp_status = 0;

   len = mg_get_config("SYSTEM", MG_P_SSLP_TCP_PORT, "", buffer, 255, core_data.mg_ini);
   core_data.ssp_port = (int) strtol(buffer, NULL, 10);

#endif /* #ifdef MG_SSL */


   len = mg_get_config(MG_D_DEFAULT_SERVICE, MG_P_IP_ADDRESS, "", buffer, 255, core_data.mg_ini);
   if (!len)
      mg_set_system_defaults();

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_global_parameters: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_system_parameters(int shndle)
{
   char strMrt[8], buffer[64], encoded_pwd[8192];
   int n, i, len, mrt;

#ifdef _WIN32
__try {
#endif

   if (mg_servers[shndle] == NULL)
      return 0;

   len = mg_get_config("SYSTEM_INDEX", mg_servers[shndle]->m_server_name, "", buffer, 32, core_data.mg_ini);
   if (T_STRCMP(buffer, "Disabled") == 0)
      mg_servers[shndle]->status = 0;
   else
      mg_servers[shndle]->status = 1;

   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_IP_ADDRESS, "", mg_servers[shndle]->ip_address, 63, core_data.mg_ini);

   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_DEFAULT_USER_UCI, "", mg_servers[shndle]->default_user_uci, 61, core_data.mg_ini);
   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_NLS_TRANSLATION, "", mg_servers[shndle]->nls_translation, 61, core_data.mg_ini);

   /* CM0051 */
   len = mg_get_config((char *) mg_servers[shndle]->m_server_name, (char *) MG_P_USERNAME, "", mg_servers[shndle]->username, 250, core_data.mg_ini);
   len = mg_get_config((char *) mg_servers[shndle]->m_server_name, (char *) MG_P_PASSWORD, "", encoded_pwd, 8190, core_data.mg_ini);
   if (len > 0) {
      mg_decode_password((char *) encoded_pwd, (char *) mg_servers[shndle]->password, 2);
      n = mg_b64_encode((char *) mg_servers[shndle]->password, mg_servers[shndle]->password_trans, (int) strlen(mg_servers[shndle]->password), 0);
   }
   else {
      mg_servers[shndle]->password[0] = '\0';
      mg_servers[shndle]->password_trans[0] = '\0';
   }

   mg_servers[shndle]->tcp_port = 7041;
   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_BASE_TCP_PORT, "", buffer, 8, core_data.mg_ini);
   i = (int) T_STRTOL(buffer, NULL, 10);
   if (i > 1023)
      mg_servers[shndle]->tcp_port = i;

   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_M_RESPONSE_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   mg_servers[shndle]->server_timeout = core_data.server_timeout;
   mrt = (int) T_STRTOL(strMrt, NULL, 10);
   if (mrt >= 5)
      mg_servers[shndle]->server_timeout = mrt;

   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_NO_ACTIVITY_TIMEOUT, "", strMrt, 8, core_data.mg_ini);
   mg_servers[shndle]->idle_timeout = core_data.idle_timeout;
   mrt = (int) T_STRTOL(strMrt, NULL, 10);
   if (mrt >= 5)
      mg_servers[shndle]->idle_timeout = mrt;

   mg_servers[shndle]->max_server_connections = 9999;
   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_MAXIMUM_SERVER_SESSIONS, "", buffer, 8, core_data.mg_ini);
   i = (int) T_STRTOL(buffer, NULL, 10);
   if (i > 0 && i < 100000)
      mg_servers[shndle]->max_server_connections = i;

   len = mg_get_config(mg_servers[shndle]->m_server_name, MG_P_ENV_PARAMETERS, "", buffer, 16, core_data.mg_ini);
   if (len == 0) {
      mg_servers[shndle]->log_errors = core_data.log_errors;
      mg_servers[shndle]->verbose = core_data.verbose;
   }
   else {
      mg_servers[shndle]->log_errors = 0;
      mg_servers[shndle]->verbose = 0;
      for (i = 0; buffer[i] != '\0'; i ++) {
         if (buffer[i] == 'e' || buffer[i] == 'E' || buffer[i] == 'x' || buffer[i] == 'X')
            mg_servers[shndle]->log_errors = 1;
         if (buffer[i] == 'v' || buffer[i] == 'V' || buffer[i] == 'i' || buffer[i] == 'I') {
            mg_servers[shndle]->verbose = 1;
            if (isdigit(buffer[i + 1])) {
               len = (int) strtol(buffer + i + 1, NULL, 10);
               if (len > 1 && len < 8)
                  mg_servers[shndle]->verbose = len;
            }
         }
      }
   }

   for (n = 0; n < MG_MAX_SERVERS; n ++) {
      if (mg_alt_servers[n]) {
         if (!strcmp(mg_alt_servers[n]->default_server, mg_servers[shndle]->m_server_name))
            mg_set_alt_servers(n, mg_servers[shndle]->m_server_name);
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
      T_SPRINTF(buffer, "Exception caught in f:mg_system_parameters: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_set_global_defaults(void)
{
   int len;
   char buffer[256];

#ifdef _WIN32
__try {
#endif

   mg_set_config("SYSTEM", MG_P_M_RESPONSE_TIMEOUT, MG_D_M_RESPONSE_TIMEOUT, core_data.mg_ini);
   mg_set_config("SYSTEM", MG_P_QUEUED_REQUEST_TIMEOUT, MG_D_QUEUED_REQUEST_TIMEOUT, core_data.mg_ini);
   mg_set_config("SYSTEM", MG_P_DEFAULT_SERVICE, MG_D_DEFAULT_SERVICE, core_data.mg_ini);

   len = mg_get_config("SYSTEM", MG_P_SYSTEM_MANAGER, "", buffer, 255, core_data.mg_ini);
   if (!len)
      mg_set_config("SYSTEM", MG_P_SYSTEM_MANAGER, MG_D_SYSTEM_MANAGER, core_data.mg_ini);

   mg_set_system_defaults();

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_set_global_defaults: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}



int mg_set_system_defaults(void)
{

#ifdef _WIN32
__try {
#endif

   mg_set_config("SYSTEM_INDEX", MG_D_DEFAULT_SERVICE, MG_D_SERVICE_STATUS, core_data.mg_ini);

   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_IP_ADDRESS, MG_D_IP_ADDRESS, core_data.mg_ini);
   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_BASE_TCP_PORT, MG_D_BASE_TCP_PORT, core_data.mg_ini);
   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_DEFAULT_USER_UCI, MG_D_DEFAULT_USER_UCI, core_data.mg_ini);
   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_NLS_TRANSLATION, MG_D_NLS_TRANSLATION, core_data.mg_ini);

   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_M_RESPONSE_TIMEOUT, "", core_data.mg_ini);
   mg_set_config(MG_D_DEFAULT_SERVICE, MG_P_ENV_PARAMETERS, "", core_data.mg_ini);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_set_system_defaults: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


void mg_upgrade(void)
{
   int sn, len, status;
   char buffer[1092];
   char service[32], qm_name[64], q_name[64], server[64], routine[64], stat[32];

#ifdef _WIN32
__try {
#endif

   for (sn = 1; sn < MG_MAX_CONNECTIONS; sn ++) {

      T_SPRINTF(service, "MQ_%s_%d", "qm_name", sn);
      len = mg_get_config("SYSTEM", service, "", qm_name, 63, core_data.mg_ini);
      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);

      if (!len)
         break;

      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);

      T_SPRINTF(service, "MQ_%s_%d", "q_name", sn);
      len = mg_get_config("SYSTEM", service, "", q_name, 63, core_data.mg_ini);
      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);


      T_SPRINTF(service, "MQ_%s_%d", "server", sn);
      len = mg_get_config("SYSTEM", service, "", server, 63, core_data.mg_ini);
      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);

      T_SPRINTF(service, "MQ_%s_%d", "routine", sn);
      len = mg_get_config("SYSTEM", service, "", routine, 63, core_data.mg_ini);
      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);

      T_SPRINTF(service, "MQ_%s_%d", "status", sn);
      len = mg_get_config("SYSTEM", service, "", stat, 16, core_data.mg_ini);
      mg_set_config("SYSTEM", service, NULL, core_data.mg_ini);

      if (!strcmp(stat, "Disabled"))
         status = 0;
      else
         status = 1;

      sprintf(service, "SERVICE%d", sn);
      sprintf(buffer, "WSMQS:SERVICE%d", sn);

      mg_set_config(buffer, "Queue_Manager_Name", qm_name, core_data.mg_ini);
      mg_set_config(buffer, "Queue_Name", q_name, core_data.mg_ini);
      mg_set_config(buffer, "Server", server, core_data.mg_ini);
      mg_set_config(buffer, "Server_Routine", routine, core_data.mg_ini);
      if (status)
         mg_set_config("WSMQS_INDEX", service, "Enabled", core_data.mg_ini);
      else
         mg_set_config("WSMQS_INDEX", service, "Disabled", core_data.mg_ini);

   }
   return;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_upgrade: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return;
}
#endif
}


