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

//#define PORT 4444
//#define UDP_SERVER_SOCKET_PATH  "/home/baisacsa/training/osbmc/tpf_unix_domain_sock.server"
//#define UDP_CLIENT_SOCKET_PATH  "/home/baisacsa/training/osbmc/tpf_unix_domain_sock.client"
#define BUFFER_SIZE 1024

typedef struct unix_domain_format{
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    int client_sockfd_length;
    const char *client_socket_path;
    const char *server_socket_path;
}UNIX_DOMAIN_FORMAT_D;

typedef struct tcp_domain_format{
    struct sockaddr_in server_sockaddr;
    struct sockaddr_in client_sockaddr;
    int server_sockfd;
    int client_sockfd;
    int server_sockfd_length;
    const char *ip_address;
    const char *port;
}TCP_DOMAIN_FORMAT_D;
//----------------------TCP
int create_tcp_socket(TCP_DOMAIN_FORMAT_D *tcp_domain)
{
    int rc = EXIT_SUCCESS;
    tcp_domain->client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_domain->client_sockfd == -1){
        printf("[-] TCP SOCKET ERROR: %s\n", strerror(errno));
        rc = EXIT_FAILURE;
    }
    else{
        printf("[+]Success in creating TCP socket!\n");
    }
    return rc;
}

void initialize_tcp(TCP_DOMAIN_FORMAT_D *tcp_domain)
{
    memset(&(tcp_domain->server_sockaddr), '\0', sizeof(tcp_domain->server_sockaddr));
    tcp_domain->server_sockfd_length = sizeof(tcp_domain->server_sockaddr);
}

void set_tcp_sockaddr(TCP_DOMAIN_FORMAT_D *tcp_domain)
{
    int a = 0;
    tcp_domain->server_sockaddr.sin_family = AF_INET;
    //tcp_domain->server_sockaddr.sin_port = htons(PORT);
    //tcp_domain->server_sockaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    a = atoi(tcp_domain->port);
    tcp_domain->server_sockaddr.sin_port = htons((uint16_t)a);
    tcp_domain->server_sockaddr.sin_addr.s_addr = inet_addr(tcp_domain->ip_address);
}

int disconnect_tcp_socket_from_server(TCP_DOMAIN_FORMAT_D *tcp)
{
    int rc = EXIT_FAILURE;
    
    printf("Disconnected from %s:%d\n", inet_ntoa(tcp->client_sockaddr.sin_addr), ntohs(tcp->client_sockaddr.sin_port));
    close(tcp->client_sockfd);
                                                
    return rc;
}

void remove_newline(char *buffer)
{
    int len = 0;
    len = strlen(buffer);
    if (len > 1){
        if( buffer[len-1] == '\n' ){
            buffer[len-1] = '\0';
        }
    }
}

int verify_string_keyword(char *buffer, const char *keyword)
{
    int rc = EXIT_FAILURE;
    int string_lower_bounds = 0;
    int string_upper_bounds = 0;
    
    string_lower_bounds = strncmp(buffer, keyword, strlen(buffer)); 
    string_upper_bounds = strncmp(buffer, keyword, strlen(keyword));
    
    if(string_lower_bounds == 0 && string_upper_bounds ==0){
        rc = EXIT_SUCCESS;
    }
                    
    return rc;
}

int connect_tcp_client_to_server(TCP_DOMAIN_FORMAT_D *tcp_domain)
{
    int rc = EXIT_SUCCESS;
    char buffer[BUFFER_SIZE];
    const char *exit_keyword = ":exit";
    int message_receive_size = 0;
    
    rc = connect(tcp_domain->client_sockfd, (struct sockaddr*)&(tcp_domain->server_sockaddr), (socklen_t) tcp_domain->server_sockfd_length);
    if(rc == -1){
	printf("[-]Error in connecting to server (%s:%d): %s\n",inet_ntoa(tcp_domain->client_sockaddr.sin_addr), ntohs(tcp_domain->client_sockaddr.sin_port), strerror(errno));
	rc = EXIT_FAILURE;
    } else {
        rc = EXIT_SUCCESS;
    }
    if (rc == EXIT_SUCCESS){
        printf("[+] Connected to server (%s:%d)\n",inet_ntoa(tcp_domain->client_sockaddr.sin_addr), ntohs(tcp_domain->client_sockaddr.sin_port));
        while(1){
                    printf("Client (':exit' to end): \t");
                    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL)
                    {
                        printf("fgets error!\n");
                    }
                    remove_newline(buffer);
                    
                    rc = send(tcp_domain->client_sockfd, buffer, strlen(buffer), 0);
                    if (rc == -1){
                        rc = disconnect_tcp_socket_from_server(tcp_domain);
                        break;
                    }
                    rc = verify_string_keyword(buffer, exit_keyword);
                    
                    if(rc == EXIT_SUCCESS){
                            rc = disconnect_tcp_socket_from_server(tcp_domain);
                            break;
                    }
                    message_receive_size = recv(tcp_domain->client_sockfd, buffer, BUFFER_SIZE, 0);
                    if(message_receive_size > 0){
                            buffer[message_receive_size] = '\0';
                            printf("Server: \t%s\n", buffer);
                    }else{
                            rc = disconnect_tcp_socket_from_server(tcp_domain);
                            break;
                    }
            }   
    }
    return rc;
}

int use_tcp_communication_domain(const char *ip_address, const char *port)
{
    int rc = EXIT_SUCCESS;
    TCP_DOMAIN_FORMAT_D *tcp_domain= (TCP_DOMAIN_FORMAT_D *)malloc(sizeof(*tcp_domain));
    if (tcp_domain){
        tcp_domain->ip_address = ip_address;
        tcp_domain->port = port;
        rc = create_tcp_socket(tcp_domain);
        if (rc == EXIT_SUCCESS){
            initialize_tcp(tcp_domain);
            set_tcp_sockaddr(tcp_domain);
            rc = connect_tcp_client_to_server(tcp_domain);
        }
         close(tcp_domain->client_sockfd);
    } else {
        printf("TCP domain malloc allocation error\n");
        rc = EXIT_FAILURE;
    }
    free(tcp_domain);
    return rc;
}


//-----------------------UNIX
int create_unix_socket(UNIX_DOMAIN_FORMAT_D *unix_data_d)
{
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_socket = unix_data_d;
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

void initialize_unix(UNIX_DOMAIN_FORMAT_D *unix_data_d)
{
    UNIX_DOMAIN_FORMAT_D *unix_data = unix_data_d;
    memset(&(unix_data->client_sockaddr), '\0', sizeof(unix_data->server_sockaddr));
    unix_data->client_sockfd_length = sizeof(unix_data->client_sockaddr);
    unix_data->server_sockfd_length = sizeof(unix_data->server_sockaddr);
}

void set_unix_sockaddr(UNIX_DOMAIN_FORMAT_D *unix_data)
{
    unix_data->client_sockaddr.sun_family = AF_UNIX;
    strncpy(unix_data->client_sockaddr.sun_path, unix_data->client_socket_path, sizeof(unix_data->client_sockaddr.sun_path)); 
    unix_data->server_sockaddr.sun_family = AF_UNIX;
    strncpy(unix_data->server_sockaddr.sun_path, unix_data->server_socket_path, sizeof(unix_data->server_sockaddr.sun_path) ); 
}

int bind_unix_socket(UNIX_DOMAIN_FORMAT_D *unix_data)
{
    int rc = EXIT_SUCCESS;
    
    
    unlink(unix_data->client_socket_path);
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

int disconnect_unix_socket_from_server(UNIX_DOMAIN_FORMAT_D *unix_data)
{
    int rc = EXIT_FAILURE;
    
    printf("Disconnected from server filepath %s: %s\n",unix_data->server_sockaddr.sun_path, strerror(errno));
    close(unix_data->client_sockfd);
                                                
    return rc;
}

int connect_unix_client_to_server(UNIX_DOMAIN_FORMAT_D *unix_data)
{
    int rc = EXIT_SUCCESS;
    char buffer[BUFFER_SIZE];
    const char *exit_keyword = ":exit";
    int message_receive_size = 0;
    
    rc = connect(unix_data->client_sockfd, (struct sockaddr*)&(unix_data->server_sockaddr), (socklen_t) unix_data->server_sockfd_length);
    if(rc == -1){
	printf("[-]Error in connecting to server filepath %s: %s\n",unix_data->server_sockaddr.sun_path, strerror(errno));
	rc = EXIT_FAILURE;
    } else {
        rc = EXIT_SUCCESS;
    }
    if (rc == EXIT_SUCCESS){
        printf("[+]Connected to Server filepath %s\n",unix_data->server_sockaddr.sun_path);
        while(1){
                    printf("Client (':exit' to end): \t");
                    if(fgets(buffer,BUFFER_SIZE,stdin)== NULL)
                    {
                        printf("fgets error!\n");
                    }
                    remove_newline(buffer);
               
                    rc = send(unix_data->client_sockfd, buffer, strlen(buffer), 0);
                    if (rc == -1){
                        rc = disconnect_unix_socket_from_server(unix_data);
                        break;
                    }
                    rc = verify_string_keyword(buffer, exit_keyword);
                    if(rc == EXIT_SUCCESS){
                            rc = disconnect_unix_socket_from_server(unix_data);
                            break;
                    }

                    message_receive_size = recv(unix_data->client_sockfd, buffer, BUFFER_SIZE, 0);
                    if(message_receive_size > 0){
                            buffer[message_receive_size] = '\0';
                            printf("Server: \t%s\n", buffer);
                    }else{
                            rc = disconnect_unix_socket_from_server(unix_data);
                            break;
                    }
            }   
    }
    return rc;
}

int use_unix_communication_domain(const char *server_socket_path, const char *client_socket_path)
{
    int rc = EXIT_SUCCESS;
    UNIX_DOMAIN_FORMAT_D *unix_data= (UNIX_DOMAIN_FORMAT_D *)malloc(sizeof(*unix_data));
    if (unix_data){
        unix_data->server_socket_path = server_socket_path;
        unix_data->client_socket_path = client_socket_path;
        rc = create_unix_socket(unix_data);
        if (rc == EXIT_SUCCESS){
            initialize_unix(unix_data);
            set_unix_sockaddr(unix_data);
            rc = bind_unix_socket(unix_data);
            if (rc == EXIT_SUCCESS){
               rc = connect_unix_client_to_server(unix_data);
            }
        }
        close(unix_data->client_sockfd);
    } else {
        printf("Unix domain malloc allocation error\n");
        rc = EXIT_FAILURE;
    }
    free(unix_data);
    return rc;
}
//-----------------------------------------------------------------------

void display_proper_argument_format()
{
    printf ("Follow proper argument format!\n");
    printf ("UNIX CONNECTION: $./multiplex_server unix server_socket_path client_socket_path\n");
    printf ("TCP CONNECTION: $./multiplex_server tcp server_ip_address server_port_number\n");
}


int main(int argc, char *argv[]){
    int rc = EXIT_SUCCESS;
    char *domain_type;
    const char *tcp_keyword = "tcp";
    const char *unix_keyword = "unix";
    int tcp_type = 0;
    int unix_type = 0;
    if (argc <= 4){
        if (argc == 4){
            domain_type = argv[1];
            printf ("%s\n", domain_type);
            tcp_type = verify_string_keyword(domain_type, tcp_keyword);
            unix_type = verify_string_keyword(domain_type, unix_keyword);
            if (tcp_type == 0){
                printf ("Connection via TCP!");
                rc = use_tcp_communication_domain(argv[2],argv[3]);
            } else if (unix_type == 0){
                printf ("Connection via UNIX DOMAIN SOCKET!");
                rc = use_unix_communication_domain(argv[2],argv[3]);
            } else {
                display_proper_argument_format();
                rc = EXIT_FAILURE;
            }
        } else {
            display_proper_argument_format();
            rc = EXIT_FAILURE;
        }
    }
    else {
        display_proper_argument_format();
        rc = EXIT_FAILURE;
    }
    
      
    return rc;
}