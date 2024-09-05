# mgsi

MGateway Service Integration Gateway (**SIG**) for InterSystems **Cache/IRIS** and **YottaDB**.

Chris Munt <cmunt@mgateway.com>  
5 September 2024, MGateway Ltd [http://www.mgateway.com](http://www.mgateway.com)

* Current Release: Version: 3.1; Revision 103 (27 May 2023).
* SuperServer (%zmgsi routines) Release: Version: 4.5; Revision 34 (5 September 2024).
* [Release Notes](#RelNotes) can be found at the end of this document.

Contents

* [Overview](#Overview") 
* [Pre-requisites](#PreReq") 
* [Installing the DB Superserver](#InstallSS)
* [Starting the DB Superserver](#StartSS)
* [Installing the SIG](#InstallSIG)
* [Starting the SIG](#StartSIG)
* [Using the SIG](#UsingSIG)
* [Resources used by the DB Superserver](#Resources)
* [License](#License)

## <a name="Overview"></a> Overview

The MGateway Service Integration Gateway (**SIG**) is an Open Source network-based service developed for InterSystems **Cache/IRIS** and the **YottaDB** Database Servers.  It will also work with the **GT.M** database and other **M-like** Databases Servers.  Its core function is to manage connectivity, process and resource pooling for **M-like** DB Servers.  The pooled resources can be used by any of the client-facing technologies in this product series (for example **mg\_php** and **mg\_go** etc ...).

An M-based client to the **SIG** is included (**%zmgsic**).  This module provides easy access to global data, functions and InterSystems classes held on remote systems.  Documentation for this component can be found [here](./mclient.md).  The client and server do not need to be of the same type.  You can use **%zmgsic** on InterSystems **Cache/IRIS** to access data on **YottaDB** and _vice versa_. 

Also included in this package is a message/work queue manager for **YottaDB** and InterSystems **Cache/IRIS**.  This is implemented as a stand-alone module written in M code, documentation for which can be found [here](./workqueue.md).

There are two parts to a complete SIG installation:

* The **DB Superserver**.  This component resides inside the DB Server and is independently required by other MGateway Open Source products.
* The **SIG** itself.  This is effectively a client to the DB Superserver.


## <a name="PreReq"></a> Pre-requisites

InterSystems **Cache/IRIS** or **YottaDB** (or similar M DB Server):

       https://www.intersystems.com/
       https://yottadb.com/


## <a name="InstallSS"></a> Installing the DB Superserver

The DB Superserver consists of two M routines (**%zmgsi** and **%zmgsis**).

### InterSystems Cache/IRIS

Log in to the %SYS Namespace and install the Superserver **zmgsi** routines held in **/isc/zmgsi\_isc.ro**.

       do $system.OBJ.Load("/isc/zmgsi_isc.ro","ck")

Change to your development Namespace and check the installation:

       do ^%zmgsi

       MGateway Ltd - Service Integration Gateway
       Version: 4.5; Revision 32 (3 June 2024)

### YottaDB

The instructions given here assume a standard 'out of the box' installation of **YottaDB** (v3.8) deployed in the following location:

       /usr/local/lib/yottadb/r138

The primary default location for routines:

       /root/.yottadb/r1.38_x86_64/r

Copy all the Superserver routines (i.e. all files with an 'm' extension) held in the GitHub **/yottadb** directory to:

       /root/.yottadb/r1.38_x86_64/r

Change directory to the following location and start a **YottaDB** command shell:

       cd /usr/local/lib/yottadb/r138
       ./ydb

Link all the **zmgsi** routines and check the installation:

       do ylink^%zmgsi

       do ^%zmgsi

       MGateway Ltd - Service Integration Gateway
       Version: 4.5; Revision 32 (3 June 2024)


Note that the version of **zmgsi** is successfully displayed.


## <a name="StartSS"></a> Starting the DB Superserver

The default TCP server port for the DB Superserver (**zmgsi**) is **7041**.  If you wish to use an alternative port then modify the following instructions accordingly.

In YottaDB, the DB Superserver can either be started from the DB (i.e. M) command prompt or Superserver processes can be started by the **xinetd** daemon.

### Starting the DB Superserver from the DB command prompt

* For InterSystems DB servers the concurrent TCP service should be started in the **%SYS** Namespace.

Start the DB Superserver using the following command:

       do start^%zmgsi(0) 

To use a server TCP port other than 7041, specify it in the start-up command (as opposed to using zero to indicate the default port of 7041).

#### Using InterSystems TLS configurations

DB Superserver version 4.4 (and later) can accept secured connections from clients over TLS.  This facility is only available with InterSystems DB Servers.  To use an InterSystems TLS **Server** configuration, specify this configuration name as the second argument to the DB Superserver start function.

       Do start^%zmgsi(0,"MyInterSystemsTLSServerConfiguration")

### Starting YottaDB Superserver processes via the xinetd daemon


Network connectivity to **YottaDB** is managed via the **xinetd** service.  First create the following launch script (called **zmgsi\_ydb** here):

       /usr/local/lib/yottadb/r138/zmgsi_ydb

Content:

       #!/bin/bash
       cd /usr/local/lib/yottadb/r138
       export ydb_dir=/root/.yottadb
       export ydb_dist=/usr/local/lib/yottadb/r138
       export ydb_routines="/root/.yottadb/r1.38_x86_64/o*(/root/.yottadb/r1.38_x86_64/r /root/.yottadb/r) /usr/local/lib/yottadb/r138/libyottadbutil.so"
       export ydb_gbldir="/root/.yottadb/r1.38_x86_64/g/yottadb.gld"
       $ydb_dist/ydb -r xinetd^%zmgsis

Note that you should, if necessary, modify the permissions on this file so that it is executable.  For example:

       chmod a=rx /usr/local/lib/yottadb/r138/zmgsi_ydb


Create the **xinetd** script (called **zmgsi\_xinetd** here): 

       /etc/xinetd.d/zmgsi_xinetd

Content:

       service zmgsi_xinetd
       {
            disable         = no
            type            = UNLISTED
            port            = 7041
            socket_type     = stream
            wait            = no
            user            = root
            server          = /usr/local/lib/yottadb/r138/zmgsi_ydb
       }

* Note: sample copies of **zmgsi\_xinetd** and **zmgsi\_ydb** are included in the **/unix** directory.

Edit the services file:

       /etc/services

Add the following line to this file:

       zmgsi_xinetd          7041/tcp                        # zmgsi

Finally restart the **xinetd** service:

       /etc/init.d/xinetd restart


## <a name="InstallSIG"></a> Installing the SIG

There are three parts to a complete **SIG** installation and configuration.

* The **SIG** executable (a UNIX Daemon or Windows Service) (**mgsi** or **mgsi.exe**).
* The **DB Superserver** described previously: the **%zmgsi** routines.
* A network configuration to bind the former two elements together.

### Building the SIG executable

The **SIG** (**mgsi** or **mgsi.exe**) is written in standard C.  The GNU C compiler (gcc) can be used for Linux systems:

Ubuntu:

       apt-get install gcc

Red Hat and CentOS:

       yum install gcc

Apple OS X can use the freely available **Xcode** development environment.

Windows can use the free "Microsoft Visual Studio Community" edition of Visual Studio for building the **SIG**:

* Microsoft Visual Studio Community: [https://www.visualstudio.com/vs/community/](https://www.visualstudio.com/vs/community/)

There are built Windows x64 binaries available from:

* [https://github.com/chrisemunt/mgsi/blob/master/bin/winx64](https://github.com/chrisemunt/mgsi/blob/master/bin/winx64)

Having created a suitable development environment, **Makefiles** are provided to build the **SIG** for UNIX and Windows.

#### UNIX

Invoke the build procedure from the /src directory (i.e. the directory containing the **Makefile** file).

       make

#### Windows

Invoke the build procedure from the /src directory (i.e. the directory containing the **Makefile.win** file).

       nmake /f Makefile.win


## <a name="StartSIG"></a> Starting the SIG

The **SIG** executable can be installed in a directory of your choice.  When started, it will create a configuration file called **mgsi.ini**.  The event log file will be called **mgsi.log**.

### UNIX

Starting the **SIG**:

       ./mgsi

Stopping the **SIG**:

       ./mgsi -stop

### Windows

Starting the **SIG**:

       mgsi -start

Stopping the **SIG**:

       mgsi -stop

When the **SIG** is started for the first time it will register itself as a Windows Service.  Thereafter it can be managed from the Windows Services Control Panel if desired.


## <a name="UsingSIG"></a> Using the SIG

When the **SIG** is up and running its services are immediately available to participating clients.  The **SIG** provides a web-based user interface for the purpose of maintaining the configuration and service management.  By default the **SIG** listens on TCP port 7040.  The web-based management suite may be accessed as follows.

       http://[server]:7040/mgsi/mgsisys.mgw


## <a name="Resources"></a> Resources used by the DB Superserver

The **zmgsi** server-side code will write to the following global:

* **^zmgsi**: The event Log. 


## <a name="License"></a> License

Copyright (c) 2018-2024 MGateway Ltd,
Surrey UK.                                                      
All rights reserved.
 
http://www.mgateway.com                                                  
Email: cmunt@mgateway.com
 
 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.      

## <a name="RelNotes"></a>Release Notes

### v3.1.102 (13 March 2020)

* Initial Release

### v3.1.102a (21 December 2020)

* Updates to the documentation.

### v3.1.102a; Superserver v4.0.16 (11 February 2021)

* Introduce a native concurrent TCP server for YottaDB.
	* The Superserver can be started from the M command prompt using **d start^%zmgsi(<tcp port>)**.
	* Invocation of Superserver processes from the xinetd daemon is still supported. 
* Introduce commands to support M Transaction Processing.
* Review the Superserver code base and remove unnecessary/defunct code.

### v3.1.102a; Superserver v4.1.17 (23 March 2021)

* Improve the performance of network connectivity for YottaDB.

### v3.1.102a; Superserver v4.1.18 (25 March 2021)

* Correct a fault in the $$nvpair^%zmgsis() web helper function. It wasn't coping with unescaped '=' characters in values.

### v3.1.102a; Superserver v4.2.19 (5 April 2021)

* Introduce improved support for InterSystems Objects for the standard (PHP/Python/Ruby) connectivity protocol.

### v3.1.102a; Superserver v4.2.20 (20 April 2021)

* Add functionality to parse multipart MIME content for **mg\_web**.

### v3.1.102a; Superserver v4.2.21 (23 April 2021)

* Related to Superserver v4.2.20: move multipart section headers into a separate array.

### v3.1.102a; Superserver v4.3.22 (18 June 2021)

* Create the infrastructure to allow **mg\_web** to handle request payloads that exceed the maximum string length of the target DB Server.

### v3.1.102a; Superserver v4.4.23 (18 August 2021)

* Introduce support for TLS secured connectivity for InterSystems DB Servers.
	* Do start^%zmgsi([port],[name of InterSystems TLS server configuration])
* Correct an occasional 'undefined %payload' error in the content^%zmgsis() function.
* Remove the 'incoming connection ...' event log message unless the global node ^%zmgsi("loglevel") is set to a value greater than 1.
* Introduce support for native Unicode (UTF16) for InterSystems DB Servers.

### v3.1.102a; Superserver v4.4.24 (20 August 2021)

* Correct a regression introduced in v4.4.23 that led to %zmgsis processes spinning for **mg\_web** applications.

### v3.1.102a; Superserver v4.4.25 (2 September 2021)

* Reinstate support for native Unicode (UTF16) for InterSystems DB Servers - this enhancement is mainly for **mg\-dbx**.

### v3.1.102a; Superserver v4.5.26 (2 December 2021)

* Add the functionality to support clients that implement an 'idle timeout' facility to close down inactive connections after a specified time.
* Introduce an options mask to allow the type of requests serviced by the DB Superserver to be restricted.
	* Request types include: **http**, **https**, **globals**, **functions**, and **classes**
	* The allowed request types are listed as the third parameter in the start-up function: e.g. Do start^%zmgsi(0,"","http,functions").
	* The default behaviour is for all request types to be accepted by the DB Superserver.
* Suppress the recording of 'uci error: ...' messages in the event log unless the log level is set to 1 (or higher).

### v3.1.102a; Superserver v4.5.27 (1 March 2022)

* Add the network commands for global node lock and unlock (used by mg_pwind and mg-dbx).
* Add the network command for a 'close object reference' operation (used by mg_pwind and mg-dbx).

### v3.1.102a; Superserver v4.5.28 (3 February 2023)

* Allow the initial worker initialization message for the DBX protocol to be resent to an open/active connection.  This change was made primarily to allow the M client to seamlessly reconnect to an existing (open) connection to the Superserver.

### v3.1.103; Superserver v4.5.28 (27 May 2023)

* Documentation update.

### v3.1.103; Superserver v4.5.29 (7 November 2023)

* Correct a fault affecting the return of Unicode data to Node.js through 
* SQL.

### v3.1.103; Superserver v4.5.30 (10 November 2023)

* Correct a fault in the operation to get previous global node with data.

### v3.1.103; Superserver v4.5.31 (18 November 2023)

* Avoid occasional **mgsql** failures in YottaDB by always reloading the generated code before execution.

### v3.1.103; Superserver v4.5.32 (3 June 2024)

* Introduce support for Server-Sent Events (SSE) in **mg\_web**.

### v3.1.103; Superserver v4.5.33 (9 July 2024)

* Introduce **mg\_web** support for multipart payloads exceeding the DB Server maximum string length.

### v3.1.103; Superserver v4.5.34 (5 September 2024)

* Introduce **mg\_web** support for payloads of type 'application/x-www-form-urlencoded' that exceed the DB Server maximum string length.
