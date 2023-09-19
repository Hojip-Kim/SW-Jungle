#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400



/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr =
    "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 "
    "Firefox/10.0.3\r\n";

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *request_ip, char *port, char *filename);
void serve_static(int clientfd, int fd);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);
void header_make(char *method, char *request_ip, char *user_agent_hdr, char *version, int clientfd, char *filename);


/* client의 요청 라인을 확인하고 정적, 동적 콘텐츠를 확인하고 return */
void doit(int fd) {  // fd는 connfd라는 것이 중요
  int clientfd;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char request_ip[MAXLINE], port[MAXLINE], filename[MAXLINE];
  rio_t rio;
  rio_t rio_com;

  Rio_readinitb(&rio, fd); // fd의 주소값을 rio로
  Rio_readlineb(&rio, buf, MAXLINE); // 한줄을 읽어오기 (GET / gozilla.gif HTTP/1.1)
  printf("Request headers:\n");
  printf("%s", buf);  // 요청된 라인을 printf로 보여준다. (최초 요청 라인 : GET/HTTP/1.1)

  sscanf(buf, "%s http://%s %s", method, uri, version);  // buf의 내용을 method, uri, version이라는 문자열에 저장한다.
  if (strcasecmp(method, "GET") && strcasecmp(method, "HEAD")) {  // GET, HEAD 메소드만 지원한다. (두 문자가 같으면 0)
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");  // 다른 메소드가 들어올 경우, 에러를 출력하고
    return;  // main으로 return시킨다.
  }
  
  parse_uri(uri, request_ip, port, filename); // 파싱작업

  clientfd = open_clientfd(request_ip, port);
  header_make(method, request_ip, user_agent_hdr, version, clientfd, filename); // 헤더 만들고, tiny로 보냄


  serve_static(clientfd, fd);
  Close(clientfd);
}

int parse_uri(char *uri, char *request_ip, char *port, char *filename) {
  char *ptr;
  ptr = strchr(uri, 58); // ASCII CODE : ':' => http:// 뒤에서부터 :로 나눔.

  if(ptr != NULL){ // port가 있을 때
    *ptr = '\0';
    strcpy(request_ip, uri); // 앞의 ip를 가져옴.
    strcpy(port, ptr+1); // 뒤로 다 가져옴 8000/index.html

    ptr = strchr(port, 47); // ASCII CODE : '/'
    if (ptr != NULL){ // '/'가 있다면
    strcpy(filename, ptr); // /에서부터 뒤에전부를 filename에 저장.
    *ptr = '\0'; // ptr을 null로
    strcpy(port, port); // port에 
    }else{
      strcpy(port,port);
    }
  }
  else{ // port가 없을 때
    ptr = strchr(request_ip, 47);
    strcpy(filename, ptr+1);
    ptr = '\0';
    strcpy(request_ip, uri); // ASCII : /
    
    strcpy(port, "80");
  }
}

void header_make(char *method, char *request_ip, char *user_agent_hdr, char *version, int clientfd, char *filename){
  char buf[MAXLINE]; // 임시 버퍼

  sprintf(buf, "%s %s %s\r\n", method, filename, "HTTP/1.0");
  sprintf(buf, "%sHost: %s\r\n", buf, request_ip);
  sprintf(buf, "%s%s", buf, user_agent_hdr);
  sprintf(buf, "%sConnection: %s\r\n", buf, "close");
  sprintf(buf, "%sProxy-Connection: %s\r\n\r\n", buf, "close");

  Rio_writen(clientfd, buf, strlen(buf));
}


void serve_static(int clientfd, int fd){
  int src_size;
  char *srcp, *p, content_length[MAXLINE], buf[MAXBUF];
  rio_t server_rio;

  Rio_readinitb(&server_rio, clientfd);

  Rio_readlineb(&server_rio, buf, MAXLINE);
  Rio_writen(fd, buf, strlen(buf));

  while(strcmp(buf, "\r\n")){
    if(strncmp(buf, "Content-length:", 15) == 0){
      p = index(buf, 32);
      strcpy(content_length, p+1);
      src_size=atoi(content_length);
    }

    Rio_readlineb(&server_rio, buf, MAXLINE);
    Rio_writen(fd, buf, strlen(buf));
  }
  // 헤더 끝

  // body 보내기 시작
  srcp = malloc(src_size);
  Rio_readnb(&server_rio, srcp, src_size); // 읽고
  Rio_writen(fd, srcp, src_size); // 보내고
  free(srcp); // malloc free

}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
  char buf[MAXLINE], body[MAXBUF];

  /* Build the HTTP response body */
  // sprintf는 출력하는 결과 값을 변수에 저정하게 해주는 기능이 있다.
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  /* Print the HTTP response */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

int main(int argc, char **argv){
  int listenfd, connfd; // listenfd : 프록시 듣기 식별자, connfd : 프록시 연결 식별자
  char hostname[MAXLINE], port[MAXLINE]; // 클라이언트에게 받은 uri정보를 담을 공간
  socklen_t clientlen; // 소켓 길이를 저장할 구조체
  struct sockaddr_storage clientaddr; // 소켓 구조체 clientaddress 들어감

  if (argc != 2){ // 입력인자가 2개인지 확인 => ./tiny 8080
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  listenfd = Open_listenfd(argv[1]); // listen 소켓 오픈

  /* 무한서버루프, 반복적으로 연결 요청을 접수 */
  while(1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); // 연결 요청 접수, socket address(SA)
    // => 포트번호는 내가 정하고있고 사용자가 주소를 치면 받아서 비교후 연결

    Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    //받은걸 토대로 호스트네임과 포트 저장

    printf("Accepted connection from (%s %s)\n", hostname, port);

    doit(connfd);

    Close(connfd);
  }
}