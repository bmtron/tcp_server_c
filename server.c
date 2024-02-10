#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8081

int socket_create();
void communicate(int sock);

int main() {
    printf("Hello, world.\nThis will be a TCP server maybe.\n");
    int sock;
    struct sockaddr_in my_addr, client;

    sock = socket_create();

    memset(&my_addr, '\0', sizeof(struct sockaddr_in));
    
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(PORT);
    printf("The chosen port is %d\n", my_addr.sin_port); 
    //printf("%d", htons(PORT));
    int binding = bind(sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr_in));

    if (binding == -1) {
        printf("Error binding to socket on port %d\n", PORT);
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


        int len = sizeof(client);
        int newconn = accept(sock, (struct sockaddr *)&client, &len);
        if (newconn < 0) {
            printf("Server failed to accept\n");
        }
        else {
            printf("connect successful\n");
        }


    communicate(newconn);
    close(sock);
    return 0;
}


int socket_create() {
    int socket_result;
    socket_result = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_result == -1) {
        printf("socket creation failed.\n");
    } 
    else {
        printf("socket result: %d\n", socket_result);
    }

    return socket_result;
}

void communicate(int conn) {
    char recv_buf[80];
    char terminator[] = "exit";
    int n;
    for (;;) {
        
        read(conn, recv_buf, sizeof(recv_buf));

        if (strstr(recv_buf, terminator) != NULL) {
            break;
        }

        printf("From client: %s", recv_buf);
        bzero(recv_buf, 80);
        n = 0;

        while ((recv_buf[n++] = getchar()) != '\n')
            ;
        write(conn, recv_buf, sizeof(recv_buf));
        bzero(recv_buf, 80);
    }
}
