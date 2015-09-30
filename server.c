#include "inet.h"
#include "serverFunc.c"



int main() /* Creating Socket for multiple Client */
{

     int num;
     int sin_size; 

     int sockfd, newsockfd,pid;

     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
	 
	 printf("\n----Server Waiting For New Client Connection----\n");
	 
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(SERV_TCP_PORT);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
		printf("Client has connected from %s on socket %d\n", inet_ntoa(cli_addr.sin_addr), newsockfd);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             process(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}


