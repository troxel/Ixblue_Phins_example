#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <byteswap.h>

#include "udp_util.h"
#include "phins_util.h"

#include "parameters.h"

double haversine(double lat1, double lon1, double lat2, double lon2);

/* -------------------------------------------*/
void error(const char *msg) {
    perror(msg);
    exit(1);
}

/* -------------------------------------------*/
FILE open_file() {

    time_t t;
    struct tm *tmp;
    char filename[64];

    // Get current time
    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL)
        error("ERROR on localtime");

    // Create file name
    strftime(filename, sizeof(filename), "%Y%m%d%H%M%S.txt", tmp);

    // Write to file
    FILE *fp = fopen(filename, "a");
    if (fp == NULL) 
        error("ERROR opening file");

    return(*fp);
}        

struct stbn_hdr_t stbn_hdr;
struct stbn_payload_v1_t stbn_payload;

/* -------------------------------------------*/
int main() {

    int sockfd;
    uint8_t buffer[BUFFER_SIZE];

    // Create socket
    sockfd = open_udp_sock(PHINS_STDBIN_PORT);
    uint32_t cnt = 0;
    ssize_t dg_size; 
    memset(buffer, 0, BUFFER_SIZE);

    while (cnt < 900) {

        dg_size = read_udp_socket(sockfd, buffer, BUFFER_SIZE);
        
        if ( dg_size < 1 ) {
            printf("Error datagram size is %ld\n",dg_size);
            continue; 
        }

        uint32_t chksum = checksum(buffer,dg_size);
        uint32_t chksum_pyld = b2l_32(&buffer[dg_size-4]);

        if ( chksum != chksum_pyld) {
            printf("Error! Checksum validation failed %d %d\n", chksum, chksum_pyld);
            continue; 
        }  

        stbn_hdr.h1    = buffer[0];
        stbn_hdr.h2    = buffer[1];
        stbn_hdr.proto = buffer[2];
        stbn_hdr.navg_mask   = b2l_32(&buffer[3]);
        stbn_hdr.extd_mask   = b2l_32(&buffer[7]);
        stbn_hdr.extn_mask   = b2l_32(&buffer[11]);
        stbn_hdr.stbn_size = b2l_16(buffer[15],buffer[16]);
        stbn_hdr.data_validty_tm = b2l_32(&buffer[17]);
        stbn_hdr.counter         = b2l_32(&buffer[21]);

        //printf("> %c %c %X %lX %lX %lX %u %lu %lu\n", stbn_hdr.h1,stbn_hdr.h2,stbn_hdr.proto,stbn_hdr.navg_mask,stbn_hdr.extn_mask,stbn_hdr.extd_mask,
        //    stbn_hdr.stbn_size,stbn_hdr.data_validty_tm,stbn_hdr.counter);

        // ---- payload ----

        // int br = 0;
        // for (int i=25; i<37; i++) {
        //     printf("%u %X\n",i,buffer[i]);
        //     if (!(++br % 4)) printf("---\n"); 
        // }

        stbn_payload.attitude[0] = b2l_f(&buffer[25]);
        stbn_payload.attitude[1] = b2l_f(&buffer[29]);
        stbn_payload.attitude[2] = b2l_f(&buffer[33]);

        stbn_payload.lat = b2l_lf(&buffer[61]);
        stbn_payload.lon = b2l_lf(&buffer[69]);
        if ( stbn_payload.lon > 180 ) { stbn_payload.lon = stbn_payload.lon - 360; }

        stbn_payload.alt_ref = buffer[77];
        stbn_payload.alt = b2l_f(&buffer[78]);
        
        stbn_payload.ins_algo_stat[0] = b2l_32(&buffer[110]);
        stbn_payload.ins_algo_stat[1] = b2l_32(&buffer[114]);
        stbn_payload.ins_algo_stat[2] = b2l_32(&buffer[118]);
        stbn_payload.ins_algo_stat[3] = b2l_32(&buffer[122]);

        stbn_payload.ins_sys_stat[0] = b2l_32(&buffer[126]);
        stbn_payload.ins_sys_stat[1] = b2l_32(&buffer[130]);
        stbn_payload.ins_sys_stat[2] = b2l_32(&buffer[134]);

        stbn_payload.ins_usr_stat = b2l_32(&buffer[138]);

        printf("%X %X %X %X  ",stbn_payload.ins_algo_stat[0],stbn_payload.ins_algo_stat[1],stbn_payload.ins_algo_stat[2],stbn_payload.ins_algo_stat[3]);
        printf("%X %X %X ",stbn_payload.ins_sys_stat[0],stbn_payload.ins_sys_stat[1],stbn_payload.ins_sys_stat[2]);
        printf("%f %f %f ",stbn_payload.attitude[0],stbn_payload.attitude[1],stbn_payload.attitude[2]);
        printf("%lf %lf ",stbn_payload.lat,stbn_payload.lon);
    
        double  xdiff = haversine( (double) 47.9794444, (double) -116.5602778, stbn_payload.lat, stbn_payload.lon);
       
        printf("%lf %03u\n",xdiff,cnt+1);

        //printf("\nalt_ref = %d, alt = %lf\n",stbn_payload.alt_ref,stbn_payload.alt);

        fflush(stdout);

        ++cnt;
        continue; 
    }

    close(sockfd);
    return 0;
}
