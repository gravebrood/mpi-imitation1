

#include <iostream>		
#include <stdio.h>	
#include <stdlib.h>	
#include <string.h>	
#include <netinet/in.h>		
#include <unistd.h>		
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <iostream>		
#include <sstream>
#include <fcntl.h>

using namespace std;
extern char **environ;
int rank;
string process_name;
int number_of_processes;

const int BUFSIZE=1024;

void Error(const char *msg){
	
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
	char readInBuf[BUFSIZE];	
	string parsingString;
	string delimiter= " ";

	
	if(argc==1) {
		
		cout<<"Enter the port number! "<<endl;
		exit(1);
	}
		
	s = MakeServerSocket(argv[1]);

	while (1) { 
		//this part will let the server stay up if the client it killed so you can reconnect
		struct sockaddr_in sa;
		int sa_len = sizeof(sa);
		int fd = accept(s, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
		if(fd==-1)
			exit(1);
		//read the clients information 
		//name,rank, and the number of processes
		len=read(fd, readInBuf, BUFSIZE);
		if(len==-1) {
			cout<< "reading error"<<endl;
			exit(1);
		}
		close(fd);
		//parsing out the information givin from the client
		parsingString.assign(readInBuf);
		//cout<<parsingString<<endl;
		process_name=parsingString.substr(0,parsingString.find(delimiter));//gets the process name out
		parsingString.erase(0, parsingString.find(delimiter) + delimiter.length());// parsingString is now shorter to check again
		rank=atoi(parsingString.substr(0,parsingString.find(delimiter)).c_str());		// parsing out rank and converting into int
		parsingString.erase(0, parsingString.find(delimiter) + delimiter.length());// parsingString is now shorter to check again
		number_of_processes=atoi(parsingString.substr(0,parsingString.find(delimiter)).c_str());// parsing out number of processors and converting into int
		//cout<<"My rank is " <<rank<<endl;
		//cout<<"My name is " <<process_name<<endl;
		//cout<<"number of processes " <<number_of_processes<<endl;
		
		
		//fork and exec with rank name & number of processes as parameters
	
		char nop[15],one[15],r[50],pn[50],program_name[50],port_Num[10];
		string tem="./main";
		//cout<<"1"<<endl;
		sprintf(nop,"%i",number_of_processes);
		//cout<<"1"<<endl;
		int t=1;
		int p=50044;
		sprintf(one,"%d",t);
		sprintf(port_Num,"%d",p);
		sprintf(r,"%d",rank);
		strcpy(pn,process_name.c_str());
		strcpy(program_name,tem.c_str());
		
		//cout<<nop<<" "<<one<<" "<<r<<" "<<pn<<" "<<program_name<<endl;
		const char *prog="/mirror/./main";
		
		
		char *newargv[] = {program_name,nop,nop,port_Num,one,r, pn,(char *)0};
										//2nd  arugments are just junk to be passed
		
		char *newenviron[] = { NULL };

		int er=execve(prog,newargv,newenviron);
		if(er==-1)
			perror("exec");
		//cout<<"hi"<<endl;
		exit(1);
		
	}
}



