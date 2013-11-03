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

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold

#define filename "output.txt"


int new_fd;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);


void* serve_it(void*);


int main(void)
{
    int sockfd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
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
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }



    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Creating thread attributes
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO); // FIFO scheduling for threads
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // Don't want threads (particualrly main)
                                                                 // waiting on each other

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        cout<<"accepted!"<<endl;
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);


        //reserve client socket on behalf of child thread

        int n=pthread_create(&t_id, &attr, &serve_it,(void*)new_fd);
        cout<<"thread number: "<<n<<endl;
        sleep(0); // Giving threads some CPU time

    }//while
    
    return 0;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void* serve_it(void* arg)
{
	string receive;
 	wordCount wc=wordCount();

 	int Client = (int) (arg);
    char buf[256];
 	bzero(buf,256);



	printf("Server %d: accepted = %d\n", getpid(), Client);

	if (new_fd < 0) 
	{ 
       	 	printf("Accept error on server\n");
       		printf("ERROR on accept"); 
        	exit(1);
	}

 	printf("Connection %d accepted\n", new_fd);

	while(1){
		bzero(buf,256);
		int bytes_receive = recv(Client,buf,sizeof(buf),0);
		if (bytes_receive == 0)
			break;

		printf("%d\n", bytes_receive);

		if (bytes_receive < 0)
		{
			printf("ERROR reading from socket\n");
			exit(1);
		}

		receive+=buf;
	}
	close(Client);

	wc.setStr(receive);
	//wc.printFWord();

	cout<<"number of characters:";
	cout<<wc.getNchar()<<endl;

	cout<<"number of word:";
	cout<<wc.getNword()<<endl;

	cout<<"number of sentences:";
	cout<<wc.getNSentence()<<endl;

	pthread_exit(0);
}

