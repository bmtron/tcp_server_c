#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>

#define PORT 8081

int socket_create();
void communicate(int conn);
void* clean_exit_thread(void* sock);
void socket_run(int sock, struct sockaddr_in my_addr, struct sockaddr_in client);
void* client_thread(void* client_args);
void interpret_and_reply(int conn);

struct client_thread_args {
    int sock;
    struct sockaddr_in client;
};

int main() {
    int sock;
    struct sockaddr_in my_addr, client;

    sock = socket_create();
    if (sock == -1) {
        return 0;
    }
    socket_run(sock, my_addr, client);
    printf("exiting and closing...");
    return 0;
}

void socket_run(int sock, struct sockaddr_in my_addr, struct sockaddr_in client) {
    memset(&my_addr, '\0', sizeof(struct sockaddr_in));
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);
    
    int binding = bind(sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    if (binding == -1) {
        printf("Error binding to socket on port %d\n", PORT);
        printf("Error reason: %s\n", strerror(errno));
        return;
    }
    else {
        printf("bound socket at port %d successfully\n", PORT);
    }    

    int listen_ret = listen(sock, 3);

    if (listen_ret == -1) {
        printf("Error listening to socket on port %d\n", PORT);
    }
    else {
        printf("Successfully listening on port %d\n", PORT);
    }
    int exit_bit;
    pthread_t exit_thread_id;
    exit_bit = 0;
    pthread_create(&exit_thread_id, NULL, clean_exit_thread, (void*)&sock);

    struct client_thread_args* client_args;
    client_args = malloc(sizeof(struct client_thread_args));

    (*client_args).sock = sock;
    (*client_args).client = client;

    for (;;) {
        socklen_t len = (socklen_t)sizeof((*client_args).client);
        int newconn = accept((*client_args).sock, (struct sockaddr *)&(*client_args).client, &len);
        pthread_t client_thread_id;
        pthread_create(&client_thread_id, NULL, client_thread, (void*)&newconn);
    }

    free(client_args);
}

void *client_thread(void* conn_arg) {

    int newconn = *(int*)conn_arg; 
    if (newconn < 0) {
        printf("Server failed to accept\n");
        return conn_arg;
    }
    else {
        printf("connect successful\n");
        communicate(newconn);
    }
    return conn_arg;
}

int socket_create() {
    int socket_result;
    socket_result = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_result == -1) {
        printf("socket creation failed.\n");
        return -1;
    } 

    return socket_result;
}

void communicate(int conn) {
    char recv_buf[1024];
    int n;
    bzero(recv_buf, 1024);
    int conn_open = read(conn, recv_buf, sizeof(recv_buf));

    if (conn_open <= 0) {
        //this means the connection was abruptly closed
        //on the client side. return out of this.
        //if we don't check and return,
        //we get a SIGPIPE from the OS
        //and the program shuts down completely.
        return;
    } 
    else {
        printf("From client: %s", recv_buf);
        interpret_and_reply(conn);
    }
}

void *clean_exit_thread(void* sock) {
    char terminator[] = "exit";
    int n;
    char buf[80];
    for (;;) { 
        while ((buf[n++] = getchar()) != '\n')
                ;

        if (strstr(buf, terminator) != NULL) {
            printf("broke out of exit thread\n");
            close(*(int*)sock);
            exit(0);
        }
    }
    return sock;
}

void interpret_and_reply(int conn) {
    char send_buf[] = "Message received: SUCCESS\n";
    char http_resp[] = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "\r\n"
                        "<html>Hello, world. This is a crappy HTTP server.</html>\r\n";
    write(conn, http_resp, strlen(http_resp));
    close(conn);
}
