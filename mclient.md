# mclient

M/Gateway M client to the Service Integration Gateway (**SIG**)

Chris Munt <cmunt@mgateway.com>  
1 December 2022, M/Gateway Developments Ltd [http://www.mgateway.com](http://www.mgateway.com)

Contents

* [Overview](#overview") 
* [Prerequisites](#prereq")
* [Installing the SIG M Client](#install)
* [Connecting to the database](#connect)
* [Invocation of database commands](#dbcommands)
* [Invocation of database functions](#dbfunctions)
* [Transaction Processing](#tprocessing)
* [Direct access to InterSystems classes (IRIS and Cache)](#dbclasses)
* [License](#license)
* [Release Notes](#relnotes)

## <a name="overview">Overview</a>

The M/Gateway **SIG M Client (%zmgsic)** is an Open Source solution for **YottaDB** and InterSystems **Cache/IRIS**.  It provides remote access to data, functions and InterSystems classes.  The **SIG** (or superserver) must be running on the target remote system.  The remote system can be of the same type as the client or different.  In other words, **YottaDB** clients can access data and functionality on remote InterSystems **Cache/IRIS** systems and _vice versa_.

## <a name="prereq">Prerequisites</a>

InterSystems **Cache/IRIS** or **YottaDB** (or similar M DB Server):

       https://www.intersystems.com/
       https://yottadb.com/


## <a name="install">Installing the SIG M Client</a>

The **SIG M Client** consists of a single M routine (**%zmgsic**).

### InterSystems Cache/IRIS

Log in to the %SYS Namespace and install the **SIG M Client** routine held in **/isc/zmgsic\_isc.ro**.

       do $system.OBJ.Load("/isc/zmgsic_isc.ro","ck")

Change to your development Namespace and check the installation:

       do ^%zmgsic

       M/Gateway Developments Ltd - Service Integration Gateway Client
       Version: 1.0; Revision 1 (1 December 2022)

### YottaDB

The instructions given here assume a standard 'out of the box' installation of **YottaDB** deployed in the following location:

       /usr/local/lib/yottadb/r130

The primary default location for routines:

       /root/.yottadb/r1.30_x86_64/r

Copy the **SIG M Client** routine (_zmgsic.m) held in the GitHub **/yottadb** directory to:

       /root/.yottadb/r1.30_x86_64/r

Change directory to the following location and start a **YottaDB** command shell:

       cd /usr/local/lib/yottadb/r130
       ./ydb

Link the **zmgsic** routine and check the installation:

       zlink "_zmgsic.m"

       do ^%zmgsic

       M/Gateway Developments Ltd - Service Integration Gateway Client
       Version: 1.0; Revision 1 (1 December 2022)


Note that the version of **zmgsic** is successfully displayed.


## <a name="connect">Connecting to the database</a>

       set result=$$open^%zmgsic(.cx,<host>,<port>[,<nspace>,<timeout>])

Where:

* **cx** is the connection handle which must always be passed by reference.
* **host** is the network name of the target server.
* **port** is the TCP port on which the target **SIG** superserver is listening (usually 7041).
* **nspace** is the name of the target InterSystems namespace.
* **timeout** is the timeout (in seconds) to be applied to each **SIG M Client** function.

Example 1 (InterSystems Cache or IRIS):

       set result=$$open^%zmgsic(.cx,"localhost",7041,"USER")

Example 2 (YottaDB):

       set result=$$open^%zmgsic(.cx,"localhost",7041)


### Return the version of the SIG M Client

       set result=$$version^%zmgsic()


### Returning (and optionally changing) the current directory (or Namespace) on the remote server

       set current_namespace=$$namespace^%zmgsic(.cx[,<new_namespace>])

Example 1 (Get the current Namespace): 

       set nspace=$$namespace^%zmgsic(.cx)

* Note this will return the current Namespace for InterSystems databases and the value of the current global directory for YottaDB (i.e. $ZG).

Example 2 (Change the current Namespace): 

       set newnspace=$$namespace^%zmgsic(.cx,"SAMPLES")

* If the operation is successful this method will echo back the new Namespace name.  If not successful, the method will return the name of the current (unchanged) Namespace.


### Close database connection

       set result=$$close^%zmgsic(.cx) 


## <a name="dbcommands">Invocation of database commands</a>

### Set a record

       set result=$$set^%zmgsic(.cx,<global>,<key>,<data>)
      
Example:

       set result=$$set^%zmgsic(.cx,"person",1,"John Smith")


### Get a record

       set result=$$get^%zmgsic(.cx,<global>,<key>)
      
Example:

       set name=$$get^%zmgsic(.cx,"person",1)


### Delete a record

       set result=$$kill^%zmgsic(.cx,<global>,<key>)
      
Example:

       set result=$$kill^%zmgsic(.cx,"person",1)


### Check whether a record is defined

       set result=$$data^%zmgsic(.cx,<global>,<key>)
      
Example:

       set result=$$data^%zmgsic(.cx,"person",1)


### Parse a set of records (in order)

       set result=$$order^%zmgsic(.cx,<global>,<key>)
      
Example:

       set key = ""
       for  set key=$$order^%zmgsic(.cx,"person",key) quit:key=""  do
       . write !,"Person: ",key,' : ',$$get^%zmgsic(.cx,"person",key)
       . quit


### Parse a set of records (in reverse order)

       set result=$$previous^%zmgsic(.cx,<global>,<key>)
      
Example:

       set key = ""
       for  set key=$$previous^%zmgsic(.cx,"person",key) quit:key=""  do
       . write !,"Person: ",key,' : ',$$get^%zmgsic(.cx,"person",key)
       . quit


### Increment the value of a global node

       set result=$$increment^%zmgsic(.cx,<global>,<key>,<increment_value>)
      
Example (increment the value of the "counter" node by 1.5 and return the new value):

       set result=$$increment^%zmgsic(.cx,"person","counter",1.5)


### Lock a global node

       set result=$$lock^%zmgsic(.cx,<global>,<key>,<timeout>)
      
Example (lock global node '1' with a timeout of 30 seconds):

       set result=$$lock^%zmgsic(.cx,"person",1,30)

* Note: Specify the timeout value as '-1' for no timeout (i.e. wait until the global node becomes available to lock).


### Unlock a (previously locked) global node

       set result=$$unlock^%zmgsic(.cx,<global>,<key>)
      
Example (unlock global node '1'):

       set result=$$unlock^%zmgsic(.cx,"person",1)


### Merge (or copy) part of one global to another


Merge from global2 to global1:

       set result=$$merge^%zmgsic(.cx,<global1>[,<key1>],<global2>[,<key2>])
      
Example 1 (merge ^MyGlobal2 to ^MyGlobal1):

       set result=$$merge^%zmgsic(.cx,"^MyGlobal1","^MyGlobal2")

Example 2 (merge ^MyGlobal2(0) to ^MyGlobal1(1)):

       set result=$$merge^%zmgsic(.cx,"^MyGlobal1",1,"^MyGlobal2",0)


## <a name="dbfunctions">Invocation of database functions</a>

       set result=$$function^%zmgsic(.cx,<function>,<parameters>)
      
Example:

M routine called 'math':

       add(a, b) ; Add two numbers together
                 quit (a+b)

Client invocation:

      set result=$$function^%zmgsic(.cx,"add^math",2,3)


## <a name="tprocessing">Transaction Processing</a>

M DB Servers implement Transaction Processing by means of the functions described in this section.

### Start a Transaction

       set result=$$tstart^%zmgsic(.cx)


### Determine the Transaction Level

       set result=$$tlevel^%zmgsic(.cx)

* Transactions can be nested and this method will return the level of nesting.  If no Transaction is active this method will return zero.  Otherwise a positive integer will be returned to represent the current depth of Transaction nesting.


### Commit a Transaction

       set result=$$tcommit^%zmgsic(.cx)

* On successful completion this method will return zero, or an error code on failure.


### Rollback a Transaction

       set result=$$trollback^%zmgsic(.cx)

* On successful completion this method will return zero, or an error code on failure.


## <a name="dbclasses">Direct access to InterSystems classes (IRIS and Cache)</a>

### Invocation of a ClassMethod

       set result=$$classmethod^%zmgsic(.cx,<class_name>,<classmethod_name>,<parameters>)
      
Example (Encode a date to internal storage format):

       set result=$$classmethod^%zmgsic(.cx,"%Library.Date","DisplayToLogical","1/12/2022")


### Creating and manipulating instances of objects

The following simple class will be used to illustrate this facility.

       Class User.Person Extends %Persistent
       {
          Property Number As %Integer;
          Property Name As %String;
          Property DateOfBirth As %Date;
          Method Age(AtDate As %Integer) As %Integer
          {
             Quit (AtDate - ..DateOfBirth) \ 365.25
          }
       }


### Create an entry for a new Person

       set person=$$classmethod^%zmgsic(.cx,"User.Person","%New")

Add Data:

       set result=$$setproperty^%zmgsic(.cx,person,"Number",1)
       set result=$$setproperty^%zmgsic(.cx,person,"Name","John Smith")
       set result=$$setproperty^%zmgsic(.cx,person,"DateOfBirth","12/8/1995")

Save the object record:

       set result=$$method^%zmgsic(.cx,person,"%Save")


### Retrieve an entry for an existing Person

Retrieve data for object %Id of 1.
 
       set person=$$classmethod^%zmgsic(.cx,"User.Person","%OpenId",1)

Return properties:

       set number=$$getproperty^%zmgsic(.cx,person,"Number")
       set name=$$getproperty^%zmgsic(.cx,person,"Name")
       set dob=$$getproperty^%zmgsic(.cx,person,"DateOfBirth")

Calculate person's age at a particular date:

       set today=$$classmethod^%zmgsic(.cx,"%Library.Date","DisplayToLogical","1/12/2022")
       set age=$$method^%zmgsic(.cx,person,"Age",today)


## <a name="license">License</a>

Copyright (c) 2021-2022 M/Gateway Developments Ltd,
Surrey UK.                                                      
All rights reserved.
 
http://www.mgateway.com                                                  
Email: cmunt@mgateway.com
 
 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.      


## <a name="relnotes">Release Notes</a>

### v1.0.1 (1 December 2022)

* Initial Release
