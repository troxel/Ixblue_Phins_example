/*--------------------------------------------------

Utility Function include to facilitate UDP communications.

Version History
----------------
Orignal: Troxel Jan 2024  
---------------------------------------------------*/

#ifndef phins_util_h
#define phins_util_h

// -------------- Types ------------
// STDBIN v3 structs
struct stbn_hdr_t {
    uint8_t h1;
    uint8_t h2;
    uint8_t proto;
    uint32_t navg_mask;
    uint32_t extd_mask;
    uint32_t extn_mask;
    uint16_t stbn_size;
    uint32_t data_validty_tm;
    uint32_t counter;
};

struct stbn_payload_v1_t {
    float attitude[3];
    double lat;
    double lon;
    uint8_t alt_ref;
    float alt;
    uint32_t ins_algo_stat[4];
    uint32_t ins_sys_stat[3];
    uint32_t ins_usr_stat;
    uint32_t AHRS_algo_stat;
    uint32_t AHRS_sys_stat[3];
    uint32_t AHRS_usr_stat;
 };

// GPS GGA data structs
struct gps_data_t {
    double latitude;
    double longitude;
    double altitude;        // Altitude in meters
    int quality;            // GPS Quality indicator
    float hdop;             // Horiz dilution of precision
    int orth_height;        // Orthometric Height (depth?)
    int satellites;         // Number of satellites in use
};

struct cmd_mnl_gps_data_t {
   double lat;
   double lon;
   double alt;
   float lat_stddev;
   float lon_stddev;
   float alt_stddev;
};

// -------------- Protos ------------
uint32_t b2l_32(uint8_t * bp);
uint16_t b2l_16(uint8_t b1,uint8_t b2);
uint16_t l2b_16(uint8_t b1,uint8_t b2);
float b2l_f(uint8_t * bp);
float b2l_lf(uint8_t * bp);

uint32_t checksum(uint8_t * bp, ssize_t length);
unsigned char checksum_gps(const char *nmea_str);

void buildGGA(const struct gps_data_t *gps_data, char *nmea_str);
void cnvt_degree_to_gga(double degree_in, char* gga_latlon);

void cmd_mnl_gps(const struct cmd_mnl_gps_data_t *gps_data, char *msg_str_out);
void cmd_zupt(uint8_t mode, char *cmd_str);
void cmd_reset(char *cmd_str);


#endif
