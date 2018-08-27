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
#define UDP_SOCKET_PATH  "/home/baisacsa/training/osbmc/tpf_unix_sock.server"
/*
 * 
 */
/*
typedef struct socket_structure{
    int socket_domain;
    int socket_type;
    int socket_protocol;
}SOCKET_STRUCTURE_T;

typedef struct server_socket_support{
    SOCKET_STRUCTURE_T tcp_socket;
    SOCKET_STRUCTURE_T udp_socket;
}SERVER_SOCKET_SUPPORT_T;
*/
typedef struct tcp_server_data{
    struct sockaddr_in server_sockaddr;
    struct sockaddr_in client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int backlog;
    int rc;
}TCP_SERVER_DATA_T;


typedef struct udp_server_data{
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int backlog;
    int rc;
}UDP_SERVER_DATA_T;

typedef struct udp_client_data{
    struct sockaddr_un client_sockaddr;
    int client_sockfd;
    int client_sockfd_length;
}UDP_CLIENT_DATA_T;

typedef struct thread_info{
    pthread_t pid;
    int i;
}THREAD_INFO_T;

int create_udp_socket(UDP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UDP_SERVER_DATA_T *udp = server_sockfd_d;
    udp->server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (udp->server_sockfd == -1){
        printf("[-] UDP SOCKET ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in creating UDP socket!\n");
    }
    
    return rc;
}

int create_tcp_socket(TCP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_SERVER_DATA_T *tcp = server_sockfd_d;
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

void initialize_udp_server(UDP_SERVER_DATA_T *server_sockfd_d)
{
    UDP_SERVER_DATA_T *udp = server_sockfd_d;
    memset(&(udp->server_sockaddr), 0, sizeof(struct sockaddr_un));
    memset(&(udp->client_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(udp->server_sockfd), 0, sizeof(int));
    memset(&(udp->client_sockfd), 0, sizeof(int));
    memset(&(udp->server_sockfd_length), 0, sizeof(int));
    memset(&(udp->rc), 0, sizeof(int));
    udp->server_sockfd_length = sizeof(udp->server_sockaddr);
    udp->backlog = (int) MAX_QUEUE_LENGHT;
}

void initialize_tcp_server(TCP_SERVER_DATA_T *server_sockfd_d)
{
    TCP_SERVER_DATA_T *tcp = server_sockfd_d;
    memset(&(tcp->server_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(tcp->client_sockaddr), 0, sizeof(struct sockaddr_in));
    memset(&(tcp->server_sockfd), 0, sizeof(int));
    memset(&(tcp->client_sockfd), 0, sizeof(int));
    memset(&(tcp->server_sockfd_length), 0, sizeof(int));
    memset(&(tcp->rc), 0, sizeof(int));
    tcp->server_sockfd_length = sizeof(tcp->server_sockaddr);
    tcp->backlog = (int) MAX_QUEUE_LENGHT;
}

void set_tcp_server_sockaddr(TCP_SERVER_DATA_T *server_sockfd_d)
{
   TCP_SERVER_DATA_T *tcp = server_sockfd_d;
   tcp->server_sockaddr.sin_addr.s_addr= inet_addr("127.0.0.1");
   //tcp->server_sockaddr.sin_addr.s_addr= INADDR_ANY;
   tcp->server_sockaddr.sin_family = AF_INET;
   tcp->server_sockaddr.sin_port = htons(PORT);
}

void set_udp_server_sockaddr(UDP_SERVER_DATA_T *server_sockfd_d)
{
    UDP_SERVER_DATA_T *udp = server_sockfd_d;
    udp->server_sockaddr.sun_family = AF_UNIX;
    strcpy(udp->server_sockaddr.sun_path, UDP_SOCKET_PATH); 
}

int listen_tcp_client(TCP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_SERVER_DATA_T *tcp = server_sockfd_d;
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

int listen_udp_client(UDP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UDP_SERVER_DATA_T *udp = server_sockfd_d;
    rc = listen(udp->server_sockfd, udp->backlog);
    if (rc == -1){
        printf("[-] UDP LISTENING ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+] Listening for UDP client...\n");
    }
    return rc;
}

int bind_udp_socket(UDP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    UDP_SERVER_DATA_T *udp = server_sockfd_d;
    unlink(UDP_SOCKET_PATH);
    rc = bind(udp->server_sockfd, (struct sockaddr *) &(udp->server_sockaddr),(socklen_t) udp->server_sockfd_length);
    if (rc == -1){
        printf("[-] UDP BIND ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in binding UDP socket to filepath: %s\n",udp->server_sockaddr.sun_path);
    }
    return rc;
}

int bind_tcp_socket(TCP_SERVER_DATA_T *server_sockfd_d)
{
    int rc = EXIT_SUCCESS;
    TCP_SERVER_DATA_T *tcp = server_sockfd_d;
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
int accept_tcp_client(TCP_SERVER_DATA_T *server_sockfd_d)
{
   TCP_SERVER_DATA_T *tcp = server_sockfd_d;
   int rc = EXIT_SUCCESS;
   pid_t childpid;
   char buffer[1024];
   while(1){
		tcp->client_sockfd = accept(tcp->server_sockfd, (struct sockaddr*)&(tcp->client_sockaddr), (socklen_t *) &(tcp->server_sockfd_length));
		if(tcp->client_sockfd == -1){
                    printf("[-] TCP ACCEPT ERROR: %s\n", strerror(errno));
                    rc = EXIT_FAILURE;
                    //exit(1);
		}
                
                if (rc == EXIT_SUCCESS){
                    printf("[+] TCP connection accepted from %s:%d\n",  inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port));

                    if((childpid = fork()) == 0){
                            //close(tcp->server_sockfd);

                            while(1){
                                    recv(tcp->client_sockfd, buffer, 1024, 0);
                                    if(strcmp(buffer, ":exit") == 0){
                                            printf("Disconnected from %s:%d\n", inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port));
                                            close(tcp->client_sockfd);
                                            break;
                                    }else{
                                            printf("TCP client (%s:%d): %s\n",inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port), buffer);
                                            send(tcp->client_sockfd, buffer, strlen(buffer), 0);
                                            bzero(buffer, sizeof(buffer));
                                    }
                            }
                    }
                }
	}
    return rc;
}

int accept_udp_client(UDP_SERVER_DATA_T *server_sockfd_d)
{
   UDP_SERVER_DATA_T *udp = server_sockfd_d;
   int rc = EXIT_SUCCESS;
   pid_t childpid;
   char buffer[1024];
   while(1){
		udp->client_sockfd = accept(udp->server_sockfd, (struct sockaddr*)&(udp->client_sockaddr), (socklen_t *) &(udp->server_sockfd_length));
		if(udp->client_sockfd == -1){
                    
                    printf("[-] UDP ACCEPT ERROR: %s\n", strerror(errno));
                    rc = EXIT_FAILURE;
                    //exit(1);
		}
                if(rc == EXIT_SUCCESS){
                    printf("[+] UDP connection accepted from filepath: %s\n", udp->client_sockaddr.sun_path);
                    if((childpid = fork()) == 0){
                            //close(udp->server_sockfd);
                            while(1){
                                    recv(udp->client_sockfd, buffer, 1024, 0);
                                    if(strcmp(buffer, ":exit") == 0){
                                            printf("Disconnected from filepath: %s\n", udp->client_sockaddr.sun_path);
                                            close(udp->client_sockfd);
                                            break;
                                    }else{
                                            printf("UDP Client (%s): %s\n", udp->client_sockaddr.sun_path, buffer);
                                            send(udp->client_sockfd, buffer, strlen(buffer), 0);
                                            bzero(buffer, sizeof(buffer));
                                    }
                            }
                    }
                }
	}
    return rc;
}

void *start_udp_server(void *arg){
    int rc = EXIT_SUCCESS;
    UDP_SERVER_DATA_T *udp = arg;
    initialize_udp_server(udp);
    rc = create_udp_socket(udp);
    set_udp_server_sockaddr(udp);
    if (rc == EXIT_SUCCESS){
        rc = bind_udp_socket(udp);
    }
    if (rc == EXIT_SUCCESS){
        rc = listen_udp_client(udp);
    }
    if (rc == EXIT_SUCCESS){
        rc = accept_udp_client(udp);
    }
    sleep(5);
    return (void *)udp;
}

void *start_tcp_server(void *arg){
    int rc = EXIT_SUCCESS;
    TCP_SERVER_DATA_T *tcp = arg;
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



int main() {
    int rc = EXIT_SUCCESS;
    UDP_SERVER_DATA_T *udp_server_data = (UDP_SERVER_DATA_T *)malloc(sizeof(udp_server_data));
    TCP_SERVER_DATA_T *tcp_server_data = (TCP_SERVER_DATA_T *)malloc(sizeof(tcp_server_data));
    pthread_t tcp_server_pid;
    pthread_t udp_server_pid;
    
    rc = pthread_create(&udp_server_pid, NULL,start_udp_server, (void *) udp_server_data);
    if (rc != EXIT_SUCCESS){
        printf("error creating udp thread!");
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
        rc = pthread_join(udp_server_pid, (void  *)&udp_server_data);
        if (rc != EXIT_SUCCESS){
            printf("error joining udp thread!");
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
    
    printf("Heyy!");
    //printf("--------------%d", udp_server_data->rc);
   /* while (i < 2){
        THREAD_INFO_T *thread_info = (THREAD_INFO_T*)malloc(sizeof(thread_info));
        rc = pthread_create(&(thread_info->pid), NULL,start_routine, (void *) udp_server_data);
        if (rc != 0){
            printf("error creating thread!");
            rc = EXIT_FAILURE;
         //   break;
        }
        rc = pthread_join(thread_info->pid, (void  *)&udp_server_data);
        i++;
    }*/
    close(udp_server_data->client_sockfd);
    close(tcp_server_data->client_sockfd);
    close(udp_server_data->server_sockfd);
    close(tcp_server_data->server_sockfd);

    return rc;
}

