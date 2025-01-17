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

// 這個函式用來處理跟單一個 client 的互動邏輯
void dostuff(int sockfd)
{
    char buffer[256];
    int n;
    bzero(buffer, 256);

    // 讀取 client 傳來的訊息
    n = read(sockfd, buffer, 255);
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n", buffer);

    // 回覆 client
    n = write(sockfd, "I got your message", 18);
    if (n < 0) error("ERROR writing to socket");
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pid_t pid;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // 1) 建立 socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // 2) 綁定(Bind)到指定埠口
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    // 3) 進入聆聽狀態
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // 4) 不斷接受連線 (多迴圈)
    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");

        // 5) fork 子行程去處理該連線
        pid = fork();
        if (pid < 0) {
            error("ERROR on fork");
        }
        if (pid == 0)  {
            // 子行程
            close(sockfd);         // 子行程不需要原本的 listen socket
            dostuff(newsockfd);    // 處理該連線的讀寫邏輯
            close(newsockfd);      // 結束前關閉
            exit(0);               // 子行程結束
        } 
        else {
            // 父行程
            close(newsockfd);      // 父行程不需要這個連線的 socket
            // 然後回到 while(1) 繼續 accept 下一個連線
        }
    }

    // 理論上不會執行到這裡
    close(sockfd);
    return 0;
}
