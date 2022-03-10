# %zmgmq

M/Gateway Message Queue Manager for **YottaDB** and InterSystems **Cache/IRIS**.

Chris Munt <cmunt@mgateway.com>  
7 March 2022, M/Gateway Developments Ltd [http://www.mgateway.com](http://www.mgateway.com)

* Current Release: Version: 1.0; Revision 1 (7 March 2022).
* [Release Notes](#RelNotes) can be found at the end of this document.

Contents

* [Overview](#Overview") 
* [Pre-requisites](#PreReq") 
* [Installing the Message Queue Manager](#InstallMQ)
* [Starting the Message Queue Manager](#StartMQ)
* [Using the Message Queue Manager](#UsingMQ)
* [Resources used by the Message Queue Manager](#Resources)
* [License](#License)

## <a name="Overview"></a> Overview

The M/Gateway **Message Queue Manager** is an Open Source solution for **YottaDB** and InterSystems **Cache/IRIS**.  The idea behind this module is a fixed set of worker processes running in M that are dedicated to performing specific tasks.  Clients (other M applications) can submit tasks to the Message Queue and either wait for a response to be sent back or simply submit a task to be completed by the **Message Queue Manager** asynchronously.

A likely use case is a set of worker processes that are dedicated to interfacing to an external system.  For example, an external database.  Rather than each M application connecting to, and disconnecting from, the remote resource the work can instead be performed, and the complexity of interfacing with the external resource, handled by the dedicated pool of worker processes.

## <a name="PreReq"></a> Pre-requisites

InterSystems **Cache/IRIS** or **YottaDB** (or similar M DB Server):

       https://www.intersystems.com/
       https://yottadb.com/


## <a name="InstallMG"></a> Installing the Message Queue Manager

The **Message Queue Manager** consists of a single M routine (**%zmgmq**).

### InterSystems Cache/IRIS

Log in to the %SYS Namespace and install the **Message Queue Manager** routine held in **/isc/zmgmq\_isc.ro**.

       do $system.OBJ.Load("/isc/zmgmq_isc.ro","ck")

Change to your development Namespace and check the installation:

       do ^%zmgmq

       M/Gateway Developments Ltd - Message Queue Manager
       Version: 1.0; Revision 1 (7 March 2022)

### YottaDB

The instructions given here assume a standard 'out of the box' installation of **YottaDB** deployed in the following location:

       /usr/local/lib/yottadb/r130

The primary default location for routines:

       /root/.yottadb/r1.30_x86_64/r

Copy the **Message Queue Manager** routine (_zmgmq.m) held in the GitHub **/yottadb** directory to:

       /root/.yottadb/r1.30_x86_64/r

Change directory to the following location and start a **YottaDB** command shell:

       cd /usr/local/lib/yottadb/r130
       ./ydb

Link the **zmgmq** routines and check the installation:

       zlink "_zmgmq.m"

       do ^%zmgmg

       M/Gateway Developments Ltd - Message Queue Manager
       Version: 1.0; Revision 1 (7 March 2022)


Note that the version of **zmgmq** is successfully displayed.


## <a name="StartMQ"></a> Starting the Message Queue Manager

Start the **Message Queue Manager** using the following command:

       do start^%zmgmq(<no_workers>, <configuration_name>) 

Specify the number of worker processes to start (**no\_workers**) and, optionally, supply a **configuration\_name**.  The configuration name will, for example, relate to the parameters required to connect to an external data source.

Example 1: A configuration defining an API-based connection to a Cache database.

The configuration name in this example is **iscapi**

       ^zmgmq("conf","iscapi","db","dbtype")="Cache"
       ^zmgmq("conf","iscapi","db","path")="/opt/cache20181/mgr"
       ^zmgmq("conf","iscapi","db","username")="_SYSTEM"
       ^zmgmq("conf","iscapi","db","password")="SYS"
       ^zmgmq("conf","iscapi","db","namespace")="USER"

Example 2: A configuration defining a network-based connection to a Cache database.

The configuration name in this example is **iscnet**

       ^zmgmq("conf","iscnet","db","dbtype")="Cache"
       ^zmgmq("conf","iscnet","db","host")="localhost"
       ^zmgmq("conf","iscnet","db","port")=7041
       ^zmgmq("conf","iscnet","db","password")="SYS"
       ^zmgmq("conf","iscnet","db","username")="_SYSTEM"
       ^zmgmq("conf","iscnet","db","namespace")="USER"

Finally, to start a pool of 10 processes connected to the Cache API:

       do start^%zmgmq(10,"iscapi")

To reset the Message Queue (The **Message Queue Manager** must be closed down before running this procedure):

       do reset^%zmgmq

This procedure will clear the **^zmgmq** global except for the part holding configurations (^zmgmq("config")).

## <a name="UsingMQ"></a> Using the Message Queue Manager

### Ping the Message Queue Manager

       set result=$$mess^%zmgmq("p")

### Get the version of the Message Queue Manager

       set version=$$mess^%zmgmq("v")

### Close down all Message Queue Manager worker processes

       set version=$$mess^%zmgmq("s")

### Submit a function to be synchronously processed by the Message Queue Manager

       set result=$$fun^%zmgmq(<function_name>, <arguments ...>)

Example:  Submitting a function to get properties from a remote Cache class using **mg\_pwind**:

       Class User.customer Extends %Persistent
       {
          Property number As %Integer;
          Property name As %String;
       }

The M routine (^MyRoutine) in YottaDB:

       getcust(id) ; Function to get customer number and name
                   new rc,num,name
                   set rc=$&pwind.dbclassmethod(.oref,"User.customer","%OpenId",id)
                   set rc=$&pwind.dbgetproperty(.num,oref,"number")
                   set rc=$&pwind.dbgetproperty(.name,oref,"name")
                   quit (num_":"_name)

Submitting this function to the Message Queue Manager:

       set result=$$fun^%zmgmq("getcust^MyRoutine", 1)


### Submit a function to be asynchronously processed by the Message Queue Manager

       set tn=$$funa^%zmgmq(<function_name>, <arguments ...>)

* Where **'tn'** is the task number.
* This function will return immediately and the result, when available, will be held in the following global node
	* ^zmgmq("task",tn,"result")

Example:  Submitting a function to update properties in a remote Cache class using **mg\_pwind**

       Class User.customer Extends %Persistent
       {
          Property number As %Integer;
          Property name As %String;
       }

The M routine (^MyRoutine) in YottaDB:

       updcust(num,name) ; Function to get customer number and name
                   new rc,oref,result
                   set rc=$&pwind.dbclassmethod(.oref,"User.customer","%New")
                   set rc=$&pwind.dbsetproperty(num,oref,"number")
                   set rc=$&pwind.dbsetproperty(name,oref,"name")
                   set rc=$&pwind.dbmethod(.result,oref,"%Save")
                   quit result

Submitting this function to the Message Queue Manager to be processed asynchronously

       set tn=$$funa^%zmgmq("updcust^MyRoutine", 2, "Chris Munt")


## <a name="Resources"></a> Resources used by the DB Superserver

The **zmgmq** server-side code will write to the following global:

* **^zmgmq**


## <a name="License"></a> License

Copyright (c) 2018-2022 M/Gateway Developments Ltd,
Surrey UK.                                                      
All rights reserved.
 
http://www.mgateway.com                                                  
Email: cmunt@mgateway.com
 
 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.      

## <a name="RelNotes"></a>Release Notes

### v1.0.1 (7 March 2022)

* Initial Release

