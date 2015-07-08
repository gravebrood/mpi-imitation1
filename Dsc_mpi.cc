#include "Dsc_mpi.h"
#include <stdio.h>	
#include <stdlib.h>
#include <iostream>			
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
#include <fstream>
#include <sys/poll.h>
#include <pthread.h>
using namespace std;


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
		perror("Miss spelled host name");

	
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
	//cout << "Connect to host " << host  << " port " << port << endl;
	return s;
}

int Dsc_rank(){
	
	return rank;
}

int Dsc_size(){
	return number_of_processes;
}

string Dsc_processor_name(){
	
	return process_name;
}


void masterNode(){
	string delimiter= " ";
	int len;
	int len1;
	int error;
	int where_to;
	
	string parsingString;
	char readInBuf[BUFSIZE];
	for(int i=1;i<5;i++){
		pollfds[i].events = POLLIN;
		
	}
	/*
	for(int i=0;i<8;i++){
	cout<<pollfds[i].fd <<" "<<pollfds[i].events<<endl;
		
	}*/
	while (1) { 
		error=poll(pollfds, 5, -1);//seeing who is avaible
		if (error == -1) perror("poll error");
		
		//read the clients information 
		//name,rank, and the number of processes
		for(int i=1;i<5;i++){
			
			if (pollfds[i].revents & POLLIN) {//if there is someone waiting read send there info out
				
				parsingString="";
				for(int j=0;j<BUFSIZE;j++)
					readInBuf[j]='\0';
				//cout<<"who did what"<<endl;
				//cout<<i<<endl;
				//cout<<pollfds[i].revents<<endl<<pollfds[i].fd<<endl;
				len=read(pollfds[i].fd, readInBuf, BUFSIZE);
				if(len<=0){ 
					pollfds[i].fd=0;
					pollfds[i].events=0;
					pollfds[i].revents=0;
					//perror("reading error");
					cout<<"rank "<<i<<" is not active"<<endl;
					aCount--;
					if(number_of_processes- aCount==number_of_processes) //
						cout<<"no one is connected"<<endl;
					break;
					
				}
					
				
				//parsing out the information givin from the node
				//where_to = -1;
				parsingString.assign(readInBuf);
				//cout<<parsingString<<"."<<endl;
				where_to=atoi(parsingString.substr(0,parsingString.find(delimiter)).c_str());//gets the int to where to send it to(the rank) out
				//parsingString.erase(0, parsingString.find(delimiter) + delimiter.length());// parsingString is now shorter to check again
				//cout<<"where your sending to " <<where_to<<"."<<endl;
				//cout<<"your data your sending" <<parsingString<<"."<<endl;
				
				//len1=write(pollfds[where_to].fd,parsingString.c_str(),sizeof(parsingString.c_str())+1);
				cout<< "len " <<len<<" im writing this to "<<where_to<<" and im writing "<<readInBuf<<endl;
				char one[100];
				sprintf(one,"%d",parsingString.length());
				write(pollfds[where_to+1].fd,one,100);
				len1=write(pollfds[where_to+1].fd,readInBuf,parsingString.length());
				if(len1==-1) 
					perror("wrting  error");
				cout<<len1<<endl;
				//sleep(1);
			} //end of if 
		}//end of for loops
		
		
	}//end of while
}

void Dsc_mpi_init(char *file_name, char *Number_of_processes, 
					char *port,char *phase,char *Rank,char *Process_name){
	
	int check=atoi(phase);
	
	if(check==0){//phase 0 only happends once --- talks to the deamon to get mpi running
		cout<<file_name<<" "<< Number_of_processes<< " " << port<<endl;
		ifstream myReadFile;
		myReadFile.open(file_name);
		char output[100];
		char writeOutBuf[50];
		int writeOutError;
		int i=0;
		string iConverted;
		ostringstream strm;
		
		if (myReadFile.is_open()) {
			myReadFile >> output;
			while (!myReadFile.eof()) {

				//reads the machinefile and creates a socket for each node to talk to.
				
				
				//cout<<output<<endl;
				socketHolder[i]=MakeSocket(output, port);
				if(socketHolder[i]<0){
					cout<< "makeSocket error"<<endl;
					exit(1);
				}
				//sends each node a there name,rank, and the number of processes seperated by a space
				strcpy(writeOutBuf,output);
				strcat(writeOutBuf," ");
				
				//the "rank" i has to be converted from int to char *
				strm.str("");//clears the stream out each time
				strm << i;
				iConverted=strm.str();
				
				strcat(writeOutBuf,iConverted.c_str());
				strcat(writeOutBuf," ");
				strcat(writeOutBuf,Number_of_processes);
				// ^^ that whole process is for what you are writing to each server
				writeOutError=write(socketHolder[i], writeOutBuf, sizeof(writeOutBuf));
				if(writeOutError==-1){
					cout<<"writing error"<<endl;
					exit(1);
				}
				close(socketHolder[i]);
				//cout<<i<<" i "<<endl;
				i++;
				myReadFile >> output;
			}
			myReadFile.close();
		}
		number_of_processes=atoi(Number_of_processes);//set golbal
		check=2;
	}//end of if phase -0
	if(check==2) {
		//clear out socket array
		//server socket stuff----must have set port 50025
		//resets up rank to the socket info
		char new_port[10]="50033";
		S_socket = MakeServerSocket(new_port);
		if(S_socket==-1) perror("server socket error");
		//cout<<file_name<<" "<<Number_of_processes<<" "<<port<<" "<<phase<< " " <<Rank<<" "<<Process_name<<endl;
	
		pollfds[0].fd = S_socket;		
		pollfds[0].events = POLLIN; // first fd is the socket to listen to
		//int count=0;
		int error;
		//cout<<"here"<<endl;
		while(1) {
			
			error=poll(pollfds, 5, -1);//seeing who is avaible
			if (error == -1) perror("poll error");
			if (pollfds[0].revents & POLLIN) {//if there is someone waiting do accept them
				struct sockaddr_in sa;
				int sa_len = sizeof(sa);
				aCount++;//gobal veriable
				pollfds[aCount].fd = accept(S_socket, (struct sockaddr *)&sa, (unsigned int *)&sa_len);
				
				if (pollfds[aCount].fd == -1) perror("Accept bad");
				//else cout<<"connection accepted from client "<<aCount<<endl;	
			}//if
			//cout<<number_of_processes<<endl;
			if(number_of_processes==aCount) {// 4 is the # of nodes if all nodes are connected your all good stop listening for connections
					cout<<"all connected"<<endl;
					break;
					
			}
		}//edn while
		//cout<<"here"<<endl;
		masterNode();
	}//end if
	if(check==1){
		//client socket stuff---connect to 50025
		
		
		sleep(atoi(Rank)*.5+1);//wait 1 unit for every rank you are, connecting means they are already in order
		char host[10]="head";
		
		head_fd=MakeSocket(host, port);
		if(head_fd==-1) perror("make socket error");
		//cout<<file_name<<" "<<Number_of_processes<<" "<<port<<" "<<phase<< " " <<Rank<<" "<<Process_name<<endl;
		//set the globals
		rank=atoi(Rank);
		number_of_processes=atoi(Number_of_processes);
		process_name=Process_name;
		sleep((number_of_processes-1)-((rank*.5)+1));
	}
			
}
//takes the data and converts it into a string to be sent over
string type(void* data,int type) {//only does ints and strings can add others easy
	
	string finala="";
	stringstream ss;
	
	if(type==0) {
		
		ss << *((int *)data);
		finala=ss.str();
	}
	if(type==1) {
		finala=*((double *)data);
	}
	/*
	if(type==1) {
		finala=*(((char *) *)data);
	}
	*/	 
	 return finala;
}


void Dsc_mpi_revc(void* data, int type_of_data, int where_from){
	string delimiter= " ";
	cout<<"hi, im waiting to read"<<endl;
	char readIn[BUFSIZE];
	string parsingString="";
	int whereFrom,typeOfData;
	char tut[100];
	read(head_fd,tut,100);
	int er=read(head_fd,readIn, atoi(tut));
	if(er==-1) perror("reading error");
	cout<<"this is what i read "<<readIn<<"."<<endl;
	cout<<er<<endl;
	for(int i=er;i<BUFSIZE;i++)
		readIn[i+1]='\0';
	parsingString.assign(readIn);
	whereFrom=atoi(parsingString.substr(0,parsingString.find(delimiter)).c_str());
	parsingString.erase(0, parsingString.find(delimiter) + delimiter.length());
	typeOfData=atoi(parsingString.substr(0,parsingString.find(delimiter)).c_str());
	
	//cout<<whereFrom<< " " << where_from<<endl;
	//cout<< typeOfData<< " "<<type_of_data<<endl;
	if(rank==where_from || where_from<0){
		if(typeOfData== type_of_data){
			parsingString.erase(0, parsingString.find(delimiter) + delimiter.length());
			//cout<<parsingString.c_str()<<endl;
			
			int qwe=atoi(parsingString.c_str() );
			
			data=memcpy(data,&qwe,sizeof(qwe));
			
			//reinterpret_cast<void*>(strvect.front().c_str());
			cout<<*((int *)data)<<endl;
			
		}
		else cout<<"data type sent is incorrect"<<endl;
	}
	else cout<<"not accepting data from rank "<<whereFrom<<endl;
	
	//make method passing in parsingString and type_of_data
			
}

void Dsc_mpi_send(void* what_your_sending, int type_of_data, int where_to){
	
	string temp= "";
	stringstream ss;
	ss << where_to;
	temp = ss.str();// adding where to
	ss.str("");
	ss << type_of_data;
	temp= temp+" "+ss.str();//space keeps the data sperate
	temp=temp + " " + type(what_your_sending,type_of_data);
	//cout<<"temp " <<temp<<endl;
	int er;
	cout<<temp.c_str()<<endl;
	// temp is whereto _ type _ data
	er=write(head_fd,temp.c_str(),temp.length());//sizeof(temp.c_str())+1
	if(er==-1){
		cout<<"writing error"<<endl;
		exit(1);
	}
	cout<<er<<endl;
	
}
int main(int argc, char *argv[]) {
 
	
	//need to have at least first 3 to start in normal program---others are used for starting up clients on the other machines
	Dsc_mpi_init(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
	int r=Dsc_rank();
	int wr=Dsc_size();
	int rut,tut,eut,wut;
	int funsize=100+r;
	string pn=Dsc_processor_name();
	
	cout<<"rank "<<r<<" world size "<<wr<< " proc name "<<pn<<endl;
	/*
	if(r==1)
		Dsc_mpi_send(&funsize,0,2);
	cout<<rut<<endl;
	if(r==2){
		Dsc_mpi_revc(&rut,0,-1);
		cout<<rut<<" rut"<<endl;
		
	}
	*/
	
	
	Dsc_mpi_send(&funsize,0,0);
	
	if(r==0){
		
		Dsc_mpi_revc(&rut,0,-1);
		Dsc_mpi_revc(&tut,0,-1);
		Dsc_mpi_revc(&eut,0,-1);
		Dsc_mpi_revc(&wut,0,-1);
		
		
		cout<<rut+tut+wut+eut<<" rut"<<endl;
		

	}

	sleep(5);
}
