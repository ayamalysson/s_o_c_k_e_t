#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/un.h>

#define PORT 4444
#define UDP_SERVER_SOCKET_PATH  "/home/baisacsa/training/osbmc/tpf_unix_sock.server"
#define UDP_CLIENT_SOCKET_PATH  "/home/baisacsa/training/osbmc/tpf_unix_sock.client"
int main(){

	int clientSocket, ret;
	struct sockaddr_un serverAddr,clientAddr;
	char buffer[1024];
        int clen,slen;
        int rc = 0 ;

	clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if(clientSocket < 0){
		printf("[-] UDP SOCKET ERROR: %s\n", strerror(errno));
	}
	printf("[+]UDP Client Socket is created.\n");
        
        memset(&clientAddr, '\0', sizeof(serverAddr));
	clientAddr.sun_family = AF_UNIX;
	strcpy(clientAddr.sun_path, UDP_CLIENT_SOCKET_PATH); 
        clen = sizeof(clientAddr); 
        
    
	
        
        unlink(UDP_CLIENT_SOCKET_PATH);
        rc = bind(clientSocket, (struct sockaddr *) &clientAddr,(socklen_t) clen);
        if (rc == -1){
            printf("[-] UDP BIND ERROR: %s\n", strerror(errno));
            close(clientSocket);
            exit(1);
        }
        
        
	serverAddr.sun_family = AF_UNIX;
	strcpy(serverAddr.sun_path, UDP_SERVER_SOCKET_PATH); 
        slen = sizeof(serverAddr); 
	ret = connect(clientSocket, (struct sockaddr*)&serverAddr, (socklen_t) slen);
	if(ret < 0){
		printf("[-]Error in connecting: %s\n", strerror(errno));
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	while(1){
		printf("Client (':exit' to end): \t");
		scanf("%s", &buffer[0]);
		send(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, ":exit") == 0){
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, 1024, 0) < 0){
			printf("[-]Error in receiving data.\n");
		}else{
			printf("Server: \t%s\n", buffer);
		}
	}
        close(clientSocket);
	return 0;
}