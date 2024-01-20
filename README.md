# IXBLUE PHINS 7 connection examples. 

IXBLUE PHINS Inertial Navigation System is a navigation quality instrument. This repo has several C functions that communication with the PHINS that were developed during course of integrating the PHINS Compact C7 into a project.  These C functions were developed in accordance to the IXBLUE document 

**Phins Compact C7 Interface Library MU-PHINC7IL-AN-001-C**

## Reading STDBIN v3 protocol

The STDBIN protocol is an input/output binary protocol and thus more efficient to process in C than NMEA strings. When selecting this protocol as output (UDP Port 2002 in this example) you first select which navigation blocks of data you want from the IXBLUE web interface. The offsets into the binary byte stream then have to be counted for the various bits of data. 

There is Perl helper function in the ./util directory that will give you the offsets to the requested frames of data. You will need to edit this Perl script and select the bit number of the navigational data frames you selected via the webgui. 

Note: IXBLUE send this data as big-endian and byte swaps are necessary and handled by the STDBIN example function. 

Also the STDBIN output to the console gives the diff between the initial position and current INS reported positions via the Haversine formula. This was used to test PHINS unaided drift performance, which is outstanding btw.  

When ***make*** successfully compiles the STDBIN program will be created:

./bin/read_stdbin_phins.exe  
 
## Writing GPS protocol

For sending correction updates to the PHINS I select GPS data and focused on the GPGGA standard string. In this example the UDP port is set to 8121.

When ***make*** successfully compiles the write gps protocol program will be created 

./bin/write_phins.exe 

## Write Commands

The PHINS can send configuration and monitoring commands during operation on a special TCP port 8110 or serial port. In this example the TCP port is used. Although note once a connection has been made during a PHINS session a re-connect cannot be accomplished and power reset (or via the web interface) is required. 

There are only 3 commands configured manual gps, zupt and reset as discussed in Chapter 8 of the Interface Document. 

When ***make*** successfully compiles the write commands protocol program will be created 

./bin/write_phins_cmd.exe 

This program requires one of three options 

>./bin/write_phins_cmd.exe gps

The gps coordinates are hard-coded in the parameters include file.

>./bin/write_phins_cmd.exe zupt $mode

Where $mode is 1 through 6 as identified in Chapter 8.

>./bin/write_phins_cmd.exe reset

## Building

Find the parameters.h file in the root directory and fill in with your relevant data. Run make. Configure the PHINS appropriately and have fun. 

Send any bug reports to 

Steve Troxel
troxel@perlworks.com