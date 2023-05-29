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
#include "mgsidso.h"


MGMQLIB        mg_mqlib;

#if defined(_WIN32)
MGSOCK         mg_sock     = {0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
#else
MGSOCK         mg_sock     = {0, 0, 0, 0};
#endif

#ifdef MG_SSL
MGSSL          mg_ssl;
#endif /* #ifdef MG_SSL */

#ifdef MG_CRYPT32
MGCRYPT32      mg_crypt32;
#endif /* #ifdef MG_CRYPT32 */


int mg_dso_load(MGDSO *p_mgdso, char * library)
{

#if defined(_WIN32)

   p_mgdso->h_library = LoadLibrary(library);

#else

   p_mgdso->h_library = dlopen(library, RTLD_NOW);

#endif

   if (p_mgdso->h_library)
      return 1;
   else
      return 0;
}


MGPROC mg_dso_sym(MGDSO *p_mgdso, char * symbol)
{
   MGPROC p_proc;
   int (* proc) (void);

   p_proc = NULL;
   proc = NULL;

#if defined(_WIN32)

   p_proc = GetProcAddress(p_mgdso->h_library, symbol);

#else
   p_proc  = (void *) dlsym(p_mgdso->h_library, symbol);

#endif

   return p_proc;
}



int mg_dso_unload(MGDSO *p_mgdso)
{

#if defined(_WIN32)

   FreeLibrary(p_mgdso->h_library);

#else

   dlclose(p_mgdso->h_library); 

#endif

   return 1;
}


int mg_load_openssl(int context)
{
   int result, result1, result2, n;
   char buffer[256], path1[256], path2[256], message[256];

#ifdef _WIN32
__try {
#endif

   result = 0;
   result1= 0;
   result2 = 0;

   n = 0;
   *path1 = '\0';
   *path2 = '\0';
   *message = '\0';
   *buffer = '\0';

#ifdef MG_SSL

   mg_ssl.ssl = 0;
   mg_ssl.mgdso.flags = 0;
   mg_ssl.mgdso_libeay.flags = 0;

#ifdef MG_SSL_USE_DSO

   /* Try to Load the SSL libraries */

   mg_ssl.ssl = 0;
   strcpy(buffer, "");

#ifdef _WIN32
   sprintf(path1, "%sssleay32.dll", core_data.config_path);
   result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
   if (!result1) {
      sprintf(path1, "%sbin/ssleay32.dll", core_data.config_path);
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         sprintf(path1, "%s../bin/ssleay32.dll", core_data.config_path);
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            sprintf(path1, "%s../../bin/ssleay32.dll", core_data.config_path);
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
            if (!result1) {
               strcpy(path1, "ssleay32.dll");
               result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
            }
         }
      }
   }
#else

   result1 = 0;

   if (!result1) {
      sprintf(path1, "%slibssl.so", core_data.config_path);
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         sprintf(path1, "%slibssl.sl", core_data.config_path);
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            sprintf(path1, "%slibssl.dylib", core_data.config_path);
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      sprintf(path1, "%sbin/libssl.so", core_data.config_path);
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         sprintf(path1, "%sbin/libssl.sl", core_data.config_path);
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            sprintf(path1, "%sbin/libssl.dylib", core_data.config_path);
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      sprintf(path1, "%s../bin/libssl.so", core_data.config_path);
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         sprintf(path1, "%s../bin/libssl.sl", core_data.config_path);
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            sprintf(path1, "%s../bin/libssl.dylib", core_data.config_path);
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      sprintf(path1, "%s../../libssl.so", core_data.config_path);
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         sprintf(path1, "%s../../libssl.sl", core_data.config_path);
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            sprintf(path1, "%s../../libssl.dylib", core_data.config_path);
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      strcpy(path1, "libssl.so");
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         strcpy(path1, "libssl.sl");
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            strcpy(path1, "libssl.dylib");
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      strcpy(path1, "/usr/local/lib/libssl.so");
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         strcpy(path1, "/usr/local/lib/libssl.sl");
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            strcpy(path1, "/usr/local/lib/libssl.dylib");
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

   if (!result1) {
      strcpy(path1, "/usr/lib/libssl.so");
      result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
      if (!result1) {
         strcpy(path1, "/usr/lib/libssl.sl");
         result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         if (!result1) {
            strcpy(path1, "/usr/lib/libssl.dylib");
            result1 = mg_dso_load(&(mg_ssl.mgdso), path1);
         }
      }
   }

#endif

#ifdef _WIN32
   sprintf(path2, "%slibeay32.dll", core_data.config_path);
   result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
   if (!result2) {
      sprintf(path2, "%sbin/libeay32.dll", core_data.config_path);
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         sprintf(path2, "%s../bin/libeay32.dll", core_data.config_path);
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            sprintf(path2, "%s../../bin/libeay32.dll", core_data.config_path);
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
            if (!result2) {
               strcpy(path2, "libeay32.dll");
               result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
            }
         }
      }
   }

#else

   if (!result2) {
      sprintf(path2, "%slibcrypto.so", core_data.config_path);
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         sprintf(path2, "%slibcrypto.sl", core_data.config_path);
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            sprintf(path2, "%slibcrypto.dylib", core_data.config_path);
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      sprintf(path2, "%sbin/libcrypto.so", core_data.config_path);
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         sprintf(path2, "%sbin/libcrypto.sl", core_data.config_path);
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            sprintf(path2, "%sbin/libcrypto.dylib", core_data.config_path);
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      sprintf(path2, "%s../bin/libcrypto.so", core_data.config_path);
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         sprintf(path2, "%s../bin/libcrypto.sl", core_data.config_path);
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            sprintf(path2, "%s../bin/libcrypto.dylib", core_data.config_path);
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      sprintf(path2, "%s../../libcrypto.so", core_data.config_path);
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         sprintf(path2, "%s../../libcrypto.sl", core_data.config_path);
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            sprintf(path2, "%s../../libcrypto.dylib", core_data.config_path);
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      strcpy(path2, "libcrypto.so");
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         strcpy(path2, "libcrypto.sl");
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            strcpy(path2, "libcrypto.dylib");
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      strcpy(path2, "/usr/local/lib/libcrypto.so");
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         strcpy(path2, "/usr/local/lib/libcrypto.sl");
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            strcpy(path2, "/usr/local/lib/libcrypto.dylib");
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }

   if (!result2) {
      strcpy(path2, "/usr/lib/libcrypto.so");
      result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
      if (!result2) {
         strcpy(path2, "/usr/lib/libcrypto.sl");
         result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         if (!result2) {
            strcpy(path2, "/usr/lib/libcrypto.dylib");
            result2 = mg_dso_load(&(mg_ssl.mgdso_libeay), path2);
         }
      }
   }
/*
   mg_ssl.h_libeay32 = mg_ssl.h_ssleay32;
*/
#endif

   if (!result1) {
#ifdef _WIN32
      T_STRCPY(message, "Information: The SSL library (SSLEAY32) is not present on this system");
#else
      T_STRCPY(message, "Information: The SSL library (libssl) is not present on this system");
#endif
      goto ssl_init_end;
   }


   if (!result2) {
#ifdef _WIN32
      T_STRCPY(message, "Information: The SSL library (LIBEAY32) is not present on this system");
#else
      T_STRCPY(message, "Information: The SSL library (libcrypto) is not present on this system");
#endif
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_library_init = (LPFN_SSL_LIBRARY_INIT) mg_dso_sym(&(mg_ssl.mgdso), "SSL_library_init");
   if (!mg_ssl.p_SSL_library_init) {
      T_STRCPY(buffer,  "SSL_library_init");
      goto ssl_init_end;
   }

   mg_ssl.p_SSLv2_client_method = (LPFN_SSLV2_CLIENT_METHOD) mg_dso_sym(&(mg_ssl.mgdso), "SSLv2_client_method");
   if (!mg_ssl.p_SSLv2_client_method) {
      T_STRCPY(buffer,  "SSLv2_client_method");
      goto ssl_init_end;
   }

   mg_ssl.p_SSLv23_server_method = (LPFN_SSLV23_SERVER_METHOD) mg_dso_sym(&(mg_ssl.mgdso), "SSLv23_server_method");
   if (!mg_ssl.p_SSLv23_server_method) {
      T_STRCPY(buffer,  "SSLv23_server_method");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_load_error_strings = (LPFN_SSL_LOAD_ERROR_STRINGS) mg_dso_sym(&(mg_ssl.mgdso), "SSL_load_error_strings");
   if (!mg_ssl.p_SSL_load_error_strings) {
      T_STRCPY(buffer,  "SSL_load_error_strings");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_new = (LPFN_SSL_CTX_NEW) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_new");
   if (!mg_ssl.p_SSL_CTX_new) {
      T_STRCPY(buffer,  "SSL_CTX_new");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_new = (LPFN_SSL_NEW) mg_dso_sym(&(mg_ssl.mgdso), "SSL_new");
   if (!mg_ssl.p_SSL_new) {
      T_STRCPY(buffer,  "SSL_new");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_set_fd = (LPFN_SSL_SET_FD) mg_dso_sym(&(mg_ssl.mgdso), "SSL_set_fd");
   if (!mg_ssl.p_SSL_set_fd) {
      T_STRCPY(buffer,  "SSL_set_fd");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_use_certificate_file = (LPFN_SSL_CTX_USE_CERTIFICATE_FILE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_use_certificate_file");
   if (!mg_ssl.p_SSL_CTX_use_certificate_file) {
      T_STRCPY(buffer,  "SSL_CTX_use_certificate_file");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_use_PrivateKey_file = (LPFN_SSL_CTX_USE_PRIVATEKEY_FILE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_use_PrivateKey_file");
   if (!mg_ssl.p_SSL_CTX_use_PrivateKey_file) {
      T_STRCPY(buffer,  "SSL_CTX_use_PrivateKey_file");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_check_private_key = (LPFN_SSL_CTX_CHECK_PRIVATE_KEY) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_check_private_key");
   if (!mg_ssl.p_SSL_CTX_check_private_key) {
      T_STRCPY(buffer,  "SSL_CTX_check_private_key");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_accept = (LPFN_SSL_ACCEPT) mg_dso_sym(&(mg_ssl.mgdso), "SSL_accept");
   if (!mg_ssl.p_SSL_accept) {
      T_STRCPY(buffer,  "SSL_accept");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_connect = (LPFN_SSL_CONNECT) mg_dso_sym(&(mg_ssl.mgdso), "SSL_connect");
   if (!mg_ssl.p_SSL_connect) {
      T_STRCPY(buffer,  "SSL_connect");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_current_cipher = (LPFN_SSL_GET_CURRENT_CIPHER) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_current_cipher");
   if (!mg_ssl.p_SSL_get_current_cipher) {
      T_STRCPY(buffer,  "SSL_get_current_cipher");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CIPHER_get_name = (LPFN_SSL_CIPHER_GET_NAME) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CIPHER_get_name");
   if (!mg_ssl.p_SSL_CIPHER_get_name) {
      T_STRCPY(buffer,  "SSL_CIPHER_get_name");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_peer_certificate = (LPFN_SSL_GET_PEER_CERTIFICATE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_peer_certificate");
   if (!mg_ssl.p_SSL_get_peer_certificate) {
      T_STRCPY(buffer,  "SSL_get_peer_certificate");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_write = (LPFN_SSL_WRITE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_write");
   if (!mg_ssl.p_SSL_write) {
      T_STRCPY(buffer,  "SSL_write");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_read = (LPFN_SSL_READ) mg_dso_sym(&(mg_ssl.mgdso), "SSL_read");
   if (!mg_ssl.p_SSL_read) {
      T_STRCPY(buffer,  "SSL_read");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_shutdown = (LPFN_SSL_SHUTDOWN) mg_dso_sym(&(mg_ssl.mgdso), "SSL_shutdown");
   if (!mg_ssl.p_SSL_shutdown) {
      T_STRCPY(buffer,  "SSL_shutdown");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_free = (LPFN_SSL_FREE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_free");
   if (!mg_ssl.p_SSL_free) {
      T_STRCPY(buffer,  "SSL_free");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_free = (LPFN_SSL_CTX_FREE) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_free");
   if (!mg_ssl.p_SSL_CTX_free) {
      T_STRCPY(buffer,  "SSL_CTX_free");
      goto ssl_init_end;
   }



   mg_ssl.p_SSLeay_version = (LPFN_SSLEAY_VERSION) mg_dso_sym(&(mg_ssl.mgdso_libeay), "SSLeay_version");
   if (!mg_ssl.p_SSLeay_version) {
      T_STRCPY(buffer,  "SSLeay_version");
      goto ssl_init_end;
   }

   mg_ssl.p_X509_get_subject_name = (LPFN_X509_GET_SUBJECT_NAME) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_get_subject_name");
   if (!mg_ssl.p_X509_get_subject_name) {
      T_STRCPY(buffer,  "X509_get_subject_name");
      goto ssl_init_end;
   }

   mg_ssl.p_X509_NAME_oneline = (LPFN_X509_NAME_ONELINE) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_NAME_oneline");
   if (!mg_ssl.p_X509_NAME_oneline) {
      T_STRCPY(buffer,  "X509_NAME_oneline");
      goto ssl_init_end;
   }

   mg_ssl.p_CRYPTO_free = (LPFN_CRYPTO_FREE) mg_dso_sym(&(mg_ssl.mgdso_libeay), "CRYPTO_free");
   if (!mg_ssl.p_CRYPTO_free) {
      T_STRCPY(buffer,  "CRYPTO_free");
      goto ssl_init_end;
   }

   mg_ssl.p_X509_get_issuer_name = (LPFN_X509_GET_ISSUER_NAME) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_get_issuer_name");
   if (!mg_ssl.p_X509_get_issuer_name) {
      T_STRCPY(buffer,  "X509_get_issuer_name");
      goto ssl_init_end;
   }

   mg_ssl.p_X509_free = (LPFN_X509_FREE) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_free");
   if (!mg_ssl.p_X509_free) {
      T_STRCPY(buffer,  "X509_free");
      goto ssl_init_end;
   }


   mg_ssl.p_X509_STORE_CTX_get_current_cert = (LPFN_X509_STORE_CTX_GET_CURRENT_CERT) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_STORE_CTX_get_current_cert");
   if (!mg_ssl.p_X509_STORE_CTX_get_current_cert) {
      T_STRCPY(buffer,  "X509_STORE_CTX_get_current_cert");
      goto ssl_init_end;
   }

   mg_ssl.p_X509_STORE_CTX_set_error = (LPFN_X509_STORE_CTX_SET_ERROR) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_STORE_CTX_set_error");
   if (!mg_ssl.p_X509_STORE_CTX_set_error) {
      T_STRCPY(buffer,  "X509_STORE_CTX_set_error");
      goto ssl_init_end;
   }
   mg_ssl.p_X509_STORE_CTX_get_error = (LPFN_X509_STORE_CTX_GET_ERROR) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_STORE_CTX_get_error");
   if (!mg_ssl.p_X509_STORE_CTX_get_error) {
      T_STRCPY(buffer,  "X509_STORE_CTX_get_error");
      goto ssl_init_end;
   }
   mg_ssl.p_X509_STORE_CTX_get_error_depth = (LPFN_X509_STORE_CTX_GET_ERROR_DEPTH) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_STORE_CTX_get_error_depth");
   if (!mg_ssl.p_X509_STORE_CTX_get_error_depth) {
      T_STRCPY(buffer,  "X509_STORE_CTX_get_error_depth");
      goto ssl_init_end;
   }
   mg_ssl.p_X509_STORE_CTX_get_ex_data = (LPFN_X509_STORE_CTX_GET_EX_DATA) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_STORE_CTX_get_ex_data");
   if (!mg_ssl.p_X509_STORE_CTX_get_ex_data) {
      T_STRCPY(buffer,  "X509_STORE_CTX_get_ex_data");
      goto ssl_init_end;
   }
   mg_ssl.p_X509_verify_cert_error_string = (LPFN_X509_VERIFY_CERT_ERROR_STRING) mg_dso_sym(&(mg_ssl.mgdso_libeay), "X509_verify_cert_error_string");
   if (!mg_ssl.p_X509_verify_cert_error_string) {
      T_STRCPY(buffer,  "X509_verify_cert_error_string");
      goto ssl_init_end;
   }


   mg_ssl.p_SSL_CTX_set_verify = (LPFN_SSL_CTX_SET_VERIFY) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_set_verify");
   if (!mg_ssl.p_SSL_CTX_set_verify) {
      T_STRCPY(buffer,  "SSL_CTX_set_verify");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_set_verify_depth = (LPFN_SSL_CTX_SET_VERIFY_DEPTH) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_set_verify_depth");
   if (!mg_ssl.p_SSL_CTX_set_verify_depth) {
      T_STRCPY(buffer,  "SSL_CTX_set_verify_depth");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_set_cert_verify_callback = (LPFN_SSL_CTX_SET_CERT_VERIFY_CALLBACK) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_set_cert_verify_callback");
   if (!mg_ssl.p_SSL_CTX_set_cert_verify_callback) {
      T_STRCPY(buffer,  "SSL_CTX_set_cert_verify_callback");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_CTX_load_verify_locations = (LPFN_SSL_CTX_LOAD_VERIFY_LOCATIONS) mg_dso_sym(&(mg_ssl.mgdso), "SSL_CTX_load_verify_locations");
   if (!mg_ssl.p_SSL_CTX_load_verify_locations) {
      T_STRCPY(buffer,  "SSL_CTX_load_verify_locations");
      goto ssl_init_end;
   }


   mg_ssl.p_SSL_set_verify_result = (LPFN_SSL_SET_VERIFY_RESULT) mg_dso_sym(&(mg_ssl.mgdso), "SSL_set_verify_result");
   if (!mg_ssl.p_SSL_set_verify_result) {
      T_STRCPY(buffer,  "SSL_set_verify_result");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_verify_result = (LPFN_SSL_GET_VERIFY_RESULT) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_verify_result");
   if (!mg_ssl.p_SSL_get_verify_result) {
      T_STRCPY(buffer,  "SSL_get_verify_result");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_set_verify = (LPFN_SSL_SET_VERIFY) mg_dso_sym(&(mg_ssl.mgdso), "SSL_set_verify");
   if (!mg_ssl.p_SSL_set_verify) {
      T_STRCPY(buffer,  "SSL_set_verify");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_set_verify_depth = (LPFN_SSL_SET_VERIFY_DEPTH) mg_dso_sym(&(mg_ssl.mgdso), "SSL_set_verify_depth");
   if (!mg_ssl.p_SSL_set_verify_depth) {
      T_STRCPY(buffer,  "SSL_set_verify_depth");
      goto ssl_init_end;
   }



   mg_ssl.p_SSL_set_ex_data = (LPFN_SSL_SET_EX_DATA) mg_dso_sym(&(mg_ssl.mgdso), "SSL_set_ex_data");
   if (!mg_ssl.p_SSL_set_ex_data) {
      T_STRCPY(buffer,  "SSL_set_ex_data");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_ex_data = (LPFN_SSL_GET_EX_DATA) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_ex_data");
   if (!mg_ssl.p_SSL_get_ex_data) {
      T_STRCPY(buffer,  "SSL_get_ex_data");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_ex_new_index = (LPFN_SSL_GET_EX_NEW_INDEX) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_ex_new_index");
   if (!mg_ssl.p_SSL_get_ex_new_index) {
      T_STRCPY(buffer,  "SSL_get_ex_new_index");
      goto ssl_init_end;
   }

   mg_ssl.p_SSL_get_ex_data_X509_STORE_CTX_idx = (LPFN_SSL_GET_EX_DATA_X509_STORE_CTX_IDX) mg_dso_sym(&(mg_ssl.mgdso), "SSL_get_ex_data_X509_STORE_CTX_idx");
   if (!mg_ssl.p_SSL_get_ex_data_X509_STORE_CTX_idx) {
      T_STRCPY(buffer,  "SSL_get_ex_data_X509_STORE_CTX_idx");
      goto ssl_init_end;
   }


   mg_ssl.p_ERR_get_error = (LPFN_ERR_GET_ERROR) mg_dso_sym(&(mg_ssl.mgdso_libeay), "ERR_get_error");
   if (!mg_ssl.p_ERR_get_error) {
      T_STRCPY(buffer,  "ERR_get_error");
      goto ssl_init_end;
   }
   mg_ssl.p_ERR_error_string = (LPFN_ERR_ERROR_STRING) mg_dso_sym(&(mg_ssl.mgdso_libeay), "ERR_error_string");
   if (!mg_ssl.p_ERR_error_string) {
      T_STRCPY(buffer,  "ERR_error_string");
      goto ssl_init_end;
   }

   mg_ssl.p_HMAC = (LPFN_HMAC) mg_dso_sym(&(mg_ssl.mgdso_libeay), "HMAC");
   if (!mg_ssl.p_HMAC) {
      T_STRCPY(buffer,  "HMAC");
      goto ssl_init_end;
   }

   mg_ssl.p_EVP_sha256 = (LPFN_EVP_SHA256) mg_dso_sym(&(mg_ssl.mgdso_libeay), "EVP_sha256");
   if (!mg_ssl.p_EVP_sha256) {
      T_STRCPY(buffer,  "EVP_sha256");
      goto ssl_init_end;
   }

   mg_ssl.p_EVP_sha1 = (LPFN_EVP_SHA1) mg_dso_sym(&(mg_ssl.mgdso_libeay), "EVP_sha1");
   if (!mg_ssl.p_EVP_sha1) {
      T_STRCPY(buffer,  "EVP_sha1");
      goto ssl_init_end;
   }

   mg_ssl.p_EVP_sha = (LPFN_EVP_SHA) mg_dso_sym(&(mg_ssl.mgdso_libeay), "EVP_sha");
   if (!mg_ssl.p_EVP_sha) {
      T_STRCPY(buffer,  "EVP_sha");
      goto ssl_init_end;
   }

   mg_ssl.p_EVP_md5 = (LPFN_EVP_MD5) mg_dso_sym(&(mg_ssl.mgdso_libeay), "EVP_md5");
   if (!mg_ssl.p_EVP_md5) {
      T_STRCPY(buffer,  "EVP_md5");
      goto ssl_init_end;
   }

   mg_ssl.p_SHA256 = (LPFN_SHA1) mg_dso_sym(&(mg_ssl.mgdso_libeay), "SHA256");
   if (!mg_ssl.p_SHA256) {
      T_STRCPY(buffer,  "SHA256");
      goto ssl_init_end;
   }

   mg_ssl.p_SHA1 = (LPFN_SHA1) mg_dso_sym(&(mg_ssl.mgdso_libeay), "SHA1");
   if (!mg_ssl.p_SHA1) {
      T_STRCPY(buffer,  "SHA1");
      goto ssl_init_end;
   }

   mg_ssl.p_SHA = (LPFN_SHA) mg_dso_sym(&(mg_ssl.mgdso_libeay), "SHA");
   if (!mg_ssl.p_SHA) {
      T_STRCPY(buffer,  "SHA");
      goto ssl_init_end;
   }

   mg_ssl.p_MD5 = (LPFN_MD5) mg_dso_sym(&(mg_ssl.mgdso_libeay), "MD5");
   if (!mg_ssl.p_MD5) {
      T_STRCPY(buffer,  "MD5");
      goto ssl_init_end;
   }

   result = 1;

ssl_init_end:

   if (result) {
      mg_ssl.ssl = 1;
/*
      SSLeay_version(SSLEAY_VERSION));
      SSLeay_version(SSLEAY_BUILT_ON));
      SSLeay_version(SSLEAY_CFLAGS));
      SSLeay_version(SSLEAY_VERSION));
      SSLeay_version(SSLEAY_BUILT_ON));
      SSLeay_version(SSLEAY_PLATFORM));
      SSLeay_version(SSLEAY_CFLAGS));
      SSLeay_version(SSLEAY_DIR));
*/

#ifdef _WIN32
      T_SPRINTF(message, "The SSL libraries (%s and %s) are loaded - Version: %s", path1, path2, mg_ssl.p_SSLeay_version(SSLEAY_VERSION));
#else
      T_SPRINTF(message, "The SSL libraries (%s and %s) are loaded - Version: %s", path1, path2, mg_ssl.p_SSLeay_version(SSLEAY_VERSION));
#endif

   }
   else {

      if (T_STRLEN(buffer)) {
#ifdef _WIN32
         T_SPRINTF(message, "Information: The SSL libraries (%s and %s) found on this system are not usable - missing '%s' function", path1, path2, buffer);
#else
         T_SPRINTF(message, "Information: The SSL libraries (%s and %s) found on this system are not usable - missing '%s' function", path1, path2, buffer);
#endif

      }

   }

#endif /* #ifdef MG_SSL_USE_DSO */

#ifdef MG_SSL_STATIC_LINK

   mg_ssl.ssl = 1;

#ifdef _WIN32
   T_SPRINTF(message, "The SSL libraries (SSLEAY32 and LIBEAY32) are incorporated in this distribution - Version %s.", mg_SSLeay_version(SSLEAY_VERSION));
#else
   T_SPRINTF(message, "The SSL libraries (libssl and libcrypto) are incorporated in this distribution - Version %s.", mg_SSLeay_version(SSLEAY_VERSION));
#endif

#endif /* #ifdef MG_SSL_STATIC_LINK */

   mg_ssl.load_attempted = 1;

   if (T_STRLEN(message))
      mg_log_event(message, "Initialization");

#endif /* #ifdef MG_SSL */

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_load_openssl: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_load_bdb(int context)
{
#if defined(MG_BDB)
   int major, minor, patch;
   char *vers;
   char buffer[256];
#endif

#ifdef _WIN32
__try {
#endif

#if defined(MG_BDB)

   vers = db_version(&major, &minor, &patch);
   if (vers) {
      T_SPRINTF(buffer, "The Berkeley Database (BDB) library is incorporated in (or statically linked to) this distribution - Version %s.", vers);
      mg_log_event(buffer, "MGateway SIG: Initialization");
   }

#endif

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_load_bdb: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_load_mq(int context)
{
   short mqc;
   int result, n;
   char buffer[256], path[256], lib[256], filename[32], imagename[32], vers[64];

#ifdef _WIN32
__try {
#endif

   result = 0;
   mqc = 0;

   *buffer = '\0';
   *path = '\0';
   *lib = '\0';
   *filename = '\0';
   *imagename = '\0';
   *vers = '\0';


   strcpy(buffer, core_data.mg_ini);
   for (n = (int) T_STRLEN(buffer) - 1; n >= 0; n --) {
      if (buffer[n] == '/' || buffer[n] == '\\') {
         buffer[n + 1] = '\0';
         break;
      }
   }
   for (n = 0; buffer[n]; n ++) {
      if (buffer[n] == '\\') {
         buffer[n] = '/';
      }
   }
   if (!strstr(buffer, "/"))
      strcpy(buffer, "./");
   strcpy(core_data.config_path, buffer);


   mg_mqlib.mqlib = 0;

#ifdef MG_MQ_USE_DSO

   /* Try to Load the m_ibm_mq library */

   mg_mqlib.mqlib = 0;

#ifdef _WIN32
   strcpy(lib, "mgsimq.dll");
   result = mg_dso_load(&(mg_mqlib.mgdso), lib);
   if (!result) {
      T_SPRINTF(lib, "%s%s", core_data.config_path, "mgsimq.dll");
      result = mg_dso_load(&(mg_mqlib.mgdso), lib);
   }

   if (!result) {
      strcpy(lib, "mgsimqc.dll");

      result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      if (!result) {
         T_SPRINTF(lib, "%s%s", core_data.config_path, "mgsimqc.dll");
         result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      }

      if (result)
         mqc = 1;
   }


#else

   strcpy(lib, "./mgsimq.so");
   result = mg_dso_load(&(mg_mqlib.mgdso), lib);
   if (!result) {
      strcpy(lib, "mgsimq.sl");
      result = mg_dso_load(&(mg_mqlib.mgdso), lib);
   }

   if (!result) {
      T_SPRINTF(lib, "%s%s", "/usr/bin/", "mgsimq.so");
      result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      if (!result) {
         T_SPRINTF(lib, "%s%s", "/usr/bin/", "mgsimq.sl");
         result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      }
   }

   if (!result) {
      strcpy(lib, "./mgsimqc.so");
      result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      if (!result) {
         strcpy(lib, "mgsimqc.sl");
         result = mg_dso_load(&(mg_mqlib.mgdso), lib);
      }
      if (!result) {
         T_SPRINTF(lib, "%s%s", "/usr/bin/", "mgsimqc.so");
         result = mg_dso_load(&(mg_mqlib.mgdso), lib);
         if (!result) {
            T_SPRINTF(lib, "%s%s", "/usr/bin/", "mgsimqc.sl");
            result = mg_dso_load(&(mg_mqlib.mgdso), lib);
         }
      }

      if (result)
         mqc = 1;
   }

#endif


   if (result) {

      mg_mqlib.p_mgmq_version   = (LPFN_MG_MQ_VERSION)    mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_version");
      mg_mqlib.p_mgmq_connx     = (LPFN_MG_MQ_CONNX)      mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_connx");
      mg_mqlib.p_mgmq_disc      = (LPFN_MG_MQ_DISC)       mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_disc");
      mg_mqlib.p_mgmq_begin     = (LPFN_MG_MQ_BEGIN)      mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_begin");
      mg_mqlib.p_mgmq_back      = (LPFN_MG_MQ_BACK)       mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_back");
      mg_mqlib.p_mgmq_cmit      = (LPFN_MG_MQ_CMIT)       mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_cmit");
      mg_mqlib.p_mgmq_put       = (LPFN_MG_MQ_PUT)        mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_put");
      mg_mqlib.p_mgmq_get       = (LPFN_MG_MQ_GET)        mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_get");
      mg_mqlib.p_mgmq_req       = (LPFN_MG_MQ_REQ)        mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_req");
      mg_mqlib.p_mgmq_listen    = (LPFN_MG_MQ_LISTEN)     mg_dso_sym(&(mg_mqlib.mgdso), "mgmq_listen");

      if (
            mg_mqlib.p_mgmq_version   != NULL && 
            mg_mqlib.p_mgmq_connx     != NULL && 
            mg_mqlib.p_mgmq_disc      != NULL && 
            mg_mqlib.p_mgmq_begin     != NULL && 
            mg_mqlib.p_mgmq_back      != NULL && 
            mg_mqlib.p_mgmq_cmit      != NULL && 
            mg_mqlib.p_mgmq_put       != NULL && 
            mg_mqlib.p_mgmq_get       != NULL && 
            mg_mqlib.p_mgmq_req       != NULL && 
            mg_mqlib.p_mgmq_listen    != NULL) {

            mg_mqlib.mqlib = 1;

            mg_mgmq_version(vers);
            T_SPRINTF(buffer, "The IBM MQ %s library (%s) is loaded - Version: %s.", mqc ? "CLIENT" : "SERVER", lib, vers);
            mg_log_event(buffer, "MGateway SIG: Initialization");
      }
      else {
         mg_dso_unload((MGDSO *) &(mg_mqlib.mgdso));
         T_SPRINTF(buffer, "Information: The IBM MQ library (%s) found on this system is not usable", lib);
         mg_log_event(buffer, "MGateway SIG: Initialization");
      }
   }
   else {
      T_SPRINTF(buffer, "Information: The IBM MQ library is not present on this system");
      mg_log_event(buffer, "MGateway SIG: Initialization");
   }

#endif /* #ifdef MG_MQ_USE_DSO */

#ifdef MG_MQ_STATIC_LINK

   mg_mqlib.mqlib = 1;

   mg_mgmq_version(vers);
   T_SPRINTF(buffer, "The IBM MQ library is incorporated in (or statically linked to) this distribution - Version %s.", vers);
   mg_log_event(buffer, "MGateway SIG: Initialization");

#endif /* #ifdef MG_MQ_STATIC_LINK */

   if (mg_mqlib.mqlib)
      core_data.ws_loaded = 1;
   else
      core_data.ws_loaded = 0;

   mg_wsmq_init(0);

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER ) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_load_mq: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_load_crypt32(int context)
{
   int result, mem_locked, n;
   char message[256];
   MGDSO mgdso;

#ifdef _WIN32
__try {
#endif

   result = 0;
   n = 0;
   mem_locked = 0;
   *message = '\0';
   mgdso.flags = 0;

#ifdef MG_CRYPT32

   if (mg_crypt32.load_attempted)
      return 1;
/*
   mem_locked = mg_mutex_lock(core_data.p_memlockDSO, 0, NULL);
*/
   if (mg_crypt32.load_attempted)
      goto mg_load_crypt32Exit;


#ifdef MG_CRYPT32_USE_DSO

   /* Try to Load the crypt32 library */

   mg_crypt32.crypt32 = 0;
   result = mg_dso_load(&(mg_crypt32.mgdso), "crypt32.dll");

   if (result) {

      mg_crypt32.p_CryptProtectData   = (LPFN_CRYPTPROTECTDATA)     mg_dso_sym(&(mg_crypt32.mgdso), "CryptProtectData");
      mg_crypt32.p_CryptUnprotectData = (LPFN_CRYPTUNPROTECTDATA)   mg_dso_sym(&(mg_crypt32.mgdso), "CryptUnprotectData");

      if (mg_crypt32.p_CryptProtectData != NULL && mg_crypt32.p_CryptUnprotectData != NULL) {

            mg_crypt32.crypt32 = 1;

            T_STRCPY(message, "The crypt32 library (crypt32.dll) is loaded");
      }
      else {
         T_STRCPY(message, "Information: The crypt32 library (crypt32.dll) found on this system is not usable");
      }
   }
   else {
      T_STRCPY(message, "Information: The crypt32 library (crypt32.dll) is not present on this system");
   }

#endif /* #ifdef MG_CRYPT32_USE_DSO */

#ifdef MG_CRYPT32_STATIC_LINK

   mg_crypt32.crypt32 = 1;

   T_STRCPY(message, "The crypt32 library (crypt32.dll) is incorporated in this distribution");

#endif /* #ifdef MG_CRYPT32_STATIC_LINK */

   mg_crypt32.load_attempted = 1;

mg_load_crypt32Exit:
/*
   mg_mutex_unlock(core_data.p_memlockDSO);
*/
   if (T_STRLEN(message))
      mg_log_event(message, "Initialization");

#endif /* #ifdef MG_CRYPT32 */

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   unsigned long code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_load_crypt32: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}


int mg_load_winsock(int context)
{
   int result, mem_locked;
#if defined(_WIN32)
   WORD VersionRequested;
#endif
   char buffer[1024], path[32];

#ifdef _WIN32
__try {
#endif

   result = 1;
   mem_locked = 0;
   *buffer = '\0';
   *path = '\0';

#ifdef _WIN32

   if (mg_sock.load_attempted)
      return result;

   if (mg_sock.load_attempted)
      goto mg_load_winsock_exit;

   mg_sock.sock = 0;

   /* Try to Load the Winsock 2 library */

   core_data.winsock = 2;
   T_STRCPY(path, "WS2_32.DLL");
   result = mg_dso_load(&(mg_sock.mgdso), path);

   if (!result) {
      core_data.winsock = 1;
      T_STRCPY(path, "WSOCK32.DLL");
      result = mg_dso_load(&(mg_sock.mgdso), path);
   }

   if (!result) {
      mg_log_event("Cannot load Winsock library", "Initialization Error");
      goto mg_load_winsock_exit;
   }

   mg_sock.p_WSASocket             = (LPFN_WSASOCKET)              mg_dso_sym(&(mg_sock.mgdso), "WSASocketA");
   mg_sock.p_WSAGetLastError       = (LPFN_WSAGETLASTERROR)        mg_dso_sym(&(mg_sock.mgdso), "WSAGetLastError");
   mg_sock.p_WSAStartup            = (LPFN_WSASTARTUP)             mg_dso_sym(&(mg_sock.mgdso), "WSAStartup");
   mg_sock.p_WSACleanup            = (LPFN_WSACLEANUP)             mg_dso_sym(&(mg_sock.mgdso), "WSACleanup");
   mg_sock.p_WSAFDIsSet            = (LPFN_WSAFDISSET)             mg_dso_sym(&(mg_sock.mgdso), "__WSAFDIsSet");
   mg_sock.p_WSARecv               = (LPFN_WSARECV)                mg_dso_sym(&(mg_sock.mgdso), "WSARecv");
   mg_sock.p_WSASend               = (LPFN_WSASEND)                mg_dso_sym(&(mg_sock.mgdso), "WSASend");

#if defined(MG_IPV6)
   mg_sock.p_WSAStringToAddress    = (LPFN_WSASTRINGTOADDRESS)     mg_dso_sym(&(mg_sock.mgdso), "WSAStringToAddressA");
   mg_sock.p_WSAAddressToString    = (LPFN_WSAADDRESSTOSTRING)     mg_dso_sym(&(mg_sock.mgdso), "WSAAddressToStringA");
   mg_sock.p_getaddrinfo           = (LPFN_GETADDRINFO)            mg_dso_sym(&(mg_sock.mgdso), "getaddrinfo");
   mg_sock.p_freeaddrinfo          = (LPFN_FREEADDRINFO)           mg_dso_sym(&(mg_sock.mgdso), "freeaddrinfo");
   mg_sock.p_getnameinfo           = (LPFN_GETNAMEINFO)            mg_dso_sym(&(mg_sock.mgdso), "getnameinfo");
   mg_sock.p_getpeername           = (LPFN_GETPEERNAME)            mg_dso_sym(&(mg_sock.mgdso), "getpeername");
   mg_sock.p_inet_ntop             = (LPFN_INET_NTOP)              mg_dso_sym(&(mg_sock.mgdso), "InetNtop");
   mg_sock.p_inet_pton             = (LPFN_INET_PTON)              mg_dso_sym(&(mg_sock.mgdso), "InetPton");
#else
   mg_sock.p_WSAStringToAddress    = NULL;
   mg_sock.p_WSAAddressToString    = NULL;
   mg_sock.p_getaddrinfo           = NULL;
   mg_sock.p_freeaddrinfo          = NULL;
   mg_sock.p_getnameinfo           = NULL;
   mg_sock.p_getpeername           = NULL;
   mg_sock.p_inet_ntop             = NULL;
   mg_sock.p_inet_pton             = NULL;
#endif

   mg_sock.p_closesocket           = (LPFN_CLOSESOCKET)            mg_dso_sym(&(mg_sock.mgdso), "closesocket");
   mg_sock.p_gethostname           = (LPFN_GETHOSTNAME)            mg_dso_sym(&(mg_sock.mgdso), "gethostname");
   mg_sock.p_gethostbyname         = (LPFN_GETHOSTBYNAME)          mg_dso_sym(&(mg_sock.mgdso), "gethostbyname");
   mg_sock.p_getservbyname         = (LPFN_GETSERVBYNAME)          mg_dso_sym(&(mg_sock.mgdso), "getservbyname");
   mg_sock.p_gethostbyaddr         = (LPFN_GETHOSTBYADDR)          mg_dso_sym(&(mg_sock.mgdso), "gethostbyaddr");
   mg_sock.p_htons                 = (LPFN_HTONS)                  mg_dso_sym(&(mg_sock.mgdso), "htons");
   mg_sock.p_htonl                 = (LPFN_HTONL)                  mg_dso_sym(&(mg_sock.mgdso), "htonl");
   mg_sock.p_ntohl                 = (LPFN_NTOHL)                  mg_dso_sym(&(mg_sock.mgdso), "ntohl");
   mg_sock.p_ntohs                 = (LPFN_NTOHS)                  mg_dso_sym(&(mg_sock.mgdso), "ntohs");
   mg_sock.p_connect               = (LPFN_CONNECT)                mg_dso_sym(&(mg_sock.mgdso), "connect");
   mg_sock.p_inet_addr             = (LPFN_INET_ADDR)              mg_dso_sym(&(mg_sock.mgdso), "inet_addr");
   mg_sock.p_inet_ntoa             = (LPFN_INET_NTOA)              mg_dso_sym(&(mg_sock.mgdso), "inet_ntoa");

   mg_sock.p_socket                = (LPFN_SOCKET)                 mg_dso_sym(&(mg_sock.mgdso), "socket");
   mg_sock.p_setsockopt            = (LPFN_SETSOCKOPT)             mg_dso_sym(&(mg_sock.mgdso), "setsockopt");
   mg_sock.p_getsockopt            = (LPFN_GETSOCKOPT)             mg_dso_sym(&(mg_sock.mgdso), "getsockopt");
   mg_sock.p_getsockname           = (LPFN_GETSOCKNAME)            mg_dso_sym(&(mg_sock.mgdso), "getsockname");

   mg_sock.p_select                = (LPFN_SELECT)                 mg_dso_sym(&(mg_sock.mgdso), "select");
   mg_sock.p_recv                  = (LPFN_RECV)                   mg_dso_sym(&(mg_sock.mgdso), "recv");
   mg_sock.p_send                  = (LPFN_SEND)                   mg_dso_sym(&(mg_sock.mgdso), "send");
   mg_sock.p_shutdown              = (LPFN_SHUTDOWN)               mg_dso_sym(&(mg_sock.mgdso), "shutdown");
   mg_sock.p_bind                  = (LPFN_BIND)                   mg_dso_sym(&(mg_sock.mgdso), "bind");
   mg_sock.p_listen                = (LPFN_LISTEN)                 mg_dso_sym(&(mg_sock.mgdso), "listen");
   mg_sock.p_accept                = (LPFN_ACCEPT)                 mg_dso_sym(&(mg_sock.mgdso), "accept");

   if (   (mg_sock.p_WSASocket              == NULL && core_data.winsock == 2)
       ||  mg_sock.p_WSAGetLastError        == NULL
       ||  mg_sock.p_WSAStartup             == NULL
       ||  mg_sock.p_WSACleanup             == NULL
       ||  mg_sock.p_WSAFDIsSet             == NULL
       || (mg_sock.p_WSARecv                == NULL && core_data.winsock == 2)
       || (mg_sock.p_WSASend                == NULL && core_data.winsock == 2)

#if defined(MG_IPV6)
       || (mg_sock.p_WSAStringToAddress     == NULL && core_data.winsock == 2)
       || (mg_sock.p_WSAAddressToString     == NULL && core_data.winsock == 2)
       ||  mg_sock.p_getpeername            == NULL
#endif

       ||  mg_sock.p_closesocket            == NULL
       ||  mg_sock.p_gethostname            == NULL
       ||  mg_sock.p_gethostbyname          == NULL
       ||  mg_sock.p_getservbyname          == NULL
       ||  mg_sock.p_gethostbyaddr          == NULL
       ||  mg_sock.p_htons                  == NULL
       ||  mg_sock.p_htonl                  == NULL
       ||  mg_sock.p_ntohl                  == NULL
       ||  mg_sock.p_ntohs                  == NULL
       ||  mg_sock.p_connect                == NULL
       ||  mg_sock.p_inet_addr              == NULL
       ||  mg_sock.p_inet_ntoa              == NULL
       ||  mg_sock.p_socket                 == NULL
       ||  mg_sock.p_setsockopt             == NULL
       ||  mg_sock.p_getsockopt             == NULL
       ||  mg_sock.p_getsockname            == NULL
       ||  mg_sock.p_select                 == NULL
       ||  mg_sock.p_recv                   == NULL
       ||  mg_sock.p_send                   == NULL
       ||  mg_sock.p_shutdown               == NULL
       ||  mg_sock.p_bind                   == NULL
       ||  mg_sock.p_listen                 == NULL
       ||  mg_sock.p_accept                 == NULL
      ) {

      T_SPRINTF(buffer, "Cannot use Winsock library (WSASocket=%p; WSAGetLastError=%p; WSAStartup=%p; WSACleanup=%p; WSAFDIsSet=%p; WSARecv=%p; WSASend=%p; WSAStringToAddress=%p; WSAAddressToString=%p; closesocket=%p; gethostname=%p; gethostbyname=%p; getservbyname=%p; gethostbyaddr=%p; getaddrinfo=%p; freeaddrinfo=%p; getnameinfo=%p; getpeername=%p; htons=%p; htonl=%p; ntohl=%p; ntohs=%p; connect=%p; inet_addr=%p; inet_ntoa=%p; socket=%p; setsockopt=%p; getsockopt=%p; getsockname=%p; select=%p; recv=%p; p_send=%p; shutdown=%p; bind=%p; listen=%p; accept=%p;)",
            mg_sock.p_WSASocket,
            mg_sock.p_WSAGetLastError,
            mg_sock.p_WSAStartup,
            mg_sock.p_WSACleanup,
            mg_sock.p_WSAFDIsSet,
            mg_sock.p_WSARecv,
            mg_sock.p_WSASend,

            mg_sock.p_WSAStringToAddress,
            mg_sock.p_WSAAddressToString,

            mg_sock.p_closesocket,
            mg_sock.p_gethostname,
            mg_sock.p_gethostbyname,
            mg_sock.p_getservbyname,
            mg_sock.p_gethostbyaddr,

            mg_sock.p_getaddrinfo,
            mg_sock.p_freeaddrinfo,
            mg_sock.p_getnameinfo,
            mg_sock.p_getpeername,

            mg_sock.p_htons,
            mg_sock.p_htonl,
            mg_sock.p_ntohl,
            mg_sock.p_ntohs,
            mg_sock.p_connect,
            mg_sock.p_inet_addr,
            mg_sock.p_inet_ntoa,
            mg_sock.p_socket,
            mg_sock.p_setsockopt,
            mg_sock.p_getsockopt,
            mg_sock.p_getsockname,
            mg_sock.p_select,
            mg_sock.p_recv,
            mg_sock.p_send,
            mg_sock.p_shutdown,
            mg_sock.p_bind,
            mg_sock.p_listen,
            mg_sock.p_accept
            );
      mg_log_event(buffer, "Initialization Error");
      mg_dso_unload((MGDSO *) &(mg_sock.mgdso));
   }
   else {
      mg_sock.sock = 1;
   }

   result = mg_sock.sock;

   mg_sock.load_attempted = 1;

   /* CMT715 */
   if (mg_sock.p_getaddrinfo == NULL ||  mg_sock.p_freeaddrinfo == NULL ||  mg_sock.p_getnameinfo == NULL)
      core_data.ipv6 = 0;

mg_load_winsock_exit:

   if (result) {

      if (core_data.winsock == 2)
         VersionRequested = MAKEWORD(2, 2);
      else
         VersionRequested = MAKEWORD(1, 1);

      core_data.wsastartup = MGNET_WSASTARTUP(VersionRequested, &(core_data.wsadata));

      if (core_data.wsastartup != 0 && core_data.winsock == 2) {
         VersionRequested = MAKEWORD(2, 0);
         core_data.wsastartup = MGNET_WSASTARTUP(VersionRequested, &(core_data.wsadata));
         if (core_data.wsastartup != 0) {
            core_data.winsock = 1;
            VersionRequested = MAKEWORD(1, 1);
            core_data.wsastartup = MGNET_WSASTARTUP(VersionRequested, &(core_data.wsadata));
         }
      }
      if (core_data.wsastartup == 0) {
         if ((core_data.winsock == 2 && LOBYTE(core_data.wsadata.wVersion) != 2)
               || (core_data.winsock == 1 && (LOBYTE(core_data.wsadata.wVersion) != 1 || HIBYTE(core_data.wsadata.wVersion) != 1))) {
  
            T_SPRINTF(buffer, "Wrong version of Winsock library (%s) (%d.%d)", path, LOBYTE(core_data.wsadata.wVersion), HIBYTE(core_data.wsadata.wVersion));
            mg_log_event(buffer, "Initialization Error");
            MGNET_WSACLEANUP();
            core_data.wsastartup = -1;
         }
         else {
            if (strlen(path))
               T_SPRINTF(buffer, "Windows Sockets library loaded (%s) Version: %d.%d", path, LOBYTE(core_data.wsadata.wVersion), HIBYTE(core_data.wsadata.wVersion));
            else
               T_SPRINTF(buffer, "Windows Sockets library Version: %d.%d", LOBYTE(core_data.wsadata.wVersion), HIBYTE(core_data.wsadata.wVersion));

            mg_log_event(buffer, "Initialization");
            mg_sock.winsock_ready = 1;
         }
      }
      else {
         mg_log_event("Unusable Winsock library", "Initialization Error");
      }
   }


#endif /* #ifdef _WIN32 */

   return 1;

#ifdef _WIN32
}
__except (EXCEPTION_EXECUTE_HANDLER) {

   DWORD code;
   char buffer[256];

   __try {
      code = GetExceptionCode();
      T_SPRINTF(buffer, "Exception caught in f:mg_load_winsock: %x", code);
      mg_log_event(buffer, "Error Condition");
   }
   __except (EXCEPTION_EXECUTE_HANDLER ) {
      ;
   }

   return 0;
}
#endif

}

