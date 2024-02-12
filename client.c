#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define PORT 8081
void communicate(int conn);

int main() {
    int sock, conn;
    struct sockaddr_in servaddr, client;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1) {
        printf("socket creation failed\n");
    }
    else {
        printf("Successfully created a socket\n");
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    conn = connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (conn != 0) {
        printf("Failed to connect to server at port %d\n", PORT);
        printf("Error: %s\n", strerror(errno));
        printf("Raw: %d\n", errno);
    }
    else {
        printf("Connected to the server\n");
        communicate(sock);
    }

}


void communicate(int conn) {
    char buf[8000];
    char terminator[] = "exit";

    int n;
    for (;;) {
        n = 0;
        bzero(buf, 8000);
        while ((buf[n++] = getchar()) != '\n')
            ;

        write(conn, buf, sizeof(buf));
        if (strstr(buf, terminator) != NULL) {
            break;
        }
        bzero(buf, 8000);

        read(conn, buf, sizeof(buf));
        if (strstr(buf, terminator) != NULL) {
            break;
        }
        printf("From server: %s", buf);
    }
}
