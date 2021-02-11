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
start(port) ; start daemon
 new $ztrap set $ztrap="zgoto "_$zlevel_":starte^%zmgsi"
 s port=+$g(port)
 k ^%zmgsi("stop")
 ; Concurrent tcp service (Cache, IRIS, M21, MSM, YottaDB)
 i $$isidb^%zmgsis()!$$ism21^%zmgsis()!$$ismsm^%zmgsis()!$$isydb^%zmgsis() j accept($g(port)) q
 w !,"This M system does not support a concurrent TCP server"
 q
starte ; error
 q
 ;
eeestart ; start
 d start(0)
 q
 ;
stop(port) ; stop
 w !,"Terminating the %zmgsi service ... "
 s pport=+$g(port) i pport="" q
 i 'pport s pport=7041
 s job=$g(^%zmgsi("server",pport))
 d killproc(job)
stopx ; service should have terminated
 k ^%zmgsi("server",pport)
 w !!,"%zmgsi service terminated",!
 q
 ;
killproc(pid) ; stop this listener
 i '$l(pid) q
 w !,"stop: "_pid
 zsy "kill -term "_pid
 q
 ;
accept(port) ; concurrent tcp service (cache, m21, msm)
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
 d event^%zmgsis("incoming connection from "_$piece($key,"|",3)_", starting child server process")
 s childsock=$p($key,"|",2)
 u dev:(detach=childsock)
 s childproc="child^%zmgsis(port,port):(output="_"""SOCKET:"_childsock_""""_":input="_"""SOCKET:"_childsock_""""_")"
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
ylink ; link all routines
 ;;zlink "_zmgsi.m"
 zlink "_zmgsis.m"
 q
 ;
