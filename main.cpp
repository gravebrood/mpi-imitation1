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
#include <time.h>
#include <sys/time.h>

using namespace std;



	
int main(int argc, char *argv[]) {

	Dsc_mpi *mpi= new Dsc_mpi();
	mpi->Dsc_mpi_init(argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
	int r=mpi->Dsc_rank();
	int wr=mpi->Dsc_size();
	int rut,tut,eut,wut;
	int funsize=100+r;
	string pn=mpi->Dsc_processor_name();
	
	cout<<"rank "<<r<<" world size "<<wr<< " proc name "<<pn<<endl;
	
	struct timeval tim;
	long double average=10000000.0;
	long double holder1;
	long double holder2;

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
	
	
	int bu=0;
	if(r==0) bu=19;
		//cout<<"____measurement in milliseconds for one way_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		int er;
		for(int i=0;i<200;i++){
			
			
			int ty=mpi->Dsc_mpi_bcast(&bu,DSC_INT,0);
			if(ty!=DSC_mpi_success)
					cout<<"failed on bcast on rank" <<r <<endl;
			
		}
		gettimeofday(&tim, NULL);
		holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed bcast for rank--"<<r<<endl;
		
	
	
	cout<<"bcast"<<endl;
	
	if(r==1||r==0){
		//cout<<"____measurement in milliseconds for back and forth_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		int er;
		for(int i=0;i<200;i++){
			
			if(r==1){
				er=mpi->Dsc_mpi_send(&funsize,0,0);
				if(er!=DSC_mpi_success){
					cout<<"failed on sending on rank" <<r <<endl;
					mpi->Dsc_mpi_abort(0);//abort the rank your sending to
				}
			}
			if(r==0){
				er=mpi->Dsc_mpi_revc(&rut,0,-1);
				if(er!=DSC_mpi_success)
					cout<<"failed on revc on rank" <<r <<endl;
			}
			if(r==0){
				mpi->Dsc_mpi_send(&funsize,0,1);
				if(er!=DSC_mpi_success){
					cout<<"failed on sending on rank" <<r <<endl;
					mpi->Dsc_mpi_abort(1);//abort the rank your sending to
				}
			}
			if(r==1){
				mpi->Dsc_mpi_revc(&rut,0,-1);
				if(er!=DSC_mpi_success)
					cout<<"failed on revc on rank" <<r <<endl;
			}
			
		}
		gettimeofday(&tim, NULL);
		holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed both ways for rank--"<<r<<endl;
		
	}
	cout<<"next"<<endl;
	if(r==1||r==0){
		//cout<<"____measurement in milliseconds for one way_____"<<endl;
		gettimeofday(&tim, NULL);
		holder1=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		int er;
		for(int i=0;i<200;i++){
			
			if(r==1){
				er=mpi->Dsc_mpi_send(&funsize,0,0);
				if(er!=DSC_mpi_success){
					cout<<"failed on sending on rank" <<r <<endl;
					mpi->Dsc_mpi_abort(0);//abort the rank your sending to
				}
			}
			if(r==0){
				er=mpi->Dsc_mpi_revc(&rut,0,-1);
				if(er!=DSC_mpi_success)
					cout<<"failed on revc on rank" <<r <<endl;
			}
			
			
		}
		gettimeofday(&tim, NULL);
		holder2=(tim.tv_sec*1000000000.0)+(tim.tv_usec*1000);
		cout<<fixed<<((holder2-holder1)/200)/1000000<<" network speed one way for rank--"<<r<<endl;
		
	}
	
	
	
	
	if(r==0) bu=21;
	int rt=mpi->Dsc_mpi_bcast(&bu,DSC_INT,0);
	if(rt!=DSC_mpi_success)
					cout<<"failed on bcast on rank" <<r <<endl;
	cout<<bu<<endl;
	mpi->Dsc_mpi_finalize();
	
	
	
	
	
return 0;
}
