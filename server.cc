
/* My dumb server -- just an illustration */

#include <iostream>		// cout, cerr, etc
#include <stdio.h>		// perror
#include <string.h>		// bcopy
#include <netinet/in.h>		// struct sockaddr_in
#include <unistd.h>		// read, write, etc
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>		
#include <sstream>
#include <fcntl.h>

using namespace std;


const int BUFSIZE=1024;
void Error(const char *msg)
{
	perror(msg);
	exit(1);
}
int MakeSocket(char *host, char *port) {
	int s; 			// socket
	int len;	//length
	struct sockaddr_in sa; //class with no method everything is public
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;
	
	hp = gethostbyname(host);// get ip! ************ i can just put an ip address here!> "12.12.12.12" <
	if (hp == 0) 
		Error("Miss spelled host name");

	
	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length); // copies bytes
	sa.sin_family = hp->h_addrtype;// socket address


	sscanf(port, "%d", &portnum);// look for the given port number
	if (portnum > 0) {
		sa.sin_port = htons(portnum);
	}
	else {
		sp=getservbyname(port, "tcp");//if there is no portnum there is an error
		portnum = sp->s_port;
		sa.sin_port = sp->s_port;
	}
	s = socket(hp->h_addrtype, SOCK_STREAM, 0);

	ret = connect(s, (struct sockaddr *)&sa, sizeof(sa));
	if (ret < 0) {
     perror("Unable to connect the address to the socket");
	 exit(1);
	}
	cout << "Connect to host " << host  << " port " << port << endl;
	return s;
}

int MakeServerSocket(char *port) {
	const int MAXNAMELEN = 255;			 
	const int BACKLOG = 3;				
	char localhostname[MAXNAMELEN]; 
	int s; 		//socket
	int len;	//length
	struct sockaddr_in sa; //class with no method everything is pulblic
	struct hostent *hp;
	struct servent *sp;
	int portnum;	
	int ret;

	gethostname(localhostname,MAXNAMELEN);//tell yourself
	hp = gethostbyname(localhostname);// get ip
	

	sscanf(port, "%d", &portnum);	
	if (portnum > 0) {
		sa.sin_port = htons(portnum);
	}
	if(portnum <= 1024) {
		cout<<"Error: Port is under 1024"<<endl;
		exit(1);
	}


	sa.sin_port = htons(portnum);
	bcopy((char *)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	s = socket(hp->h_addrtype, SOCK_STREAM, 0);

	ret = bind(s, (struct sockaddr *)&sa, sizeof(sa));

	listen(s, BACKLOG);
	
	cout << "Waiting for connection on port " << port << " hostname " << localhostname << endl;

	return s;
}

int main(int argc, char *argv[]) {

	int s; 			
	int len;		
	char buf[BUFSIZE];	
	int ret;	
	int out;

	
	if(argc==1) {
		
		cout<<"Enter the port number! "<<endl;
		exit(1);
	}
		
	s = MakeServerSocket(argv[1]);

	while (1) {
		struct sockaddr_in sa;
		int sa_len = sizeof(sa);
		int fd = accept(s, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
		if(fd==-1)
			exit(1);
		cout << "Connection:  ";
		cout << " address is " << (unsigned int)sa.sin_addr.s_addr;
		cout << " family " << sa.sin_family;
		cout << " port " << ntohs(sa.sin_port);
		cout << endl;

			out = open("jj.png",O_WRONLY|O_CREAT,0666);

			if (out == -1) {
				cout<< "server out"<<endl;
				exit(2);
			}

			len = read(fd, buf, BUFSIZE);
			if (len == -1) 
				exit(3);
			
			int tyu;
			while(len>0) {
				tyu=write(out, buf, len);
				if(tyu==-1)
					exit(1);
				len = read(fd, buf, BUFSIZE);
				if (len == -1) 
					exit(3);
			}
	}
}



