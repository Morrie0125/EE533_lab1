#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    socklen_t fromlen;
    struct sockaddr_in serv_addr, from; 
    char buffer[1024];

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // 1) 建立 Datagram socket (UDP)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // 2) bind 到指定埠口
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // 3) 進入無限迴圈：等待收資料 (recvfrom) → 回覆 (sendto)
    fromlen = sizeof(struct sockaddr_in);

    while (1) {
        bzero(buffer, 1024);

        // 收資料 (阻塞等待)
        n = recvfrom(sockfd, buffer, 1024, 0,
                     (struct sockaddr *)&from, &fromlen);
        if (n < 0)
            error("ERROR in recvfrom");

        printf("Received a datagram: %s\n", buffer);

        // 回覆給同一個來源
        n = sendto(sockfd, "Got your message\n", 17, 0,
                   (struct sockaddr *)&from, fromlen);
        if (n < 0)
            error("ERROR in sendto");
    }

    close(sockfd);
    return 0;
}
