/*
//========================================================
//HTTP SERVER (httpserver.h)
//========================================================

Developed by: Kamal Panthi 
Email: kamal210@hotmail.com

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

//==================Header Declarations===================
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>



//================Global Variable Declaration=============
int PORT,BROWSERCOUNTER=0;
time_t CURRENTTIME;
struct tm *LOCALTIME;
char TEMPBUFFER[1024];

//=============simple web page displayed for newly CONN client only once through the lifetime==============
char welcomePage[1024]={"<html><body><h1>HTTP SERVER</h1><h4>Simple web page for first time connecting client</h4><p>HTTP server support RFC1945 standard and the following status code responses are implmented here.<br> <li>200 OK<li>400 Bad Request<li>404 Not Found<li>501 Not Implemented<br><h6>HTTP SERVER ABILITIES</h6><br><br>---Multiple simultaneus connections<br>---Every client responses are recorded in log file with the time and resource name<br>---When client(new IP) contacts to the server first time, it receives this welcome web page and the IP is saved to know the client for future references<br>---Log file is accessible through a browser as logfile.html<br>---After sending each packet of 1024 bytes the server sleeps for 3 seconds<br><br><br>Other Links: <br><a href=\"index.html\">Click here for Index Page</a><br><a href=\"logfile.html \">Click here for Log File</a></p></body></html>"};

//Other Files are: <br><b>index.html</b><br><b>logfile.html</b></p></body></html>"};


//=============Structure for storing browser IP==========================
//=======BROWSER ==========
typedef struct 
{
	  
	  char IP[16];//client IP
	  
	  
}browsers;
browsers BROWSERINSTANCE[100];//user instance for browsers

//==========ENUMERATED DATATYPES FOR METHODS===================
enum METHODS { GET, HEAD, UNSUPPORTED };//Method varieties

//==========ENUMERATED DATATYPES FOR TYPES===================
enum TYPES   { SIMPLE, FULL };//Type varieties

//=============Structure for storing Client Request Information==========================
struct CLIENTIFORMATION 
{
	enum METHODS method;//method
	enum TYPES type;//type
	char *referer;//referer
	char *useragent;//useragent
	char *resource;//resource
	int status;//status code
};

//==========================LIST OF FUNCTIONS=============================================
//=================Function to Convert lower case to uppercase================  
int CONVERTTOUPPER(char * package); 
//==================Function to write into file==========================
void WRITETOFILE(char* msg, char* file);
//=============Function to Read a line from a socket====================
ssize_t READSOCK(int FDSOCK, void *PACKAGE, size_t PACKAGELENGTH);
//==========Function to Write a line to a socket================
ssize_t WRITESOCK(int FDSOCK, const void *PACKAGE, size_t PACKAGELENGTH);
//===============FUNCTION to parse request header===================
int REQUESTPARSER(char * buffer, struct CLIENTIFORMATION * CLIENT_REQUEST);
//===========Function to Service an HTTP request===============
int SERVICING(int CONN, int FIRSTTIME);



//=================Function to Convert lower case to uppercase================  
int CONVERTTOUPPER(char * package) 
{
	while ( *package ) 
	{
		*package = toupper(*package);
		++package;
	}
	return 0;
}


//==================Function to write into file==========================
void WRITETOFILE(char* msg, char* file)
{
	FILE *fp;
	int fd;
	fp = fopen(file,"a");//opening the file
	fd= fileno(fp);//converting the file stream to int
	flock(fd, LOCK_EX);//Locking the file for writing by single program
	fprintf(fp,"%s\n",msg);//writing to file
	flock(fd, LOCK_UN);//unlocking the file
	fclose(fp);//vlosing the file
}

//=============Function to Read a line from a socket====================
ssize_t READSOCK(int FDSOCK, void *PACKAGE, size_t PACKAGELENGTH) 
{
	char    c, *buffer;
	ssize_t length, ReadLength;
	

	buffer = PACKAGE;

	for ( length = 1; length < PACKAGELENGTH; length++ ) 
	{

		if ( (ReadLength = read(FDSOCK, &c, 1)) == 1 ) 
		{
			*buffer++ = c;
			if ( c == '\n' )
				break;
		}
		else if ( ReadLength == 0 ) 
		{
			if ( length == 1 )
				return 0;
			else
				break;
		}
		else 
		{
			if ( errno == EINTR )
				continue;
				{
				    perror("Server- READSOCK() error");
				    exit(EXIT_FAILURE);
				}
			
		}
	}

	*buffer = 0;
	return length;
}


//==========Function to Write a line to a socket================
ssize_t WRITESOCK(int FDSOCK, const void *PACKAGE, size_t PACKAGELENGTH) 
{
	size_t      PACKAGELENGTHLEFT;
	ssize_t     PACKAGELENGTHWRITTEN;
	const char *buffer;

	buffer = PACKAGE;
	PACKAGELENGTHLEFT  = PACKAGELENGTH;

	while ( PACKAGELENGTHLEFT > 0 ) 
	{
		if ( (PACKAGELENGTHWRITTEN = write(FDSOCK, buffer, PACKAGELENGTHLEFT)) <= 0 ) 
		{
			if ( errno == EINTR )
				PACKAGELENGTHWRITTEN = 0;
			/*else
			{
			    perror("Server- WRITESOCK() error");
			    exit(EXIT_FAILURE);
			}
				*/
		}
		PACKAGELENGTHLEFT  -= PACKAGELENGTHWRITTEN;
		buffer += PACKAGELENGTHWRITTEN;
	}

	return PACKAGELENGTH;
}


//===============FUNCTION to parse request header===================
int REQUESTPARSER(char * buffer, struct CLIENTIFORMATION * CLIENT_REQUEST) 
{

	static int FIRSTLINEREQUEST = 1;
	char      *temp;
	char      *ENDPOINTER;
	int        HEADERLENGTH;

	//==========If FIRSTLINEREQUEST == 0, it is the first line of the HTTP request=====
	if ( FIRSTLINEREQUEST == 1 )//request line
	{	
                                   

		if ( !strncmp(buffer, "GET ", 4) ) 
		{
			CLIENT_REQUEST->method = GET;
			buffer += 4;
		}
		else if ( !strncmp(buffer, "HEAD ", 5) ) 
		{
			CLIENT_REQUEST->method = HEAD;
			buffer += 5;
		}
		else 
		{
			CLIENT_REQUEST->method = UNSUPPORTED;
			CLIENT_REQUEST->status = 501;//NOT IMPLEMENTED
			return -1;
		}


		//=======move to beginning of resource file=====
		while ( *buffer && isspace(*buffer) )
			buffer++;		

		ENDPOINTER = strchr(buffer, ' ');
		if ( ENDPOINTER == NULL )
			HEADERLENGTH = strlen(buffer);//calculating length
		else
			HEADERLENGTH = ENDPOINTER - buffer;
		if ( HEADERLENGTH == 0 ) 
		{
			CLIENT_REQUEST->status = 400;//bad request
			return -1;
		}

		

		CLIENT_REQUEST->resource = calloc(HEADERLENGTH + 1, sizeof(char));
		strncpy(CLIENT_REQUEST->resource, buffer, HEADERLENGTH);//storing resource in CLIENT_REQUEST


		if ( strstr(buffer, "HTTP/") )//if supplied the HTTP/ information it is FULL
			CLIENT_REQUEST->type = FULL;//full request
		else//else it is SIMPLE
			CLIENT_REQUEST->type = SIMPLE;//simple request

		FIRSTLINEREQUEST = 0;
		return 0;
	}


	//if only when request is FULL

	ENDPOINTER = strchr(buffer, ':');
	if ( ENDPOINTER == NULL ) 
	{
		CLIENT_REQUEST->status = 400;//bad request
		return -1;
	}

	temp = calloc( (ENDPOINTER - buffer) + 1, sizeof(char) );
	strncpy(temp, buffer, (ENDPOINTER - buffer));

	CONVERTTOUPPER(temp);
	
	buffer = ENDPOINTER + 1;//increasing buffer to point to a value
	while ( *buffer && isspace(*buffer) )
		++buffer;
	if ( *buffer == '\0' )
		return 0;


	
	if ( !strcmp(temp, "USER-AGENT") ) //user-agent
	{
		CLIENT_REQUEST->useragent = malloc( strlen(buffer) + 1 );
		strcpy(CLIENT_REQUEST->useragent, buffer);
	}
	else if ( !strcmp(temp, "REFERER") ) //referer
	{
		CLIENT_REQUEST->referer = malloc( strlen(buffer) + 1 );
		strcpy(CLIENT_REQUEST->referer, buffer);
	}

	free(temp);//free temp buffer
	return 0;
}




//===========Function to Service an HTTP request===============
int SERVICING(int CONN, int FIRSTTIME) 
{

	struct CLIENTIFORMATION  CLIENT_REQUEST;
	int RESFILE = 0;
	char   buffer[1024] = {0};
	int    SELECTVALUE;
	fd_set FDesc;
	struct timeval TIMEOUT;
	char  RESOURCELOCATION[512];

	//=======Initialize Struct=========
	CLIENT_REQUEST.useragent = NULL;
	CLIENT_REQUEST.referer   = NULL;
	CLIENT_REQUEST.resource  = NULL;
	CLIENT_REQUEST.method    = UNSUPPORTED;
	CLIENT_REQUEST.status    = 200;  

	//========Get Request================	
	TIMEOUT.tv_sec  = 500;//Timeout set to 5 seconds
	TIMEOUT.tv_usec = 0;


	do //If FULL request loop until a blank line is received or until select() times out, else If SIMPLE request loop only once. 
	{

		//======file descriptors reset=====
		FD_ZERO(&FDesc);
		FD_SET (CONN, &FDesc);		

		SELECTVALUE = select(CONN + 1, &FDesc, NULL, NULL, &TIMEOUT);	

		if ( SELECTVALUE < 0 ) //if error in  select()
		{
		    perror("Server- select() error");
		    exit(EXIT_FAILURE);
		}		
		else if ( SELECTVALUE == 0 ) //if timer expires
		{
			return -1;

		}
		else 
		{
			//====reading an input line====
			READSOCK(CONN, buffer, 1024 - 1);

			//=====trimming a whitespace at the end of the line===
			int m = strlen(buffer) - 1;
			while ( !isalnum(buffer[m]) && m >= 0 )
			buffer[m--] = '\0';

			if ( buffer[0] == '\0' )
				break;

			if ( REQUESTPARSER(buffer, &CLIENT_REQUEST) )
				break;
		}
	} while ( CLIENT_REQUEST.type != SIMPLE );

	  

	if ( CLIENT_REQUEST.status == 200 )//if client request status is OK i.e. 200
	{
		//===Cleaning urlengthcoded data from resource name========== 
		char asciinum[3] = {0};
		int i = 0, c;

		while ( CLIENT_REQUEST.resource[i] ) 
		{
			if ( CLIENT_REQUEST.resource[i] == '+' )
				CLIENT_REQUEST.resource[i] = ' ';
			else if ( CLIENT_REQUEST.resource[i] == '%' ) 
			{
				asciinum[0] = CLIENT_REQUEST.resource[i+1];
				asciinum[1] = CLIENT_REQUEST.resource[i+2];
				CLIENT_REQUEST.resource[i] = strtol(asciinum, NULL, 16);
				c = i+1;
				do 
				{
				CLIENT_REQUEST.resource[c] = CLIENT_REQUEST.resource[c+2];
				} while ( CLIENT_REQUEST.resource[2+(c++)] );
			}
			++i;
		}

		memset(TEMPBUFFER,0,1024);
		strcat(TEMPBUFFER, "..");
		strcat(TEMPBUFFER, CLIENT_REQUEST.resource);
		strcat(TEMPBUFFER, "]<br>\n");
		WRITETOFILE(TEMPBUFFER, "logfile.html");


		
		//========making the requested file path========
		memset(RESOURCELOCATION,0,512);	
		strcat(RESOURCELOCATION, "../c0346747_Kamal_Panthi");	
		strcat(RESOURCELOCATION, CLIENT_REQUEST.resource);
		

		

		if ( (RESFILE = open(RESOURCELOCATION, O_RDONLY)) < 0 ) //open the file
		{
			if ( errno == EACCES )
			{
				CLIENT_REQUEST.status = 401;//unauthorized
				
			}

			else
			{
				CLIENT_REQUEST.status = 404;//not found
				
			}
		}
		
	}


	

	if ( CLIENT_REQUEST.type == FULL )//if client request is full
	{
		char ReqBuf[100];

		sprintf(ReqBuf, "HTTP/1.0 %d OK\r\n", CLIENT_REQUEST.status);
		WRITESOCK(CONN, ReqBuf, strlen(ReqBuf));

		WRITESOCK(CONN, "Server: PGWebServ v0.1\r\n", 24);
		WRITESOCK(CONN, "Content-Type: text/html\r\n", 25);
		WRITESOCK(CONN, "\r\n", 2);
	}

	

	

	if ( CLIENT_REQUEST.status == 200 ) //when status is OK i.e. 200
	{
		if(FIRSTTIME==0)
		{
			

			if ( write(CONN, &welcomePage, 1024) < 1 )
			{
			    perror("Server- write() error");
			    exit(EXIT_FAILURE);
			}
		}
		else
		{
			char c;
			int  i, packsize=0;

			while ( (i = read(RESFILE, &c, 1)) ) 
			{
				if ( i < 0 )
				{
				    perror("Server- read() error");
				    exit(EXIT_FAILURE);
				}
				
				if ( write(CONN, &c, 1) < 1 )
				{
				    perror("Server- write() error");
				    exit(EXIT_FAILURE);
				}
				packsize++;
				if(packsize==1024)
				{
					packsize=0;
					sleep(3);//after sending 1024 chars it sleeps for 3 seconds
				}
				
			}
		}
		

		
	


	}
	else//if error occurred
	{
		char ERRORPACKAGE[100];
		char STATUSCODE[3];
		memset(STATUSCODE,0,3);
		sprintf(STATUSCODE, "%d",CLIENT_REQUEST.status);
		sprintf(ERRORPACKAGE, "<HTML>\n<HEAD>\n<TITLE>Server Error %d</TITLE>\n</HEAD>\n\n<BODY>\n<H1>Server Error %d</H1>\n<P>Error Occurred.</P>\n</BODY>\n</HTML>\n", CLIENT_REQUEST.status,CLIENT_REQUEST.status);
		WRITESOCK(CONN, ERRORPACKAGE, strlen(ERRORPACKAGE));

			
	}


	if ( RESFILE > 0 )//if resource file is opened
		if ( close(RESFILE) < 0 )
		{
		    perror("Server- Close() error");
		    exit(EXIT_FAILURE);
		}
			
	// Free the allocated memory locations
	if ( CLIENT_REQUEST.useragent )
		free(CLIENT_REQUEST.useragent);
	if ( CLIENT_REQUEST.referer )
		free(CLIENT_REQUEST.referer);
	if ( CLIENT_REQUEST.resource )
		free(CLIENT_REQUEST.resource);

	return 0;
}


