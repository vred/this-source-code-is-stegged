#include <sys/types.h>	  	  		   	  	     	     
#include <sys/stat.h> 	      	      		   	       	 	 
#include <sys/socket.h>	      	     	   	   	       	     	   
#include <netinet/in.h>  	      	      		   	  
#include <arpa/inet.h>	 	    
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include "../SNFS.h"


//contains the port where the server is listening
int port;
char* root_path;

void* request_handler(void* connection){
	int bytes_recieved = 0;
	char rec_data[4096];
	int connected = *(int*)connection;

	//Receive the request

	bytes_recieved = recv(connected,rec_data,4096,0);

	rec_data[bytes_recieved]='\0';

	fflush(stdout);

	char* response;

	char* request_copy = strdup(rec_data);
	char* tokenreq = strtok(rec_data," ,");
	char* firstarg = tokenreq;
	//switch on first argument of request which is system call name
	if( !strcmp(firstarg,"readdir") )
	{
		tokenreq = strtok(NULL, " ,");
		response = remote_readdir(tokenreq);
	} else if( !strcmp(firstarg,"getattr")){
		tokenreq = strtok(NULL, " ,");
		response = remote_getattr(tokenreq);
	} else if( !strcmp(firstarg,"opendir")){
		tokenreq = strtok(NULL, " ,");
		response = remote_opendir(tokenreq);
	} else if( !strcmp(firstarg,"releasedir")){
		tokenreq = strtok(NULL, " ,");
		response = remote_releasedir(tokenreq);
	} else if( !strcmp(firstarg,"mkdir")){
		tokenreq = strtok(NULL, " ,");
		mode_t mode = (mode_t)atoi(strtok(NULL, " ,"));
		response = remote_mkdir(tokenreq,mode);
	} else if( !strcmp(firstarg,"read")){
		tokenreq = strtok(NULL, " ,");
		size_t size = (size_t)atoi(strtok(NULL, " ,"));
		off_t offset = (off_t)atoi(strtok(NULL, " ,"));
		response = remote_read(tokenreq,size,offset);
	} else if( !strcmp(firstarg,"open")){
		tokenreq = strtok(NULL, " ,");
		int flags = atoi(strtok(NULL, " ,"));
		response = remote_open(tokenreq, flags);
	} else if( !strcmp(firstarg,"release")){
		tokenreq = strtok(NULL, " ,");
		response = remote_release(tokenreq);
	} else if( !strcmp(firstarg,"write")){
		tokenreq = strtok(NULL, " ,");
		size_t size = (size_t)atoi(strtok(NULL, " ,"));
		off_t offset = (off_t)atoi(strtok(NULL, " ,"));
		//cut off the string so it just has the info to be written
		response = remote_write(tokenreq,size,offset,request_copy);
	} else if( !strcmp(firstarg,"truncate")){
		tokenreq = strtok(NULL, " ,");
		off_t newsize = (off_t)atoi(strtok(NULL, " ,"));
		response = remote_truncate(tokenreq,newsize);
	} else if( !strcmp(firstarg,"create")){
		tokenreq = strtok(NULL, " ,");
		mode_t mode = (mode_t)atoi(strtok(NULL, " ,"));
		response = remote_create(tokenreq,mode);
	} else if( !strcmp(firstarg,"unlink")){
		tokenreq = strtok(NULL, " ,");
		response = remote_unlink(tokenreq);
	}



	send(connected,response,4096,0);

	free(response);
	close(connected);
	free((int*)connection);
	pthread_exit(0);
}

char* remote_create(const char *path, mode_t mode){
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);

	char* point = (char*)calloc(1,sizeof(int));

	int fd = creat(full_path, mode);
	sprintf(point,"%d",fd);
	return point;
}

char* remote_truncate(const char *path, off_t newsize){
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);

	int retstat = truncate(full_path, newsize);
	char* resp=(char*)malloc(sizeof(int)+1);;
	sprintf(resp,"%d",retstat);
	return resp;
}

char* remote_write(const char *pointer, size_t size, off_t offset, char* buffer){
	char* buffer_freer = buffer;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	buffer = strchr(buffer,',')+1;
	int x = pwrite(atoi(pointer),buffer,size,offset);
	char* written=(char*)malloc(sizeof(int)+1);;
	sprintf(written,"%d",x);
	free(buffer_freer);
	return written;
}

char* remote_release(const char *pointer){
	close(atoi(pointer));
	char* ret_val = (char*)calloc(1,1);
	return ret_val;
}

char* remote_read(const char *pointer, size_t size, off_t offset){
	char* buffer = (char*)calloc(size,sizeof(char));
	pread(atoi(pointer),buffer,size,offset);
	return buffer;
}

char* remote_mkdir(const char *path, mode_t mode){

	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	int retstat = mkdir(full_path, mode);
    char* resp=(char*)malloc(sizeof(int)+1);;
	sprintf(resp,"%d",retstat);
	return resp;
}

char* remote_releasedir(const char *pointer){
	closedir((DIR *) (uintptr_t) strtoull(pointer,NULL,0));
	char* ret_val = (char*)calloc(1,1);
	return ret_val;
}

char* remote_opendir(const char *path){
	DIR* dp;

	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	char* direct = (char*)calloc(sizeof(DIR*),sizeof(char));
	dp = opendir(full_path);
	sprintf(direct,"%lu",(long unsigned int)dp);
	//free(full_path);
	return direct;
}

char* remote_getattr(const char *path){
	struct stat* stat_buf = (struct stat*)malloc(sizeof(struct stat));

	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);

	lstat(full_path,stat_buf);

	char* status = (char*)calloc(sizeof(struct stat),sizeof(char));
	sprintf(status,"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d",(int)stat_buf->st_mode,(int)stat_buf->st_ino,(int)stat_buf->st_dev,(int)stat_buf->st_uid,
						(int)stat_buf->st_gid,(int)stat_buf->st_atime,(int)stat_buf->st_ctime,
							(int)stat_buf->st_mtime,(int)stat_buf->st_nlink,(int)stat_buf->st_size);
	//free(full_path);
	return status;
}

char* remote_readdir(const char *path){
	DIR *dir;
	char* entry_list = (char*)calloc(1,sizeof(char));
	struct dirent *ent = (struct dirent*)malloc(sizeof(struct dirent));

	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);

	if ((dir = opendir (full_path)) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		if(strlen(ent->d_name) > 0)
			entry_list = (char*)realloc(entry_list,strlen(entry_list)+FILENAME_MAX+2);
	    entry_list = strcat(entry_list,", ");
	    entry_list = strcat(entry_list,ent->d_name);
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
	  perror ("");
	  return NULL;
	}
	//free(full_path);
	free(ent);
	return entry_list;
}

char* remote_open(const char *path, int flags){
	int fd;

	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);

	char* file_pointer = (char*)calloc(sizeof(int),sizeof(char));
	fd = open(full_path, flags);

	sprintf(file_pointer,"%d",fd);
	//free(full_path);
	return file_pointer;
}

char* remote_unlink(const char *path){
	char* full_path = (char*)malloc(strlen(path)+strlen(root_path)+1);
	strcpy(full_path,root_path);
	strcat(full_path,path);
	unlink(full_path);
	char* ret_val = (char*)calloc(1,1);
	//free(full_path);
	return ret_val;
}

int main(int argc, char *argv[]){
	if(argc != 5){
		fprintf(stderr,"Invalid number of arguments!\n");
		return -1;
	}

	//Check the flags
	if(!strcmp(argv[1],"-port")){
		port = atoi(argv[2]);
	}
	else if(!strcmp(argv[1],"-mount")){
		root_path = strdup(argv[2]);
	}
	else{
		perror("Incorrect arguments!");
		return -1;
	}
	if(!strcmp(argv[3],"-port")){
		port = atoi(argv[4]);
	}
	else if(!strcmp(argv[3],"-mount")){
		root_path = strdup(argv[4]);
	}
	else{
		perror("Incorrect arguments!");
		return -1;
	}

	int sock, true = 1;


	struct sockaddr_in server_addr,client_addr;
	int sin_size;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&true,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(server_addr.sin_zero),8);

	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr))
																   == -1) {
		perror("Unable to bind");
		exit(1);
	}

	if (listen(sock, 5) == -1) {
		perror("Listen");
		exit(1);
	}

	fflush(stdout);


	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);
		int* connected = (int*)malloc(sizeof(int));
		*connected = accept(sock, (struct sockaddr *)&client_addr,(socklen_t * __restrict__)&sin_size);

		pthread_t thread;
		pthread_create(&thread, NULL, request_handler, (void *) connected);
		pthread_detach(thread);


	}

  close(sock);
  free(root_path);
  return 0;
}
