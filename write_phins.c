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
int main() {

    int sockfd;
    uint8_t buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
       perror("socket()");
       exit(1);
    }
    printf("sockfd %d\n",sockfd);

    struct sockaddr_in dest_addr,locl_addr;
 
    //socklen_t dest_addr_len = sizeof(struct sockaddr_in);
    //socklen_t locl_addr_len = sizeof(struct sockaddr_in);

    memset(&dest_addr, 0, sizeof(dest_addr));
    memset(&locl_addr, 0, sizeof(locl_addr));
  
    dest_addr.sin_family      = AF_INET;
    dest_addr.sin_port        = htons(PHINS_GPS_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(PHINS_IP);

 	// locl_addr.sin_family = AF_INET;
	// locl_addr.sin_port = htons(2000);
	// locl_addr.sin_addr.s_addr = inet_addr(LOCAL_IP);
    // //locl_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // if (bind(sockfd, (struct sockaddr *)&locl_addr, sizeof(locl_addr)) < 0) {
    //   perror("ERROR on binding\n");
    //   return -1;
    // }

    struct gps_data_t gps_data;
    char nmea_str[256];
    memset(nmea_str, 0, 256);

    uint32_t cnt = 0;

    double lat_in = INIT_LAT;
    double lon_in = INIT_LON;

    void cnvt_degree_to_gga(double degree_in, char* gga_latlon);

    while (cnt < 500) {

        // Absurd check of incoming
        if ( (lat_in > 49) || (lat_in < 47) ) continue;
        if ( (lon_in < -117 ) || (lon_in > -116) ) continue;

        // gps quality decoder
        // 0 Data invalid N/A
        // 1 10m    Natural
        // 2 3m     Differential
        // 3 10m    Military
        // 4 0.1m   RTK
        // 5 0.3m  Float RTK

        gps_data.latitude = lat_in;
        gps_data.longitude = fabs(lon_in);
        gps_data.altitude = 15.0;
        gps_data.quality = 5;
        gps_data.hdop = 1.1;
        gps_data.satellites = 8;
        
        // $GPGGA,001052.00,4758.76758057,N,11633.61604700,E,6,03,38.603,-2.112,M,,M,,*60
        // $GPGGA,005704.01,4802.37924115,N,11632.77614893,E,6,03,85592.812,-0.214,M,,M,,*55
        // $GPGGA,054913,4802.07087454,N,11633.61666800,W,1,08,,15.00,M,,M,,*6A

        buildGGA(&gps_data, nmea_str);

        // char lat_str[48];
        // cnvt_degree_to_gga(gps_data.latitude, lat_str);
        // char lon_str[48]; 
        // cnvt_degree_to_gga(gps_data.longitude, lon_str);

        // printf("%s, %s\n",lat_str,lon_str);

        printf("%u %s",cnt, nmea_str);
        fflush(stdout);
        cnt++;

        //ssize_t rtn = write_udp_socket(sockfd, nmea_str, 256, &dest_addr, addrlen);
        ssize_t rtn = sendto(sockfd,  (const char *)nmea_str, strlen(nmea_str) , 0,(struct sockaddr *) &dest_addr, sizeof(dest_addr));

        //printf(">%ld %d %s %lu\n",rtn,errno,strerror(errno),strlen(nmea_str));

        fflush(stdout);

        sleep(1);
    }

    //close(sockfd);
    return 0;
}

