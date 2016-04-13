#####################################################
# The code developed by Travis Chung
#
# CNT 5505 Network Emulator
# Spring 2016 Computer Science, FSU
#####################################################

1. How to compile the code

    platform: MAC OS X
    just type "make"


3. Commands supported in stations/routers/bridges

   3.1 stations:

	   send <destination> <message> // send message to a destination host
	   show arp 		   // show the ARP cache table information
	   show pq 		      // show the pending_queue
	   show	host 		   // show the IP/name mapping table
	   show	iface 		// show the interface information
	   show	rtable 		// show the contents of routing table
	   debug             // verbose output
	   quit              // close the station

   3.2 routers:

	   show	arp 		   // show the ARP cache table information
	   show	pq 		   // show the pending_queue
	   show	host 		   // show the IP/name mapping table
	   show	iface 		// show the interface information
	   show	rtable 		// show the contents of routing table
	   debug             // verbose output
	   quit              // close the router


   3.3 bridges:

	   show sl 		      // show the contents of self-learning table
	   debu              // verbose output
	   quit              // close the bridge


4. To start the emulation, run

   	run_simulation

   , which emulates the following network topology

   
          B              C                D
          |              |                |
         cs1-----R1------cs2------R2-----cs3
          |              |                |
          -------A--------                E

    cs1, cs2, and cs3 are bridges.
    R1 and R2 are routers.
    A to E are hosts/stations.
    Note that A is multi-homed, but it is not a router.
