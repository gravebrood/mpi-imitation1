//DIllon cowan Senoir project for MPI
// this client will send the MPI program once compiled over the other machines in waiting.
// then kill its self
// i will have a set list of ip address to send them over to... at first
// the servers will have a set port number to 50002
#include <iostream>		
#include <sstream>
#include <stdio.h>	
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
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

main(int argc, char *argv[]) {


	int s; 		//socket
	int len;	//length
	char buf[BUFSIZE];//pointer
	int ret;
	int out;		
	
	if(argc==1) {

		cout<<"No server name or port number given"<<endl;
		exit(1);
	}

	if(argc==2) {

		cout<<"No port number given"<<endl;
		exit(1);
	}



	s = MakeSocket(argv[1], argv[2]);

	if (s == -1) {
			cout << "Failed connection " << endl;
			exit(1);
	}
	//_____________________________________

		if(s==-1)
			exit(1);

		out= open("test.png",O_RDONLY,0666);

		if (out == -1) {
			cout<<"out didnt work"<<endl;
			exit(2);
		}
	
		len = read(out, buf, BUFSIZE);
		if (len == -1) 
			exit(3);
		
		int rty;
		while (len > 0){
			rty=write(s, buf, len);
			if(rty==-1)
				exit(1);
			len = read(out, buf, BUFSIZE);
			if (len == -1) 
				exit(3);
			cout<< len<<" =len"<<endl;
		}

	
}

