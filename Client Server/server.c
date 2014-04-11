

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define SEND_BINARY

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

#define rdtsc(x)      __asm__ __volatile__("rdtsc \n\t" : "=A" (*(x)))
unsigned long long start, finish;


#define SERVER_PORT	27015
// #define MAX_PENDING	5


#define MAX_PENDING     1
#define MAX_LINE	512
#define MaxThreads 10  

typedef struct {
	int id;
	int connections;
	
	
	
} Sobj;
int curThreads = 1;
int totalConnections = 0;
int totalSum = 0;

pthread_mutex_t mutexloc;
pthread_t callThd[MaxThreads];

void *cHandler(void *socket_desc)
{
	char f = 'f';
	char buf[MAX_LINE];
	 char reply[MAX_LINE];
	char sendf[MAX_LINE];
	 int len;
 	 int new_s = (int) socket_desc;
 	 int n;
	pthread_mutex_lock(&mutexloc);//------
	
	rdtsc(&finish);					
	double rtime = ((double)(finish-start))/(double)800000000;   			
	sprintf(sendf, "Greetings. Server has served %d clients and has been running for %lf seconds.\n",totalConnections, rtime);		
	len = strlen( sendf);
	send( new_s, sendf, len, 0);    
			
	pthread_mutex_unlock(&mutexloc);//-----	
    		
	while ((len = recv(new_s, buf, sizeof(buf), 0)) > 0)
	{
		n = atoi(buf);
		pthread_mutex_lock(&mutexloc);//-----	
		totalSum += n;
		pthread_mutex_unlock(&mutexloc);//-----	
		fprintf(stderr, "Total Sum:%d socket descriptor %d sent:%s \" \n ",totalSum, new_s, buf);
		
		sprintf(reply, "You sent:%s \n",buf);			
		len = strlen( reply);
		send( new_s, reply, len, 0); 
		memset(reply,0, MAX_LINE);  	
			

	} //end while(len>0)
    close(new_s); 
    pthread_mutex_lock(&mutexloc);//------      
    curThreads--;
    pthread_mutex_unlock(&mutexloc);//-----	
    pthread_exit(NULL);

    return 0;
}  

//////////////////////////////////////
int main(int argc, char *argv[])
{

  uint32_t ubsend[MAX_LINE];
  struct sockaddr_in sin;
  char buf[MAX_LINE];
  char reply[MAX_LINE];
  int conn, line;
  int len;
  int s, new_s;
  int c_one = 1;
  int rc;
  

  rdtsc(&start);		



/*							
	pthread_t *threads;
	pthread_attr_t pthread_custom_attr;
	Sobj *p;
	
	threads=(pthread_t *)malloc(10*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

	p=(Sobj *)malloc(sizeof(Sobj)*10);	
*/	

  printf("Starting Server...\n");

  //Initialize the addres data structure
  memset((void *)&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);

  //Create a socket
  if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    fprintf(stderr, "%s: socket - %s\n", argv[0], strerror(errno));
    exit(1);
  }

  // set the "reuse" option 
  rc = setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &c_one, sizeof(c_one));

  //Bind an address to the socketh 
  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    fprintf(stderr, "%s: bind - %s\n", argv[0], strerror(errno));
    exit(1);
  }

  
  //Set the length of the listen queue  
  if (listen(s, MAX_PENDING) < 0) {
    fprintf(stderr, "%s: listen - %s\n", argv[0], strerror(errno));
    exit(1);
  }

  conn = 0;

  //Loop accepting new connections and servicing them
  while (1) 
  {

    len = sizeof(sin);
    if ((new_s = accept(s, (struct sockaddr *)&sin, (socklen_t*)&len)) < 0) {
      fprintf(stderr, "%s: accept - %s\n",
	      argv[0], strerror(errno));
      exit(1);
    }
    ++totalConnections;
    ++curThreads;
    fprintf(stderr, "curThreads: %d  .\n", curThreads);	
    if(curThreads > 5)
    {
	fprintf(stderr, "Attempted connection while full...\n");	
	char sendf[MAX_LINE];
	sprintf( sendf, "%s\n", "Peer limit of 5 reached. Please try again later. \n" );
	len = strlen( sendf);
	send( new_s, sendf, len, 0);   
	close(new_s); 
	--curThreads;
    }
    else
    {
    	
	rdtsc(&finish);
	double rtime = ((double)(finish-start))/(double)800000000;        
	fprintf(stderr, "Time Elasped: %lf seconds.\n", rtime);	

	printf("New connection on fd:%d\n",new_s);
	++conn;
	line = 0;
	////////////////////////////////////////////////////////// 
	pthread_attr_t attr;
	pthread_mutex_init(&mutexloc,NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
	pthread_create(&callThd[new_s],&attr,cHandler,(void*)new_s);
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mutexloc); 
	/////////////////////////////////////////////////
     }

  } //end while(1)p
  
  close(s);
  
}








