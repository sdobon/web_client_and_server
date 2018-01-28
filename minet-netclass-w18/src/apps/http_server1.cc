#include "minet_socket.h"
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
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
  int sockfd_listen,sockfd_client;
  struct sockaddr_in sa_listen,sa_client;
  int rc;

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

  printf("%s\n", "check1");

  /* initialize and make socket */
  sockfd_listen = minet_socket(SOCK_STREAM);
  if (sockfd_listen < 0)
	 return -1;
  minet_bind(sockfd_listen, &sa_listen);

  printf("%i\n", sockfd_listen);

  memset(&sa_listen, 0, sizeof(sa_listen));
  sa_listen.sin_family = AF_INET;
  sa_listen.sin_addr.s_addr = htonl(gethostbyname(NULL));
  sa_listen.sin_port = htons(server_port);

  printf("%i\n", sa_listen.sin_addr.s_addr);
  printf("%s\n%s\n", gethostbyname(NULL).h_name, gethostbyname(NULL).h_addr_list[0]);

  if (minet_bind(sockfd_listen, &sa_listen) != 0) {
    minet_close(sockfd_listen);
    return -1;
  }
  /* set server address*/
  printf("%s\n", "successs");
  /* bind listening socket */

  /* start listening */

  /* connection handling loop */
  while(1)
  {
    /* handle connections */
    rc = handle_connection(sockfd_client);
  }
}

int handle_connection(int sock2)
{
  char filename[FILENAMESIZE+1];
  int rc;
  int fd;
  struct stat filestat;
  char buf[BUFSIZE+1];
  char *headers;
  char *endheaders;
  char *bptr;
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

  /* first read loop -- get request and headers*/

  /* parse request to get file name */
  /* Assumption: this is a GET request and filename contains no spaces*/

    /* try opening the file */

  /* send response */
  if (ok)
  {
    /* send headers */

    /* send file */
  }
  else // send error response
  {
  }

  /* close socket and free space */

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
