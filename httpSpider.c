#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <arap/inet.h>
#include <sys/socket.h>
#endif 
#include "httpSpider.h"

#define logfile "spiderLog.txt"
const char *httpHeader = "GET %s HTTP/1.0 \r\n" \
             "User-Agent: Mozilla/5.0(compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0 \r\n\r\n";

#ifdef _WIN32
void initSocket()
{
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 0), &ws);
}
void cleanSocket()
{
    WSACleanup();
}
#endif

FILE *flog;
void initSpider()
{
    flog = fopen(logfile, "w");
}
void termSpider()
{
    fflush(flog);
    fclose(flog);
}

#ifdef __linux__
typedef int SOCKET;
#endif
#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif
#ifdef _WIN32
#define close closesocket
#endif

int request(char *host, char *path, int port, char *buffer, int maxb)
{
    SOCKET sokfd;
    struct sockaddr_in sokad;
    int sdlen, rlen;
	
    memset(&sokad, 0, sizeof(sokad));
    sokad.sin_family = AF_INET;     //ipv4
    sokad.sin_port = htons(port);
    if((sokad.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
    {
        puts("ip无效，具体信息已输入日志");
        fprintf(flog, "-- ip无效，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
        return -1;
    }
	
    if((sokfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        puts("创建socket描述符失败，具体信息已输入日志");
        fprintf(flog, "-- 创建socket描述符失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
        return -1;
    }
    
    if(connect(sokfd, (struct sockaddr *)&sokad, sizeof(sokad)) < 0)
    {
        puts("连接失败，具体信息已输入日志");
        fprintf(flog, "-- 连接失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
    }
    char *rqbuf = (char *)malloc(sizeof(char) * 512);
    sdlen = sprintf(rqbuf, httpHeader, path);
	
    send(sokfd, rqbuf, sdlen, 0);
    free(rqbuf);
	
    rlen = recv(sokfd, buffer, maxb, 0);
    if(rlen < 0)
    {
        puts("接收数据失败，具体信息已输入日志");
        fprintf(flog, "-- 接收数据失败，具体信息:\n  主机:%s 资源:%s 端口:%d\n\n", host, path, port);
        return -1;
    }
    close(sokfd);
    return rlen;
}

void attachPlug()
{
    
}
void detachPlug()
{
    
}

int main()
{
    #ifdef _WIN32
    initSocket();
    #endif
    initSpider(); 
    
    attachPlug();
    
    detachPlug();
	
    termSpider();
    #ifdef _WIN32
    cleanSocket();
    #endif
    return 0;
}