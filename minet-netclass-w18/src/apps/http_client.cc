#include "minet_socket.h"
#include <stdlib.h>
#include <ctype.h>
#include <cstring>

#define BUFSIZE 1024

int write_n_bytes(int fd, char * buf, int count);

int main(int argc, char * argv[]) {
    char * server_name = NULL;
    int server_port = 0;
    char * server_path = NULL;

    int sockfd = 0;
    int rc = -1;
    int datalen = 0;
    bool ok = true;
    struct sockaddr_in sin;
    FILE * wheretoprint = stdout;
    struct hostent * site = NULL;
    char * req = NULL;

    char buf[BUFSIZE + 1];
    char * bptr = NULL;
    char * bptr2 = NULL;
    char * endheaders = NULL;
    int written;

    struct timeval timeout;
    fd_set set;

    /*parse args */
    if (argc != 5) {
	fprintf(stderr, "usage: http_client k|u server port path\n");
	exit(-1);
    }

    server_name = argv[2];
    server_port = atoi(argv[3]);
    server_path = argv[4];



    /* initialize minet */
    if (toupper(*(argv[1])) == 'K') {
	minet_init(MINET_KERNEL);
    } else if (toupper(*(argv[1])) == 'U') {
	minet_init(MINET_USER);
    } else {
	fprintf(stderr, "First argument must be k or u\n");
	exit(-1);
    }

    /* create socket */
    sockfd = minet_socket(SOCK_STREAM);
    if (sockfd == -1)
      return sockfd;
    minet_bind(sockfd, &sin);
    // Do DNS lookup
    /* Hint: use gethostbyname() */
    site = gethostbyname(server_name);
    if (site == NULL) {
      minet_close(sockfd);
      return -1;
    }
    /* set address */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = *(unsigned long*) site->h_addr_list[0];

    /* connect socket */
    if (minet_connect(sockfd, &sin) != 0) {
      close(sockfd);
      return -1;
    }

    strcat(req, "GET ");
    strcat(req, "/");
    strcat(req, server_path);
    strcat(req, " HTTP/1.1\r\nHost: ");
    strcat(req, server_name);
    strcat(req, "\r\nConnection: close\r\n\r\n");

    printf("%s\n", req);

    /* send request */
    written = write_n_bytes(sockfd, req, 1024)
    printf("%i\n", written);
    /* wait till socket can be read */
    FD_SET(sockfd, &set);
    /* Hint: use select(), and ignore timeout for now. */

    status = minet_select(sockfd+1, &set, NULL, NULL, NULL);

    /* first read loop -- read headers */
    if (FD_ISSET(sockfd, &set)) {
      printf("%s\n", "success");
      minet_read(sockfd, buf, 1024);
      printf("%s\n", buf);
    };

    /* examine return code */
    //Skip "HTTP/1.0"
    //remove the '\0'
    // Normal reply has return code 200

    /* print first part of response */

    /* second read loop -- print out the rest of the response */

    /*close socket and deinitialize */


    if (ok) {
	return 0;
    } else {
	return -1;
    }
}

int write_n_bytes(int fd, char * buf, int count) {
    int rc = 0;
    int totalwritten = 0;

    while ((rc = minet_write(fd, buf + totalwritten, count - totalwritten)) > 0) {
	totalwritten += rc;
    }

    if (rc < 0) {
	return -1;
    } else {
	return totalwritten;
    }
}
