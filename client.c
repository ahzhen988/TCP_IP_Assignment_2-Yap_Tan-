#include "inet.h"
#include "clientFunc.c"





void printMenu() {
  printf ("+=================================================+\n");
  printf ("| 	         File Repository System           |\n");
  printf ("+=================================================+\n");
  printf ("| 1 - Create File                                 |\n");
  printf ("| 2 - Download File from Server                   |\n");
  printf ("| 3 - Send File To Server                         |\n");
  printf ("| 4 - Delete File                                 |\n");
  printf ("| 4 - Exit Program                                |\n");
  printf ("+=================================================+\n");
}

int main(int argc, char *argv[])	//Connecting to Server (SOCKET)
{
    int sockfd , n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    char buffer[256];
    if (argc < 2) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(SERV_TCP_PORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    printf("\n\nYou are now connected to the Server!\n");



    int count = 0;
    while(count == 0){	//Getting Client's choice
	int proceed = 0;
	//printf("\n\n1.Create 2.Download 3.Send 4.Delete 5.Exit : ");
	printMenu();
	printf("Please insert your choice: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
    char input[256];
	strcpy(input, buffer);
	
	//Sending Client's choice to Server
	int datalen = strlen(buffer);
	int tmp = htonl(datalen);
	n = write(sockfd, (char*)&tmp, sizeof(tmp));
	if(n < 0) error("ERROR writing to socket");
	n = write(sockfd,buffer,datalen);
	if (n < 0) error("ERROR writing to socket");
	

	if((strcmp(input, "1\n")) == 0){	//Create file on client-site
	   createFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "2\n")) == 0){	//Client download file from Server
	   downloadFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "3\n")) == 0){	//Client send file to Server
	   sendFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "4\n")) == 0){	//Delete file on client-site
	   deleteFile(sockfd);
	   count = 0;
	}
	else if((strcmp(input, "5\n")) == 0){	//Client disconnect from Server
	   count = 1;
	   proceed = 1;
	}
	else{
	   printf("\nWrong input, please try again.");	//Invalid input from Client
	   count = 0;
	}
    }
	
    close(sockfd);
    printf("\nYou have disconnected from the Server.\n\n");
    return 0;
}