
The attached folder contains the following files
	httpserver.c
	httpserver.h
	Makefile
	Readme.txt
	index.html

//========================================================
//HTTPSERVER
//This is an application reads RFC1945 implements an HTTP server, and returns the requiested page to the client.
//========================================================
//Developed by: Kamal Panthi 
//Email: kamal210@hotmail.com
//        
//To Compile type:"gcc -g httpserver.c -o httpserver" 

//or Use Makefile to compile.
--> type make 


//To run server type: "./httpserver portnumber"




//======================================================== 
//BROWSER RUNNING INFORMATION
//========================================================

From any browser type: http://hostaddress:portnumber/index.html
--> hostaddress is that host address where httpserver is running
--> portnumber is that port where httpserver is running

==>When the client from a new IP requests a page..the client receives a welcome page 
   and the IP address of that client is stored in server.
==>If registered client sends request for a page than welcome page is not displayed..the desired requested page is displayed
==>The logfile.html is created if it doesnot exist already, 
==>The logfile.html saves the record of the request from the client browsers. 
   It stores the request arrival time, request resource and IP of requesting client.
==>This logfile can be viewed from client browsers by typing : http://hostaddress:portnumber/logfile.html
==>The server sleeps for 3 seconds on every 1024 bytes transferred to client
==>The server supports multiple client connections.

 
//======================================================== 
//ACKNOWLEDGEMENT:
//========================================================


http://guillaume.maillard.free.fr/libc/libc_403.html
http://publib.boulder.ibm.com/iseries/v5r1/ic2924/index.htm?info/apis/waitpid.htm
http://www.faqs.org/rfcs/rfc1945.html
http://www.paulgriffiths.net/program/c/webserv.php
Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 
Stevens, W.R.: Unix Network Programming, The Sockets Networking API, Vol. 1, 3rd Ed., Prentice Hall, 2004.
Internetworking with TCP/IP Vol. 3: Client-Server Programming and Application, Linux/POSIX Socket Version (Comer, D.E., Stevens, D.), 2000. 


//======================================================== 


//======================================================== 

 
