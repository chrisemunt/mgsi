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


#ifndef MGSISYS_H
#define MGSISYS_H

/* Build old mgwsi version of the SIG */
/*
#define MG_MGWSI                 1
*/

#if defined(MG_MGWSI)
#define MAJORVERSION             2
#define MINORVERSION             3
#define BUILDNUMBER              103
#define PATCHNUMBER              0
#define PRODUCTVERSIONSTR        "2.3.103.0"
#define FILEVERSIONSTR           PRODUCTVERSIONSTR
#define MG_VERSION              "2.3.103"
#define MG_VERSION_DISPLAY      "2.3"
#else
#define MAJORVERSION             3
#define MINORVERSION             1
#define BUILDNUMBER              103
#define PATCHNUMBER              0
#define PRODUCTVERSIONSTR        "3.1.103.0"
#define FILEVERSIONSTR           PRODUCTVERSIONSTR
#define MG_VERSION              "3.1.103"
#define MG_VERSION_DISPLAY      "3.1"
#endif

/*
2.0.81
#define MG_VERSION_CREATED      "5 July 2008"
*/
/*
2.0.82
#define MG_VERSION_CREATED      "11 August 2008"
*/
/*
2.0.83
#define MG_VERSION_CREATED      "10 September 2008"
*/
/*
2.0.84
#define MG_VERSION_CREATED      "17 September 2008"
*/
/*
2.0.85
#define MG_VERSION_CREATED      "25 September 2008"
*/
/*
2.0.86
#define MG_VERSION_CREATED      "10 October 2008"
*/
/*
2.0.87
#define MG_VERSION_CREATED      "30 October 2008"
*/
/*
2.0.88
#define MG_VERSION_CREATED      "3 November 2008"
*/
/*
2.0.89
#define MG_VERSION_CREATED      "19 November 2008"
*/
/*
2.0.90
#define MG_VERSION_CREATED      "25 November 2008"
*/
/*
2.0.91
#define MG_VERSION_CREATED      "19 December 2008"
*/
/*
2.0.92
#define MG_VERSION_CREATED      "12 February 2009"
*/
/*
2.0.93
#define MG_VERSION_CREATED      "17 February 2009"
*/
/*
2.0.94
#define MG_VERSION_CREATED      "19 February 2009"
*/
/*
2.0.95
#define MG_VERSION_CREATED      "1 July 2009"
*/
/*
2.0.96
#define MG_VERSION_CREATED      "20 February 2010"
*/
/*
2.1.97
#define MG_VERSION_CREATED      "12 August 2013"
*/
/*
2.1.98
#define MG_VERSION_CREATED      "19 August 2013"
*/
/*
2.2.99
#define MG_VERSION_CREATED      "7 August 2014"
*/
/*
2.2.100
#define MG_VERSION_CREATED      "18 September 2014"
*/

/*
3.1.102
#define MG_VERSION_CREATED      "11 February 2020"
*/

#define MG_VERSION_CREATED      "27 May 2023"


#if defined(_WIN32)              /* Windows */

#if defined(_MSC_VER)
#if (_MSC_VER >= 1400)
#define _CRT_SECURE_NO_DEPRECATE    1
#define _CRT_NONSTDC_NO_DEPRECATE   1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#endif
#endif

/* Bind to Windows crypt32 library */

#define MG_CRYPT32              1
#ifndef MG_CRYPT32_STATIC_LINK
#define MG_CRYPT32_USE_DSO
#endif

#elif defined(__linux__) || defined(__linux) || defined(linux)

#if !defined(LINUX)
#define LINUX                       1
#endif

#elif defined(__APPLE__)

#if !defined(MACOSX)
#define MACOSX                      1
#endif

#elif defined(SOLARIS) || defined(__sun) || defined(__SVR4)

#if !defined(SOLARIS)
#define SOLARIS                     1
#endif

#include <sys/filio.h>
#ifndef __GNUC__
#define  __attribute__(x)
#endif

#elif defined(__hpux)

#if !defined(HPUX)
#define HPUX                     1
#endif

#define MG_BS_GEN_PTR           1
#ifndef _HPUX_SOURCE
#define _HPUX_SOURCE             1
#endif

#elif defined(_AIX) || defined(_AIX5)

#if !defined(AIX)
#define AIX                      1
#endif

#elif defined(__FreeBSD__) || defined(__FreeBSD)

#if !defined(FREEBSD)
#define FREEBSD                  1
#endif

#endif


/* Bind to SSL libraries if available */
#if defined(__has_include)
#if __has_include(<openssl/sslxx.h>)
#define MG_SSL                  1
#endif
#endif

#if defined(MG_SSL)
#define MG_SSL_SERVER
#ifndef MG_SSL_STATIC_LINK
#define MG_SSL_USE_DSO          1
#endif
#endif

/* IBM MQ support */
#define MG_MQ_USE_DSO           1

/* Experimental BDB support */
/*
#define MG_BDB                  1
*/

/* IPv6 support */
#define MG_IPV6                  1

#endif /* #ifndef MGSISYS_H */

