%zmgsi ;(CM) Service Integration - Core Server
 ;
 ;  ----------------------------------------------------------------------------
 ;  | %zmgsi                                                                   |
 ;  | Author: Chris Munt cmunt@mgateway.com, chris.e.munt@gmail.com            |
 ;  | Copyright (c) 2016-2021 M/Gateway Developments Ltd,                      |
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
a0 d vers^%zmgsis
 q
 ;
defport() ; Return the default TCP port number for this service.
 q 7041
 ;
start(port,tls) ; start daemon
 n pwnd,v,vn
 new $ztrap set $ztrap="zgoto "_$zlevel_":starte^%zmgsi"
 i $g(port)="" s port=$$defport()
 s pwnd=0 i port["pw",port[":" s pwnd=1,port=$p(port,":",2)
 s port=+$g(port)
 i 'port s port=$$defport()
 k ^%zmgsi("stop",port)
 ; Concurrent tcp service (Cache, IRIS, M21, MSM, YottaDB)
 i '$$isidb^%zmgsis(),'$$ism21^%zmgsis(),'$$ismsm^%zmgsis(),'$$isydb^%zmgsis() g starte
 i 'pwnd j accept($g(port),$g(tls)) q
 d &pwind.version(.v)
 s vn=$p(v,":",2)
 s vn=$p(vn,".",1)*100+($p(vn,".",2)*10)
 i vn<120 w !,"This version of mg_pwind ("_v_") does not support networking" g starte
 j acceptpw($g(port),$g(tls))
 q
starte ; error
 w !,"This M system does not support a concurrent TCP server"
 q
 ;
eeestart ; start
 d start(0)
 q
 ;
stop(port) ; stop
 n pwnd,job
 w !,"Terminating the %zmgsi service ... "
 i $g(port)="" s port=$$defport()
 s pwnd=0 i port["pw",port[":" s pwnd=1,port=$p(port,":",2)
 s port=+$g(port)
 i 'port s port=$$defport()
 s ^%zmgsi("stop",port)=1
 s job=$g(^%zmgsi("server",port))
 d killproc(job)
stopx ; service should have terminated
 k ^%zmgsi("server",port)
 w !!,"%zmgsi service terminated",!
 q
 ;
killproc(pid) ; stop this listener
 i '$l(pid) q
 w !,"stop: "_pid
 zsy "kill -term "_pid
 q
 ;
accept(port,tls) ; concurrent tcp service (cache, m21, msm)
 new $ztrap set $ztrap="zgoto "_$zlevel_":halt^%zmgsi"
 d seterror^%zmgsis("")
 s port=+$g(port)
 i 'port s port=7041
 s ^%zmgsi("server",port)=$j
 s dev="server$"_$j
 s errors=0
 s timeout=10
accept1 ; main accept loop
 ; set up socket server
 c dev
 ; open tcp server device
 open dev:(listen=port_":tcp":attach="server"):timeout:"socket"
 ;
 ; use tcp server device
 use dev
 write /listen(5) 
 ;
accept2 ; accept connection
 new $ztrap set $ztrap="zgoto "_$zlevel_":accepte^%zmgsi"
 set %znsock="",%znfrom="",timeout=30
 s ok=1 f  d  q:ok  i $d(^%zmgsi("stop")) s ok=0 k ^%zmgsi("stop") q
 . write /wait(timeout)
 . i $key'="" s ok=1 q
 . s ok=0
 . q
 i 'ok g acceptx
 i $g(^%zmgsi("loglevel"))>0 d event^%zmgsis("incoming connection from "_$piece($key,"|",3)_", starting child server process")
 s childsock=$p($key,"|",2)
 u dev:(detach=childsock)
 s childproc="child^%zmgsis(port,port,tls):(output="_"""SOCKET:"_childsock_""""_":input="_"""SOCKET:"_childsock_""""_")"
 j @childproc ; fork a process to handle the detached socket
 ;
 s errors=0
 g accept2
acceptx ; exit
 d event^%zmgsis("closing server")
 c dev
 q
 ;
accepte ; error
 new $ztrap set $ztrap="zgoto "_$zlevel_":halt^%zmgsi"
 s errors=errors+1
 i $$error^%zmgsis()["int" h
 d event^%zmgsis("accept loop - program error: "_$$error^%zmgsis())
 i errors>7 d event^%zmgsis("accept loop - too many errors - closing down") h
 i $g(dev)'="" u dev
 g accept2
halt ; halt
 h
 ;
acceptpw(port,tls) ; concurrent tcp service (YottaDB/mg_pwind)
 n key,error,ds
 new $ztrap set $ztrap="zgoto "_$zlevel_":acceptpwe^%zmgsi"
 s ^%zmgsi("server",port)=$j
 d &pwind.tcpservinit(port,"",.error) i error'="" d event^%zmgsis("mg_pwind error (tcpservinit): "_error) g acceptpwx
acceptpw1 ; main accept loop
 s key="",error=""
acceptpw2 ; main accept retry loop
 i $d(^%zmgsi("stop",port)) k ^%zmgsi("stop",port) g acceptpwx
 d &pwind.tcpservaccept(.key,.error)
 i error="<EINTR>" s error="" g acceptpw2
 i error="<TIMEOUT>" s error="" g acceptpw2
 i error'="" d event^%zmgsis("mg_pwind error (tcpservaccept): "_$h_error) g acceptpwx
 s ds=$p(key,"|",5) i ds'="",'$d(^%zmgsi("server",port,"ds")) s ^%zmgsi("server",port,"ds")=ds
 i key="" h 1 g acceptpw1
 i $g(^%zmgsi("loglevel"))>0 d event^%zmgsis("incoming connection from "_$piece(key,"|",6)_", starting child server process")
 j child^%zmgsis(key,port,tls)
 g acceptpw1
acceptpwx ; exit
 s ds=$g(^%zmgsi("server",port,"ds")) i ds="" s ds="server"
 d &pwind.tcpservclose(ds)
 d event^%zmgsis("mg_pwind server process exit")
 h
acceptpwe ; error
 s error=$$error^%zmgsis()
 d event^%zmgsis("mg_pwind error: "_error)
 h
 ;
ylink ; link all routines
 ;;zlink "_zmgsi.m"
 zlink "_zmgsis.m"
 q
 ;
