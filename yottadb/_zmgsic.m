%zmgsic ;(CM) Service Integration - Client
 ;
 ;  ----------------------------------------------------------------------------
 ;  | %zmgsic                                                                  |
 ;  | Author: Chris Munt cmunt@mgateway.com, chris.e.munt@gmail.com            |
 ;  | Copyright (c) 2016-2022 M/Gateway Developments Ltd,                      |
 ;  | Surrey UK.                                                               |
 ;  | All rights reserved.                                                     |
 ;  |                                                                          |
 ;  | http://www.mgateway.com                                                  |
 ;  |                                                                          |
 ;  | Licensed under the Apache License, Version 2.0 (the "License");          |
 ;  | you may not use this file except in compliance with the License.         |
 ;  | You may obtain a copy of the License at                                  |
 ;  |                                                                          |
 ;  | http://www.apache.org/licenses/LICENSE-2.0                               |
 ;  |                                                                          |
 ;  | Unless required by applicable law or agreed to in writing, software      |
 ;  | distributed under the License is distributed on an "AS IS" BASIS,        |
 ;  | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
 ;  | See the License for the specific language governing permissions and      |
 ;  | limitations under the License.                                           |
 ;  ----------------------------------------------------------------------------
 ;
 ;  DBX_DSORT_INVALID      0
 ;  DBX_DSORT_DATA         1
 ;  DBX_DSORT_SUBSCRIPT    2
 ;  DBX_DSORT_GLOBAL       3
 ;  DBX_DSORT_EOD          9
 ;  DBX_DSORT_STATUS       10
 ;  DBX_DSORT_ERROR        11
 ;
 ;  DBX_DTYPE_NONE         0
 ;  DBX_DTYPE_STR          1
 ;  DBX_DTYPE_STR8         2
 ;  DBX_DTYPE_STR16        3
 ;  DBX_DTYPE_INT          4
 ;  DBX_DTYPE_INT64        5
 ;  DBX_DTYPE_DOUBLE       6
 ;  DBX_DTYPE_OREF         7
 ;  DBX_DTYPE_NULL         10
 ;
 ;  DBX_CMND_OPEN          1
 ;  DBX_CMND_CLOSE         2
 ;  DBX_CMND_NSGET         3
 ;  DBX_CMND_NSSET         4
 ;
 ;  DBX_CMND_GSET          11
 ;  DBX_CMND_GGET          12
 ;  DBX_CMND_GNEXT         13
 ;  DBX_CMND_GPREVIOUS     14
 ;  DBX_CMND_GDELETE       15
 ;  DBX_CMND_GDEFINED      16
 ;  DBX_CMND_GINCREMENT    17
 ;  DBX_CMND_GLOCK         18
 ;  DBX_CMND_GUNLOCK       19
 ;  DBX_CMND_GMERGE        20
 ;
 ;  DBX_CMND_FUNCTION      31
 ;
 ;  DBX_CMND_CCMETH        41
 ;  DBX_CMND_CGETP         42
 ;  DBX_CMND_CSETP         43
 ;  DBX_CMND_CMETH         44
 ;  DBX_CMND_CCLOSE        45
 ;
 ;  DBX_CMND_TSTART        61
 ;  DBX_CMND_TLEVEL        62
 ;  DBX_CMND_TCOMMIT       63
 ;  DBX_CMND_TROLLBACK     64
 ;
 ;  DBX_INPUT_BUFFER_SIZE  32768
 ;
a0 d vers q
 ;
 ; v1.0.1:    1 December  2022
 ;
v() ; version and date
 n v,r,d
 s v="1.0"
 s r=1
 s d="1 December 2022"
 q v_"."_r_"."_d
 ;
vers ; version information
 n v
 s v=$$v()
 w !,"M/Gateway Developments Ltd - Service Integration Gateway Client"
 w !,"Version: "_$p(v,".",1,2)_"; Revision "_$p(v,".",3)_" ("_$p(v,".",4)_")"
 w !
 q
 ;
version()
 q $p($$v(),".",1,3)
 ;
open(fd,host,port,nspace,timeout,system,user,pwd)
 n buffer,result
 new $ztrap set $ztrap="zgoto "_$zlevel_":opene^%zmgsic"
 k fd
 s fd("host")=$s($d(host):$g(host),1:"localhost")
 s fd("port")=$s($d(port):$g(port),1:7041)
 s fd("timeout")=$s($d(timeout):$g(timeout),1:10)
 s fd("nspace")=$s($d(nspace):$g(nspace),1:"")
 s fd("system")=$s($d(system):$g(system),1:"")
 s fd("user")=$s($d(user):$g(user),1:"")
 s fd("pwd")=$s($d(pwd):$g(pwd),1:"")
 i fd("timeout")<3 s fd("timeout")=3
 s fd("dev")="client"_":"_$j
 open fd("dev"):(connect=fd("host")_":"_fd("port")_":TCP"):fd("timeout"):"SOCKET" e  k fd q 0
 s buffer="dbx1~"_fd("nspace")_$c(10)
 s result=$$req(.fd,.buffer)
 q 1
opene ; possibly a network error
 s fd("error")="network error"
 q 0
 ;
close(fd)
 new $ztrap set $ztrap="zgoto "_$zlevel_":closee^%zmgsic"
 i $g(fd("dev"))="" k fd q 0
 c fd("dev")
 k fd
 q 1
closee ; possibly a network error
 q 0
 ;
namespace(fd,nspace)
 n offset,buffer,result
 s offset=$$binit(.buffer)
 s offset=$$adddata(.buffer,offset,$g(nspace),1,1)
 s offset=$$bterm(.buffer,offset,11)
 s result=$$req(.fd,.buffer)
 q result
 ;
set(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,11)
 s result=$$req(.fd,.buffer)
 q result
 ;
get(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,12)
 s result=$$req(.fd,.buffer)
 q result
 ;
order(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,13)
 s result=$$req(.fd,.buffer)
 q result
 
previous(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,14)
 s result=$$req(.fd,.buffer)
 q result
 ;
kill(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,15)
 s result=$$req(.fd,.buffer)
 q result
 ;
data(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,16)
 s result=$$req(.fd,.buffer)
 q result
 ;
increment(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,17)
 s result=$$req(.fd,.buffer)
 q result
 ;
lock(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,18)
 s result=$$req(.fd,.buffer)
 q result
 ;
unlock(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,19)
 s result=$$req(.fd,.buffer)
 q result
 ;
merge(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,20)
 s result=$$req(.fd,.buffer)
 q result
 ;
function(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,31)
 s result=$$req(.fd,.buffer)
 q result
 ;
classmethod(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,41)
 s result=$$req(.fd,.buffer)
 q result
 ;
getproperty(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,42)
 s result=$$req(.fd,.buffer)
 q result
 ;
setproperty(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,43)
 s result=$$req(.fd,.buffer)
 q result
 ;
method(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,44)
 s result=$$req(.fd,.buffer)
 q result
 ;
closeobject(fd,d0,d1,d2,d3,d4,d5,d6,d7,d8,d9)
 n n,d,offset,buffer,result
 s offset=$$binit(.buffer)
 f n=0:1 s d="d"_n q:'$d(@d)  s offset=$$adddata(.buffer,offset,@d,1,1)
 s offset=$$bterm(.buffer,offset,45)
 s result=$$req(.fd,.buffer)
 q result
 ;
addsize(buffer,offset,size,sort,type)
 s $e(buffer,offset,(offset+3))=$$esize256(size)
 s $e(buffer,(offset+4))=$c(((sort*20)+type))
 q (offset+5)
 ;
adddata(buffer,offset,data,sort,type)
 n len
 s len=$l(data)
 s offset=$$addsize(.buffer,offset,len,sort,type)
 s $e(buffer,offset)=data
 q (offset+len)
 ;
addhead(buffer,offset,size,cmnd)
 s $e(buffer,offset,(offset+3))=$$esize256(size)
 s $e(buffer,(offset+4))=$c(cmnd)
 q (offset+5)
 ;
binit(buffer)
 n offset,index
 s offset=1
 s index=0
 s buffer=""
 s offset=$$addsize(.buffer,offset,0,1,4)
 s offset=$$addsize(.buffer,offset,$$getmsl(),1,4)
 s offset=$$addsize(.buffer,offset,index,1,4)
 q offset
 ;
bterm(buffer,offset,cmnd)
 s offset=$$adddata(.buffer,offset,"",9,1)
 s offset=$$addhead(.buffer,1,(offset-1),cmnd)
 q offset
 ;
req(fd,buffer)
 n io,head,x,ok,len,sort,type
 new $ztrap set $ztrap="zgoto "_$zlevel_":reqe^%zmgsic"
 s io=$i
 u fd("dev") w buffer
 s head="",result="",ok=0
 f  r x#5:fd("timeout") q:'$t  s head=head_x i $l(head)=5 s ok=1 q
 i 'ok u io q ""
 s len=$$dsize256(head)
 s sort=$a(head,5)
 s type=sort#20
 s sort=sort\20
 i len>0 s ok=0 f  r x#len:fd("timeout") q:'$t  s result=result_x i $l(result)=len s ok=1 q
 u io
 i sort=11 s fd("error")=result,result=""
 q result
reqe ; possibly a network error
 u io
 s fd("error")="network error"
 q ""
 ;
esize256(dsize) ; create little-endian 32-bit unsigned integer from M decimal
 q $c(dsize#256)_$c(((dsize\256)#256))_$c(((dsize\(256**2))#256))_$c(((dsize\(256**3))#256))
 ;
dsize256(esize) ; convert little-endian 32-bit unsigned integer to M decimal
 q ($a(esize,4)*(256**3))+($a(esize,3)*(256**2))+($a(esize,2)*256)+$a(esize,1)
 ;
isydb() ; See if this is YottaDB
 n zv
 s zv=$$getzv()
 i zv["YottaDB" q 1
 q 0
 ;
isgtm() ; See if this is GT.M
 n zv
 s zv=$$getzv()
 i zv["GT.M" q 1
 q 0
 ;
isidb() ; see if this is an intersystems database
 i $zv["ISM" q 1
 i $zv["Cache" q 2
 i $zv["IRIS" q 3
 q 0
 ;
ismsm() ; see if this is msm
 i $zv["MSM" q 1
 q 0
 ;
isdsm() ; see if this is dsm
 i $zv["DSM" q 1
 q 0
 ;
ism21() ; see if this is m21
 i $zv["M21" q 1
 q 0
 ;
getzv() ; Get $ZV
 ; ISC IRIS:  IRIS for Windows (x86-64) 2019.2 (Build 107U) Wed Jun 5 2019 17:05:10 EDT
 ; ISC Cache: Cache for Windows (x86-64) 2019.1 (Build 192) Sun Nov 18 2018 23:37:14 EST
 ; GT.M:      GT.M V6.3-004 Linux x86_64
 ; YottaDB:   YottaDB r1.22 Linux x86_64
 new $ztrap set $ztrap="zgoto "_$zlevel_":getzve^%zmgsic"
 q $zyrelease
getzve ; Error
 q $zv
 ;
getzvv() ; Get version from $ZV
 n zv,i,ii,x,version
 s zv=$$getzv()
 i $$isidb() d  q version
 . f i=1:1 s x=$p(zv," ",i) q:x=""  i x["(Build" s version=$p(zv," ",i-1) q
 . q
 s x=$$isydb()
 i x=1 s version=$p($p(zv," V",2)," ",1) q version
 i x=2 s version=$p($p(zv," r",2)," ",1) q version
 s version="" f i=1:1 s x=$e(zv,i) q:x=""  i x?1n d  q
 . f ii=i:1 s x=$e(zv,ii)  q:x=""!('((x?1n)!(x=".")))  s version=version_x
 . q
 q version
 ;
getsys() ; Get system type
 n systype
 s systype=$s($$isidb()>2:"IRIS",$$isidb()=2:"Cache",$$isidb()=1:"ISM",$$ism21():"M21",$$ismsm():"MSM",$$isdsm():"DSM",$$isydb()>1:"YottaDB",$$isydb()=1:"GTM",1:"")
 q systype
 ;
getmsl() ; Get maximum string length
 n max
 new $ztrap set $ztrap="zgoto "_$zlevel_":getmsle^%zmgsic"
 q 1048576
 i $$isydb() q 1048576
 i $$ism21() q 1023
 i $$ismsm() q 250
 i $$isdsm() q 250
 q $$getmslx()
getmsle ; error
 q $$getmslx()
 ;
getmslx() ; Get maximum string length the hard way
 n x,max
 new $ztrap set $ztrap="zgoto "_$zlevel_":getmslxe^%zmgsic"
 s x="" f max=1:1 s x=x_"a"
getmslxe ; max string error
 q (max-1)
 ;
 ;
test ; A simple test - reads and writes global ^CM
 n n,x,customer
 ; edit this line to connect to your target server
 s x=$$open(.fd,"localhost",7041,"USER")
 w !,"open: ",x
 w !,"set records ..."
 s x=$$set(.fd,"^CM",1,"Chris Munt")
 s x=$$set(.fd,"^CM",2,"Rob Tweed")
 s x=$$set(.fd,"^CM",3,"Jane Howard")
 s x=$$get(.fd,"^CM",1)
 w !,"get(1): ",x
 s x=$$data(.fd,"^CM",2)
 w !,"data(2): ",x
 s x=$$kill(.fd,"^CM",2)
 w !,"kill(2): ",x
 s x=$$data(.fd,"^CM",2)
 w !,"data(2): ",x
 w !,"parse order ..."
 s x="" f  s x=$$order(.fd,"^CM",x) q:x=""  w !,x," = ",$$get(.fd,"^CM",x)
 w !,"parse previous ..."
 s x="" f  s x=$$previous(.fd,"^CM",x) q:x=""  w !,x," = ",$$get(.fd,"^CM",x)
 w !,"increment (0.5) ..."
 s x="" f n=1:1:10 s x=$$increment(.fd,"^CM",0.5) w !,x
 s x=$$function(.fd,"fun^cmfun","abc")
 w !,"function: ",x
 s x=$$classmethod(.fd,"%Library.Date","DisplayToLogical","10/10/2019")
 w !,"classmethod: %Library.Date.DisplayToLogical(""10/10/2019""): ",x
 s customer=$$classmethod(.fd,"User.customer","%OpenId",1)
 w !,"cutomer(1) oref: ",customer
 s x=$$classmethod(.fd,"User.customer","MyClassMethod",4)
 w !,"MyClassMethod: ",x
 s x=$$method(.fd,customer,"MyMethod",5)
 w !,"MyMethod: ",x
 s x=$$getproperty(.fd,customer,"name")
 w !,"get name: ",x
 s x=$$setproperty(.fd,customer,"name","Rob Tweed")
 w !,"set name: ",x
 s x=$$getproperty(.fd,customer,"name")
 w !,"get new name: ",x
 s x=$$closeobject(.fd,customer)
 w !,"close object: ",x
 s x=$$close(.fd)
 q
 ;
