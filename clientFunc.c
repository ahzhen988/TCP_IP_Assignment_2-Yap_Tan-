void error(const char *msg){
    perror(msg);
    exit(0);
}

void createFile(int sockfd){	//Creating file on client-site
  	printf("Creating a file...");
	
	//Setting directory
	char content[256];
	char dir[256] = "/root/Desktop/Testing2/";
	char file[256] = "Client/";
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	//Create directory if it does not exist	
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}
	
	//Create file name
	char filename[256];
	printf("\nPlease enter file name: ");
	fgets(filename, 256, stdin);
	
	if(filename != NULL){
		strcat(dir, filename);
		printf("File location: %s", dir);
		//Create file
		FILE *fp;
		fp = fopen(dir, "w+");
		if(fp == NULL){
		  printf("\nERROR: File cannot be created");
		  perror("fopen");	
		  exit(0);	
		}
		else{	//Client insert content
		  printf("Please insert the content here: ");
		  fgets(content, 256, stdin);
		  printf("Content: %s", content);
		 
		  fprintf(fp, "%s", content);	//Write content into the file
		  fclose(fp);
		  printf("\nFile created successfully!");
		}
	}
	else{
		printf("\nERROR: Filename cannot be NULL");		
		printf("\nERROR: Please try again later");
		exit(0);
	}
}

void downloadFile(int sockfd){	//Client download file from Server
	printf("Downloading file from Server... ");
	
	int n;
	int buflen;

	//Setting directory
	char revBuff[256];
	char dir[256] = "/root/Desktop/Testing2/";
	char file[256] = "Client/";
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	//Create directory if it does not exist
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}

	//Getting available file from Server
	char tempo[256];
	bzero(tempo,256);
	n = read(sockfd, (char*)&buflen, sizeof(buflen));
	if (n < 0) error("ERROR reading from socket");
	buflen = htonl(buflen);
	n = read(sockfd,tempo,buflen);
	if (n < 0) error("ERROR reading from socket");
	printf("\nAvailable file: \n");
	printf("%s", tempo);

	printf("Please enter the file name that you wanted to download: ");
	char selectFile[256];
	bzero(selectFile,256);
	fgets(selectFile,255,stdin);
    	char input[256];
	
	//Sending file name that Client wants to download to Server
	int datalen = strlen(selectFile);
	int tmp = htonl(datalen);
	n = write(sockfd, (char*)&tmp, sizeof(tmp));
	if(n < 0) error("ERROR writing to socket");
	n = write(sockfd,selectFile,datalen);
	if (n < 0) error("ERROR writing to socket");
	
	char filename[256];
	printf("Save file name as: ");
	fgets(filename, 256, stdin);

	if(filename != NULL){
		strcat(dir, filename);	//Concatenate directory and filename
		printf("File location: %s", dir);

		FILE *fr = fopen(dir, "ab");
		if(fr == NULL){
		  printf("File cannot be opened");
		  perror("fopen");
		  exit(0);
		}
		else{	//Receiving file from Server 
		  bzero(revBuff, 256);
		  int fr_block_sz = 0;
		  while((fr_block_sz = recv(sockfd, revBuff, 256, 0)) > 0){
		  	int write_sz = fwrite(revBuff, sizeof(char), fr_block_sz, fr);
			if(write_sz < fr_block_sz){
			  error("File write failed on server.\n");
			}
			bzero(revBuff, 256);
			if(fr_block_sz == 0 || fr_block_sz != 256){
			  break;			
			}
		  }
		  printf("\nFile downloaded successfully");
		  fclose(fr);
		}
	}
	else{
		printf("\nERROR: Filename cannot be NULL");		
		printf("\nERROR: Please try again later");
		exit(0);
	}
}

void sendFile(int sockfd){	//Client send file to Server

	printf("Sending file to Server...");
	char buff[256];
	int n;
	
	//Setting directory
	char dir[256] = "/root/Desktop/Testing2/";
	char file[256] = "Client/";
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	//Create directory if it does not exist
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}
	
	//Printing files that is available from the directory
	printf("\nAvailable file: \n");
	DIR *directory;
	struct dirent *ent;
	if((directory = opendir(dir)) != NULL){
	  while((ent = readdir(directory)) != NULL){
		printf("%s", ent->d_name);
	  }
	  closedir(directory);
	}
	else{
	  perror("ERROR");
	  exit(0);
	}
	
	//Selecting file to be sent to Server
	char tempo[256];
	printf("\nPlease enter the file name that you want to send: ");
	fgets(tempo, 256, stdin);
	char filename[256];
	strcpy(filename, tempo); 
	
	if(filename != NULL){

		//Sending the file name to Server
		int datalen = strlen(tempo);
		int tmp = htonl(datalen);
		n = write(sockfd, (char*)&tmp, sizeof(tmp));
		if(n < 0) error("ERROR writing to socket");
		n = write(sockfd,tempo,datalen);
		if (n < 0) error("ERROR writing to socket");
	
		char split[2] = "\n";
	 	strtok(tempo, split);

		strcat(dir, filename);
		printf("Sending %s to Server... ", tempo);
		printf("\nDir: %s", dir);
	
		FILE *fs = fopen(dir, "rb");	//Read file
		if(fs == NULL){
		  printf("\nERROR: File not found.\n");
		  perror("fopen");
		  exit(0);
		}
		else{	//Sending file to Server
		  bzero(buff, 256);
		  int fs_block_sz;
		  while((fs_block_sz = fread(buff, sizeof(char), 256, fs)) > 0){
		    if(send(sockfd, buff, fs_block_sz, 0) < 0){
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
		
}

void deleteFile(int sockfd){	//Deleting file on client-site
	printf("Deleting a file...");
	
	//Setting directory
	char content[256];
	char dir[256] = "/root/Desktop/Testing2/";
	char file[256] = "Client/";
	strcat(dir, file);
	printf("\nPath: %s", dir);
	
	//Create directory if it does not exist	
	struct stat st = {0};
	if(stat(dir, &st) == -1){
	  mkdir(dir, 0700);
	}

	//Printing files that is available from the directory
	printf("\nAvailable file: \n");
	DIR *directory;
	struct dirent *ent;
	if((directory = opendir(dir)) != NULL){
	  while((ent = readdir(directory)) != NULL){
		printf("%s", ent->d_name);
	  }
	  closedir(directory);
	}
	else{
	  perror("ERROR");
	  exit(0);
	}

	//Getting file name to be deleted
	char filename[256];
	printf("\nPlease enter the file name that you want to delete: ");
	fgets(filename, 256, stdin);

	
	if(filename != NULL){

		strcat(dir, filename);
		FILE *fp;
		
		//Check if file available
		fp = fopen(dir, "r");
		if(fp == NULL){
		  printf("\nERROR: File cannot be created");
		  perror("fopen");	
		  exit(0);	
		}
		else{	//Deleting file
		  int status = remove(dir);
		  if(status == 0){
			printf("\nFile deleted successfully!");
			fclose(fp);
		  }else{
			printf("\nERROR: unable to delete the file");
			exit(0);
		  }
		}
	}
}