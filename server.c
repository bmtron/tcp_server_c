#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define PORT 8081

int socket_create();
void communicate(int conn);

int main() {
    int sock;
    struct sockaddr_in my_addr, client;

    sock = socket_create();
    if (sock == -1) {
        return 0;
    }
    memset(&my_addr, '\0', sizeof(struct sockaddr_in));
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);
    
    int binding = bind(sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    if (binding == -1) {
        printf("Error binding to socket on port %d\n", PORT);
        printf("Error reason: %s\n", strerror(errno));
        return 0;
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
    for (;;) {
        int len = sizeof(client);
        int newconn = accept(sock, (struct sockaddr *)&client, &len);

        if (newconn < 0) {
            printf("Server failed to accept\n");
            break;
        }
        else {
            printf("connect successful\n");
        }

        communicate(newconn);
    }
    close(sock);
    return 0;
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
    char recv_buf[80];
    char terminator[] = "exit";
    int n;
    int count = 0;
    for (;;) {
        bzero(recv_buf, 80);
        read(conn, recv_buf, sizeof(recv_buf));
        if (count == 0) {
            count++;
            printf("%s", recv_buf);
        }
        if (strstr(recv_buf, terminator) != NULL) {
            break;
        }

        printf("From client: %s", recv_buf);
        bzero(recv_buf, 80);

        char send_buf[] = "Message received: SUCCESS\n";

        write(conn, send_buf, sizeof(send_buf));
        bzero(recv_buf, 80);
    }
}
