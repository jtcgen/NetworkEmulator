#####################################################
# The code developed by Travis Chung
#
# CNT 5505 Network Emulator
# Spring 2016 Computer Science, FSU
#####################################################
1. Compiled on Linux machine. 

2. Commands you can run in stations/hosts

   debug <on|off> // turn on or off debugging
   send <destination> <message> // send message to a destination host
   sh host // show the IP/name mapping table
   sh arp // show the ARP cache table information

3. To start the emulation, run

   run_emulation

   which emulates the following network topology

   
          B              C                D
          |              |                |
         cs1-----R1------cs2------R2-----cs3
          |              |                |
          -------A--------                E

    cs1, cs2, and cs3 are bridges.
    R1 and R2 are routers.
    A to E are hosts/stations.
    Note that A is multi-homed, but it is not a router.
