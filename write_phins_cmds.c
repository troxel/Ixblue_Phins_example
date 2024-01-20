#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <time.h>
#include <unistd.h>
#include <byteswap.h>
#include <math.h>
#include <error.h>
#include <errno.h>

#include "udp_util.h"
#include "tcp_util.h"
#include "phins_util.h"

#include "parameters.h"

struct stbn_hdr_t stbn_hdr;
struct stbn_payload_v1_t stbn_payload;

/* -------------------------------------------*/
int main(int argc, char *argv[]) {

    int sockfd;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
       perror("socket()");
       exit(1);
    }

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
  
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(PHINS_CMD_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(PHINS_IP);

    // Connect to the server
    uint16_t cnt = 1;
    char err_msg[256];
    printf("Attempt connection\n");
    while ( connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0 ) {
        sprintf(err_msg,"%d Connection Failed... Retrying",cnt);
        perror(err_msg);
        if ( cnt++ > 100 ) { exit(0); }
    } 

    char cmd_str[256]; 
    memset(cmd_str, 0, 256);

    if( argc < 2 ) {
      printf("At least one argument needs to be supplied.\n1. gps\n 2. zupt mode\n, 3. reset\n");
    }


    if ( strcmp("gps", argv[1]) == 0 )  {

        // Cmd manual gps. Note once this is sent was not able to send commands to GNNS1. 
        // Might be able on GNNS2... don't know yet. 
        struct cmd_mnl_gps_data_t gps_data; 
        gps_data.lat = INIT_LAT;
        gps_data.lon = INIT_LON;
        gps_data.alt = 15;
        gps_data.lat_stddev = 1;
        gps_data.lon_stddev = 1;
        gps_data.alt_stddev = 1;

        cmd_mnl_gps(&gps_data, cmd_str);
    }
    else if ( strcmp("zupt", argv[1]) == 0 ) {
        uint8_t mode = atoi(argv[2]);
        if ( ( mode > 6 ) || ( mode < 0 ) ) mode = 0; 
    
        cmd_zupt(mode, cmd_str);
    }
    else if ( strcmp("reset", argv[1]) == 0 ) { 
        cmd_reset(cmd_str);
    }
    else {
        printf("No valid command requested");
        exit(0);
    }    

    printf("> %s", cmd_str);
    fflush(stdout);

    ssize_t rtn = send(sockfd, cmd_str, strlen(cmd_str), 0);
    if ( rtn < 0 ) {  
        perror("Send failed");
    } 
    printf(">%ld %d %s\n",rtn,errno,strerror(errno));

    close(sockfd);
    return 0;
}

