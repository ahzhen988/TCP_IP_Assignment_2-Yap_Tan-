void dostuff(int); 
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void process (int sock)
{

   
   int count = 0;
   int buflen;
   while(count == 0){

	int n;
	char buffer[256];

	//Get Client's choice
	bzero(buffer,256);
	n = read(sock, (char*)&buflen, sizeof(buflen));
	if (n < 0) error("ERROR reading from socket");
	buflen = htonl(buflen);
	n = read(sock,buffer,buflen);
	if (n < 0) error("ERROR reading from socket");

	printf("\nClient's selection: %s\n",buffer);
	
	//Function for for different choices
	if(buffer != NULL){
	if((strcmp(buffer, "1\n")) == 0){	//Client perform create file on client-site
		count = 0;
	}
	else if((strcmp(buffer, "2\n")) == 0){	//Client request to download file from server
	
		printf("Sending file to Client...");
		char buff[256];
		int n;
		
		//Setting directory
		char dir[256] = "/root/Desktop/Testing2/";
		char file[256] = "Server/";
		strcat(dir, file);
		printf("\nPath: %s", dir);
			
		//Retrieving files from directory
		char tempo[256];
		printf("\nAvailable file: ");
		DIR *directory;
		struct dirent *ent;
		if((directory = opendir(dir)) != NULL){
		  while((ent = readdir(directory)) != NULL){
			//printf("\n%s", ent->d_name);
			strcat(tempo, ent->d_name);
			strcat(tempo, "\n");
		  }
		  closedir(directory);
		}
		else{
		  perror("ERROR");
		  exit(0);
		}

		//Sending files available to Client
		int datalen = strlen(tempo);
		int tmp = htonl(datalen);
		n = write(sock, (char*)&tmp, sizeof(tmp));
		if(n < 0) error("ERROR writing to socket");
		n = write(sock,tempo,datalen);
		if (n < 0) error("ERROR writing to socket");

		//Storing filename that Client wanted to download
		char fileRev[256];
		bzero(fileRev,256);
		n = read(sock, (char*)&buflen, sizeof(buflen));
		if (n < 0) error("ERROR reading from socket");
		buflen = htonl(buflen);
		n = read(sock,fileRev,buflen);
		if (n < 0) error("ERROR reading from socket");

		char split[2] = "\n";
		strtok(fileRev, split);
		printf("\nSending file %s to Client... \n", fileRev);
		
		//Sending files to Client
		if(fileRev != NULL){
			strcat(dir, fileRev);
			FILE *fs = fopen(dir, "rb");
			if(fs == NULL){
			  printf("ERROR: File not found.\n");
			  perror("fopen");
			  break;
			}
			else{
			  //Writing file to Client
			  bzero(buff, 256);
			  int fs_block_sz;
			  while((fs_block_sz = fread(buff, sizeof(char), 256, fs)) > 0){
			    if(send(sock, buff, fs_block_sz, 0) < 0){
				fprintf(stderr, "ERROR: Failed to send file. %d", errno);
				break;
			    }
			    bzero(buff, 256);
			  }
			  printf("\nFile sent successfully!\n");
		 	  fclose(fs);
			}
		}
		else{
			printf("\nERROR: Filename cannot be NULL");		
			printf("\nERROR: Please try again later");
			exit(0);
		}
		count = 0;
	}
	else if((strcmp(buffer, "3\n")) == 0){ //Client sending file to Server

		printf("Receiving file from Client... ");
		char revBuff[256];
		
		//Setting directory
		char dir[256] = "/root/Desktop/Testing2/";
		char file[256] = "Server/";
		strcat(dir, file);
		printf("\nPath: %s", dir);
		
		//Create directory if it does not exist
		struct stat st = {0};
		if(stat(dir, &st) == -1){
		  mkdir(dir, 0700);
		}
			
		//Getting file name that is send by Client
		char tempo[256];
		bzero(tempo,256);
		n = read(sock, (char*)&buflen, sizeof(buflen));
		if (n < 0) error("ERROR reading from socket");
		buflen = htonl(buflen);
		n = read(sock,tempo,buflen);
		if (n < 0) error("ERROR reading from socket");

		char split[2] = "\n";
		strtok(tempo, split);

		printf("\nFile saved as: %s\n", tempo);
		
		char filename[256];
		strcpy(filename, tempo);

		//Receiving file from Client
		if(filename != NULL){
			strcat(dir, filename);
			printf("\nFile directory: %s", dir);
			FILE *fr = fopen(dir, "w+");
			if(fr == NULL){
			  printf("File cannot be opened");
			  perror("fopen");
			  break;
			}
			else{
			  //Reading file from Client
			  bzero(revBuff, 256);
			  int fr_block_sz = 0;
			  while((fr_block_sz = recv(sock, revBuff, 256, 0)) > 0){
			  	int write_sz = fwrite(revBuff, sizeof(char), fr_block_sz, fr);
				if(write_sz < fr_block_sz){
				  error("File write failed on server.\n");
				}
	 			bzero(revBuff, 256);
				if(fr_block_sz == 0 || fr_block_sz != 256){
				  break;			
				}
			  }
			  printf("\nReceived successfully\n");
			  fclose(fr);
			}
		}		
		else{
			printf("\nERROR: Filename cannot be NULL");		
			printf("\nERROR: Please try again later");
			break;
		}
		count = 0;
	}
	else if((strcmp(buffer, "4\n")) == 0){	//Client perform delete function on client-site
		count = 0;
	}
	else if((strcmp(buffer, "5\n")) == 0){	//Client request to disconnect from Server
		printf("Client is disconnecting from the Server... ");
		count = 1;
	}
	else{	//Invalid input 
		printf("\nClient inserted wrong input.\n\n");
		count = 0;
		break;
	}
	}

   }
}