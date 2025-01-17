#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    socklen_t serverlen;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    // 1) 建立 Datagram socket (UDP)
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // 2) 解析主機名 -> IP
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }

    // 3) 設定要發送的目標位址 (server IP + port)
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    serverlen = sizeof(serv_addr);

    // 4) 輸入要傳給伺服器的資料
    printf("Please enter the message: ");
    bzero(buffer,1024);
    fgets(buffer,1023,stdin);

    // 5) 用 sendto() 發送資料到 server
    n = sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&serv_addr, serverlen);
    if (n < 0)
      error("ERROR in sendto");

    // 6) 用 recvfrom() 接收伺服器回覆
    bzero(buffer, 1024);
    n = recvfrom(sockfd, buffer, 1024, 0,
                 (struct sockaddr *)&serv_addr, &serverlen);
    if (n < 0)
      error("ERROR in recvfrom");

    printf("Server reply: %s\n", buffer);

    close(sockfd);
    return 0;
}
