%zmgmq ;(CM) Message Queue Management
 ;
 ;  ----------------------------------------------------------------------------
 ;  | %zmgsi                                                                   |
 ;  | Author: Chris Munt cmunt@mgateway.com, chris.e.munt@gmail.com            |
 ;  | Copyright (c) 2016-2022 MGateway Ltd                                     |
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
 ; v1.0.1:    7 March     2022 (Initial release)
 ; v1.0.2:   17 March     2022 (Add pwind wait/signal mechanism to improve performance)
 ; 
v() ; version and date
 n v,r,d
 s v="1.0"
 s r=2
 s d="17 March 2022"
 q v_"."_r_"."_d
 ;
vers ; version information
 n v
 s v=$$v()
 w !,"MGateway Ltd - Message Queue Manager"
 w !,"Version: "_$p(v,".",1,2)_"; Revision "_$p(v,".",3)_" ("_$p(v,".",4)_")"
 w !
 q
 ;
start(no,conf,param) ; Start the required number of workers
 n n,pwndv,nosig,int
 k ^zmgmq("stop"),^zmgmq("rt")
 s param=$$lcase($g(param))
 s nosig=$s(param["nosig":1,1:0) i nosig s ^zmgmq("rt","ns")=0
 s int=$s(param["int":1,1:0)
 i nosig s pwndv=$$pwind() i $p(pwndv,".",3)<6 w !,"Install mg_pwind build 6 (or later)" q
 s no=+$g(no)
 i int d work($g(conf),nosig,int) q
 f n=1:1:no J work($g(conf),nosig,int)
 q
 ;
stop() ; Close down the message queue workers
 n rc
 s rc=$$mess("s")
 q
 ;
reset() ; Reset work queue
 k ^zmgmq("worker")
 k ^zmgmq("queue")
 k ^zmgmq("task")
 k ^zmgmq("stop")
 k ^zmgmq("rt")
 q
 ;
work(conf,nosig,int) ; Worker
 n tn,n,r,cpid,type,rc,idle,res
 s ^zmgmq("worker",$j)=""
 i conf'="",$d(^zmgmq("conf",conf,"db")) d dbopen(conf)
 s idle=0
loop ; Message processing loop
 i $d(^zmgmq("stop")) k ^zmgmq("worker",$j) g exit
 l +^zmgmq("queue")
 s tn=$o(^zmgmq("queue",""))
 i tn'="" k ^zmgmq("queue",tn)
 l -^zmgmq("queue")
 i tn="" g wait
 s idle=0
 s r=$g(^zmgmq("task",tn)),cpid=$p(r,"~",1),type=$p(r,"~",2)
 s res=$$task(tn,cpid,type)
 s ^zmgmq("task",tn,"result")=res
 i nosig g loop
 s rc=$&pwind.signal(cpid)
 g loop
wait ; Wait for messages to come in
 s idle=idle+1
 i 'nosig s n=$&pwind.signalwait(.rc,$s(idle<100:10,1:100)) g loop
 h $s(idle<100:0.01,1:0.1)
 g loop
exit ; Exit
 i conf'="",$d(^zmgmq("conf",conf,"db")) d dbclose()
 q
 ;
task(tn,cpid,type) ; Do task
 n conf,nosig,int,idle,r,n,fun,res,var
 new $ztrap set $ztrap="zgoto "_$zlevel_":taske^%zmgmq"
 s res=""
 i type="v" s res=$$v() q res
 i type="p" s res="OK "_$j q res
 i type="s" s res="OK STOP" s ^zmgmq("stop")="" q res
 i type="f" d  q res
 . s fun="$$"_$g(^zmgmq("task",tn,0))_"("
 . f n=1:1 q:'$d(^zmgmq("task",tn,n))  s:n>1 fun=fun_"," s fun=fun_"var("_n_")",var(n)=$g(^zmgmq("task",tn,n))
 . s fun=fun_")"
 . s @("res="_fun)
 . s ^zmgmq("task",tn,"result")=res
 q res
taske ; Error
 s res=$$error
 q res
 ;
dbopen(conf) ; Open ISC database
 n dbtype,path,host,port,username,password,namespace,rc
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbopene^%zmgmq"
 s dbtype=$g(^zmgmq("conf",conf,"db","dbtype"))
 s path=$g(^zmgmq("conf",conf,"db","path"))
 s host=$g(^zmgmq("conf",conf,"db","host"))
 s port=$g(^zmgmq("conf",conf,"db","port"))
 s username=$g(^zmgmq("conf",conf,"db","username"))
 s password=$g(^zmgmq("conf",conf,"db","password"))
 s namespace=$g(^zmgmq("conf",conf,"db","namespace"))
 s rc=$&pwind.dbopen(dbtype,path,host,port,username,password,namespace)
 q
dbopene ; Error
 q
 ;
dbclose()
 n rc
 new $ztrap set $ztrap="zgoto "_$zlevel_":dbclosee^%zmgmq"
 i conf'="",$d(^zmgmq("conf",conf,"db")) s rc=$&pwind.dbclose()
 q
dbclosee ; Error
 q
 ;
mess(type,p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
 n tn,n,res,nosig
 s nosig=$d(^zmgmq("rt","ns"))
 s tn=$$put(type,nosig)
 f n=0:1:9 q:'$d(@("p"_n))  s ^zmgmq("task",tn,n)=@("p"_n)
 s ^zmgmq("queue",tn)=""
 s res=$$get(tn,10,nosig)
 q res
 ;
fun(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
 n tn,n,res,nosig
 s nosig=$d(^zmgmq("rt","ns"))
 s tn=$$put("f",nosig)
 f n=0:1:9 q:'$d(@("p"_n))  s ^zmgmq("task",tn,n)=@("p"_n)
 s ^zmgmq("queue",tn)=""
 s res=$$get(tn,10,nosig)
 q res
 ;
funa(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9)
 n tn,n,nosig
 s nosig=$d(^zmgmq("rt","ns"))
 s tn=$$put("f",nosig)
 f n=0:1:9 q:'$d(@("p"_n))  s ^zmgmq("task",tn,n)=@("p"_n)
 s ^zmgmq("queue",tn)=""
 q tn
 ;
put(type,nosig)
 n tn,wpid,rc
 s tn=$i(^zmgmq("queue"))
 s ^zmgmq("task",tn)=$j_"~"_type
 i nosig q tn
 s wpid="" f  s wpid=$o(^zmgmq("worker",wpid))  q:wpid=""  s rc=$&pwind.signal(wpid)
 q tn
 ;
get(tn,timeout,nosig) ; Get result
 n res,rc,n,wait,twait
 i tn="" q ""
 i nosig d  g get1
 . s wait=0.01,twait=0 f n=1:1 q:$d(^zmgmq("task",tn,"result"))!(twait>timeout)  h wait s twait=twait+wait s:n>100 wait=0.1
 . q
 s wait=0.01,twait=0 f n=1:1 q:$d(^zmgmq("task",tn,"result"))!(twait>timeout)  s n=$&pwind.signalwait(.rc,(wait*1000)) s twait=twait+wait s:n>100 wait=0.1
get1 s res=$g(^zmgmq("task",tn,"result"))
 k ^zmgmq("task",tn)
 q res
 ;
pwind() ; check if pwind is installed and return version
 n rc,vers
 new $ztrap set $ztrap="zgoto "_$zlevel_":pwinde^%zmgmq"
 s rc=0,vers=""
 s rc=$&pwind.version(.vers)
 q vers
pwinde ; error
 q ""
 ;
lcase(string) ; convert to lower case
 q $tr(string,"ABCDEFGHIJKLMNOPQRSTUVWXYZ","abcdefghijklmnopqrstuvwxyz")
 ;
error() ; get last error
 q $zs
 ;
test(a) ; Test function - local
 q "test function received input '"_$g(a)_"' at "_$h_" (Worker process "_$j_")" 
 ;
testrf() ; Test function - remote
 n rc,res
 new $ztrap set $ztrap="zgoto "_$zlevel_":testrfe^%zmgmq"
 s res=""
 s rc=$&pwind.dbfunction(.res,"getzv^%zmgsis")
 q res
testrfe ; Error
 q $$error^%zmgmq()
 ;
testrc(mdate) ; Test classmethod - remote
 n rc,res
 new $ztrap set $ztrap="zgoto "_$zlevel_":testrce^%zmgmq"
 s res=""
 s rc=$&pwind.dbclassmethod(.res,"%Library.Date","LogicalToDisplay",mdate)
 q res
testrce ; Error
 q $$error^%zmgmq()
 ;
