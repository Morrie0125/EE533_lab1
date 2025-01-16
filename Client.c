#include <stdio.h>              // printf, fprintf, perror
#include <stdlib.h>             // exit, atoi
#include <string.h>             // bzero, strlen, etc.
#include <unistd.h>             // read, write, close
#include <sys/types.h>          // 基本型態 (如 size_t, pid_t)
#include <sys/socket.h>         // socket, connect
#include <netinet/in.h>         // struct sockaddr_in, htons
#include <netdb.h>              // gethostbyname, struct hostent

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    // 檢查參數：需帶 hostname + port
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    // 1. 取得 Port
    portno = atoi(argv[2]);

    // 2. 建立 socket (AF_INET + SOCK_STREAM)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // 3. 解析主機名稱 -> IP
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(1);
    }

    // 4. 設置 serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    // 5. connect 到 Server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // 6. 請使用者輸入訊息
    printf("Please enter the message: ");
    bzero(buffer, 256);
    fgets(buffer, 255, stdin);

    // 7. 傳送給 Server
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

    // 8. 從 Server 讀取回覆
    bzero(buffer, 256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
         error("ERROR reading from socket");
    printf("%s\n", buffer);

    // 9. 關閉 socket
    close(sockfd);
    return 0;
}
