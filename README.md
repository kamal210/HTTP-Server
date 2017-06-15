# HTTP-Server
This is a C application that reads RFC1945 Hypertext Trasfer Protocol -HTTP/1.0 implements an HTTP server, and returns the requiested page to the client.
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

