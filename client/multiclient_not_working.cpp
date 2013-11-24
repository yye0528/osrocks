#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <iomanip>
#include "mpi.h"

using namespace std;

void *get_in_addr(struct sockaddr *sa); // get ip address
void nsleep(int milisec); //sleep in millisecond

int main(int argc, char *argv[])
{

	int numtasks, rank;
	char send_file[50];
	char recv_file[50];
	string task_name="";
	string input;
	string report;
	string end_mark="$END$";
	double start_time, end_time;
	const char PORT[]="3490"; // the port client will be connecting to
	const int MAXDATASIZE=4096; // max number of bytes we can get at once
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    int sent_total=0;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc < 2) {
        cout<<"ERROR: server IP not specified";
        exit(1);
    }

    //get task from user
    while(task_name==""){
        string input;
    	cout<<"Client: please enter the name of the task from the following list"<<endl;
        cout<<"<harry potter> 	17 chapters of Harry Potter each with file size ~20k"<<endl;
        cout<<"<repeat>		all client process send the same designated text file"<<endl;
        cin>>input;
        if(input=="harry potter" || input=="harrypotter"){
        	task_name="harry potter";
        }else if(input=="repeat"){
        	task_name="repeat";
    		cout<<"Enter your file name:"<<endl;
    		cin.getline(send_file, sizeof send_file);
    		sprintf (recv_file, "out.txt");
        }else{
        	cout<<"please enter the correct task name"<<endl;
        }
    }

	MPI_Init(&argc,&argv); //multi-process section begin
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks); //get number of tasks
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the task rank for each process

	nsleep(rank*10); //delay some time so tasks are ordered in execution
	cout<<"Client no."<<rank+1<<": process created"<<endl;

    //process the task type 'harry potter'
	if(task_name=="harry potter"){
		sprintf (send_file, "harrypotter/ch%d.txt", rank+1);
		sprintf (recv_file, "harrypotter/out/ch%d.txt", rank+1);
	}


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    cout<<"Client no."<<rank+1<<": connected to "<<s<<endl;

    freeaddrinfo(servinfo); // all done with this structure

	cout<<"Client no."<<rank+1<<": reading from file "<<send_file<<endl;

    //read file to input
	FILE *file;
	file = fopen(send_file, "r");
	if (file == NULL)
		cout<<"Client no."<<rank+1<<": file not found!"<<endl;

	while(1)
	{
		int bytes_read = fread(buf,1,sizeof(buf),file);
		if (bytes_read == 0)
			break;
		if (bytes_read < 0)
			perror("error reading from file");
		input+=buf;
	}

	input+=end_mark;
	fclose(file);

	//sent input
	while (input.length()>0){
		bzero(buf,MAXDATASIZE);

		string tmp=input.substr(0,(input.length()>MAXDATASIZE)?MAXDATASIZE:input.length());
		input.erase(0,tmp.length());//reduce the report to unsent part
		strcpy(buf,tmp.c_str());

		int bytes_sent = send(sockfd,buf,tmp.length(),0);
		sent_total+=bytes_sent;
		if (bytes_sent <= 0)
		{
			perror("ERROR writing to socket");
			exit(0);
		}
	}
	cout<<"Client no."<<rank+1<<": "<<sent_total<<" bytes have been sent. Waiting for server response..."<<endl;
	start_time = MPI_Wtime(); //start to record time

	//read server response
	while(1){
		bzero(buf,sizeof(buf));
		int bytes_receive = recv(sockfd,buf,sizeof(buf),0);

		if (bytes_receive == 0)
		{
			break;
		}else if (bytes_receive < 0)
		{
			perror("ERROR: receive failed in thread.");
			close(sockfd);
			exit(1);
		}

		report+=buf;

		unsigned found = report.rfind(end_mark);
		if (found!=string::npos){//stop looping after end mark is detected
			report.erase(found,end_mark.length()); //remove the end mark
		    break;
		}

	}
	cout<<"Client no."<<rank+1<<": report received."<<endl;

	FILE *file1;
	file1 = fopen(recv_file, "w+");

	while(report.length()>0){
		bzero(buf,MAXDATASIZE);
		string tmp=report.substr(0,(report.length()>MAXDATASIZE)?MAXDATASIZE:report.length());
		report.erase(0,tmp.length());//reduce the report to unsent part
		strcpy(buf,tmp.c_str());

		int bytes_write = fwrite(buf, sizeof(char), tmp.length(), file1);
		if(bytes_write < tmp.length())
		{
		   perror("File write failed.\n");
		}
	}
	fclose(file1);

	cout<<"Client no."<<rank+1<<": report saved to "<<recv_file<<endl;

	//

    close(sockfd);
    end_time   = MPI_Wtime();
	cout<<"Client no."<<rank+1<<": task finished. Time usage of this task: "
			<<end_time-start_time<<"s."<<endl;

    MPI_Finalize(); //multi-process section finish
    return 0;
}

// get ip address
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//sleep in millisecond
void nsleep(int millisec)
{
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec =millisec * 1000000L;

    if (nanosleep(&delay, NULL)) {
	  perror("nanosleep");
    }

}
