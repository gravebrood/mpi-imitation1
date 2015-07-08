#ifndef _Dsc_mpi_
#define _Dsc_mpi_

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

const int DSC_INT=0;
const double DSC_DOUBLE=1;
const int DSC_mpi_success=0;
class Dsc_mpi {
	
private:
	
	const int BUFSIZE=1024;
	
	int socketHolder[100];//used to talk to the deameons
	const int DSC_MAX_PROCESSOR_NAME=20;
	int rank;
	string process_name;
	int number_of_processes;
	int S_socket;
	int head_fd;
	int aCount=0;//keeps track of nodes connected
	struct pollfd pollfds[5];

public:
	
	int MakeServerSocket(char *port) ;
	int MakeSocket(char *host, char *port); 
	int Dsc_rank();
	void Dsc_mpi_abort(int rank);
	void Dsc_mpi_finalize();
	int Dsc_mpi_bcast(void* data,int type,int root);
	

	int Dsc_size();

	string Dsc_processor_name();


	void masterNode();

	void Dsc_mpi_init(char *file_name, char *Number_of_processes, 
						char *port,char *phase,char *Rank,char *Process_name);
						
	string type(void* data,int type);

	int Dsc_mpi_revc(void* data, int type_of_data, int where_from);

	int Dsc_mpi_send(void* what_your_sending, int type_of_data, int where_to);
};

#endif
