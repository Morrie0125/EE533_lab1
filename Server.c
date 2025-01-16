#include <stdio.h>       // printf, fprintf
#include <sys/types.h>   // 基本型態定義 (如 pid_t, size_t)
#include <sys/socket.h>  // socket, bind, listen, accept
#include <netinet/in.h>  // struct sockaddr_in, htons, etc.


#include <strings.h>   // bzero()
#include <stdlib.h>    // atoi(), exit()

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, n;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    // 檢查是否有帶 port 當參數
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // 1) 建立 socket (AF_INET + SOCK_STREAM)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // 2) 將 serv_addr 結構清為 0
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // 由指令列參數取得埠號
    portno = atoi(argv[1]);

    // 3) 設定 serv_addr 欄位
    serv_addr.sin_family = AF_INET;          // IPv4
    serv_addr.sin_addr.s_addr = INADDR_ANY;  // 綁定到本機所有可用網卡
    serv_addr.sin_port = htons(portno);      // 轉為網路位元序

    // 4) bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // 5) listen，允許 5 條連線在佇列中
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // 6) accept (阻塞等待 Client)
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    // 7) 收 Client 訊息
    bzero(buffer, 256);
    n = read(newsockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);

    // 8) 回覆給 Client
    n = write(newsockfd, "I got your message", 18);
    if (n < 0) error("ERROR writing to socket");

    // 9) 結束程式前可關閉 socket
    //    （此示範程式只處理一次 Client 通訊就結束）
    return 0;
}
