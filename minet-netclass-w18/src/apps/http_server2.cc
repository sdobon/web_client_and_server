#include "minet_socket.h"
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <cstring>

#define BUFSIZE 1024
#define FILENAMESIZE 100

int handle_connection(int);
int writenbytes(int,char *,int);
int readnbytes(int,char *,int);

int main(int argc,char *argv[])
{
  int server_port;
  int sockfd_listen,sockfd_connect;
  struct sockaddr_in sa_listen,sa_connect;

  int rc,i;
  fd_set readlist;
  fd_set connections;
  int maxfd;

  /* parse command line args */
  if (argc != 3)
  {
    fprintf(stderr, "usage: http_server1 k|u port\n");
    exit(-1);
  }
  server_port = atoi(argv[2]);
  if (server_port < 1500)
  {
    fprintf(stderr,"INVALID PORT NUMBER: %d; can't be < 1500\n",server_port);
    exit(-1);
  }

  /* initialize minet */
  if (toupper(*(argv[1])) == 'K') {
minet_init(MINET_KERNEL);
  } else if (toupper(*(argv[1])) == 'U') {
minet_init(MINET_USER);
  } else {
fprintf(stderr, "First argument must be k or u\n");
exit(-1);
  }

  /* initialize and make socket */
  sockfd_listen = minet_socket(SOCK_STREAM);
  if (sockfd_listen < 0)
   return -1;

  /* set server address*/
  memset(&sa_listen, 0, sizeof(sa_listen));
  sa_listen.sin_family = AF_INET;
  sa_listen.sin_addr.s_addr = htonl(INADDR_ANY);
  sa_listen.sin_port = htons(server_port);

  /* bind listening socket */
  if (minet_bind(sockfd_listen, &sa_listen) != 0){
    minet_close(sockfd_listen);
    return -1;
  };

  /* start listening */
  if (minet_listen(sockfd_listen, 1000) != 0){
    minet_close(sockfd_listen);
    return -1;
  };

  printf("%s\n", "now accepting connections...");
  //FD_SET(sockfd_listen, &connections);
  FD_ZERO(&connections);
  maxfd = sockfd_listen;

  /* connection handling loop */
  while(1)
  {
    /* create read list */
    readlist = connections;
    printf("%i\n", &readlist);
    printf("%i\n", &connections);
    FD_SET(sockfd_listen, &readlist);
    /* do a select */
    minet_select(maxfd + 1, &readlist, NULL, NULL, NULL);
    /* process sockets that are ready */
    for(int i; i < maxfd + 1; i++){
      if(FD_ISSET(i, &readlist)){
        /* for the accept socket, add accepted connection to connections */
        if (i == sockfd_listen)
        {
          sockfd_connect = minet_accept(sockfd_listen, &sa_connect);
          FD_SET(sockfd_connect, &connections);
          if (sockfd_connect > maxfd){
            maxfd = sockfd_connect;
          }
        }
        else /* for a connection socket, handle the connection */
        {
  	       rc = handle_connection(i);
           FD_CLR(i, &connections);
        }
      }
    }
  }
}

int handle_connection(int sockfd_connect)
{
  printf("%s\n", "connection!");
  char filename[FILENAMESIZE+1];
  int rc;
  int fd;
  struct stat filestat;
  char buf[BUFSIZE+1];
  char *headers;
  char *endheaders;
  char *bptr;
  char * fileresp;
  int datalen=0;
  char *ok_response_f = "HTTP/1.0 200 OK\r\n"\
                      "Content-type: text/plain\r\n"\
                      "Content-length: %d \r\n\r\n";
  char ok_response[100];

  char *notok_response = "HTTP/1.0 404 FILE NOT FOUND\r\n"\
                         "Content-type: text/html\r\n\r\n"\
                         "<html><body bgColor=black text=white>\n"\
                         "<h2>404 FILE NOT FOUND</h2>\n"
                         "</body></html>\n";
  bool ok=true;
  char baseurl[120] = "./src/apps";

  printf("%s\n", "finish init");

  memset(&buf, 0, sizeof(buf));

  /* first read loop -- get request and headers*/

  minet_set_nonblocking(sockfd_connect);

  rc = minet_read(sockfd_connect,buf,BUFSIZE);
  buf[rc] = '\0';
  printf("%s\n", buf);
  printf("%i\n", rc);

  /* parse request to get file name */
  /* Assumption: this is a GET request and filename contains no spaces*/
  if (rc > 0) {
    headers = (char *) malloc(BUFSIZE);
    bptr = headers;
    headers = strtok(buf, " ");
    headers = strtok(NULL, " ");

    //   /* try opening the file */
    if ((fd = open(strcat(baseurl, headers), O_RDONLY)) == -1){
      ok = false;
    }
    free(bptr);

  }
  else {
    ok = false;
  }

  /* send response */
  if (ok)
  {
    /* send headers */
    fstat(fd, &filestat);
    sprintf(ok_response, ok_response_f, filestat.st_size);
    writenbytes(sockfd_connect, ok_response, (int) strlen(ok_response));

    /* send file */
    fileresp = (char *) malloc(filestat.st_size + 1);
    read(fd, fileresp, filestat.st_size);
    // printf("%s\n", fileresp);
    writenbytes(sockfd_connect, fileresp, filestat.st_size);
    free(fileresp);
    close(fd);
  }
  else // send error response
  {
    printf("%s\n", "404");
    writenbytes(sockfd_connect, notok_response, (int) strlen(notok_response));
  }

  /* close socket and free space */
  minet_close(sockfd_connect);


  if (ok)
    return 0;
  else
    return -1;
}

int readnbytes(int fd,char *buf,int size)
{
  int rc = 0;
  int totalread = 0;
  while ((rc = minet_read(fd,buf+totalread,size-totalread)) > 0)
    totalread += rc;

  if (rc < 0)
  {
    return -1;
  }
  else
    return totalread;
}

int writenbytes(int fd,char *str,int size)
{
  int rc = 0;
  int totalwritten =0;
  while ((rc = minet_write(fd,str+totalwritten,size-totalwritten)) > 0)
    totalwritten += rc;

  if (rc < 0)
    return -1;
  else
    return totalwritten;
}
