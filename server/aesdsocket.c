#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <getopt.h>
#include <malloc.h>

#define BUFFER_SIZE 1000000
int sockfd;
int file_fd;

void sigint_handler(int signum) {
    syslog(LOG_INFO, "Caught signal, exiting");
    close(file_fd);
    close(sockfd);
    unlink("/var/tmp/aesdsocketdata"); // Delete the file
    closelog();
    exit(EXIT_SUCCESS);
}

size_t get_available_heap_size() {
    struct mallinfo mi = mallinfo();
    size_t available_heap = mi.fordblks;

    return available_heap;
}

void daemonize() {
    pid_t pid, sid;

    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS); // Parent process exits
    }

    umask(0); // Set file permissions

    sid = setsid(); // Create a new session
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


int main(int argc, char *argv[]) {
    int client_sock;
    struct sockaddr_in server_addr, client_addr;
    int port = 9000;

    openlog("aesdsocket_server", LOG_CONS | LOG_PID, LOG_USER);

    bool daemon_mode = false;
    
    char *buffer;
    buffer = (char*)malloc(BUFFER_SIZE*sizeof(char));
    memset(buffer, 0, BUFFER_SIZE*sizeof(char));
    size_t available_heap = get_available_heap_size();
    char *ptr = NULL;
    
    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd':
                daemon_mode = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [-d]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Daemonize if in daemon mode
    if (daemon_mode) {
        daemonize();
    }


    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

	int enable = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
    		perror("setsockopt(SO_REUSEADDR) failed");
	}


    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind the socket to the specified port
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == -1) {
        perror("listen");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //printf("Listening for incoming connections...\n");

    socklen_t client_addr_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];

    // Set up the signal handler for SIGINT (Ctrl+C) and SIGTERM
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

    while (1) {
        // Accept a connection
        client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("accept");
            continue;
        }
	
        // Log the accepted connection to syslog with client IP address
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        syslog(LOG_INFO, "Accepted connection from %s", client_ip);

        // Receive data and append to file
        file_fd = open("/var/tmp/aesdsocketdata", O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (file_fd == -1) {
            perror("open");
            close(client_sock);
            continue;
        }

	
	//char buffer[1024];
        ssize_t total_received = 0;
        ssize_t bytes_received;
	//printf("Available heap: %lu\n\r",available_heap);
        while ((bytes_received = recv(client_sock, buffer, BUFFER_SIZE*sizeof(char) - 1, 0)) > 0) {
            //printf("Available heap: %lu\n\r",available_heap);
            if ((total_received > available_heap)) {
                syslog(LOG_INFO, "Packet too large for available heap, closing connection");
                printf("Packet too large for available heap, closing connection");
                close(file_fd);
                close(client_sock);
                break;
            }
            //(bytes_received > available_heap)
            ptr = strchr(buffer, '\n');
            if (ptr != NULL){
            	//memset(buffer, 0, BUFFER_SIZE*sizeof(char));
            	break;
            }
            total_received += bytes_received;
            //write(file_fd, buffer, bytes_received);
        }
        write(file_fd, buffer, bytes_received);
        //printf("This is what we recieved: \n\r%s\n\r",buffer);
	/*
        bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
	write(file_fd, buffer, bytes_received);
        close(file_fd);
	*/
        // Echo back the received data to the client
        file_fd = open("/var/tmp/aesdsocketdata", O_RDONLY);
        if (file_fd == -1) {
            perror("open");
            close(client_sock);
            continue;
        }

        while ((bytes_received = read(file_fd, buffer, sizeof(buffer))) > 0) {
            ssize_t bytes_sent = send(client_sock, buffer, bytes_received, 0);
            }
	     memset(buffer, 0, BUFFER_SIZE*sizeof(char));

        close(file_fd);

        // Log closed connection to syslog
        syslog(LOG_INFO, "Closed connection from %s", client_ip);
        close(client_sock);
    }

    // The signal handler will handle the cleanup

    return 0;
}

