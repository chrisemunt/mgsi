%zmgsis ;(CM) Service Integration - Child Process
 ;
 ;  ----------------------------------------------------------------------------
 ;  | %zmgsis                                                                  |
 ;  | Author: Chris Munt cmunt@mgateway.com, chris.e.munt@gmail.com            |
 ;  | Copyright (c) 2016-2025 MGateway Ltd                                     |
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
a0 d vers q
 ;
 ; v2.0.6:   17 February  2009
 ; v2.0.7:    1 July      2009
 ; v2.0.8:   15 July      2009
 ; v2.0.9:   19 October   2009
 ; v2.1.10:  12 August    2013
 ; v2.2.11:   9 December  2013
 ; v2.3.12:   6 September 2018 (Add Intersystems IRIS)
 ; v2.3.13:   3 February  2019 (Add all-out to xdbc)
 ; v2.3.14:  18 March     2019 (Release as Open Source, Apache v2 license)
 ; v3.0.1:   13 June      2019 (Renamed to %zmgsi and %zmgsis)
 ; v3.1.2:   10 September 2019 (Add protocol upgrade for mg_dba library - Go)
 ; v3.2.3:    1 November  2019 (Add SQL interface)
 ; v3.2.4:    8 January   2020 (Add support for $increment to old protocol)
 ; v3.2.5:   17 January   2020 (Finalise the ifc interface)
 ; v3.2.6:    3 February  2020 (Send version of DB back to the client)
 ; v3.2.7:    5 May       2020 (Add the 'Merge' command to the YottaDB API)
 ; v3.3.8:   25 May       2020 (Add protocol upgrade for mg-dbx - TCP based connectivity from Node.js)
 ; v3.3.9:   17 June      2020 (Add web protocol for mg_web. Support for nested ISC Object References)
 ; v3.3.10:   7 July      2020 (Improve web protocol for mg_web)
 ; v3.3.11:   3 August    2020 (Fix the stop^%zmgsi() facility. Fix a UNIX connectivity issue)
 ; v3.4.12:  10 August    2020 (Introduce streamed write for mg_web; export the data-types for the SQL interface)
 ; v3.5.13:  29 August    2020 (Introduce ASCII streamed write for mg_web; Introduce websocket support; reset ISC namespace after each web request)
 ; v3.5.14:  24 September 2020 (Add a getdatetime() function)
 ; v3.6.15:   6 November  2020 (Add functionality to support load balancing and failover in mg_web)
 ; v4.0.16:  11 February  2021 (Implement native concurrent TCP server for YottaDB; Transaction Processing; Review code base and remove unnecessary/defunct code)
 ; v4.1.17:  23 March     2021 (Improve performance of network of YottaDB; Introduce support for the mg_pwind I/O subsystem)
 ; v4.1.18:  25 March     2021 (Correct a fault in the $$nvpair() web helper - it wasn't coping with unescaped '=' characters in values)
 ; v4.2.19:   5 April     2021 (Introduce improved support for InterSystems Objects for the standard (PHP/Python/Ruby) connectivity protocol)
 ; v4.2.20:  20 April     2021 (Add functionality to parse multipart MIME content for mg_web)
 ; v4.2.21:  23 April     2021 (Related to v4.2.20: move multipart section headers into a separate array)
 ; v4.3.22:  18 June      2021 (Create the infrastructure to allow mg_web to handle request payloads that exceed the maximum string length of the target DB Server)
 ; v4.4.23:  18 August    2021 (Add support for TLS secured connectivity for ISC DB Servers;
 ;                              Correct 'undefined' %payload in content();
 ;                              Remove 'incoming connection' ... log message;
 ;                              Add support for native Unicode (UTF16) for InterSystems DB Servers)
 ; v4.4.24:  20 August    2021 (Correct a regression introduced in v4.4.23 that led to %zmgsis processes spinning for mg_web applications)
 ; v4.4.25:   2 September 2021 (Reinstate support for native Unicode (UTF16) for InterSystems DB Servers - mainly for mg-dbx)
 ; v4.5.26:   2 December  2021 (Add an 'idle timeout' facility to close down inactive processes;
 ;                              Suppress the recording of 'uci error: ...' messages in the event log unless the log level is set to 1 (or higher);
 ;                              Introduce an options mask to allow the type of requests serviced by the DB Superserver to be restricted)
 ; v4.5.27:   1 March     2022 (Add the network commands for lock and unlock (16, 19) and also the 'close oref' operation (45))
 ; v4.5.28:   3 February  2023 (Allow the initial worker initialization message for the DBX protocol to be resent to an open/active connection)
 ; v4.5.29:   7 November  2023 (Correct a fault affecting the return of Unicode data to Node.js through SQL;
 ;                              Improve and optimise wire protocol for mg-dbx-napi)
 ; v4.5.30:  10 November  2023 (Correct a fault in the operation to get previous global node with data).
 ; v4.5.31:  18 November  2023 (Set flag to always recompile SQL queries for YottaDB).
 ; v4.5.32:   3 June      2024 (Introduce support for SSE in mg_web).
 ; v4.5.33:   9 July      2024 (Introduce mg_web support for multipart payloads exceeding the DB Server maximum string length).
 ; v4.5.34:   5 September 2024 (Introduce mg_web support for payloads of type 'application/x-www-form-urlencoded' that exceed the DB Server maximum string length).
 ; v4.5.35:   6 September 2024 (Ensure that superserver processes close down gracefully on encountering network errors).
 ; v4.5.36:   7 October   2024 (Add support for network invocation of the setchildnodes() and getchildnodes() methods for Node.js).
 ; v4.5.37:   9 March     2025 (Add a function to detect a 'client gone' message from mg_web SSE channels
 ;                              Function: $$clientgone^%zmgsis(.%sys) returns 1 if client has aborted).
 ;
v() ; version and date
 n v,r,d
 s v="4.5"
 s r=37
 s d="9 March 2025"
 q v_"."_r_"."_d
 ;
vers ; version information
 n v
 s v=$$v()
 w !,"MGateway Ltd - Service Integration Gateway"
 w !,"Version: "_$p(v,".",1,2)_"; Revision "_$p(v,".",3)_" ("_$p(v,".",4)_")"
 w !
 q
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
 new $ztrap set $ztrap="zgoto "_$zlevel_":getzve^%zmgsis"
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
 new $ztrap set $ztrap="zgoto "_$zlevel_":getmsle^%zmgsis"
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
 new $ztrap set $ztrap="zgoto "_$zlevel_":getmslxe^%zmgsis"
 s x="" f max=1:1 s x=x_"a"
getmslxe ; max string error
 q (max-1)
 ;
getdatetime(h) ; Get the date and time in text form
 n dt
 s dt=$zd(h)_" "_$$dtime($p(h,",",2),1)
 q dt
 ;
trim(x,chrs) ; trim leading/trailing spaces from text
 q $$ltrim($$rtrim(x,chrs),chrs)
 ;
ltrim(x,chrs) ; trim leading spaces from text
 i chrs="" s chrs=" "
 f  q:chrs'[$e(x,1)  s x=$e(x,2,9999) i x="" q
 q x
 ;
rtrim(x,chrs) ; trim trailing spaces from text
 n len
 i chrs="" s chrs=" "
 s len=$l(x) f  q:chrs'[$e(x,len)  s x=$e(x,1,len-1),len=len-1 i x="" q
 q x
 ;
lcase(string) ; convert to lower case
 q $tr(string,"ABCDEFGHIJKLMNOPQRSTUVWXYZ","abcdefghijklmnopqrstuvwxyz")
 ;
ucase(string) ; convert to upper case
 q $tr(string,"abcdefghijklmnopqrstuvwxyz","ABCDEFGHIJKLMNOPQRSTUVWXYZ")
 ;
utf8in(string) ; Convert UTF8 to raw Unicode (UTF16)
 q string
 ; 
utf8out(string) ; Convert raw Unicode (UTF16) to UTF8
 q string
 ;
vars(a) ; public  system variables
 ;
 ; the following variables can be modified in accordance with the documentation
 s a("extra")=$c(1)    ; key marker for oversize data strings
 s a("mqinfo")=0       ; set to 1 to place all mq error/information messages in %zmgmq("info")
 ;                       otherwise, error messages will be placed in %zmgmq("error")
 ;                       and 'information only' messages in %zmgmq("info")
 s a("avar")="req"     ; prefix for argument variables.
 s a("buffer")=""      ; output buffer.
 s a("idle_timeout")=0 ; idle timeout
 ;
 ; the following variables must not be modified
 i '($d(a("global"))#10) s a("global")=0
 i '($d(a("oversize"))#10) s a("oversize")=0
 ; #define mg_tx_data     0
 ; #define mg_tx_akey     1
 ; #define mg_tx_arec     2
 ; #define mg_tx_eod      3
 s a("ddata")=0,a("dakey")=1,a("darec")=2,a("deod")=3
 s a("maxlen")=$$getmsl()
 q
 ;
opts(a,opts) ; superserver options mask
 n dlm,i,x,error
 s dlm=",",error=""
 i opts="" k a(0) q ""
 f i=1:1:4 s a(0,i)=0
 f i=1:1:$l(opts,dlm) s x=$$lcase($p(opts,dlm,i)) d  i error'="" q
 . i x="global"!(x="globals") s a(0,1)=1 q
 . i x="function"!(x="functions") s a(0,2)=1 q
 . i x="class"!(x="classes") s a(0,3)=1 q
 . i x="http"!(x="https") s a(0,4)=1 q
 . s error="Invalid superserver option: "_x
 . q
 q error
 ;
bvars(a) ; initialise buffer control variables
 s a("maxlen")=$$getmsl()
 s (a("recvsize"),a("recvptr"),a("recvrlen"))=0
 s a("recvbuf")=""
 q
 ;
delavars(a) ; delete argument variables
 n i
 f i=1:1:37 k @(a("avar")_i)
 k a("buffer") s a("buffer")=""
 q
 ;
esize(esize,size,base) ; encode record size
 n i,x
 i base'=10 g esize1
 s esize=size
 q $l(esize)
esize1 ; up to base 62
 s esize=$$ebase62(size#base,base)
 f i=1:1 s x=(size\(base**i)) q:'x  s esize=$$ebase62(x#base,base)_esize
 q $l(esize)
 ;
dsize(esize,len,base) ; decode record size
 n i,x,size
 i base'=10 g dsize1
 s size=+$e(esize,1,len)
 q size
dsize1 ; up to base 62
 s size=0
 f i=len:-1:1 s x=$e(esize,i) s size=size+($$dbase62(x,base)*(base**(len-i)))
 q size
 ;
ebase62(n10,base) ; encode to single digit (up to) base-62 number
 i n10'<0,n10<10 q $c(48+n10)
 i n10'<10,n10<36 q $c(65+(n10-10))
 i n10'<36,n10<62 q $c(97+(n10-36))
 q ""
 ;
dbase62(nxx,base) ; decode single digit (up to) base-62 number
 n x
 s x=$a(nxx) i base<17,x'<97 s x=x-32
 i x'<48,x<58 q (x-48)
 i x'<65,x<91 q ((x-65)+10)
 i x'<97,x<123 q ((x-97)+36)
 q ""
 ;
esize256(dsize) ; create little-endian 32-bit unsigned integer from M decimal
 q $c(dsize#256)_$c(((dsize\256)#256))_$c(((dsize\(256**2))#256))_$c(((dsize\(256**3))#256))
 ;
dsize256(esize) ; convert little-endian 32-bit unsigned integer to M decimal
 q ($a(esize,4)*(256**3))+($a(esize,3)*(256**2))+($a(esize,2)*256)+$a(esize,1)
 ;
ehead(head,size,byref,type) ; encode header record
 n slen,hlen,code
 s slen=$$esize(.esize,size,10)
 s code=slen+(type*8)+(byref*64)
 s head=$c(code)_esize
 s hlen=slen+1
 q hlen
 ;
dhead(head,size,byref,type) ; decode header record
 n slen,hlen,code
 s code=$a(head,1)
 s byref=code\64
 s type=(code#64)\8
 s slen=code#8
 s hlen=slen+1
 s size=0 i $l(head)'<hlen s size=$$dsize($e(head,2,slen+1),slen,10)
 q hlen
 ;
recvex(%zcs,len) ; read 'len' bytes from client
 n x,nmax,n,ncnt
 i 'len q ""
 s x="",nmax=len,n=0,ncnt=0 f  r y#nmax d  q:'nmax  i ncnt>100 q
 . i y="" s ncnt=ncnt+1 q
 . s ncnt=0,x=x_y,n=n+$l(y),nmax=len-n
 . q
 i ncnt s x="" d halt(.%zcs) ; client disconnect
 q x
 ;
recv(%zcs,len,clen,rlen) ; buffered read from client - initialize buffer using bvars(.%zcs)
 n result,get,avail
 ;
 i $d(%zcs("ifc")) s result=$e(request,%zcs("ifc"),%zcs("ifc")+(len-1)),%zcs("ifc")=%zcs("ifc")+len,rlen=rlen+len q result
 ;
 ;s result="" f get=1:1:len r *x s result=result_$c(x)
 ;s rlen=rlen+len q result
 ;
 s get=len,result=""
 i 'len q result
 f  d  i 'get q
 . s avail=%zcs("recvsize")-%zcs("recvptr")
 . ;d event("i="_i_";len="_len_";avail="_avail_";get="_get_"=("_recvbuf_") "_clen_" "_rlen)
 . i get'>avail s result=result_$e(%zcs("recvbuf"),%zcs("recvptr")+1,%zcs("recvptr")+get),%zcs("recvptr")=%zcs("recvptr")+get,get=0 q
 . ;s result=%zcs("recvbuf"),%zcs("recvptr")=0,get=get-avail
 . s result=$e(%zcs("recvbuf"),%zcs("recvptr")+1,%zcs("recvptr")+avail),%zcs("recvptr")=0,get=get-avail
 . s avail=clen-%zcs("recvrlen") i 'avail q
 . i avail>%zcs("maxlen") s avail=%zcs("maxlen")
 . s %zcs("recvbuf")=$$recvex(.%zcs,avail),%zcs("recvsize")=avail,%zcs("recvptr")=0,%zcs("recvrlen")=%zcs("recvrlen")+avail
 . ;d event("%zcs(""recvbuf"")="_i_"="_%zcs("recvbuf"))
 . q
 s rlen=rlen+len
 q result
 ;
inetd ; entry point from [x]inetd
xinetd ; someone is sure to use this label
 new $ztrap set $ztrap="zgoto "_$zlevel_":inetde^%zmgsis"
 d child(0,0,"","")
 q
inetde ; error
 w $$error()
 q
 ;
ifc(ctx,request,param) ; entry point from fixed binding
 n %zcs,argc,sn,type,var,req,res,cmnd,pcmnd,mcmnd,buf,byref,hlen,clen,rlen,oref,result
 new $ztrap set $ztrap="zgoto "_$zlevel_":ifce^%zmgsis"
 i param["$zv" q $zv
 i param["dbx" q $$dbx(.%zcs,ctx,$a($e(request,5)),$e(request,6,$$getmsl()),$$dsize256(request),param)
 i param["websn" q $$dbxwebsn(ctx,$p(param,":",2),request,$p(param,":",3))
 d vars(.%zcs)
 s %zcs("ifc")=1
 s argc=1
 k ^mgsi($j)
 d delavars(.%zcs)
 d bvars(.%zcs)
 s sn=0,type=0,var=%zcs("avar")_argc,req(argc)=var,(cmnd,pcmnd,buf)=""
 s buf=$p(request,$c(10),1),%zcs("ifc")=%zcs("ifc")+$l(buf)+1
 s type=0,byref=0 d req2(.%zcs,.req,argc,type,byref,buf,.cmnd,.pcmnd,.var) s @var=buf
 s cmnd=$p(buf,"^",2)
 s hlen=$l(buf),clen=0
 i cmnd="P" s clen=$$dsize($e(buf,hlen-(5-1),hlen),5,62)
 s %zcs("client")=$e(buf,4)
 ;d event("request cmnd="_cmnd_"; size="_clen_" ("_$e(buf,hlen-(5-1),hlen)_"); client="_%zcs("client")_" ;header = "_buf)
 s rlen=0
 i clen d req(.%zcs,.req,.argc,clen,.rlen,buf,.cmnd,.pcmnd,.var)
 s req=$g(@req(1)),res="" i req="" q ""
 s cmnd=$p(req,"^",2),mcmnd=$p(req,"^",4)
 k %zcs("buffer") s %zcs("buffer")="" s %zcs("buffer",1)="00000cv"_$c(10)
 i cmnd="A" d ayt(.%zcs,.req)
 i cmnd="S" d dint(.%zcs,.req)
 i cmnd="P" s res=$$mcom(.%zcs,.req,argc,mcmnd,.%oref)
 i cmnd="H" d info(.%zcs,.req)
 i cmnd="X" d halt(.%zcs)
 d end(.%zcs) s result=$g(%zcs("buffer"))
 d delavars(.%zcs)
 q result
ifce ; error
 q "00000ce"_$c(10)_"m server error : ["_$g(req(2))_"]"_$tr($$error(),"<>%","[]^")
 ;
child(pport,port,tls,opts) ; child
 n %zcs,%oref,x,argc,sn,type,var,req,res,oref,cmnd,pcmnd,mcmnd,buf,byref,hlen,clen,rlen,error
 new $ztrap set $ztrap="zgoto "_$zlevel_":childe^%zmgsis"
 i pport["|" s %zcs("pwnd")=pport,pport=$p(pport,"|",2)
 i $d(%zcs("pwnd")) s %zcs("pwnd")=%zcs("pwnd")_"|"_$zd d &pwind.tcpchldinit(%zcs("pwnd"),"",.error) g:error="" child1 d event(error) d halt(.%zcs)
 i 'pport g child1
 u $principal
 ;
child1 ; prepare for child request loop
 d vars(.%zcs)
 s error=$$opts(.%zcs,$g(opts)) i error'="" d:$g(^%zmgsi("loglevel"))>0 event(error)
child2 ; child request loop
 k ^mgsi($j)
 d delavars(.%zcs)
 k req s argc=1
child3 ; read request
 ;d event("******* get next request *******")
 d bvars(.%zcs)
 s sn=0,type=0,var=%zcs("avar")_argc,req(argc)=var,(cmnd,pcmnd,buf)=""
 i $d(%zcs("pwnd")) d &pwind.tcpread(.buf,0,$g(%zcs("idle_timeout")),.error) s buf=$p(buf,$c(10),1) g:error="" child4 d event(error) d halt(.%zcs)
 i '%zcs("idle_timeout") r *x
 i %zcs("idle_timeout") r *x:%zcs("idle_timeout") i '$t d halt(.%zcs) ; idle timeout
 i x=0 d halt(.%zcs) ; client disconnect
 s buf=$c(x) f  r *x:10 q:'$t  q:x=10!(x=0)  s buf=buf_$c(x)
 i x=0 d halt(.%zcs) ; client disconnect
child4 ; request header received
 i buf="xDBC" g main^%mgsqln
 i buf?1u.e1"HTTP/"1n1"."1n1c s buf=buf_$c(10) g main^%mgsqlw
 i $e(buf,1,4)="dbx1" d dbxnet^%zmgsis(.%zcs,buf) d halt(.%zcs)
 s type=0,byref=0 d req2(.%zcs,.req,argc,type,byref,buf,.cmnd,.pcmnd,.var) s @var=buf
 s cmnd=$p(buf,"^",2)
 s hlen=$l(buf),clen=0
 i cmnd="P" s clen=$$dsize($e(buf,hlen-(5-1),hlen),5,62)
 s %zcs("client")=$e(buf,4)
 ;d event("request cmnd="_cmnd_"; size="_clen_" ("_$e(buf,hlen-(5-1),hlen)_"); client="_%zcs("client")_" ;header = "_buf)
 s rlen=0
 i clen d req(.%zcs,.req,.argc,clen,.rlen,buf,.cmnd,.pcmnd,.var)
 ;
 ;f i=1:1:argc d event("arg "_i_" = "_$g(@req(i)))
 ;
 s req=$g(@req(1)) i req="" g child2
 s cmnd=$p(req,"^",2)
 k %zcs("buffer") s %zcs("buffer")="" s %zcs("buffer",1)="00000cv"_$c(10)
 s mcmnd=$p(req,"^",4)
 i cmnd="P",mcmnd="a" tstart  s res=0 d res(.%zcs,.req,argc,res),end(.%zcs) g child2
 i cmnd="P",mcmnd="b" s res=$tlevel d res(.%zcs,.req,argc,res),end(.%zcs) g child2
 i cmnd="P",mcmnd="c" tcommit  s res=0 d res(.%zcs,.req,argc,res),end(.%zcs) g child2
 i cmnd="P",mcmnd="d" trollback  s res=0  d res(.%zcs,.req,argc,res),end(.%zcs) g child2
 i cmnd="A" d ayt(.%zcs,.req)
 i cmnd="S" d dint(.%zcs,.req)
 i cmnd="P" s res=$$mcom(.%zcs,.req,argc,mcmnd,.%oref)
 i cmnd="H" d info(.%zcs,.req)
 i cmnd="X" d halt(.%zcs)
 d end(.%zcs)
 g child2
 ;
childe ; error
 d:$g(^%zmgsi("loglevel"))>0 event($$error())
 i $$error()["read"!($$error()["READ") d halt(.%zcs)
 i $$error()["write"!($$error()["WRITE") d halt(.%zcs)
 i $$error()["%gtm-e-ioeof" d halt(.%zcs)
 i $$error()["%GTM-E-IOEOF" d halt(.%zcs)
 g child2
 ;
halt(%zcs) ; halt
 i $d(%zcs("pwnd")) d &pwind.tcpclose()
 h
 ;
req(%zcs,req,argc,clen,rlen,buf,cmnd,pcmnd,var) ; read request data
 n x,hlen,size,byref,type,slen,esize,sn,get,got
req1 ; get next argument
 s x=$$recv(.%zcs,1,clen,.rlen),hlen=$$dhead(x,.size,.byref,.type)
 s slen=hlen-1
 s esize=$$recv(.%zcs,slen,clen,.rlen)
 s size=$$dsize(esize,slen,10)
 s argc=argc+1,sn=0
 d req2(.%zcs,.req,argc,type,byref,buf,.cmnd,.pcmnd,.var)
 i type=%zcs("darec") d array(.%zcs,.req,argc,clen,.rlen,buf,var) g reqz
 s got=0 f sn=0:1 s get=size-got s:get>%zcs("maxlen") get=%zcs("maxlen") s buf=$$recv(.%zcs,get,clen,.rlen) d  i got=size q
 . s got=got+get
 . i 'sn s @var=buf q
 . i sn s @(var_"(%zcs(""extra""),sn)")=buf q
 . q
reqz ; argument read
 i rlen<clen g req1
 q
 ;
req2(%zcs,req,argc,type,byref,buf,cmnd,pcmnd,var) ; initialize next argument
 n sysp,uci,ucic,offset
 i argc=1 d
 . s cmnd=$p(buf,"^",2)
 . s sysp=$p(buf,"^",3)
 . s uci=$p(sysp,"#",2) i uci'="" d
 . . s ucic=$$getuci()
 . . i ucic'="",uci=ucic q
 . . d uci(uci)
 . . q
 . s offset=$p(sysp,"#",3)+0
 . s %zcs("global")=$p(sysp,"#",7)+0
 . s pcmnd=$p(buf,"^",4)
 . q
 s var=%zcs("avar")_argc
 s req(argc)=var
 s req(argc,0)=type i type=%zcs("darec") s req(argc,0)=1
 s req(argc,1)=byref
 i type=1 s req(argc,1)=1
 q
 ;
array(%zcs,req,argc,clen,rlen,buf,var) ; read array
 n xkey,kn,sn,hlen,size,esize,byref,type,slen,size,type,get,got,val
 s kn=0
array1 ; read next element (key or data)
 s sn=0
 s xkey=$$recv(.%zcs,1,clen,.rlen),hlen=$$dhead(xkey,.size,.byref,.type)
 s slen=hlen-1
 s esize=$$recv(.%zcs,slen,clen,.rlen)
 s size=$$dsize(esize,slen,10)
 i type=%zcs("deod") q
 s got=0 f sn=0:1 s get=size-got s:get>%zcs("maxlen") get=%zcs("maxlen") s buf=$$recv(.%zcs,get,clen,.rlen) d  i got=size q
 . s got=got+get
 . i type=%zcs("dakey") s kn=kn+1,xkey(kn)=buf 
 . i type=%zcs("ddata") s val=buf d array2(.%zcs,.req,argc,.xkey,kn,sn,val) k xkey s kn=0
 . q
 g array1
 ;
array2(%zcs,req,argc,xkey,kn,sn,val) ; read array - set a single node
 n n,i,ref,com,key,eref
 new $ztrap set $ztrap="zgoto "_$zlevel_":array2e^%zmgsis"
 s (key,com)="" f i=1:1:kn q:i=kn&($g(xkey(i))=" ")  s key=key_com_"xkey("_i_")",com=","
 i %zcs("global") d
 . i $l(key) s ref="^mgsi($j,argc-2,"_key_")",eref="^mgsi($j,argc-2,"_key_",%zcs(""extra""),sn)"
 . i '$l(key) s ref="^mgsi($j,argc-2)",eref="^mgsi($j,argc-2,%zcs(""extra""),sn)"
 . q
 i '%zcs("global") d
 . i $l(key) s ref=req(argc)_"("_key_")",eref=req(argc)_"("_key_",%zcs(""extra""),sn)"
 . i '$l(key) s ref=req(argc),eref=req(argc)_"(%zcs(""extra""),sn)"
 . q
 i $l(ref) x "s "_ref_"=val"
 q
array2e ; error
 d:$g(^%zmgsi("loglevel"))>0 event("array: "_$$error())
 q
 ;
end(%zcs) ; terminate response
 n len,len62,i,head,x
 i '$d(%zcs("ifc")),$e($g(%zcs("buffer",1)),6,7)="sc" d writem(.%zcs,$p(%zcs("buffer",1),"0",1),1) q  ; streamed response
 s len=0
 f i=1:1 q:'$d(%zcs("buffer",i))  s len=len+$l(%zcs("buffer",i))
 s len=len-8
 s head=$e($g(%zcs("buffer",1)),1,8)
 s x=$$esize(.len62,len,62)
 f  q:$l(len62)'<5  s len62="0"_len62
 s head=len62_$e(head,6,8) i $l(head)'=8 s head=len62_"cv"_$c(10)
 s %zcs("buffer",1)=head_$e($g(%zcs("buffer",1)),9,99999)
 ; flush the lot out
 i $d(%zcs("ifc")) g end1
 f i=1:1 q:'$d(%zcs("buffer",i))  d writem(.%zcs,%zcs("buffer",i),0)
 d flush
 q
end1 ; interface
 s %zcs("buffer")="" f i=1:1 q:'$d(%zcs("buffer",i))  s %zcs("buffer")=%zcs("buffer")_%zcs("buffer",i)
 q
 ;
flush ; flush output buffer
 q
 ;
ayt(%zcs,req) ; are you there?
 n txt
 s req=$g(@req(1))
 s txt=$p($h,",",2)
 f  q:$l(txt)'<5  s txt="0"_txt
 s txt="m"_txt
 f  q:$l(txt)'<12  s txt=txt_"0"
 d send(.%zcs,txt)
 q
 ; 
dint(%zcs,req) ; initialise the service link
 n username,password,usrchk,version,itimeout,nls,uci,x,systype,sysver,txt
 s (username,password)=""
 s req=$p($g(@req(1)),"^S^",2,9999)
 ;"^S^version=%s&timeout=%d&nls=%s&uci=%s"
 s username=$p(req,"&unpw=",2,999)
 s password=$p(username,$c(1),2),username=$p(username,$c(1),1)
 s usrchk=1
 s usrchk=$$checkunpw(password)
 i 'usrchk d halt(.%zcs)
 ;d event(username_"|"_password)
 s version=$p($p(req,"version=",2),"&",1)
 s itimeout=+$p($p(req,"timeout=",2),"&",1)
 s %zcs("idle_timeout")=itimeout
 s nls=$p($p(req,"nls=",2),"&",1)
 s uci=$p($p(req,"uci=",2),"&",1)
 i $l(uci) d uci(uci)
 s x=$$setio(nls)
 s uci=$$getuci()
 s systype=$$getsys()
 s sysver=$$getzvv()
 s txt="pid="_$j_"&uci="_uci_"&server_type="_systype_"&server_version="_sysver_"&version="_$p($$v(),".",1,3)_"&child_port=0"
 d send(.%zcs,txt)
 q
 ;
uci(uci) ; change namespace/uci
 n x
 new $ztrap set $ztrap="zgoto "_$zlevel_":ucie^%zmgsis"
 i uci="" q
 s $zg=uci
 q
ucie ; error
 i $g(^%zmgsi("loglevel"))>0 d event("uci error: "_uci_" : "_$$error())
 q
 ;
getuci() ; get namespace/uci
 n uci
 new $ztrap set $ztrap="zgoto "_$zlevel_":getucie^%zmgsis"
 s uci=$zg
 q uci
getucie ; error
 q ""
 ;
info(%zcs,req) ; connection information
 d send(.%zcs,"HTTP/1.1 200 OK"_$char(13,10)_"Connection: close"_$char(13,10)_"Content-Type: text/html"_$char(13,10,13,10))
 d send(.%zcs,"<html><head><title>mgsi - connection test</title></head><body bgcolor=#ffffcc>")
 d send(.%zcs,"<h2>mgsi - connection test successful</h2>")
 d send(.%zcs,"<table border=1>")
 d send(.%zcs,"<tr><td>"_$$getsys()_" version:</td><td><b>"_$$getzv()_"<b><tr>")
 d send(.%zcs,"<tr><td>uci:</td><td><b>"_$$getuci()_"<b><tr>")
 d send(.%zcs,"</table>")
 d send(.%zcs,"</body></html>")
 q
 ;
mcom(%zcs,req,argc,cmnd,%oref) ; execute M command
 n argz,ref,var,varn,com,i,ok,res
 new $ztrap set $ztrap="zgoto "_$zlevel_":mcome^%zmgsis"
 s %zcs("buffer")="",res=""
 ; check options mask
 i $d(%zcs(0)) s ok=1 d  i 'ok q res
 . i $g(%zcs(0,1))=0,"SGKDOPIMm"[cmnd s ok=0 q
 . i $g(%zcs(0,2))=0,"HX"[cmnd s ok=0 q
 . i $g(%zcs(0,3))=0,"yxhij"[cmnd s ok=0 q
 . q
 i cmnd="S",argc>2 s ref=$$ref(.req,argc,argc-1,0) x "s "_ref_"="_%zcs("avar")_argc d res(.%zcs,.req,argc,"") q res
 i cmnd="G",argc>1 s ref=$$ref(.req,argc,argc,0) x "s res=$g("_ref_")" d res(.%zcs,.req,argc,res) q res
 i cmnd="K",argc>0 s ref=$$ref(.req,argc,argc,0) x "k "_ref d res(.%zcs,.req,argc,"") q res
 i cmnd="D",argc>1 s ref=$$ref(.req,argc,argc,0) x "s res=$d("_ref_")" d res(.%zcs,.req,argc,res) q res
 i cmnd="O",argc>1 s ref=$$ref(.req,argc,argc,0) x "s res=$o("_ref_")" d res(.%zcs,.req,argc,res) q res
 i cmnd="P",argc>1 s ref=$$ref(.req,argc,argc,0) x "s res=$o("_ref_",-1)" d res(.%zcs,.req,argc,res) q res
 i cmnd="I",argc>2 s ref=$$ref(.req,argc,argc-1,0) x "s res=$i("_ref_","_%zcs("avar")_argc_")" d res(.%zcs,.req,argc,res) q res
 i cmnd="a" tstart  s res=0 d res(.%zcs,.req,argc,res) q res
 i cmnd="b" s res=$tlevel d res(.%zcs,.req,argc,res) q res
 i cmnd="c" tcommit  s res=0 d res(.%zcs,.req,argc,res) q res
 i cmnd="d" trollback  s res=0 d res(.%zcs,.req,argc,res) q res
 i cmnd="M",argc>2 d  d res(.%zcs,.req,argc,"") q res ; global merge from client to database
 . s var="" f argz=1:1 q:'$d(req(argz))  i $g(req(argz,0))=1 s var=req(argz) q
 . i var="" q
 . s argz=argz-1
 . s ref=$$ref(.req,argc,argz,0)
 . i ref["()" s ref=$p(ref,"()",1)
 . i $g(@req(argz+2))["ks" x "k "_ref
 . x "m "_ref_"="_var
 . q
 i cmnd="m",argc>2 d  d res(.%zcs,.req,argc,"") q res ; global merge from database to client
 . s var="" f argz=1:1 q:'$d(req(argz))  i $g(req(argz,0))=1 s var=req(argz) q
 . i var="" q
 . s argz=argz-1
 . s ref=$$ref(.req,argc,argz,0)
 . i ref["()" s ref=$p(ref,"()",1)
 . x "m "_var_"="_ref
 . q
 i cmnd="H" d  q res ; stream HTML content to client using M function
 . i '$d(%zcs("ifc")) s %zcs("buffer",1)=$c(1,2,1,10)_"0sc"_$c(10) d writem(.%zcs,%zcs("buffer",1),0)
 . i argc<2 q
 . s (varn,com)="" f i=1:1:argc s varn=varn_com_%zcs("avar")_i,com="," 
 . s ref=$$ref(.req,argc,argc,0)
 . x "n ("_varn_") d "_ref
 . q
 i cmnd="y" d  q res ; stream HTML content to client using ISC ClassMethod
 . i '$d(%zcs("ifc")) s %zcs("buffer",1)=$c(1,2,1,10)_"0sc"_$c(10) d writem(.%zcs,%zcs("buffer",1),0)
 . i argc<1 q
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i 
 . s ref=$$oref(.req,argc,0)
 . i argc=1 x "n ("_varn_") s res=$ClassMethod()"
 . i argc>1 x "n ("_varn_") s res=$ClassMethod("_ref_")"
 . q
 i cmnd="X",argc>1 d  d res(.%zcs,.req,argc,res) q res ; invocation of M extrinsic function
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i 
 . s ref=$$ref(.req,argc,argc,1)
 . i argc=2 x "n ("_varn_") s res=$$"_ref_"()"
 . i argc>2 x "n ("_varn_") s res=$$"_ref
 . q
 i cmnd="x",argc>0 d  d res(.%zcs,.req,argc,res) q res ; invocation of ISC ClassMethod
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i 
 . s ref=$$oref(.req,argc,0)
 . i argc=1 x "n ("_varn_") s res=$ClassMethod()"
 . i argc>1 x "n ("_varn_") s res=$ClassMethod("_ref_")"
 . q
 i cmnd="h",argc>0 d  d res(.%zcs,.req,argc,res) q res ; retrieve ISC Class property value
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i
 . s ref=$$oref(.req,argc,0)
 . i argc<3 q
 . s @req(2)=%oref(@req(2))
 . x "n ("_varn_") s res=$Property("_ref_")"
 . q
 i cmnd="i",argc>0 d  d res(.%zcs,.req,argc,res) q res ; set ISC Class property value
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i
 . s ref=$$oref(.req,argc-1,0)
 . i argc<4 q
 . s @req(2)=%oref(@req(2))
 . x "n ("_varn_") s $Property("_ref_")="_req(argc)
 . q
 i cmnd="j",argc>0 d  d res(.%zcs,.req,argc,res) q res ; invocation of ISC Method
 . s varn="res" f i=1:1:argc s varn=varn_","_%zcs("avar")_i
 . s ref=$$oref(.req,argc,0)
 . i argc<3 q
 . s @req(2)=%oref(@req(2))
 . x "n ("_varn_") s res=$Method("_ref_")"
 . q
 q res
mcome ; error
 d:$g(^%zmgsi("loglevel"))>0 event($$client(.%zcs)_" error : "_$$error())
 k %zcs("buffer") s %zcs("buffer")="",%zcs("buffer",2)="m server error : ["_$g(%zcs("buffer",2))_"]"_$tr($$error(),"<>%","[]^")_$g(ref)
 s %zcs("buffer",1)="00000ce"_$c(10)
 q res
 ;
client(%zcs) ; get client name
 s name="mg_client"
 i $g(%zcs("client"))="z" s name="mg_php"
 i $g(%zcs("client"))="p" s name="mg_python"
 i $g(%zcs("client"))="r" s name="mg_ruby"
 q name
 ;
ref(req,argc,argz,fun) ; global reference
 n ref,com,i,strt,var
 i argc<2 q ""
 s var=$g(@req(2))
 s strt=2 i var?1"^"."^" s strt=strt+1
 s ref=@req(strt) i argc=strt q ref
 i strt'<argz q ref
 s ref=ref_"("
 s com="" f i=strt+1:1:argz s ref=ref_com_$s(fun:".",1:"")_req(i),com=","
 s ref=ref_")"
 q ref
 ;
oref(req,argc,fun) ; object reference
 n ref,com,i,strt,var
 i argc<2 q ""
 s var=$g(@req(2))
 s strt=2 i var?1"^"."^" s strt=strt+1
 i '$d(req(strt)) q ""
 s ref=""
 s com="" f i=strt:1:argc s ref=ref_com_$s(fun&((i-strt)>1):".",1:"")_req(i),com=","
 q ref
 ;
res(%zcs,req,argz,res) ; return result
 n maxlen,anybyref,byref,argc,sn,size,head,array,x
 s maxlen=$$getmsl()
 s anybyref=0 f argc=1:1:argz q:'$d(req(argc))  i $g(req(argc,1)) s anybyref=1 q
 i 'anybyref d  q
 . d send(.%zcs,$g(res))
 . i %zcs("oversize") f sn=1:1 q:'$d(^mgsi($j,0,%zcs("extra"),sn))  d send(.%zcs,$g(^(sn)))
 . q
 s %zcs("buffer",1)="00000cc"_$c(10)
 s size=$l($g(res)),byref=0
 i %zcs("oversize") f sn=1:1 q:'$d(^mgsi($j,0,%zcs("extra"),sn))  s size=size+$l(^(sn))
 s x=$$ehead(.head,size,byref,%zcs("ddata"))
 d send(.%zcs,head)
 d send(.%zcs,$g(res))
 i %zcs("oversize") f sn=1:1 q:'$d(^mgsi($j,0,%zcs("extra"),sn))  d send(.%zcs,$g(^(sn)))
 f argc=1:1:argz q:'$d(req(argc))  d
 . s byref=$g(req(argc,1))
 . s array=$g(req(argc,0))
 . i 'byref s size=0,x=$$ehead(.head,size,byref,%zcs("ddata")) d send(.%zcs,head) q
 . i 'array d  q
 . . s size=$l($g(@req(argc)))
 . . f sn=1:1 q:'$d(@(req(argc)_"(%zcs(""extra""),sn)"))  s size=size+$l($g(@(req(argc)_"(%zcs(""extra""),sn)")))
 . . s x=$$ehead(.head,size,byref,%zcs("ddata"))
 . . d send(.%zcs,head)
 . . d send(.%zcs,$g(@req(argc)))
 . . f sn=1:1 q:'$d(@(req(argc)_"(%zcs(""extra""),sn)"))  d send(.%zcs,$g(@(req(argc)_"(%zcs(""extra""),sn)")))
 . . q
 . s x=$$ehead(.head,0,0,%zcs("darec"))
 . d send(.%zcs,head)
 . d resa(.%zcs,.req,argc)
 . s x=$$ehead(.head,0,0,%zcs("deod"))
 . d send(.%zcs,head)
 . q
 q
 ;
resa(%zcs,req,argc) ; return array
 n avar,byref,xkey,fkey,spc,x,size,sn,kn,ref
 new $ztrap set $ztrap="zgoto "_$zlevel_":resae^%zmgsis"
 s byref=0
 s avar=req(argc),fkey="" i %zcs("global") s avar="^mgsi",fkey="$j,argc-2"
 i avar="" q
 i %zcs("global") d
 . i ($d(@(avar_"("_fkey_")"))#10) d
 . . s spc=" ",x=$$ehead(.head,$l(spc),byref,%zcs("dakey")) d send(.%zcs,head),send(.%zcs,spc)
 . . s size=0
 . . s size=size+$l($g(@req(argc)))
 . . f sn=1:1 q:'$d(@(avar_"("_fkey_","_"%zcs(""extra""),sn)"))  s size=size+$l($g(^(sn)))
 . . s x=$$ehead(.head,size,byref,%zcs("ddata")) d send(.%zcs,head)
 . . d send(.%zcs,$g(@req(argc)))
 . . f sn=1:1 q:'$d(@(avar_"("_fkey_","_"%zcs(""extra""),sn)"))  d send(.%zcs,$g(^(sn)))
 . . q
 . s fkey=fkey_","
 . q
 i '%zcs("global") d
 . i ($d(@avar)#10),$l($g(@avar)) d
 . . s spc=" ",x=$$ehead(.head,$l(spc),byref,%zcs("dakey")) d send(.%zcs,head),send(.%zcs,spc)
 . . s size=0
 . . s size=size+$l($g(@avar))
 . . f sn=1:1 q:'$d(@(avar_"(%zcs(""extra""),sn)"))  s size=size+$l($g(@(avar_"(%zcs(""extra""),sn)")))
 . . s x=$$ehead(.head,size,byref,%zcs("ddata")) d send(.%zcs,head)
 . . d send(.%zcs,$g(@avar))
 . . f sn=1:1 q:'$d(@(avar_"(%zcs(""extra""),sn)"))  d send(.%zcs,$g(@(avar_"(%zcs(""extra""),sn)")))
 . . q
 . q
 s kn=1,xkey(kn)="",ref="xkey("_kn_")"
 f  s xkey(kn)=$o(@(avar_"("_fkey_ref_")")) i $$resa1(.%zcs,avar,fkey,.ref,byref,.xkey,.kn) q
 q
resae ; error
 new $ztrap set $ztrap="zgoto "_$zlevel_":"
 q
 ;
resa1(%zcs,avar,fkey,ref,byref,xkey,kn) ; array node
 n def,data
 i xkey(kn)=%zcs("extra") q 0
 i xkey(kn)="",kn=1 q 1
 i xkey(kn)="" s kn=kn-1,ref=$p(ref,",",1,$l(ref,",")-1) q
 s def=$d(@(avar_"("_fkey_ref_")")) i (def\10),$$resa3(a,ref) s def=1
 s data=$g(@(avar_"("_fkey_ref_")"))
 i (def#10) d resa2(.%zcs,avar,fkey,ref,data,byref,.xkey,kn)
 i (def\10) s kn=kn+1,xkey(kn)="",ref=ref_","_"xkey("_kn_")"
 q 0
 ;
resa2(%zcs,avar,fkey,ref,data,byref,xkey,kn) ; array node data
 n i,spc,x,head,size,sn
 f i=1:1:kn s x=$$ehead(.head,$l(xkey(i)),byref,%zcs("dakey")) d send(.%zcs,head),send(.%zcs,xkey(i))
 i $g(%zcs("client"))="z",(def\10) s spc=" ",x=$$ehead(.head,$l(spc),byref,%zcs("dakey")) d send(.%zcs,head),send(.%zcs,spc)
 s size=$l(data)
 f sn=1:1 q:'$d(@(avar_"("_fkey_ref_",%zcs(""extra""),sn)"))  s size=size+$l($g(@(avar_"("_fkey_ref_",%zcs(""extra""),sn)")))
 s x=$$ehead(.head,size,byref,%zcs("ddata")) d send(.%zcs,head)
 d send(.%zcs,data)
 f sn=1:1 q:'$d(@(avar_"("_fkey_ref_",%zcs(""extra""),sn)"))  d send(.%zcs,$g(@(avar_"("_fkey_ref_",%zcs(""extra""),sn)")))
 q
 ;
resa3(avar,ref) ; array node data with descendants - test for non-extra data
 n x,def
 s def=0
 s x="" f  s x=$o(@(avar_"("_ref_",x)")) q:x=""  i x'=%zcs("extra") q
 i x="" s def=1
 q def
 ;
send(%zcs,data) ; send data
 n n
 s n=$o(%zcs("buffer",""),-1)
 i n="" s n=1
 i $l($g(%zcs("buffer",n)))+$l(data)>%zcs("maxlen") s n=n+1
 s %zcs("buffer",n)=$g(%zcs("buffer",n))_data
 q
 ;
error() ; get last error
 i $$isydb() q $zs
 q $ze
 ;
seterror(v) ; set error
 q
 ;
setio(tblname) ; set i/o translation
 new $ztrap set $ztrap="zgoto "_$zlevel_":setioe^%zmgsis"
 q ""
setioe ; error - do nothing
 q ""
 ;
htest ; return html
 n systype
 s systype=$$getsys()
 w "<i>a line of html from "_systype_"</i>"
 q
 ;
htest1(p1) ; return html
 n x,systype
 s systype=$$getsys()
 ;w "HTTP/1.1 200 OK",$c(13,10)
 ;w "Content-Type: text/html",$c(13,10)
 ;w "Connection: close",$c(13,10)
 ;w $c(13,10)
 w "<i>html content returned from "_systype_"</i>"
 w "<br><i>the input parameter passed was: <b>"_$g(p1)_"</b></i>"
 s x="" f  s x=$o(p1(x)) q:x=""  w "<br><i>array element passed: <b>"_x_" = "_$g(p1(x))_"</b></i>"
 q
 ;
ptest() ; return result
 n systype
 s systype=$$getsys()
 q "result from "_systype_" process: "_$j_"; namespace: "_$$getuci()
 q
 ;
ptest1(p1) ; return result
 n systype
 s systype=$$getsys()
 q "result from "_systype_" process: "_$j_"; namespace: "_$$getuci()_"; the input parameter passed was: "_p1
 ;
ptest2(p1,p2) ; manipulate an array
 n n,x,systype
 s systype=$$getsys()
 s n=0,x="" f  s x=$o(p1(x)) q:x=""  s n=n+1
 s p1("key from m - 1")="value 1"
 s p1("key from m - 2")="value 2"
 ; s p1("key from m - 2",%zcs("extra"),1)=" ... more data ..."
 ; s p1("key from m - 2",%zcs("extra"),2)=" ... and more data"
 s p2="stratford"
 ; s ^mgsi($j,0,%zcs("extra"),1)=" ... more output ...",^(2)=" ... and more output",%zcs("oversize")=1
 q "result from "_systype_" process: "_$j_"; namespace: "_$$getuci()_"; "_n_" elements were received in an array, 2 were added by this procedure"
 ;
setunpw(username,password)
 s ^%zmgsi(0,"username")=username
 s ^%zmgsi(0,"password")=password
 q 1
 ;
getunpw(username,password)
 s username=$g(^%zmgsi(0,"username"))
 s password=$g(^%zmgsi(0,"password"))
 q 1
 ;
checkunpw(username,password)
 n x,username1,password1
 s x=$$getunpw(.username1,.password1)
 i username1="",password1="" q 1
 i username1'=username d:$g(^%zmgsi("loglevel"))>0 event("access violation: bad username") q 0
 i password1'=password d:$g(^%zmgsi("loglevel"))>0 event("access violation: bad password") q 0
 q 1
 ;
dbx(%zcs,ctx,cmnd,data,len,param) ; entry point from fixed binding
 n %r,obufsize,idx,offset,rc,sort,res,ze,oref,type,utf16,end
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbxe^%zmgsis"
 s obufsize=$$dsize256($e(data,1,4))
 s utf16=$s($a(data,5)=255:1,1:0)
 s idx=$$dsize256($e(data,6,9))
 k %r s offset=11,end=0 f %r=1:1 s %r(%r,0)=$$dsize256($e(data,offset,offset+3)) d  i '$d(%r(%r)) s %r=%r-1 q
 . s %r(%r,1)=$a(data,offset+4)\20,%r(%r,2)=$a(data,offset+4)#20 i %r(%r,1)=9 d  i end=2 k %r(%r) q
 . . s end=end+1
 . . i cmnd'=23,cmnd'=24 s end=2 q
 . . q
 . i %r(%r,1)=-1 k %r(%r) q
 . s %r(%r)=$e(data,offset+5,offset+5+(%r(%r,0)-1))
 . i $g(utf16) s %r(%r)=$$utf8in(%r(%r))
 . s offset=offset+5+%r(%r,0)
 . q
 s %r(-1,"param")=param
 s rc=$$dbxcmnd(.%r,.%oref,cmnd,.res,.utf16)
 i rc=0 s sort=1 ; data
 i rc=-1 s sort=11 ; error
 s type=1 ; string
 i $g(%r(1))="dbxweb^%zmgsis",res=$c(255,255,255,255) q res
 i $g(utf16) s res=$$utf8out(res)
 s res=$$esize256($l(res))_$c((sort*20)+type)_res
 q res
dbxe ; Error
 s ze=$$error()
 q -1
 ;
dbxnet(%zcs,buf) ; new wire protocol for access to M
 n %oref,argc,cmnd,i,offset,ok,oref,pcmnd,port,pport,req,res,slen,sn,sort,type,uci,var,x,error
 s uci=$p(buf,"~",2),%zcs("protocol")=$p(buf,"~",3) ; v4.5.29
 i uci'="" d uci(uci)
 s %zcs("idle_timeout")=$p(buf,"~",3)+0
 s res=$zv
 s res=$$esize256($l(res))_"0"_res
 d writem(.%zcs,res,1)
 ;d event("$j="_$j_" *** Initialise ***")
dbxnet1 ; request loop
 s error="",data=""
 i $d(%zcs("pwnd")) s len=$$getmsl() d &pwind.tcpreadmessage(.data,.len,.cmnd,0,.error) g dbxnet2
 i '%zcs("idle_timeout") r head#5
 i %zcs("idle_timeout") r head#5:%zcs("idle_timeout") i '$t d halt(.%zcs) ; idle timeout
 i $e(head,1,4)="dbx1" d  g dbxnet1  ; v4.5.28
 . s buf=head f  r *x:10 q:'$t  q:x=10!(x=0)  s buf=buf_$c(x)
 . s uci=$p(buf,"~",2)
 . i uci'="" d uci(uci)
 . s %zcs("idle_timeout")=$p(buf,"~",3)+0
 . s res=$zv
 . s res=$$esize256($l(res))_"0"_res
 . d writem(.%zcs,res,1)
 . q
 s len=$$dsize256(head)-5,cmnd=$a($e(head,5))
 i len>$$getmsl() s error="DB Server string size exceeded ("_$$getmsl()_")"
 i len>0 r data#len
dbxnet2 ; request data received
 ;d event("$j="_$j_"; len="_len_"; cmnd="_$a(cmnd))
 i error'="" s sort=11,type=1,res=$$esize256($l(res))_$c((sort*20)+type)_res d writem(.%zcs,res,1) d halt(.%zcs)
 i cmnd=0 g dbxnet1
 i len<1,cmnd=90 d writem(.%zcs,$$ping(""),1) g dbxnet1
 i len<1 q
 i $a(cmnd)=61 tstart  s res=0,res=$$esize256($l(res))_$c(21)_res d writem(.%zcs,res,1) g dbxnet1
 i $a(cmnd)=62 s res=$tlevel,res=$$esize256($l(res))_$c(21)_res d writem(.%zcs,res,1) g dbxnet1
 i $a(cmnd)=63 tcommit  s res=0,res=$$esize256($l(res))_$c(21)_res d writem(.%zcs,res,1) g dbxnet1
 i $a(cmnd)=64 trollback  s res=0,res=$$esize256($l(res))_$c(21)_res d writem(.%zcs,res,1) g dbxnet1
 s res=$$dbx(.%zcs,0,cmnd,data,len,"")
 d writem(.%zcs,res,1)
 g dbxnet1
 ;
dbxcmnd(%r,%oref,cmnd,res,utf16) ; Execute command
 n %io,buf,data,head,idx,len,obufsize,offset,rc,uci,data,sort,type,ok,key
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbxcmnde^%zmgsis"
 s res=""
 ; check options mask
 i $d(%zcs(0)) s ok=1 d  i 'ok q 0
 . i $g(%zcs(0,1))=0,(((cmnd>10)&(cmnd<30))!((cmnd>100)&(cmnd<300))) s ok=0 q
 . i $g(%zcs(0,2))=0,cmnd=31,$g(%r(1))'="dbxweb^%zmgsis" s ok=0 q
 . i $g(%zcs(0,3))=0,((cmnd>40)&(cmnd<60)) s ok=0 q
 . i $g(%zcs(0,4))=0,cmnd=31,$g(%r(1))="dbxweb^%zmgsis" s ok=0 q
 . q
 i cmnd=2 s res=0 q 0
 i cmnd=3 s res=$$getuci() q 0
 i cmnd=4 d uci($g(%r(1))) s res=$$getuci() q 0
 i cmnd=11 s @($$dbxglo(%r(1))_$$dbxref(.%r,2,%r-1,0))=%r(%r),res=0 q 0
 i cmnd=12 s res=$g(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0))) q 0
 i cmnd=13 s res=$o(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0))) q 0
 i cmnd=131 d  q 0
 . s res=$o(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)))
 . s data="" i res'="" s data=$g(^(res))
 . s sort=1,type=1
 . i $g(utf16) s res=$$utf8out(res),data=$$utf8out(data),utf16=0
 . s res=$s($g(%zcs("protocol"))=2:$$esize256($l(data))_$c((sort*20)+type)_data_$$esize256($l(res))_$c((sort*20)+type)_res,1:$$esize256($l(res))_$c((sort*20)+type)_res_$$esize256($l(data))_$c((sort*20)+type)_data)
 . q
 i cmnd=14 s res=$o(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)),-1) q 0
 i cmnd=141 d  q 0
 . s res=$o(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)),-1)
 . s data="" i res'="" s data=$g(^(res))
 . s sort=1,type=1
 . i $g(utf16) s res=$$utf8out(res),data=$$utf8out(data),utf16=0
 . s res=$s($g(%zcs("protocol"))=2:$$esize256($l(data))_$c((sort*20)+type)_data_$$esize256($l(res))_$c((sort*20)+type)_res,1:$$esize256($l(res))_$c((sort*20)+type)_res_$$esize256($l(data))_$c((sort*20)+type)_data)
 . q
 i cmnd=15 k @($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)) s res=0 q 0
 i cmnd=16 s res=$d(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0))) q 0
 i cmnd=17 s res=$i(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r-1,0)),%r(%r)) q 0
 i cmnd=18 d  q 0
 . i $g(%r(%r))=-1 l +@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r-1,0)) s res=1 q
 . l +@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r-1,0)):%r(%r) s res=$t q
 . q
 i cmnd=19 l -@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)) s res=$t q 0
 i cmnd=20 d  q 0
 . n i1,i2,r1,r2
 . s (i1,i2)=1 f i=2:1 q:'$d(%r(i))  i $g(%r(i,1))=3 s i2=i q
 . s r1=$$dbxglo(%r(i1))_$$dbxref(.%r,i1+1,i2-1,0)
 . s r2=$$dbxglo(%r(i2))_$$dbxref(.%r,i2+1,%r,0)
 . m @r1=@r2
 . q
 i cmnd=21 s res=$q(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0))) s:$g(%zcs("protocol"))=2 res=$$empty()_$$gparse(.key,res)_$$eod() q 0
 i cmnd=211 d  q 0
 . s res=$q(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)))
 . s data="" i res'="" s data=$g(@res)
 . s sort=1,type=1
 . i $g(utf16) s res=$$utf8out(res),data=$$utf8out(data),utf16=0
 . s res=$s($g(%zcs("protocol"))=2:$$esize256($l(data))_$c((sort*20)+type)_data_$$gparse(.key,res)_$$eod(),1:$$esize256($l(res))_$c((sort*20)+type)_res_$$esize256($l(data))_$c((sort*20)+type)_data)
 . q
 i cmnd=22 s res=$q(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)),-1) s:$g(%zcs("protocol"))=2 res=$$empty()_$$gparse(.key,res)_$$eod() q 0
 i cmnd=221 d  q 0
 . s res=$q(@($$dbxglo(%r(1))_$$dbxref(.%r,2,%r,0)),-1)
 . s data="" i res'="" s data=$g(@res)
 . s sort=1,type=1
 . i $g(utf16) s res=$$utf8out(res),data=$$utf8out(data),utf16=0
 . s res=$s($g(%zcs("protocol"))=2:$$esize256($l(data))_$c((sort*20)+type)_data_$$gparse(.key,res)_$$eod(),1:$$esize256($l(res))_$c((sort*20)+type)_res_$$esize256($l(data))_$c((sort*20)+type)_data)
 . q
 i cmnd=23 d  q 0
 . n i,ref,com,opt
 . s ref=$$dbxglo(%r(1))_"(",com=""
 . f i=2:1 q:'$d(%r(i))  q:$g(%r(i,1))=9  s ref=ref_com_"%r("_i_")",com=","
 . s i=i+1,opt=$g(%r(i))
 . f i=i+1:2 q:'$d(%r(i+1))  s @(ref_com_"%r("_i_"))")=%r(%r)
 . q
 i cmnd=24 d  q 0
 . n i,ref,com,opt,x,name,value,getdata,max,start,end
 . s getdata=0,max=100,start="",end=""
 . s ref=$$dbxglo(%r(1))_"(",com="",res=""
 . f i=2:1 q:'$d(%r(i))  q:$g(%r(i,1))=9  s ref=ref_com_"%r("_i_")",com=","
 . s i=i+1,opt=$g(%r(i)) f i=1:1 s x=$p(opt,$c(13,10),i) q:x=""  d
 . . s name=$p(x,":",1),value=$p(x,":",2)
 . . i name="getdata" s getdata=+value
 . . i name="max" s max=+value i max=0 s max=100
 . . i name="start" s start=value
 . . i name="end" s end=value
 . . q
 . s ref=ref_com_"start"_")"
 . s sort=1,type=1
 . i start'="",$d(@ref) s data=$s(getdata:$g(@ref),1:""),res=res_$$esize256($l(start))_$c((sort*20)+type)_start_$$esize256($l(data))_$c((sort*20)+type)_data
 . f  s start=$o(@ref) q:start=""  d  i start="" q
 . . i end'="",start]end s start="" q
 . . s data=$s(getdata:$g(@ref),1:"")
 . . s res=res_$$esize256($l(start))_$c((sort*20)+type)_start_$$esize256($l(data))_$c((sort*20)+type)_data
 . . q
 . s sort=9,type=1,res=res_$$esize256($l(res))_$c((sort*20)+type)
 . q
 i cmnd=31 s res=$$dbxfun(.%r,"$$"_%r(1)_"("_$$dbxref(.%r,2,%r,1)_")",.utf16) q 0
 i cmnd=51 s:($e(%r(1))'="^")!($l(%r(1))<2) %r(1)="^%" s res=$o(@%r(1)) q 0
 i cmnd=52 s:($e(%r(1))'="^")!($l(%r(1))<2) %r(1)="^z" s res=$o(@%r(1),-1) q 0
 i cmnd=61 tstart  s res=0 q 0
 i cmnd=62 s res=$tlevel q 0
 i cmnd=63 tcommit  s res=0 q 0
 i cmnd=64 trollback  s res=0 q 0
 s res="<SYNTAX>"
 q -1
dbxcmnde ; Error
 s res=$$error()
 q -1
 ;
dbxglo(glo) ; Generate global name
 q $s($e(glo,1)="^":glo,1:"^"_glo)
 ;
dbxref(%r,strt,end,ctx) ; Generate reference
 n i,ref,com
 s ref="",com="" f i=strt:1:end s ref=ref_com_$s($g(%r(i,2))=7:"$g(%oref(%r("_i_")))",1:"%r("_i_")"),com=","
 i ctx=0,ref'="" s ref="("_ref_")"
 q ref
 ;
dbxfun(%r,fun,utf16) ; Execute function
 n %oref,%uci,a,buf,cmnd,data,head,idx,len,obufsize,offset,oref,rc,res,sort,type,uci
 i fun["$$sql",fun["^%zmgsis" s utf16=0 ; v4.5.29
 s @("res="_fun)
 q res
 ;
dbxcmeth(%r,cmeth) ; Execute function
 n %oref,%uci,a,buf,cmnd,data,head,idx,len,obufsize,offset,oref,rc,res,sort,type,uci
 s @("res=$ClassMethod("_cmeth_")")
 q res
 ;
dbxmeth(%r,meth) ; Execute function
 n %oref,%uci,a,buf,cmnd,data,head,idx,len,obufsize,offset,oref,rc,res,sort,type,uci
 s @("res=$Method("_meth_")")
 q res
 ;
ping(param) ; simple ping operation
 n res
 s res=$h_"#"
 s res=res_$e("01234567890123456789",1,20-$l(res))
 q res
 ;
dbxweb(ctx,data,param) ; mg_web function invocation
 n %r,%cgi,%var,%sys,offset,ok,sort,type,item,no,res,len,uci
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbxwebe^%zmgsis"
 s no=0
 s offset=1,ok=1 f %r=1:1 s len=$$dsize256($e(data,offset,offset+3)) d  i 'ok s %r=%r-1 q
 . s sort=$a(data,offset+4)\20,type=$a(data,offset+4)#20 i sort=9 s ok=0 q
 . i sort=-1 s ok=0 q
 . s item=$e(data,offset+5,offset+5+(len-1))
 . s offset=offset+5+len
 . i sort=5 s name=$p(item,"=",1),value=$p(item,"=",2,9999) i name'="" s %cgi(name)=value
 . i sort=6 s %var=item
 . i sort=7 s name=$p(item,"=",1),value=$p(item,"=",2,9999) i name'="" s %var(name)=value
 . i sort=8 s name=$p(item,"=",1),value=$p(item,"=",2,9999) i name'="" s %sys(name)=value
 . q
 s %sys("no")=$$dsize256($g(%sys("no")))
 s no=$g(%sys("no"))+0
 i $g(%sys("key"))=$c(0,0,0,0,0,0,0,0,0,0) s %sys("key")=""
 i $g(%sys("key"))'="" s res=$$dbxweblr(.%cgi,.%var,.%sys) i res'="" g dbxwebe1
 s %sys("server")=$p($g(%sys("server")),$c(0),1)
 s %sys("server_no")=+$g(%sys("server_no"))
 i $g(%sys("wsfunction"))'="" q $$mgwebsock(.%cgi,.%var,.%sys)
 i $g(%sys("function"))="" q $$esize256(no)_$c(0)_$$mgweb(.%cgi,.%var,.%sys)
 s uci=$$getuci()
 i $g(%sys("key"))'="" s res=$$dbxweb1(.%cgi,.%mgweb,.%sys) g dbxweb2
 s res=$$dbxweb1(.%cgi,.%var,.%sys)
dbxweb2 ; request serviced
 k %mgweb
 i $g(%sys("sse")) h
 i '$g(%sys("stream")) q $$esize256(no)_$c(0)_res
 s len=$l(res) i len d
 . i $g(%sys("stream"))=1 d writex(res,len) q
 . w res
 . q
 d streamx(.%sys)
 q $c(255,255,255,255)
dbxwebe ; Error
 s res=$$error()
dbxwebe1 ; Error - non M
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbxwebe2^%zmgsis"
 d:$g(^%zmgsi("loglevel"))>0 event(res)
 ; with API mode we must not halt the process
 i $g(%sys("mode"))="api" q $$esize256($g(no)+0)_$c(0)_res
 i '$g(%sys("stream")) q $$esize256($g(no)+0)_$c(0)_res
 ; with streamed output we must halt and close connection
 i $g(%sys("stream"))=1 d writex(res,$l(res))
 i $g(%sys("stream"))=2 w res
 d streamx(.%sys) w $c(255,255,255,255) d flush
dbxwebe2 ; Halt process if we cannot write to the primary device
 h
 ;
dbxweb1(%cgi,%var,%sys)
 n (%cgi,%var,%sys)
 q @("$$"_%sys("function")_"(.%cgi,.%var,.%sys)")
 ;
dbxweblr(%cgi,%var,%sys) ; read long request payload
 n head,len,type,data,key,cn,rc
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbxweblre^%zmgsis"
 s key=$$dsize256($e(%sys("key"),1,4))_":"_$$dsize256($e(%sys("key"),5,8))
 s %sys("key")=key
 ; with API mode the content would have been pre-loaded into a global so nothing more to do here
 i $g(%sys("mode"))="api" q ""
 s cn=0
 f  d  i len=0 q
 . r head#5
 . s len=$$dsize256(head),cmnd=$a($e(head,5))
 . s sort=$a(head,5)\20,type=$a(head,5)#20
 . i sort=9 s len=0 q
 . i len>0 r data#len s cn=cn+1,rc=$$dbxwebsn(key,cn,data,0)
 . q
 q ""
dbxweblre
 q $$error()
 ;
dbxwebsn(key,cn,data,option) ; set a chunk of request data
 i key=""!(cn="") q 0
 i 'option k:cn=1 %mgweb s %mgweb(cn)=data q 0
 k:cn=1 ^mgweb(key) s ^mgweb(key,cn)=data
 q 0
 ;
mgweb(%cgi,%var,%sys)
 n %r,offset,ok,x,sort,type,item,ctx,fun,len,name,value,request,data,param,no,header,content
 s header="HTTP/1.1 200 OK"_$c(13,10)_"Content-type: text/html"_$c(13,10)_"Connection: close"_$c(13,10)_$c(13,10)
 s content="<html>"_$c(13,10)_"<head><title>It Works</title></head>"_$c(13,10)_"<h1>It Works!</h1>"_$c(13,10)
 s content=content_"%zmgsi: v"_$p($$v(),".",1,3)_" ("_$zd(+$h,2)_")"_$c(13,10)
 s content=content_"database: "_$zv_$c(13,10)
 s content=content_"</html>"_$c(13,10)
 q (header_content)
 ;
mgwebsock(%cgi,%var,%sys)
 n (%cgi,%var,%sys)
 new $ztrap set $ztrap="zgoto "_$zlevel_":mgwebsocke^%zmgsis"
 s @("res="_"$$"_%sys("wsfunction")_"(.%cgi,.%var,.%sys)")
 d flush^%zmgsis
 h
mgwebsocke ; Error
 s res="HTTP/2 200 OK"_$c(13,10)_"Error: "_$$error()_$c(13,10,13,10)
 w $$esize256^%zmgsis($l(res)+5)_$c(0)_$$esize256^%zmgsis(($g(%sys("no"))+0))_$c(0)_res d flush^%zmgsis
 h
 ;
websocket(%sys,binary,options)
 n res
 s res="HTTP/2 200 OK"_$c(13,10)_"Binary: "_($g(binary)+0)_$c(13,10,13,10)
 w $$esize256^%zmgsis($l(res)+5)_$c(0)_$$esize256^%zmgsis(($g(%sys("no"))+0))_$c(0)_res d flush^%zmgsis
 q ""
 ;
sse(%sys,options)
 n res
 s res="HTTP/1.1 200 OK"_$c(13,10)_"Content-Type: text/event-stream"_$c(13,10)_"Cache-Control: no-cache"_$c(13,10)_"Connection: keep-alive"_$c(13,10,13,10)
 w $$esize256^%zmgsis($l(res)+5)_$c(0)_$$esize256^%zmgsis(($g(%sys("no"))+0))_$c(0)_res d flush^%zmgsis
 q ""
 ;
clientgone(%sys) ; see if we have received the client aborted message
 n rc,eof
 s rc=0
 r eof#5:0
 i eof=$c(255,255,255,255,255) s rc=1
 q rc
 ;
stream(%sys) ; set up device for streaming the response (block/binary protocol)
 n %stream
 s %stream=""
 s %sys("stream")=1
 w $c(0,0,0,0,0)_$$esize256(($g(%sys("no"))+0))_$c(1) d flush
 q %stream
 ;
streamascii(%sys) ; set up device for streaming the response (ASCII protocol)
 n %stream
 s %stream=""
 s %sys("stream")=2
 w $c(0,0,0,0,0)_$$esize256(($g(%sys("no"))+0))_$c(2) d flush
 q %stream
 ;
streamx(%sys)
 d flush
 i $g(%sys("stream"))=2 q
 q
 ;
write(%stream,content) ; write out response payload
 n len1,len2,len3
 s len1=$l(%stream)
 s len2=$l(content)
 i (len1+len2)<8192 s %stream=%stream_content q
 s len3=(8192-len1) i len3<0 s len3=0 
 i len3 s %stream=%stream_$e(content,1,len3),content=$e(content,len3+1,len2),len1=len1+len3
 d writex(%stream,len1)
 s %stream=content
 q
 ;
writex(content,len) ; write out response payload
 w $$esize256(len)_content
 q
 ;
w(%stream,content) ; write out response payload
 d write(.%stream,content)
 q
 ;
writem(%zcs,content,flush)
 n error
 i $d(%zcs("pwnd")) d &pwind.tcpwrite(content,1,.error) q
 w content
 q
 ;
content(%nv,%nvhead,%payload,%cgi) ; generic function for parsing request payload
 n ct,boundary
 s ct=$g(%cgi("CONTENT_TYPE")) i ct="" q 1
 i ct["boundary=" s boundary=$p(ct,"boundary=",2,999) s:$e(boundary)="""" @("boundary="_boundary) q $$multipart(.%nv,.%nvhead,.%payload,boundary)
 i ct["form-urlencoded" q $$nvpair(.%nv,.%payload)
 q 1
 ;
nvpair(%nv,%payload) ; parse content type: application/x-www-form-urlencoded
 n i,x,def,name,value,value1
 s %payload=$g(%payload)
 i $d(%payload(1)),'$d(%payload(2)) s x=$g(%payload(1)) k %payload s %payload=x
 i $d(%payload(1)) s:$g(%payload)'="" %payload(0)=%payload q $$nvpair1(.%nv,.%payload)
 f i=1:1:$l(%payload,"&") s x=$p(%payload,"&",i),name=$$urld($p(x,"=",1)),value=$$urld($p(x,"=",2,9999999)) i name'="" d
 . s def=$d(%nv(name))
 . i 'def s %nv(name)=value q
 . i def#10 s value1=$g(%nv(name)) k %nv(name) s %nv(name,1)=value1,%nv(name,2)=value q
 . s %nv(name,$o(%nv(name,""),-1)+1)=value q
 . q
 q 1
 ;
nvpair1(%nv,%payload) ; parse large application/x-www-form-urlencoded payload in an array
 n cn,cn1,cc,cc1,maxsize,rc,tmp
 s cn=$o(%payload("")),cn1="",cc=1,cc1=1
 s maxsize=$$getmsl()-10
 f  s cn1=$o(%payload(cn1)) q:cn1=""  d
 . f  s cc1=$f(%payload(cn1),"&",cc1) q:'cc1  d
 . . s rc=$$nvpair2(.tmp,cn,cc,cn1,cc1)
 . . s rc=$$nvpair4(.%nv,.tmp)
 . . s cn=cn1,cc=cc1
 . . q
 . q
 s rc=$$nvpair2(.tmp,cn,cc,$o(%payload(""),-1),999999)
 s rc=$$nvpair4(.%nv,.tmp)
 q 1
 ;
nvpair2(tmp,cn,cc,cn1,cc1) ; extract single name/value pair
 n n,x,rc
 k tmp
 f n=cn:1:cn1 d
 . s x=$g(%payload(n)) i x="" q
 . i cn=cn1 s rc=$$nvpair3(.tmp,$e(x,cc,cc1-2),maxsize) q
 . i n=cn s rc=$$nvpair3(.tmp,$e(x,cc,999999),maxsize) q
 . i n=cn1 s rc=$$nvpair3(.tmp,$e(x,1,cc1-2),maxsize) q
 . s rc=$$nvpair3(.tmp,x,maxsize)
 . q
 q 0
 ;
nvpair3(tmp,chunk,maxsize) ; record single name/value pair in array
 n len1,len2,x,n
 s n=$o(tmp(0,""),-1) i n="" s n=1
 s x=$g(tmp(0,n))
 s len1=$l(x)
 s len2=$l(chunk) i len2=0 q 0
 i len1<maxsize s x=x_$e(chunk,1,maxsize-len1),tmp(0,n)=x,x=$e(chunk,(maxsize-len1)+1,9999999),len2=$l(x)
 i len2 s n=n+1,tmp(0,n)=x
 q 0
 ;
nvpair4(%nv,tmp) ; add name/value pair to final array
 n n,x,name,len,vno,tmp1
 s x=$g(tmp(0,1)),name=$p(x,"=",1),tmp(0,1)=$p(x,"=",2,9999999)
 i name="" q 0
 i '$d(tmp(0,2)) s x=$g(tmp(0,1)) k tmp s tmp=$$urld(x)
 f n=1:1 q:'$d(tmp(0,n))  d   s tmp(0,n)=$$urld(x)
 . s x=$g(tmp(0,n)),len=$l(x)
 . i $e(x,len)="%",$d(tmp(0,n+1)) s x=x_$e(tmp(0,n+1),1,2),tmp(0,n+1)=$e(tmp(0,n+1),3,9999999) q
 . i $e(x,len-1)="%",$d(tmp(0,n+1)) s x=x_$e(tmp(0,n+1),1,1),tmp(0,n+1)=$e(tmp(0,n+1),2,9999999) q
 . q
 i $d(%nv(name)),'$d(%nv(name,1)) d  q 0
 . m tmp1=%nv(name)
 . k %nv(name)
 . m %nv(name,1)=tmp1,%nv(name,2)=tmp
 . q
 i $d(%nv(name,1)) d  q 0
 . s vno=$o(%nv(name,""),-1)+1 m %nv(name,vno)=tmp
 . q
 m %nv(name)=tmp
 q 0
 ;
multipart(%content,%nvhead,%payload,%boundary) ; parse content type: multipart/form-data
 n blen,sn1,sn2,snh,snc,snx,sn,n,headers,content,rc
 s %payload=$g(%payload)
 s blen=$l(%boundary) i blen="" q 1
 ; s %payload(0)=%payload q $$multipart1(.%content,.%nvhead,.%payload,%boundary,blen)
 i $d(%payload(1)) s %payload(0)=%payload q $$multipart1(.%content,.%nvhead,.%payload,%boundary,blen)
 s sn1=$f(%payload,%boundary,1),sn=0
 f  s sn2=$f(%payload,%boundary,sn1) q:'sn2  d  s sn1=sn2
 . ; move end point to start of terminating boundary marker
 . s snx=sn2-(blen+1)
 . ; move end point to point before '--' sequence introducing the boundary 
 . i $e(%payload,snx-1,snx)="--" s snx=snx-2
 . ; move end point to point before 'crlf' sequence introducing the boundary 
 . i $e(%payload,snx-1,snx)=$c(13,10) s snx=snx-2
 . s snc=sn1+2 ; move start point to after the trailing 'crlf' sequence after the previous boundary
 . s headers="",sname="",snh=$f(%payload,$c(13,10,13,10),snc) i snh,snh'>(snx+1) s headers=$e(%payload,snc,snh-5),snc=snh
 . s content=$e(%payload,snc,snx)
 . s rc=$$msection(.%content,.%nvhead,headers,.content,.sn)
 . q
 q 1
 ;
msection(%content,%nvhead,headers,data,sn) ; Set a single component in a multipart payload
 n harray,temp,temphead,n,header,hname,hvalue,sname,def
 ; process the headers for the section
 f n=1:1 s header=$p(headers,$c(13,10),n) q:header=""  d
 . s hname=$$trim($p(header,":",1)," "),hvalue=$$trim($p(header,":",2,9999999)," ") i hname="" q
 . i hname="Content-Disposition" s sname=$p($p(hvalue,"name=""",2),"""",1)
 . s harray(hname)=hvalue
 . q
 ; generate a section name if we don't have one
 i sname="" s sn=sn+1,sname="content"_sn
 s def=$d(%content(sname))
 i 'def m %nvhead(sname)=harray m %content(sname)=data q 1
 i def#10 m temp=%content(sname),temphead=%nvhead(sname) k %content(sname),%nvhead(sname) m %content(sname,1)=temp,%nvhead(sname,1)=temphead,%nvhead(sname,2)=harray,%content(sname,2)=data q
 s n=$o(%content(sname,""),-1)+1 m %nvhead(sname,n)=harray,%content(sname,n)=data q
 q 1
 ;
multipart1(%content,%nvhead,%payload,%boundary,blen) ; Large multipart payload in an array
 n n,nn,sn,cnprev,hendprev,hycurr,bno,cn,hy,c,bend,x,len,cncurr,hycurr,chunkno,rc,headers,ok
 s n=0
 s sn=0,cnprev=0,hendprev=0,hycurr=0,bno=0
 s cn=0,hy=1 f  q:'$d(%payload(cn))  s hy=$f(%payload(cn),"-",hy) d
 . i 'hy s cn=cn+1,hy=1 q
 . s c=$e(%payload(cn),hy) i c="" s cn=cn+1,hy=1,c=$e($g(%payload(cn)),hy)
 . i c'="-" q
 . s bend=(hy+1+blen)-1,x=$e(%payload(cn),hy+1,bend)
 . s len=$l(x) i len<blen s cn=cn+1,bend=(blen-len),x=$e($g(%payload(cn)),1,bend)
 . i x'=boundary q
 . s bno=bno+1
 . s cncurr=cn
 . i bno=1 s hycurr=hy-2
 . i bno>1 s hycurr=hy-4 i hycurr<1 s cncurr=cn-1,hycurr=$l($g(%payload(cncurr)))+hycurr
 . ; CRLF before boundary marker
 . s bend=bend+1,c=$e($g(%payload(cn)),bend) i c="" s cn=cn+1,bend=1,c=$e($g(%payload(cn)),n)
 . s bend=bend+1,c=$e($g(%payload(cn)),bend) i c="" s cn=cn+1,bend=1,c=$e($g(%payload(cn)),n)
 . i hendprev d
 . . n data
 . . s chunkno=0 f nn=cnprev:1:cn d
 . . . i cn=cnprev s data=$e($g(%payload(nn)),hendprev+1,hycurr) q
 . . . i nn=cnprev s chunk=$e($g(%payload(nn)),hendprev+1,9999999),chunkno=chunkno+1,data(chunkno)=chunk q
 . . . i nn=cn s chunk=$e($g(%payload(nn)),1,hycurr),chunkno=chunkno+1,data(chunkno)=chunk q
 . . . s chunkno=chunkno+1,data(chunkno)=$g(%payload(nn)) q
 . . . q
 . . s rc=$$msection(.%content,.%nvhead,headers,.data,.sn) q
 . . q
 . s n=bend+1,hend=n,headers="",ok=0 f  d  i ok q
 . . s c=$e($g(%payload(cn)),n) i c="" s cn=cn+1,n=1,c=$e($g(%payload(cn)),n)
 . . i c="" s ok=1 q
 . . s headers=headers_c i headers[$c(13,10,13,10) s hend=n,ok=1 q
 . . s n=n+1
 . . q
 . s hendprev=hend,cnprev=cn
 . q
 q 1
 ;
urld(%val) ; URL decode (unescape)
 new $ztrap set $ztrap="zgoto "_$zlevel_":urlde^%zmgsis"
 q $$urldx(%val)
urlde ; error
 q $$urldx(%val)
 ;
urldx(%val) ; URL deoode the long way
 n i,c,%vald
 s %vald=""
 f i=1:1:$l(%val) s c=$e(%val,i) d
 . i c="+" s %vald=%vald_" " q
 . i c="%" s %vald=%vald_$c($$dsize($e(%val,i+1,i+2),2,16)),i=i+2 q
 . s %vald=%vald_c
 . q 
 q %vald
 ;
urle(%val) ; URL encode (escape)
 new $ztrap set $ztrap="zgoto "_$zlevel_":urlee^%zmgsis"
 q $$urlex(%val)
urlee ; error
 q $$urlex(%val)
 ;
urlex(%val) ; URL encode the long way
 n i,c,a,len,%vale
 s %vale=""
 f i=1:1:$l(%val) s c=$e(%val,i) d
 . s a=$a(c)
 . i a'<48,a'>57 s %vale=%vale_c q
 . i a'<65,a'>90 s %vale=%vale_c q
 . i a'<97,a'>122 s %vale=%vale_c q
 . s len=$$esize(.c,a,16)
 . s %vale=%vale_"%"_c q
 . q 
 q %vale
 ;
odbcdt(x) ; Translate ODBC data type code to readable form
 q $s(x=0:"Unknown",x=1:"CHAR",x=2:"NUMERIC",x=3:"DECIMAL",x=4:"INTEGER",x=5:"SMALLINT",x=6:"FLOAT",x=7:"REAL",x=8:"DOUBLE",x=9:"DATE",x=10:"TIME",x=11:"TIMESTAMP",x=12:"VARCHAR",x=-11:"GUID",x=-10:"WLONGVARCHAR",x=-9:"WVARCHAR",x=-7:"BIT",x=-6:"TINYINT",x=-5:"BIGINT",x=-4:"LONGVARBINARY",x=-3:"VARBINARY",x=-2:"BINARY",x=-1:"LONGVARCHAR",1:"")
 ;
 ; s x=$$sqleisc^%zmgsis(0,"SELECT * FROM SQLUser.customer","")
sqleisc(id,sql,params) ; Execute InterSystems SQL query
 new $ztrap set $ztrap="zgoto "_$zlevel_":sqleisce^%zmgsis"
 n %objlasterror,result,error,data,status,tsql,cn,col,type,dtype,rset,rn,n,sort,type,utf16
 s utf16=0 i $g(params)["utf16" s utf16=1 ; v4.5.29
 k ^mgsqls($j,id)
 s result="0",error="",data="",cn=0
 s error="InterSystems SQL not available with YottaDB" g sqleisce1
 s sort=1,type=1,result=$$esize256($l(cn))_$c((sort*20)+type)_cn_data
 q $$esize256($l(result))_result
sqleisce ; M error
 s error=$$error()
sqleisce1 ; SQL error
 s sort=11,type=1,result=$$esize256($l(error))_$c((sort*20)+type)_error
 q $$esize256($l(result))_result
 ;
 ; s x=$$sqlemg^%zmgsis(0,"select * from patient","")
sqlemg(id,sql,params) ; Execute MGSQL SQL query
 new $ztrap set $ztrap="zgoto "_$zlevel_":sqlemge^%zmgsis"
 n %zi,%zo,result,error,data,cn,n,col,type,ok,sort,type,v,utf16
 s utf16=0 i $g(params)["utf16" s utf16=1 ; v4.5.29
 s result="0",error="",data="",cn=0
 s %zi("stmt")=id
 s v=$$sqlmgv() i v="" s error="MGSQL not installed" g sqlemge1
 i $$isydb() s %zi(0,"recompile")=1
 s ok=$$exec^%mgsql("",sql,.%zi,.%zo)
 i $d(%zo("error")) s error=$g(%zo("error")) g sqlemge1
 s sort=1,type=1
 f n=1:1 q:'$d(%zo(0,n))  s col=$g(%zo(0,n)) d
 . i col["." s col=$p(col,".",2)
 . s col=$tr(col,"-_.;","")
 . i col="" s col="column_"_n
 . s type=$s($d(%zo(0,n,0)):$g(%zo(0,n,0)),1:"varchar")
 . s ^mgsqls($j,%zi("stmt"),0,0,n)=col
 . s ^mgsqls($j,%zi("stmt"),0,0,n,0)=type
 . s col=col_"|"_type
 . s data=data_$$esize256($l(col))_$c((sort*20)+type)_col
 . q
 s cn=n-1
 s sort=1,type=1,result=$$esize256($l(cn))_$c((sort*20)+type)_cn_data
 q $$esize256($l(result))_result
sqlemge ; M error
 s error=$$error()
sqlemge1 ; SQL error
 s sort=11,type=1,result=$$esize256($l(error))_$c((sort*20)+type)_error
 q $$esize256($l(result))_result
 ;
sqlmgv() ; get MGSQL version
 new $ztrap set $ztrap="zgoto "_$zlevel_":sqlmgve^%zmgsis"
 s v=$$v^%mgsql()
 q v
sqlmgve ; M error
 q ""
 ;
sqlrow(id,rn,params) ; Get a row
 n result,data,n,sort,type,utf16
 s utf16=0 i $g(params)["utf16" s utf16=1 ; v4.5.29
 s result=""
 s sort=1,type=1
 i params["-1" s:rn=0 rn="" s rn=$o(^mgsqls($j,id,0,rn),-1) i rn=0 s rn=""
 i params["+1" s:rn="" rn=0 s rn=$o(^mgsqls($j,id,0,rn))
 i rn="" q result
 i '$d(^mgsqls($j,id,0,rn)) q result
 s result=result_$$esize256($l(rn))_$c((sort*20)+type)_rn
 f n=1:1 q:'$d(^mgsqls($j,id,0,rn,n))  d
 . s data=$g(^mgsqls($j,id,0,rn,n)) i utf16 s data=$$utf8out(data) ; v4.5.29
 . s result=result_$$esize256($l(data))_$c((sort*20)+type)_data
 . q
 q $$esize256($l(result))_result
 ;
sqldel(id,params) ; Delete SQL result set
 n utf16
 s utf16=0 i $g(params)["utf16" s utf16=1 ; v4.5.29
 k ^mgsqls($j,id)
 q ""
 ;
event(text) ; log m-side event
 n i,x,y,n,emax
 new $ztrap set $ztrap="zgoto "_$zlevel_":evente^%zmgsis"
 f i=1:1 s x=$e(text,i) q:x=""  s y=$s(x=$c(13):"\r",x=$c(10):"\n",1:"") i y'="" s $e(text,i)=y
 s emax=100 ; maximum log size (no. messages)
 l +^%zmgsi("log")
 s n=$g(^%zmgsi("log")) i n="" s n=0
 s n=n+1,^%zmgsi("log")=n
 l -^%zmgsi("log")
 s ^%zmgsi("log",n,0)=$$head(),^%zmgsi("log",n,1)=text
 f n=n-emax:-1 q:'$d(^%zmgsi("log",n))  k ^(n)
 q
evente ; error
 q
 ;
ddate(date) ; decode m date
 new $ztrap set $ztrap="zgoto "_$zlevel_":ddatee^%zmgsis"
 q $zd(date,2)
ddatee ; no $zd function
 q date
 ;
dtime(mtime,format) ; decode m time
 n h,m,s
 i mtime="" q ""
 i mtime["," s mtime=$p(mtime,",",2)
 i format=0 q (mtime\3600)_":"_(mtime#3600\60)
 s h=mtime\3600,s=mtime-(h*3600),m=s\60,s=s#60
 q $s(h<10:"0",1:"")_h_":"_$s(m<10:"0",1:"")_m_":"_$s(s<10:"0",1:"")_s
 ;
head() ; format header record
 n uci
 new $ztrap set $ztrap="zgoto "_$zlevel_":heade^%zmgsis"
 s uci=$$getuci()
heade ; error
 q $$ddate(+$h)_" at "_$$dtime($p($h,",",2),0)_"~"_$g(%zcs("port"))_"~"_uci
 ;
gparse(key,ref) ; parse global reference
 n i,x,y,blk,dlm,sort,type
 k key s key=0
 s blk="",sort=1,type=1
 s x=$p($e(ref,1,$l(ref)-1),"(",2,9999999)
 f i=1:1:$l(x,",") s x(i)=$p(x,",",i)
 s y="",dlm="" f i=1:1 q:'$d(x(i))  d
 . s y=y_dlm_x(i),dlm="," i ($l(y,"""")#2) s key=key+1,key(key)=y s:y["""" @("key(key)="_y) s y="",dlm="",blk=blk_$$esize256($l(key(key)))_$c((sort*20)+type)_key(key)
 . q
 q blk
 ;
empty() ; empty string marker
 n sort,type
 s sort=1,type=0
 q $c(0,0,0,0,(sort*20)+type)
 ;
eod() ; eod of data marker
 n sort,type
 s sort=9,type=0
 q $c(0,0,0,0,(sort*20)+type)
 ;
