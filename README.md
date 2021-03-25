# mgsi

M/Gateway Service Integration Gateway (**SIG**) for InterSystems **Cache/IRIS** and **YottaDB**.

Chris Munt <cmunt@mgateway.com>  
25 March 2021, M/Gateway Developments Ltd [http://www.mgateway.com](http://www.mgateway.com)

* Current Release: Version: 3.1; Revision 102a (21 December 2020).
* SuperServer (%zmgsi routines) Release: Version: 4.1; Revision 18 (25 March 2021).
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

The M/Gateway Service Integration Gateway (**SIG**) is an Open Source network-based service developed for InterSystems **Cache/IRIS** and the **YottaDB** Database Servers.  It will also work with the **GT.M** database and other **M-like** Databases Servers.  Its core function is to manage connectivity, process and resource pooling for **M-like** DB Servers.  The pooled resources can be used by any of the client-facing technologies in this product series (for example **mg\_php** and **mg\_go** etc ...).

There are two parts to a complete SIG installation:

* The **DB Superserver**.  This component resides inside the DB Server and is independently required by other M/Gateway Open Source products.
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

       M/Gateway Developments Ltd - Service Integration Gateway
       Version: 4.1; Revision 17 (23 March 2021)

### YottaDB

The instructions given here assume a standard 'out of the box' installation of **YottaDB** deployed in the following location:

       /usr/local/lib/yottadb/r130

The primary default location for routines:

       /root/.yottadb/r1.30_x86_64/r

Copy all the Superserver routines (i.e. all files with an 'm' extension) held in the GitHub **/yottadb** directory to:

       /root/.yottadb/r1.30_x86_64/r

Change directory to the following location and start a **YottaDB** command shell:

       cd /usr/local/lib/yottadb/r130
       ./ydb

Link all the **zmgsi** routines and check the installation:

       do ylink^%zmgsi

       do ^%zmgsi

       M/Gateway Developments Ltd - Service Integration Gateway
       Version: 4.1; Revision 17 (23 March 2021)


Note that the version of **zmgsi** is successfully displayed.


## <a name="StartSS"></a> Starting the DB Superserver

The default TCP server port for the DB Superserver (**zmgsi**) is **7041**.  If you wish to use an alternative port then modify the following instructions accordingly.

In YottaDB, the DB Superserver can either be started from the DB (i.e. M) command prompt or Superserver processes can be started by the **xinetd** daemon.

### Starting the DB Superserver from the DB command prompt

* For InterSystems DB servers the concurrent TCP service should be started in the **%SYS** Namespace.

Start the DB Superserver using the following command:

       do start^%zmgsi(0) 

To use a server TCP port other than 7041, specify it in the start-up command (as opposed to using zero to indicate the default port of 7041).


### Starting YottaDB Superserver processes via the xinetd daemon


Network connectivity to **YottaDB** is managed via the **xinetd** service.  First create the following launch script (called **zmgsi\_ydb** here):

       /usr/local/lib/yottadb/r130/zmgsi_ydb

Content:

       #!/bin/bash
       cd /usr/local/lib/yottadb/r130
       export ydb_dir=/root/.yottadb
       export ydb_dist=/usr/local/lib/yottadb/r130
       export ydb_routines="/root/.yottadb/r1.30_x86_64/o*(/root/.yottadb/r1.30_x86_64/r /root/.yottadb/r) /usr/local/lib/yottadb/r130/libyottadbutil.so"
       export ydb_gbldir="/root/.yottadb/r1.30_x86_64/g/yottadb.gld"
       $ydb_dist/ydb -r xinetd^%zmgsis

Note that you should, if necessary, modify the permissions on this file so that it is executable.  For example:

       chmod a=rx /usr/local/lib/yottadb/r130/zmgsi_ydb


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
            server          = /usr/local/lib/yottadb/r130/zmgsi_ydb
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

Copyright (c) 2018-2021 M/Gateway Developments Ltd,
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

