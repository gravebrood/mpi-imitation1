
#include <mpi.h>
#include <stdio.h>
#include <iostream>	
#include <time.h>
#include <sys/time.h>
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

int main (int argc, char* argv[]){
	int rank, size,rc,rut;
	
	MPI_Init (&argc, &argv);      /* starts MPI */
	MPI_Comm_rank (MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size (MPI_COMM_WORLD, &size);        /* get number of processes */
	printf( "hi from process %d of %d\n", rank, size );
	MPI_Status stat;


	struct timeval tim;
	long double average=10000000.0;
	long double holder1;
	long double holder2;
	int funsize=100+rank;
//____________________average for getting the time_____________________
	
	gettimeofday(&tim, NULL);
	holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec/1000);
	for(int i=0;i<average;i++){
		gettimeofday(&tim, NULL);
	}
	gettimeofday(&tim, NULL);
	holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec/1000);
	//cout<<fixed<<(holder2-holder1)/(average+1)<<endl;
	
//___________________empty loop____________________________________

	if(rank==1||rank==0) {
		//cout<<"____measurement in milliseconds for message back and forth_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		//int er;
		//cout<<rank<<endl;
		for(int i=0;i<200;i++) {
			//cout<<i<<endl;
			if(rank==1){
				rc = MPI_Send(&funsize, 1, MPI_INT, 0 , 0 , MPI_COMM_WORLD);
				if(rc!=MPI_SUCCESS)
					cout<<"failed on sending on rank" <<rank <<endl;
					
			}
			if(rank==0){
				rc = MPI_Recv (&rut, 1, MPI_INT ,MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD ,&stat);
				if(rc!=MPI_SUCCESS)
					cout<<"failure on MPI_SEND:    fail on->>"<<rank<<endl;
			}
			if(rank==0){
				rc = MPI_Send(&funsize, 1, MPI_INT, 1 , 0 , MPI_COMM_WORLD);
				if(rc!=MPI_SUCCESS)
					cout<<"failure on MPI_SEND:    fail on->>"<<rank<<endl;
					
			}
			if(rank==1){
				rc = MPI_Recv (&rut, 1, MPI_INT ,MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD ,&stat);
				if(rc!=MPI_SUCCESS)
					cout<<"failed on revc on rank" <<rank <<endl;
			}
			
		}
	
	gettimeofday(&tim, NULL);
	holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
	sleep(rank*2);
	cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed bothways for rank--"<<rank<<endl;
}

if(rank==1||rank==0) {
		//cout<<"____measurement in milliseconds for one way_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		int er;
		cout<<rank<<endl;
		for(int i=0;i<200;i++) {
			//cout<<i<<endl;
			if(rank==1){
				rc = MPI_Send(&funsize, 1, MPI_INT, 0 , 0 , MPI_COMM_WORLD);
				if(rc!=MPI_SUCCESS)
					cout<<"failed on sending on rank" <<rank <<endl;
					
			}
			if(rank==0){
				rc = MPI_Recv (&rut, 1, MPI_INT ,MPI_ANY_SOURCE , MPI_ANY_TAG , MPI_COMM_WORLD ,&stat);
				if(rc!=MPI_SUCCESS)
					cout<<"failure on MPI_SEND:    fail on->>"<<rank<<endl;
			}
			
			
		}
	
	gettimeofday(&tim, NULL);
	holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
	sleep((rank*2)+3);
	cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed one way for rank--"<<rank<<endl;
}




		//cout<<"____measurement in milliseconds for one way_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		int er;
		cout<<rank<<endl;
		for(int i=0;i<200;i++) {
			int bu=0;
			if(rank==0) bu=19;
			MPI_Bcast (&bu,1,MPI_INT,0,MPI_COMM_WORLD);
			
		}
	
	gettimeofday(&tim, NULL);
	holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
	sleep((rank*2)+3);
	cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed bcast for rank--"<<rank<<endl;


	sleep((rank*2)+3);
	cout<< "before bcast"<<endl;
	int bu=0;
	if(rank==0) bu=19;
	MPI_Bcast (&bu,1,MPI_INT,0,MPI_COMM_WORLD);
	sleep((rank*2)+3);
	cout<<bu<<"--rank-"<< rank<<endl;	


			
			
	
			
		
		

	 MPI_Finalize();
	 return 0;
}
