/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main_server.c
 * Author: Lucky138
 *
 * Created on August 27, 2018, 12:04 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <pthread.h>

#define PORT 4444
#define MAX_QUEUE_LENGHT 10
#define UNIX_DOMAIN_SOCKET_PATH  "/home/baisacsa/socket"
#define BUFFER_SIZE 1024


typedef struct tcp_domain_format{
    struct sockaddr_in server_sockaddr;
    struct sockaddr_in client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int backlog;
    int rc;
}TCP_DOMAIN_FORMAT_D;


typedef struct unix_domain_format{
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int backlog;
    int rc;
}UNIX_DOMAIN_FORMAT_D;

typedef struct communication_domain{
    TCP_DOMAIN_FORMAT_D tcp_format;
    UNIX_DOMAIN_FORMAT_D unix_format;
}COMMUNICATION_DOMAIN_D;




typedef struct thread_info{
    pthread_t pid;
    int i;
}THREAD_INFO_T;


int create_unix_domain_socket(UNIX_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_domain = server_sockfd_d;
    unix_domain->server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (unix_domain->server_sockfd == -1){
        printf("[-] UNIX_DOMAIN SOCKET ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in creating UNIX_DOMAIN socket!\n");
    }
    
    return rc;
}

int create_tcp_socket(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
    tcp->server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp->server_sockfd == -1){
        printf("[-] TCP SOCKET ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in creating TCP socket!\n");
    }
    return rc;
}

void initialize_unix_domain_server(UNIX_DOMAIN_FORMAT_D *server_sockfd_d)
{
    UNIX_DOMAIN_FORMAT_D *unix_domain = server_sockfd_d;
    memset(&(unix_domain->server_sockaddr), 0, sizeof(struct sockaddr_un));
    memset(&(unix_domain->client_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(unix_domain->server_sockfd), 0, sizeof(int));
    memset(&(unix_domain->client_sockfd), 0, sizeof(int));
    memset(&(unix_domain->server_sockfd_length), 0, sizeof(int));
    memset(&(unix_domain->rc), 0, sizeof(int));
    unix_domain->server_sockfd_length = sizeof(unix_domain->server_sockaddr);
    unix_domain->backlog = (int) MAX_QUEUE_LENGHT;
}

void initialize_tcp_server(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
    TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
    memset(&(tcp->server_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(tcp->client_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(tcp->server_sockfd), 0, sizeof(int));
    memset(&(tcp->client_sockfd), 0, sizeof(int));
    memset(&(tcp->server_sockfd_length), 0, sizeof(int));
    memset(&(tcp->rc), 0, sizeof(int));
    tcp->server_sockfd_length = sizeof(tcp->server_sockaddr);
    tcp->backlog = (int) MAX_QUEUE_LENGHT;
}

void set_tcp_server_sockaddr(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
   TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
   //tcp->server_sockaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
   
   int option = 1;
   setsockopt(tcp->server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
   tcp->server_sockaddr.sin_addr.s_addr= INADDR_ANY;
   tcp->server_sockaddr.sin_family = AF_INET;
   tcp->server_sockaddr.sin_port = htons(PORT);
}

void set_unix_domain_server_sockaddr(UNIX_DOMAIN_FORMAT_D *server_sockfd_d)
{
    UNIX_DOMAIN_FORMAT_D *unix_domain = server_sockfd_d;
    unix_domain->server_sockaddr.sun_family = AF_UNIX;
    strcpy(unix_domain->server_sockaddr.sun_path, UNIX_DOMAIN_SOCKET_PATH); 
}

int listen_tcp_client(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
    rc = listen(tcp->server_sockfd, tcp->backlog);
    if (rc == -1){
        printf("[-] TCP LISTENING ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+] Listening for TCP client...\n");
    }
    return rc;
}

int listen_unix_domain_client(UNIX_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_domain = server_sockfd_d;
    rc = listen(unix_domain->server_sockfd, unix_domain->backlog);
    if (rc == -1){
        printf("[-] UNIX_DOMAIN LISTENING ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+] Listening for UNIX_DOMAIN client...\n");
    }
    return rc;
}

int bind_unix_domain_socket(UNIX_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_domain = server_sockfd_d;
    unlink(UNIX_DOMAIN_SOCKET_PATH);
    rc = bind(unix_domain->server_sockfd, (struct sockaddr *) &(unix_domain->server_sockaddr),(socklen_t) unix_domain->server_sockfd_length);
    if (rc == -1){
        printf("[-] UNIX_DOMAIN BIND ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in binding UNIX_DOMAIN socket to filepath: %s\n",unix_domain->server_sockaddr.sun_path);
    }
    return rc;
}

int bind_tcp_socket(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
    rc = bind(tcp->server_sockfd, (struct sockaddr *) &(tcp->server_sockaddr),(socklen_t) tcp->server_sockfd_length);
    if (rc == -1){
        printf("[-] TCP BIND ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in binding TCP socket to %s:%d\n", inet_ntoa(tcp->server_sockaddr.sin_addr), ntohs(tcp->server_sockaddr.sin_port));
    }
    return rc;
}
//-----------------------------------------------
int disconnect_from_tcp_client(TCP_DOMAIN_FORMAT_D *tcp)
{
    int rc = EXIT_FAILURE;
    
    printf("Disconnected from %s:%d\n", inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port));
    close(tcp->client_sockfd);
                                                
    return rc;
}
const char *identify_string_keyword(char *buffer)
{
    int help_result = 0;
    int name_result = 0;
    int version_result = 0;
    int hi_result = 0;
    
    const char *help_keyword = "--help";
    const char *name_keyword = "--name";
    const char *version_keyword = "--version";
    const char *hi_keyword = "--hi";
    const char *server_respond = '\0';
    
    help_result = strncmp(buffer, help_keyword, strlen(buffer));
    name_result = strncmp(buffer, name_keyword, strlen(buffer));
    hi_result = strncmp(buffer, hi_keyword, strlen(buffer));
    version_result = strncmp(buffer, version_keyword, strlen(buffer));
    if(help_result == 0){
        server_respond = "\nSPECIAL KEYWORDS: \n--hi\n--name\n--version\n";
    } else if (name_result == 0 ) {
        server_respond = "Shaye\n";
    } else if (hi_result == 0 ) {
        server_respond = "Hello! Glad to meet you :)\n";
    } else if (version_result == 0 ) {
        server_respond = "Version 1.0";
    } else {
        server_respond =  "I don't understand you! (Try '--help')\n";
    }       
    return server_respond;
}

int accept_tcp_client(TCP_DOMAIN_FORMAT_D *server_sockfd_d)
{
   int rc = EXIT_SUCCESS;
   TCP_DOMAIN_FORMAT_D *tcp = server_sockfd_d;
   const char *server_respond_buffer = '\0';
   const char *exit_keyword = ":exit";
   int message_receive_size = 0;
   int message_compare_result = 0;
   pid_t childpid;
   char buffer[BUFFER_SIZE];
   while(1){
        tcp->client_sockfd = accept(tcp->server_sockfd, (struct sockaddr*)&(tcp->client_sockaddr), (socklen_t *) &(tcp->server_sockfd_length));
        if(tcp->client_sockfd == -1){
            printf("[-] TCP ACCEPT ERROR: %s\n", strerror(errno));
            rc = EXIT_FAILURE;
	} else {
            rc = EXIT_SUCCESS;
        }
        
        if (rc == EXIT_SUCCESS){
            printf("[+] TCP connection accepted from %s:%d\n",  inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port));
            if((childpid = fork()) == 0){
                while(1){
                    message_receive_size = recv(tcp->client_sockfd, buffer, BUFFER_SIZE, 0);
                    if (message_receive_size != -1){
                        buffer[message_receive_size] = '\0';
                        message_compare_result = strncmp(buffer, exit_keyword, sizeof(buffer));
                        if(( message_compare_result == 0) || (message_receive_size == 0)){
                            rc = disconnect_from_tcp_client(tcp);
                            break;
                        } else{
                            server_respond_buffer = identify_string_keyword (buffer);
                            printf("TCP client (%s:%d): %s\n",inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port), buffer);
                            send(tcp->client_sockfd, server_respond_buffer, strlen(server_respond_buffer), 0);
                        }
                    } else {
                        rc = disconnect_from_tcp_client(tcp);
                        break;
                    }
                }
            }
        }
    }
    return rc;
}

int disconnect_from_unix_client(UNIX_DOMAIN_FORMAT_D *unix_data)
{
    int rc = EXIT_FAILURE;
    
    printf("Disconnected from server filepath %s: %s\n",unix_data->server_sockaddr.sun_path, strerror(errno));
    close(unix_data->client_sockfd);
                                                
    return rc;
}
int accept_unix_domain_client(UNIX_DOMAIN_FORMAT_D *unix_domain)
{
   int rc = EXIT_SUCCESS;
    const char *server_respond_buffer = '\0';
   const char *exit_keyword = ":exit";
   int message_receive_size = 0;
   int message_compare_result = 0;
   pid_t childpid;
   char buffer[BUFFER_SIZE];
   while(1){
	unix_domain->client_sockfd = accept(unix_domain->server_sockfd, (struct sockaddr*)&(unix_domain->client_sockaddr), (socklen_t *) &(unix_domain->server_sockfd_length));
	if(unix_domain->client_sockfd == -1){
            printf("[-] UNIX_DOMAIN ACCEPT ERROR: %s\n", strerror(errno));
            rc = EXIT_FAILURE;
	} else {
            rc = EXIT_SUCCESS;
        }
        if(rc == EXIT_SUCCESS){
            printf("[+] UNIX_DOMAIN connection accepted from filepath: %s\n", unix_domain->client_sockaddr.sun_path);
            if((childpid = fork()) == 0){
                while(1){
                    message_receive_size = recv(unix_domain->client_sockfd, buffer, BUFFER_SIZE, 0);
                    if(message_receive_size != -1){
                        buffer[message_receive_size] = '\0';
                        message_compare_result = strncmp(buffer, exit_keyword, sizeof(buffer));
                        if(( message_compare_result == 0) || (message_receive_size == 0)){
                            rc = disconnect_from_unix_client(unix_domain);
                            break;
                        }else{
                            server_respond_buffer = identify_string_keyword (buffer);
                            printf("UNIX client (%s): %s\n",unix_domain->client_sockaddr.sun_path, buffer);
                            send(unix_domain->client_sockfd, server_respond_buffer, strlen(server_respond_buffer), 0);
                        }
                    }else{
                        rc = disconnect_from_unix_client(unix_domain);
                        break;
                    }
                }
            }
        }
    }
    return rc;
}

void *start_unix_domain_server(void *arg){
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_domain = arg;
    initialize_unix_domain_server(unix_domain);
    rc = create_unix_domain_socket(unix_domain);
    set_unix_domain_server_sockaddr(unix_domain);
    if (rc == EXIT_SUCCESS){
        rc = bind_unix_domain_socket(unix_domain);
        if (rc == EXIT_SUCCESS){
            rc = listen_unix_domain_client(unix_domain);
            if (rc == EXIT_SUCCESS){
                rc = accept_unix_domain_client(unix_domain);
             }
        }
    }
    
    
    sleep(5);
    return (void *)unix_domain;
}

void *start_tcp_server(void *arg){
    int rc = EXIT_SUCCESS;
    TCP_DOMAIN_FORMAT_D *tcp = arg;
    initialize_tcp_server(tcp);
    rc = create_tcp_socket(tcp);
    set_tcp_server_sockaddr(tcp);
    if (rc == EXIT_SUCCESS){
        rc = bind_tcp_socket(tcp);
    }
    if (rc == EXIT_SUCCESS){
        rc = listen_tcp_client(tcp);
    }
    if (rc == EXIT_SUCCESS){
        rc = accept_tcp_client(tcp);
    }
    sleep(5);
    return (void *)tcp;
}

int check_path_length(const char *socket_path)
{
    int rc = EXIT_SUCCESS;
    unsigned path_size = 0;
    struct sockaddr_un sun_path_size;
    path_size = strlen(socket_path);
    if (path_size >= sizeof(sun_path_size.sun_path)){
        printf("Path size exceeds limit size %lu\n", sizeof(sun_path_size.sun_path));
        rc = EXIT_FAILURE;
    }
    return rc;
}

/*
int invoke_threads(const char socket_path)
{
    COMMUNICATION_DOMAIN_D *communication_domain = (COMMUNICATION_DOMAIN_D *)malloc(sizeof(*communication_domain));
    THREAD_INFO_T thread_info
    if(communication_domain){
        
        
    } else {
        printf("Error in invoke_threads malloc allocation\n");
        rc = EXIT_FAILURE;
    }
    
}
*/

int main(int argc, char *argv[]) {
    int rc = EXIT_SUCCESS;
    const char *socket_path;
    UNIX_DOMAIN_FORMAT_D *unix_domain_server_data = (UNIX_DOMAIN_FORMAT_D *)malloc(sizeof(*unix_domain_server_data));
    TCP_DOMAIN_FORMAT_D *tcp_server_data = (TCP_DOMAIN_FORMAT_D *)malloc(sizeof(*tcp_server_data));
    pthread_t tcp_server_pid;
    pthread_t unix_domain_server_pid;
    
    if (argc <= 2){
        if (argc == 2)
        {
            socket_path = argv[1];
            rc = check_path_length(socket_path);
        } else {
            rc = EXIT_FAILURE;
        }
        
        if (rc != EXIT_FAILURE){
            rc = pthread_create(&unix_domain_server_pid, NULL,start_unix_domain_server, (void *) unix_domain_server_data);
            if (rc != EXIT_SUCCESS){
                printf("error creating unix_domain thread!");
                rc = EXIT_FAILURE;
                //   break;
            }

            if(rc == EXIT_SUCCESS){
                 rc = pthread_create(&tcp_server_pid, NULL,start_tcp_server, (void *) tcp_server_data);
                 if (rc != EXIT_SUCCESS){
                    printf("error creating tcp thread!");
                    rc = EXIT_FAILURE;
                 }
            }

    
    
            //-------------------join threads
            if (rc == EXIT_SUCCESS)
            {
                rc = pthread_join(unix_domain_server_pid, (void  *)&unix_domain_server_data);
                if (rc != EXIT_SUCCESS){
                    printf("error joining unix_domain thread!");
                    rc = EXIT_FAILURE;
                    //   break;
                }
            }
    
            if(rc == EXIT_SUCCESS){
                rc = pthread_join(tcp_server_pid, (void  *)&tcp_server_data);
                if (rc != EXIT_SUCCESS){
                    printf("error joining tcp thread!");
                    rc = EXIT_FAILURE;
                    //   break;
                }
            }
            
            close(unix_domain_server_data->client_sockfd);
            close(tcp_server_data->client_sockfd);
            close(unix_domain_server_data->server_sockfd);
            close(tcp_server_data->server_sockfd);
            
        }
    } else {
        rc = EXIT_FAILURE;
    }
    
    if (rc == EXIT_FAILURE){
        printf ("Follow proper argument format --> $./multiplex_server server_socket_path\n");
    }
    return rc;
}

