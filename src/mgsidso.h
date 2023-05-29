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

#ifndef MGSIDSO_H
#define MGSIDSO_H

typedef struct tagMGDSO {
   short flags;
   MGHLIB h_library;
} MGDSO;


#if defined(MG_MQ_USE_DSO)

#define mg_mgmq_version            mg_mqlib.p_mgmq_version
#define mg_mgmq_connx              mg_mqlib.p_mgmq_connx
#define mg_mgmq_disc               mg_mqlib.p_mgmq_disc
#define mg_mgmq_begin              mg_mqlib.p_mgmq_begin
#define mg_mgmq_back               mg_mqlib.p_mgmq_back
#define mg_mgmq_cmit               mg_mqlib.p_mgmq_cmit
#define mg_mgmq_put                mg_mqlib.p_mgmq_put
#define mg_mgmq_get                mg_mqlib.p_mgmq_get
#define mg_mgmq_req                mg_mqlib.p_mgmq_req
#define mg_mgmq_listen             mg_mqlib.p_mgmq_listen

#else

#define mg_mgmq_version            mgmq_version
#define mg_mgmq_connx              mgmq_connx
#define mg_mgmq_disc               mgmq_disc
#define mg_mgmq_begin              mgmq_begin
#define mg_mgmq_back               mgmq_back
#define mg_mgmq_cmit               mgmq_cmit
#define mg_mgmq_put                mgmq_put
#define mg_mgmq_get                mgmq_get
#define mg_mgmq_req                mgmq_req
#define mg_mgmq_listen             mgmq_listen

#endif /* #ifdef MG_MQ_USE_DSO */


typedef struct tagMGMQLIB {

   short                mqlib;

   MGDSO               mgdso;

   LPFN_MG_MQ_VERSION  p_mgmq_version;
   LPFN_MG_MQ_CONNX    p_mgmq_connx;
   LPFN_MG_MQ_DISC     p_mgmq_disc;
   LPFN_MG_MQ_BEGIN    p_mgmq_begin;
   LPFN_MG_MQ_BACK     p_mgmq_back;
   LPFN_MG_MQ_CMIT     p_mgmq_cmit;
   LPFN_MG_MQ_PUT      p_mgmq_put;
   LPFN_MG_MQ_GET      p_mgmq_get;
   LPFN_MG_MQ_REQ      p_mgmq_req;
   LPFN_MG_MQ_LISTEN   p_mgmq_listen;

} MGMQLIB, FAR * LPMGMQLIB;


#ifdef MG_SSL

#ifdef MG_SSL_USE_DSO

#define mg_SSL_library_init               mg_ssl.p_SSL_library_init
#define mg_SSLv2_client_method            mg_ssl.p_SSLv2_client_method
#define mg_SSLv23_server_method           mg_ssl.p_SSLv23_server_method
#define mg_SSL_load_error_strings         mg_ssl.p_SSL_load_error_strings
#define mg_SSL_CTX_new                    mg_ssl.p_SSL_CTX_new
#define mg_SSL_new                        mg_ssl.p_SSL_new
#define mg_SSL_set_fd                     mg_ssl.p_SSL_set_fd
#define mg_SSL_CTX_use_certificate_file   mg_ssl.p_SSL_CTX_use_certificate_file
#define mg_SSL_CTX_use_PrivateKey_file    mg_ssl.p_SSL_CTX_use_PrivateKey_file
#define mg_SSL_CTX_check_private_key      mg_ssl.p_SSL_CTX_check_private_key
#define mg_SSL_accept                     mg_ssl.p_SSL_accept
#define mg_SSL_connect                    mg_ssl.p_SSL_connect
#define mg_SSL_get_current_cipher         mg_ssl.p_SSL_get_current_cipher
#define mg_SSL_CIPHER_get_name            mg_ssl.p_SSL_CIPHER_get_name
#define mg_SSL_get_peer_certificate       mg_ssl.p_SSL_get_peer_certificate
#define mg_SSL_write                      mg_ssl.p_SSL_write
#define mg_SSL_read                       mg_ssl.p_SSL_read
#define mg_SSL_shutdown                   mg_ssl.p_SSL_shutdown
#define mg_SSL_free                       mg_ssl.p_SSL_free
#define mg_SSL_CTX_free                   mg_ssl.p_SSL_CTX_free


#define mg_SSLeay_version                 mg_ssl.p_SSLeay_version
#define mg_X509_get_subject_name          mg_ssl.p_X509_get_subject_name
#define mg_X509_NAME_oneline              mg_ssl.p_X509_NAME_oneline
#define mg_CRYPTO_free                    mg_ssl.p_CRYPTO_free
#define mg_X509_get_issuer_name           mg_ssl.p_X509_get_issuer_name
#define mg_X509_free                      mg_ssl.p_X509_free

#define mg_X509_STORE_CTX_get_current_cert  mg_ssl.p_X509_STORE_CTX_get_current_cert
#define mg_X509_STORE_CTX_set_error         mg_ssl.p_X509_STORE_CTX_set_error
#define mg_X509_STORE_CTX_get_error         mg_ssl.p_X509_STORE_CTX_get_error
#define mg_X509_STORE_CTX_get_error_depth   mg_ssl.p_X509_STORE_CTX_get_error_depth
#define mg_X509_STORE_CTX_get_ex_data       mg_ssl.p_X509_STORE_CTX_get_ex_data
#define mg_X509_verify_cert_error_string    mg_ssl.p_X509_verify_cert_error_string

#define mg_SSL_CTX_set_verify               mg_ssl.p_SSL_CTX_set_verify
#define mg_SSL_CTX_set_verify_depth         mg_ssl.p_SSL_CTX_set_verify_depth
#define mg_SSL_CTX_set_cert_verify_callback mg_ssl.p_SSL_CTX_set_cert_verify_callback
#define mg_SSL_CTX_load_verify_locations    mg_ssl.p_SSL_CTX_load_verify_locations

#define mg_SSL_set_verify_result            mg_ssl.p_SSL_set_verify_result
#define mg_SSL_get_verify_result            mg_ssl.p_SSL_get_verify_result
#define mg_SSL_set_verify                   mg_ssl.p_SSL_set_verify
#define mg_SSL_set_verify_depth             mg_ssl.p_SSL_set_verify_depth

#define mg_SSL_set_ex_data                  mg_ssl.p_SSL_set_ex_data
#define mg_SSL_get_ex_data                  mg_ssl.p_SSL_get_ex_data
#define mg_SSL_get_ex_new_index             mg_ssl.p_SSL_get_ex_new_index

#define mg_SSL_get_ex_data_X509_STORE_CTX_idx  mg_ssl.p_SSL_get_ex_data_X509_STORE_CTX_idx

#define mg_ERR_get_error                    mg_ssl.p_ERR_get_error
#define mg_ERR_error_string                 mg_ssl.p_ERR_error_string

#define mg_HMAC                             mg_ssl.p_HMAC
#define mg_EVP_sha256                       mg_ssl.p_EVP_sha256
#define mg_EVP_sha1                         mg_ssl.p_EVP_sha1
#define mg_EVP_sha                          mg_ssl.p_EVP_sha
#define mg_EVP_md5                          mg_ssl.p_EVP_md5

#define mg_SHA256                           mg_ssl.p_SHA256
#define mg_SHA1                             mg_ssl.p_SHA1
#define mg_SHA                              mg_ssl.p_SHA
#define mg_MD5                              mg_ssl.p_MD5

#else

#define mg_SSL_library_init               SSL_library_init
#define mg_SSLv2_client_method            SSLv2_client_method
#define mg_SSLv23_server_method           SSLv23_server_method
#define mg_SSL_load_error_strings         SSL_load_error_strings
#define mg_SSL_CTX_new                    SSL_CTX_new
#define mg_SSL_new                        SSL_new
#define mg_SSL_set_fd                     SSL_set_fd
#define mg_SSL_CTX_use_certificate_file   SSL_CTX_use_certificate_file
#define mg_SSL_CTX_use_PrivateKey_file    SSL_CTX_use_PrivateKey_file
#define mg_SSL_CTX_check_private_key      SSL_CTX_check_private_key
#define mg_SSL_accept                     SSL_accept
#define mg_SSL_connect                    SSL_connect
#define mg_SSL_get_current_cipher         SSL_get_current_cipher
#define mg_SSL_CIPHER_get_name            SSL_CIPHER_get_name
#define mg_SSL_get_peer_certificate       SSL_get_peer_certificate
#define mg_SSL_write                      SSL_write
#define mg_SSL_read                       SSL_read
#define mg_SSL_shutdown                   SSL_shutdown
#define mg_SSL_free                       SSL_free
#define mg_SSL_CTX_free                   SSL_CTX_free


#define mg_SSLeay_version                 SSLeay_version
#define mg_X509_get_subject_name          X509_get_subject_name
#define mg_X509_NAME_oneline              X509_NAME_oneline
#define mg_CRYPTO_free                    CRYPTO_free
#define mg_X509_get_issuer_name           X509_get_issuer_name
#define mg_X509_free                      X509_free

#define mg_X509_STORE_CTX_get_current_cert  X509_STORE_CTX_get_current_cert
#define mg_X509_STORE_CTX_set_error         X509_STORE_CTX_set_error
#define mg_X509_STORE_CTX_get_error         X509_STORE_CTX_get_error
#define mg_X509_STORE_CTX_get_error_depth   X509_STORE_CTX_get_error_depth
#define mg_X509_STORE_CTX_get_ex_data       X509_STORE_CTX_get_ex_data
#define mg_X509_verify_cert_error_string    X509_verify_cert_error_string

#define mg_SSL_CTX_set_verify               SSL_CTX_set_verify
#define mg_SSL_CTX_set_verify_depth         SSL_CTX_set_verify_depth
#define mg_SSL_CTX_set_cert_verify_callback SSL_CTX_set_cert_verify_callback
#define mg_SSL_CTX_load_verify_locations    SSL_CTX_load_verify_locations

#define mg_SSL_set_verify_result            SSL_set_verify_result
#define mg_SSL_get_verify_result            SSL_get_verify_result
#define mg_SSL_set_verify                   SSL_set_verify
#define mg_SSL_set_verify_depth             SSL_set_verify_depth

#define mg_SSL_set_ex_data                  SSL_set_ex_data
#define mg_SSL_get_ex_data                  SSL_get_ex_data
#define mg_SSL_get_ex_new_index             SSL_get_ex_new_index

#define mg_SSL_get_ex_data_X509_STORE_CTX_idx  SSL_get_ex_data_X509_STORE_CTX_idx

#define mg_ERR_get_error                    ERR_get_error
#define mg_ERR_error_string                 ERR_error_string

#define mg_HMAC                             HMAC
#define mg_EVP_sha256                       EVP_sha256
#define mg_EVP_sha1                         EVP_sha1
#define mg_EVP_sha                          EVP_sha
#define mg_EVP_md5                          EVP_md5

#define mg_SHA256                           SHA256
#define mg_SHA1                             SHA1
#define mg_SHA                              SHA
#define mg_MD5                              MD5


#endif /* #ifdef MG_SSL_USE_DSO */

#define mg_SSL_get_cipher(s)              mg_SSL_CIPHER_get_name(mg_SSL_get_current_cipher(s))

#define MG_WINAPI

typedef int          (MG_WINAPI * LPFN_SSL_LIBRARY_INIT)             (void);
typedef SSL_METHOD * (MG_WINAPI * LPFN_SSLV2_CLIENT_METHOD)          (void);
typedef SSL_METHOD * (MG_WINAPI * LPFN_SSLV23_SERVER_METHOD)         (void);
typedef void	      (MG_WINAPI * LPFN_SSL_LOAD_ERROR_STRINGS)       (void);
typedef SSL_CTX *    (MG_WINAPI * LPFN_SSL_CTX_NEW)                  (SSL_METHOD *meth);
typedef SSL *	      (MG_WINAPI * LPFN_SSL_NEW)                      (SSL_CTX *ctx);
typedef int	         (MG_WINAPI * LPFN_SSL_SET_FD)                   (SSL *s, int fd);
typedef int	         (MG_WINAPI * LPFN_SSL_CTX_USE_CERTIFICATE_FILE) (SSL_CTX *ctx, const char *file, int type);
typedef int	         (MG_WINAPI * LPFN_SSL_CTX_USE_PRIVATEKEY_FILE)  (SSL_CTX *ctx, const char *file, int type);
typedef int          (MG_WINAPI * LPFN_SSL_CTX_CHECK_PRIVATE_KEY)    (SSL_CTX *ctx);
typedef int 	      (MG_WINAPI * LPFN_SSL_ACCEPT)                   (SSL *ssl);
typedef int 	      (MG_WINAPI * LPFN_SSL_CONNECT)                  (SSL *ssl);
typedef SSL_CIPHER * (MG_WINAPI * LPFN_SSL_GET_CURRENT_CIPHER)       (SSL *s);
typedef const char *	(MG_WINAPI * LPFN_SSL_CIPHER_GET_NAME)          (SSL_CIPHER *c);
typedef X509 *	      (MG_WINAPI * LPFN_SSL_GET_PEER_CERTIFICATE)     (SSL *s);
typedef int 	      (MG_WINAPI * LPFN_SSL_WRITE)                    (SSL *ssl,const void *buf,int num);
typedef int 	      (MG_WINAPI * LPFN_SSL_READ)                     (SSL *ssl,void *buf,int num);
typedef int          (MG_WINAPI * LPFN_SSL_SHUTDOWN)                 (SSL *s);
typedef void	      (MG_WINAPI * LPFN_SSL_FREE)                     (SSL *ssl);
typedef void	      (MG_WINAPI * LPFN_SSL_CTX_FREE)                 (SSL_CTX *);


typedef const char * (MG_WINAPI * LPFN_SSLEAY_VERSION)               (int type);
typedef X509_NAME *	(MG_WINAPI * LPFN_X509_GET_SUBJECT_NAME)        (X509 *a);
typedef char *		   (MG_WINAPI * LPFN_X509_NAME_ONELINE)            (X509_NAME *a,char *buf,int size);
typedef void         (MG_WINAPI * LPFN_CRYPTO_FREE)                  (void *);
typedef X509_NAME *	(MG_WINAPI * LPFN_X509_GET_ISSUER_NAME)         (X509 *a);
typedef void         (MG_WINAPI * LPFN_X509_FREE)                    (X509 *server_cert);

typedef X509 *       (MG_WINAPI * LPFN_X509_STORE_CTX_GET_CURRENT_CERT)    (X509_STORE_CTX *ctx);
typedef void         (MG_WINAPI * LPFN_X509_STORE_CTX_SET_ERROR)           (X509_STORE_CTX *ctx, int s);
typedef int          (MG_WINAPI * LPFN_X509_STORE_CTX_GET_ERROR)           (X509_STORE_CTX *ctx);
typedef int          (MG_WINAPI * LPFN_X509_STORE_CTX_GET_ERROR_DEPTH)     (X509_STORE_CTX *ctx);
typedef void *       (MG_WINAPI * LPFN_X509_STORE_CTX_GET_EX_DATA)         (X509_STORE_CTX *ctx, int idx);
typedef const char * (MG_WINAPI * LPFN_X509_VERIFY_CERT_ERROR_STRING)      (long n);


typedef void         (MG_WINAPI * LPFN_SSL_CTX_SET_VERIFY)                 (SSL_CTX *ctx, int mode, int (*callback)(int, X509_STORE_CTX *));
typedef void         (MG_WINAPI * LPFN_SSL_CTX_SET_VERIFY_DEPTH)           (SSL_CTX *ctx, int depth);
typedef void         (MG_WINAPI * LPFN_SSL_CTX_SET_CERT_VERIFY_CALLBACK)   (SSL_CTX *ctx, int (*cb)(X509_STORE_CTX *, void *), void *arg);
typedef int          (MG_WINAPI * LPFN_SSL_CTX_LOAD_VERIFY_LOCATIONS)      (SSL_CTX *ctx, const char *CAfile, const char *CApath);

typedef void         (MG_WINAPI * LPFN_SSL_SET_VERIFY_RESULT)              (SSL *ssl, long v);
typedef long         (MG_WINAPI * LPFN_SSL_GET_VERIFY_RESULT)              (SSL *ssl);
typedef void         (MG_WINAPI * LPFN_SSL_SET_VERIFY)                     (SSL *s, int mode, int (*callback)(int ok, X509_STORE_CTX *ctx));
typedef void         (MG_WINAPI * LPFN_SSL_SET_VERIFY_DEPTH)               (SSL *s, int depth);

typedef int          (MG_WINAPI * LPFN_SSL_SET_EX_DATA)                    (SSL *ssl, int idx, void *data);
typedef void *       (MG_WINAPI * LPFN_SSL_GET_EX_DATA)                    (SSL *ssl, int idx);
typedef int          (MG_WINAPI * LPFN_SSL_GET_EX_NEW_INDEX)               (long argl, void *argp, CRYPTO_EX_new *new_func, CRYPTO_EX_dup *dup_func, CRYPTO_EX_free *free_func);
typedef int          (MG_WINAPI * LPFN_SSL_GET_EX_DATA_X509_STORE_CTX_IDX) (void );

typedef unsigned long   (MG_WINAPI * LPFN_ERR_GET_ERROR)                   (void);
typedef char *          (MG_WINAPI * LPFN_ERR_ERROR_STRING)                (unsigned long e,char *buf);

typedef unsigned char * (MG_WINAPI * LPFN_HMAC)                            (const EVP_MD *evp_md, const void *key, int key_len, const unsigned char *d, int n, unsigned char *md, unsigned int *md_len);

typedef const EVP_MD *  (MG_WINAPI * LPFN_EVP_SHA256)                      (void);
typedef const EVP_MD *  (MG_WINAPI * LPFN_EVP_SHA1)                        (void);
typedef const EVP_MD *  (MG_WINAPI * LPFN_EVP_SHA)                         (void);
typedef const EVP_MD *  (MG_WINAPI * LPFN_EVP_MD5)                         (void);

typedef unsigned char * (MG_WINAPI * LPFN_SHA256)                          (const unsigned char *d, unsigned long n, unsigned char *md);
typedef unsigned char * (MG_WINAPI * LPFN_SHA1)                            (const unsigned char *d, unsigned long n, unsigned char *md);
typedef unsigned char * (MG_WINAPI * LPFN_SHA)                             (const unsigned char *d, unsigned long n, unsigned char *md);
typedef unsigned char * (MG_WINAPI * LPFN_MD5)                             (const unsigned char *d, unsigned long n, unsigned char *md);


typedef struct tagMGSSL {

   short             ssl;
   short             load_attempted;

   MGDSO             mgdso;
   MGDSO             mgdso_libeay;

   LPFN_SSL_LIBRARY_INIT                     p_SSL_library_init;
   LPFN_SSLV2_CLIENT_METHOD                  p_SSLv2_client_method;
   LPFN_SSLV23_SERVER_METHOD                 p_SSLv23_server_method;
   LPFN_SSL_LOAD_ERROR_STRINGS               p_SSL_load_error_strings;
   LPFN_SSL_CTX_NEW                          p_SSL_CTX_new;
   LPFN_SSL_NEW                              p_SSL_new;
   LPFN_SSL_SET_FD                           p_SSL_set_fd;
   LPFN_SSL_CTX_USE_CERTIFICATE_FILE         p_SSL_CTX_use_certificate_file;
   LPFN_SSL_CTX_USE_PRIVATEKEY_FILE          p_SSL_CTX_use_PrivateKey_file;
   LPFN_SSL_CTX_CHECK_PRIVATE_KEY            p_SSL_CTX_check_private_key;
   LPFN_SSL_ACCEPT                           p_SSL_accept;
   LPFN_SSL_CONNECT                          p_SSL_connect;
   LPFN_SSL_GET_CURRENT_CIPHER               p_SSL_get_current_cipher;
   LPFN_SSL_CIPHER_GET_NAME                  p_SSL_CIPHER_get_name;
   LPFN_SSL_GET_PEER_CERTIFICATE             p_SSL_get_peer_certificate;
   LPFN_SSL_WRITE                            p_SSL_write;
   LPFN_SSL_READ                             p_SSL_read;
   LPFN_SSL_SHUTDOWN                         p_SSL_shutdown;
   LPFN_SSL_FREE                             p_SSL_free;
   LPFN_SSL_CTX_FREE                         p_SSL_CTX_free;

   LPFN_SSLEAY_VERSION                       p_SSLeay_version;
   LPFN_X509_GET_SUBJECT_NAME                p_X509_get_subject_name;
   LPFN_X509_NAME_ONELINE                    p_X509_NAME_oneline;
   LPFN_CRYPTO_FREE                          p_CRYPTO_free;
   LPFN_X509_GET_ISSUER_NAME                 p_X509_get_issuer_name;
   LPFN_X509_FREE                            p_X509_free;

   LPFN_X509_STORE_CTX_GET_CURRENT_CERT      p_X509_STORE_CTX_get_current_cert;
   LPFN_X509_STORE_CTX_SET_ERROR             p_X509_STORE_CTX_set_error;
   LPFN_X509_STORE_CTX_GET_ERROR             p_X509_STORE_CTX_get_error;
   LPFN_X509_STORE_CTX_GET_ERROR_DEPTH       p_X509_STORE_CTX_get_error_depth;
   LPFN_X509_STORE_CTX_GET_EX_DATA           p_X509_STORE_CTX_get_ex_data;
   LPFN_X509_VERIFY_CERT_ERROR_STRING        p_X509_verify_cert_error_string;

   LPFN_SSL_CTX_SET_VERIFY                   p_SSL_CTX_set_verify;
   LPFN_SSL_CTX_SET_VERIFY_DEPTH             p_SSL_CTX_set_verify_depth;
   LPFN_SSL_CTX_SET_CERT_VERIFY_CALLBACK     p_SSL_CTX_set_cert_verify_callback;
   LPFN_SSL_CTX_LOAD_VERIFY_LOCATIONS        p_SSL_CTX_load_verify_locations;

   LPFN_SSL_SET_VERIFY_RESULT                p_SSL_set_verify_result;
   LPFN_SSL_GET_VERIFY_RESULT                p_SSL_get_verify_result;
   LPFN_SSL_SET_VERIFY                       p_SSL_set_verify;
   LPFN_SSL_SET_VERIFY_DEPTH                 p_SSL_set_verify_depth;

   LPFN_SSL_SET_EX_DATA                      p_SSL_set_ex_data;
   LPFN_SSL_GET_EX_DATA                      p_SSL_get_ex_data;
   LPFN_SSL_GET_EX_NEW_INDEX                 p_SSL_get_ex_new_index;
   LPFN_SSL_GET_EX_DATA_X509_STORE_CTX_IDX   p_SSL_get_ex_data_X509_STORE_CTX_idx;

   LPFN_ERR_GET_ERROR                        p_ERR_get_error;
   LPFN_ERR_ERROR_STRING                     p_ERR_error_string;

   LPFN_HMAC                                 p_HMAC;
   LPFN_EVP_SHA256                           p_EVP_sha256;
   LPFN_EVP_SHA1                             p_EVP_sha1;
   LPFN_EVP_SHA                              p_EVP_sha;
   LPFN_EVP_MD5                              p_EVP_md5;

   LPFN_SHA256                               p_SHA256;
   LPFN_SHA1                                 p_SHA1;
   LPFN_SHA                                  p_SHA;
   LPFN_MD5                                  p_MD5;


} MGSSL, FAR * LPMGSSL;

 typedef struct tagMGSSLV {
   int verbose_mode;
   int verify_depth;
   int always_continue;
 } MGSSLV, FAR * LPMGSSLV;

 int mgsslv_index;


#endif /* #ifdef MG_SSL */


#ifdef MG_CRYPT32

#ifdef MG_CRYPT32_USE_DSO

#define mg_CryptProtectData           mg_crypt32.p_CryptProtectData
#define mg_CryptUnprotectData         mg_crypt32.p_CryptUnprotectData

#else

#define mg_CryptProtectData           CryptProtectData
#define mg_CryptUnprotectData         CryptUnprotectData

#endif /* #ifdef MG_CRYPT32_USE_DSO */


typedef BOOL (WINAPI * LPFN_CRYPTPROTECTDATA) (DATA_BLOB *pDataIn, LPCWSTR szDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, unsigned long dwFlags, DATA_BLOB *pDataOut);
typedef BOOL (WINAPI * LPFN_CRYPTUNPROTECTDATA) (DATA_BLOB *pDataIn, LPWSTR *ppszDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, unsigned long dwFlags, DATA_BLOB *pDataOut);


typedef struct tagMGCRYPT32 {
   short                   crypt32;
   short                   load_attempted;
   MGDSO                   mgdso;

   LPFN_CRYPTPROTECTDATA   p_CryptProtectData;
   LPFN_CRYPTUNPROTECTDATA p_CryptUnprotectData;

} MGCRYPT32, FAR * LPMGCRYPT32;

#endif /* #ifdef MG_CRYPT32 */


#if defined(_WIN32)

#define MGNET_WSASOCKET               mg_sock.p_WSASocket
#define MGNET_WSAGETLASTERROR         mg_sock.p_WSAGetLastError
#define MGNET_WSASTARTUP              mg_sock.p_WSAStartup
#define MGNET_WSACLEANUP              mg_sock.p_WSACleanup
#define MGNET_WSAFDISET               mg_sock.p_WSAFDIsSet
#define MGNET_WSARECV                 mg_sock.p_WSARecv
#define MGNET_WSASEND                 mg_sock.p_WSASend

#define MGNET_WSASTRINGTOADDRESS      mg_sock.p_WSAStringToAddress
#define MGNET_WSAADDRESSTOSTRING      mg_sock.p_WSAAddressToString
#define MGNET_GETADDRINFO             mg_sock.p_getaddrinfo
#define MGNET_FREEADDRINFO            mg_sock.p_freeaddrinfo
#define MGNET_GETNAMEINFO             mg_sock.p_getnameinfo
#define MGNET_GETPEERNAME             mg_sock.p_getpeername
#define MGNET_INET_NTOP               mg_sock.p_inet_ntop
#define MGNET_INET_PTON               mg_sock.p_inet_pton

#define MGNET_CLOSESOCKET             mg_sock.p_closesocket
#define MGNET_GETHOSTNAME             mg_sock.p_gethostname
#define MGNET_GETHOSTBYNAME           mg_sock.p_gethostbyname
#define MGNET_SETSERVBYNAME           mg_sock.p_getservbyname
#define MGNET_GETHOSTBYADDR           mg_sock.p_gethostbyaddr
#define MGNET_HTONS                   mg_sock.p_htons
#define MGNET_HTONL                   mg_sock.p_htonl
#define MGNET_NTOHL                   mg_sock.p_ntohl
#define MGNET_NTOHS                   mg_sock.p_ntohs
#define MGNET_CONNECT                 mg_sock.p_connect
#define MGNET_INET_ADDR               mg_sock.p_inet_addr
#define MGNET_INET_NTOA               mg_sock.p_inet_ntoa
#define MGNET_SOCKET                  mg_sock.p_socket
#define MGNET_SETSOCKOPT              mg_sock.p_setsockopt
#define MGNET_GETSOCKOPT              mg_sock.p_getsockopt
#define MGNET_GETSOCKNAME             mg_sock.p_getsockname
#define MGNET_SELECT                  mg_sock.p_select
#define MGNET_RECV                    mg_sock.p_recv
#define MGNET_SEND                    mg_sock.p_send
#define MGNET_SHUTDOWN                mg_sock.p_shutdown
#define MGNET_BIND                    mg_sock.p_bind
#define MGNET_LISTEN                  mg_sock.p_listen
#define MGNET_ACCEPT                  mg_sock.p_accept

#define  MGNET_FD_ISSET(fd, set)            mg_sock.p_WSAFDIsSet((SOCKET)(fd), (fd_set FAR *)(set))

typedef int (WINAPI * LPFN_WSAFDISSET)       (SOCKET, fd_set FAR *);

#else

#define MGNET_WSASOCKET               WSASocket
#define MGNET_WSAGETLASTERROR         WSAGetLastError
#define MGNET_WSASTARTUP              WSAStartup
#define MGNET_WSACLEANUP              WSACleanup
#define MGNET_WSAFDIsSet              WSAFDIsSet
#define MGNET_WSARECV                 WSARecv
#define MGNET_WSASEND                 WSASend

#define MGNET_WSASTRINGTOADDRESS      WSAStringToAddress
#define MGNET_WSAADDRESSTOSTRING      WSAAddressToString
#define MGNET_GETADDRINFO             getaddrinfo
#define MGNET_FREEADDRINFO            freeaddrinfo
#define MGNET_GETNAMEINFO             getnameinfo
#define MGNET_GETPEERNAME             getpeername
#define MGNET_INET_NTOP               inet_ntop
#define MGNET_INET_PTON               inet_pton

#define MGNET_CLOSESOCKET             closesocket
#define MGNET_GETHOSTNAME             gethostname
#define MGNET_GETHOSTBYNAME           gethostbyname
#define MGNET_SETSERVBYNAME           getservbyname
#define MGNET_GETHOSTBYADDR           gethostbyaddr
#define MGNET_HTONS                   htons
#define MGNET_HTONL                   htonl
#define MGNET_NTOHL                   ntohl
#define MGNET_NTOHS                   ntohs
#define MGNET_CONNECT                 connect
#define MGNET_INET_ADDR               inet_addr
#define MGNET_INET_NTOA               inet_ntoa
#define MGNET_SOCKET                  socket
#define MGNET_SETSOCKOPT              setsockopt
#define MGNET_GETSOCKOPT              getsockopt
#define MGNET_GETSOCKNAME             getsockname
#define MGNET_SELECT                  select
#define MGNET_RECV                    recv
#define MGNET_SEND                    send
#define MGNET_SHUTDOWN                shutdown
#define MGNET_BIND                    bind
#define MGNET_LISTEN                  listen
#define MGNET_ACCEPT                  accept

#define MGNET_FD_ISSET(fd, set) FD_ISSET(fd, set)

#endif /* #if defined(_WIN32) */


typedef struct tagMGSOCK {

   unsigned char                 winsock_ready;
   short                         sock;
   short                         load_attempted;

   MGDSO                         mgdso;

#ifdef _WIN32
   LPFN_WSASOCKET                p_WSASocket;
   LPFN_WSAGETLASTERROR          p_WSAGetLastError; 
   LPFN_WSASTARTUP               p_WSAStartup;
   LPFN_WSACLEANUP               p_WSACleanup;
   LPFN_WSAFDISSET               p_WSAFDIsSet;
   LPFN_WSARECV                  p_WSARecv;
   LPFN_WSASEND                  p_WSASend;

#if defined(MG_IPV6)
   LPFN_WSASTRINGTOADDRESS       p_WSAStringToAddress;
   LPFN_WSAADDRESSTOSTRING       p_WSAAddressToString;
   LPFN_GETADDRINFO              p_getaddrinfo;
   LPFN_FREEADDRINFO             p_freeaddrinfo;
   LPFN_GETNAMEINFO              p_getnameinfo;
   LPFN_GETPEERNAME              p_getpeername;
   LPFN_INET_NTOP                p_inet_ntop;
   LPFN_INET_PTON                p_inet_pton;
#else
   void *                        p_WSAStringToAddress;
   void *                        p_WSAAddressToString;
   void *                        p_getaddrinfo;
   void *                        p_freeaddrinfo;
   void *                        p_getnameinfo;
   void *                        p_getpeername;
   void *                        p_inet_ntop;
   void *                        p_inet_pton;
#endif

   LPFN_CLOSESOCKET              p_closesocket;
   LPFN_GETHOSTNAME              p_gethostname;
   LPFN_GETHOSTBYNAME            p_gethostbyname;
   LPFN_GETHOSTBYADDR            p_gethostbyaddr;
   LPFN_GETSERVBYNAME            p_getservbyname;

   LPFN_HTONS                    p_htons;
   LPFN_HTONL                    p_htonl;
   LPFN_NTOHL                    p_ntohl;
   LPFN_NTOHS                    p_ntohs;
   LPFN_CONNECT                  p_connect;
   LPFN_INET_ADDR                p_inet_addr;
   LPFN_INET_NTOA                p_inet_ntoa;

   LPFN_SOCKET                   p_socket;
   LPFN_SETSOCKOPT               p_setsockopt;
   LPFN_GETSOCKOPT               p_getsockopt;
   LPFN_GETSOCKNAME              p_getsockname;
   LPFN_SELECT                   p_select;
   LPFN_RECV                     p_recv;
   LPFN_SEND                     p_send;
   LPFN_SHUTDOWN                 p_shutdown;
   LPFN_BIND                     p_bind;
   LPFN_LISTEN                   p_listen;
   LPFN_ACCEPT                   p_accept;
#endif

} MGSOCK, FAR * LPMGSOCK;


/* Externals */

extern MGMQLIB          mg_mqlib;
extern MGSOCK           mg_sock;

#ifdef MG_SSL
extern MGSSL            mg_ssl;
#endif /* #ifdef MG_SSL */

#ifdef MG_CRYPT32
extern MGCRYPT32        mg_crypt32;
#endif /* #ifdef MG_CRYPT32 */


int                  mg_dso_load                (MGDSO *p_mgdso, char * library);
MGPROC               mg_dso_sym                 (MGDSO *p_mgdso, char * symbol);
int                  mg_dso_unload              (MGDSO *p_mgdso);

int                  mg_load_openssl            (int context);
int                  mg_load_bdb                (int context);
int                  mg_load_mq                 (int context);
int                  mg_load_crypt32            (int context);
int                  mg_load_winsock            (int context);

#endif