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

typedef struct unix_data{
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int client_sockfd_length;
}UNIX_DATA_T;

int create_socket(UNIX_DATA_T *unix_data_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DATA_T *unix_socket = unix_data_d;
    unix_socket->client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_socket->client_sockfd == -1){
        printf("[-] UDP SOCKET ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in creating UDP socket!\n");
    }
    return rc;
}

void initialize_unix(UNIX_DATA_T *unix_data_d)
{
    UNIX_DATA_T *unix_data = unix_data_d;
    memset(&(unix_data->client_sockaddr), '\0', sizeof(unix_data->server_sockaddr));
    unix_data->client_sockfd_length = sizeof(unix_data->client_sockaddr);
    unix_data->server_sockfd_length = sizeof(unix_data->server_sockaddr);
}

void set_unix_sockaddr(UNIX_DATA_T *unix_data_d)
{
    UNIX_DATA_T *unix_data = unix_data_d;
    unix_data->client_sockaddr.sun_family = AF_UNIX;
    strcpy(unix_data->client_sockaddr.sun_path, UDP_CLIENT_SOCKET_PATH); 
    unix_data->server_sockaddr.sun_family = AF_UNIX;
    strcpy(unix_data->server_sockaddr.sun_path, UDP_SERVER_SOCKET_PATH); 
}

int bind_unix_socket(UNIX_DATA_T *unix_data_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DATA_T *unix_data = unix_data_d;
    
    
    unlink(UDP_CLIENT_SOCKET_PATH);
    rc = bind(unix_data->client_sockfd, (struct sockaddr *) &(unix_data->client_sockaddr),(socklen_t) unix_data->client_sockfd_length);
    if (rc == -1){
        printf("[-] UDP BIND ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in binding UDP client socket to filepath: %s\n",unix_data->client_sockaddr.sun_path);
    }
    return rc;
}

int connect_unix_client_to_server(UNIX_DATA_T *unix_data_d)
{
    int rc = EXIT_SUCCESS;
    char buffer[4];
    UNIX_DATA_T *unix_data = unix_data_d;
    rc = connect(unix_data->client_sockfd, (struct sockaddr*)&(unix_data->server_sockaddr), (socklen_t) unix_data->server_sockfd_length);
    if(rc == -1){
	printf("[-]Error in connecting to server filepath %s: %s\n",unix_data->server_sockaddr.sun_path, strerror(errno));
	rc = EXIT_FAILURE;
    }
    if (rc == EXIT_SUCCESS){
        printf("[+]Connected to Server filepath %s\n",unix_data->server_sockaddr.sun_path);
        int t = 0;
        char *p;
        while(1){
                //int ch = 0;
                    printf("Client (':exit' to end): \t");
                    if(fgets(buffer,4,stdin)== NULL)
                    {
                        printf("hala");
                    }
                    //buffer[strcspn(buffer, "\r\n")] = 0;
                    if(p=strchr(buffer, '\n')){//check exist newline
                        *p = 0;
                    }
                    else {
                        scanf("%*[^\n]");scanf("%*c");//clear upto newline
                    }
                  /*  if(p=strchr(buffer, '\n')){//check exist newline
                        *p = 0;
                    } else {
                        scanf("%*[^\n]");scanf("%*c");//clear upto newline
                    }
                  */  
                    t= send(unix_data->client_sockfd, buffer, strlen(buffer), 0);
                    if (t == -1){
                        printf("ngeeeeeeeeeeeeeee");
                    }
                        
                    if(strcmp(buffer, ":exit") == 0){
                            close(unix_data->client_sockfd);
                            printf("[-]Disconnected from server.\n");
                            exit(1);
                    }

                    if(recv(unix_data->client_sockfd, buffer, 1024, 0) < 0){
                            printf("[-]Error in receiving data.\n");
                    }else{
                            printf("Server: \t%s\n", buffer);
                    }
            }   
    }
    return rc;
}
int use_unix_socket()
{
    int rc = EXIT_SUCCESS;
    UNIX_DATA_T *unix_data= (UNIX_DATA_T *)malloc(sizeof(unix_data));
    rc = create_socket(unix_data);
    if (rc == EXIT_SUCCESS){
        initialize_unix(unix_data);
        set_unix_sockaddr(unix_data);
        rc = bind_unix_socket(unix_data);
    }
    if (rc == EXIT_SUCCESS){
        rc = connect_unix_client_to_server(unix_data);
    }
    close(unix_data->client_sockfd);
    return rc;
}

int main(){
    int rc = EXIT_SUCCESS;
    rc = use_unix_socket();
        
	return rc;
}