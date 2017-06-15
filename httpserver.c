/*
//========================================================
//HTTP SERVER (httpserver.c)
//========================================================

ASSIGNMENT NO 5
SUBMITTED on: 26th October 2008 23:00
Developed by: Kamal Panthi 
Email: kamal201@hotmail.com

The attached folder(tar.gz file) contains the following files
	httpserver.c
	httpserver.h
	Makefile
	Readme.txt
	index.html


        
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

*/
//==========included header files=========================
#include "httpserver.h"

//======================MAIN==============================
int main(int argc, char *argv[]) 
{

	int    fd_socket, CONN;// integer variables
	pid_t  pid;// pricessID
	struct sockaddr_in ser_sock,cli_sock; //sockaddress for server and client
        int sinSize;   
	bool FIRSTTIME=true;
	char cli_ip[16];
	FILE *f;

	//========Check for parameters=================================
	if(argc!=2)
	{
		//otherwise
      		fprintf(stderr, "Usage: ./httpserver PORT \n");
      		exit(EXIT_FAILURE);
   	}

	//========creating socket for communication=================================
        if ((fd_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
            perror("Socket");
            exit(EXIT_FAILURE);
        }

	//============fill the address buffer========================================
	memset(&ser_sock, 0, sizeof(ser_sock));	       
        ser_sock.sin_family = AF_INET;         
      	
	PORT =atoi(argv[1]);		
	ser_sock.sin_port = htons(PORT); 
   
        ser_sock.sin_addr.s_addr = INADDR_ANY; 
        bzero(&(ser_sock.sin_zero),8); 

	//===========================Binding socket to the Port====================================
        if (bind(fd_socket, (struct sockaddr *)&ser_sock, sizeof(struct sockaddr)) < 0) 
	{
            perror("Unable to bind");
            exit(EXIT_FAILURE);
        }

	//===========================Listen for the incoming connection==================
        if (listen(fd_socket, 1024) < 0) 
	{
            perror("Listen");
            exit(EXIT_FAILURE);
        }

	//===currenttime and local time converting=============
	CURRENTTIME = time (NULL);
	LOCALTIME = localtime (&CURRENTTIME);

	int flags, logfile;
	mode_t mode;
	flags=O_RDONLY;		
	logfile=open("logfile.html", flags, mode);						
	if(logfile<0)//if doesnot exist
	{
		//=====create logfile.html file==========
		f = fopen("logfile.html","w");// creates and opens log file
		fclose(f);
		memset(TEMPBUFFER,0,1024);
		strcat(TEMPBUFFER, "<html>\n<body>\n<TITLE>LOG</TITLE>\n<h4>SERVER LOG</h4>\n<p><br>");
		strcat(TEMPBUFFER, asctime(LOCALTIME));
		strcat(TEMPBUFFER, "    :    Server started.<br><br>\n");
		WRITETOFILE(TEMPBUFFER, "logfile.html");//write to logfile.html	

	}
	else//if the file already exists
	{
		memset(TEMPBUFFER,0,1024);
		strcat(TEMPBUFFER, "<br>");
		strcat(TEMPBUFFER, asctime(LOCALTIME));
		strcat(TEMPBUFFER, "    :    Server started.<br><br>\n");
		WRITETOFILE(TEMPBUFFER, "logfile.html");//write to logfile.html		
	}								
		
		
	
        
	//=============================Program Header==============================
	printf("\n ========================================");
	printf("\n 	     HTTP SERVER.");
	printf("\n ========================================");
	printf("\n Running on Port : %d",PORT);
	printf("\n Files:");
	printf("\n \tindex.html");
	printf("\n \tlogfile.html");
	printf("\n <Developed by: Kamal Panthi c0346747 >");	
	printf("\n ========================================\n");



	//===========While Loop================================================
	while ( 1 ) 
	{
	
		//=======new connection to the server===========
		sinSize = sizeof(struct sockaddr_in);
		//===========================Accept Incoming Connections===============================
		CONN = accept(fd_socket, (struct sockaddr *)&cli_sock,&sinSize);	    	
		if(CONN < 0)
		{
			perror("Server-accept() error");
			exit(EXIT_FAILURE);
		}

		//===currenttime and local time converting=============	
		CURRENTTIME = time (NULL);		
		LOCALTIME = localtime (&CURRENTTIME);

		//========copying ip address of client===============
		bzero(cli_ip,16);			
		strcpy(cli_ip,inet_ntoa(cli_sock.sin_addr));

		//=========Writing request received to log file============
		memset(TEMPBUFFER,0,1024);
		strcat(TEMPBUFFER, "<br>");
		strcat(TEMPBUFFER, asctime(LOCALTIME));
		strcat(TEMPBUFFER, "    :    Request received--[Client : ");
		strcat(TEMPBUFFER, cli_ip);
		strcat(TEMPBUFFER, ", Resource : ");
		WRITETOFILE(TEMPBUFFER, "logfile.html");	


		//===========Checking whether the connecting client is new or registered============
		int count=0;
		FIRSTTIME=true;
		int NEWOROLDVAL=0;
		if(BROWSERCOUNTER==0)//if there is no existing registered client
		{
			FIRSTTIME=true;
		}
		else//if there exists some registered ip
		{
			for(count=0; count<=BROWSERCOUNTER; count++)//loop browsercounter times
			{
				
				if(strlen(BROWSERINSTANCE[count].IP)!=0)
					if(strcmp(BROWSERINSTANCE[count].IP,cli_ip)==0)//check exists or not...if matched
					{
						FIRSTTIME=false;
						
					}
			}
		}
		if(FIRSTTIME==true)//if it has arrived first time
		{	
			
			strcpy(BROWSERINSTANCE[BROWSERCOUNTER].IP,cli_ip);//save the IP
			
			BROWSERCOUNTER++;//increase the number of registered ip
			

			NEWOROLDVAL=0;
		}
		else
		{
			NEWOROLDVAL=1;
		}

		//==========new process for new connection==============
		if ( (pid = fork()) == 0 )
		{
			//===============closing listening socket========
			if ( close(fd_socket) < 0 )
			{
				perror("Server-close() error");
				exit(EXIT_FAILURE);
			}
			
			
			//========Function call for  service request=============
			SERVICING(CONN,NEWOROLDVAL);		

			//=========closing CONN socket================
			if ( close(CONN) < 0 )
			{
				perror("Server-close() error");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);//exiting
			
		}
	
		//==========closing CONN socket in parent process, clean child process ...accept new connection===========
		if ( close(CONN) < 0 )
		{
			perror("Server-close() error");
			exit(EXIT_FAILURE);
		}
			
			
		//==============Wait for Specific Child Process untill server suspends==================
		waitpid(-1, NULL, WNOHANG); 
		
	}

	return EXIT_FAILURE;   //exiting
	
}









