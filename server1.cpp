#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>


#include <iostream>
#include <iomanip>
#include "wordcount.h"

using namespace std;

const char PORT[]= "3490";  // the port users will be connecting to
const int BACKLOG=10;     // how many pending connections queue will hold
long serviceCount=0;

void* serve_it(void*);

int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    int rv;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    	fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
        	cerr<<"error in creating socket."<<endl;
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
        	cerr<<"error in setsockopt."<<endl;
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            cerr<<"error in biding."<<endl;
            continue;
        }

        break;
    }

    if (p == NULL)  {
        cerr<<"server: failed to bind"<<endl;
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        cerr<<"error in listening"<<endl;
        exit(1);
    }

    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // FIFO scheduling for threads
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads  waiting on each other


    while(1) {  // main accept() loop
    	 cout<<"server: waiting for connections...\n";

        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
        	cerr<<"error in accept"<<endl;
            continue;
        }

        pthread_create(&t_id, &attr, &serve_it,(void*)new_fd); //create a new thread to handle the connection
        sleep(0); // Giving threads some CPU time

    }//while
    
    return 0;
}


void* serve_it(void* arg)
{
	int csocket_df = (int) (arg);
	string receive;
 	wordCount wc=wordCount();
    char buf[256];
 	bzero(buf,sizeof(buf));

	if (csocket_df < 0)
	{ 
		cout<<"Accept error on server"<<endl;
        exit(1);
	}
	cout<<"Connection "<<csocket_df<<" accepted"<<endl;

	while(1){
		bzero(buf,sizeof(buf));
		int bytes_receive = recv(csocket_df,buf,sizeof(buf),0);
		if (bytes_receive == 0)
		{
			cout<<"0 byte meet. Finish reading!"<<endl;
			break;
		}

		cout<<bytes_receive<<"  bytes received"<<endl;

		if (bytes_receive < 0)
		{
			exit(1);
		}
		receive+=buf; //append content from buffer to string variable
	}

	cout<<"Reading done. "<<endl;

	if(receive.length()==0){ //no string was received from this connection.
		cout<<"Empty input."<<endl;
		close(csocket_df);
		pthread_exit(0);
	}

	wc.setStr(receive);
	//wc.printFWord();

	cout<<"number of characters:";
	cout<<wc.getNchar()<<endl;

	cout<<"number of word:";
	cout<<wc.getNword()<<endl;

	cout<<"number of sentences:";
	cout<<wc.getNSentence()<<endl;


	int n = send(csocket_df,"I got your message",18,0);
    if (n < 0)
    {
    	cout<<"n: "<<n<<endl;
        perror("ERROR writing to socket");
        exit(1);
    }


	serviceCount++;
	cout<<serviceCount<<" client has been served."<<endl;

	close(csocket_df); //close the client socket file description
	pthread_exit(0);
}

