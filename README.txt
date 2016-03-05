The network emulator will be implemented using the client-server socket programming paradigm. A bridge is implemented as a server and station attached to it as clients.

The Networking concepts, elements, and functionalities will all be implemented in software, including physical links, stations, bridges, and routers. 

TCP socket connections will be used to emulate physical links, and all stations, bridges, and routers are implemented in software.

The server-client paradigm requires the client to know the address of the connecting server. Each bridge will require a command line argument: NAME_OF_LAN

INPUTS:

./bridge lan-name
./station lan-name
./router lan-name

Once a bridge is ready to listen as a TCP socket server, it stores its IP address and the port number by creating two files (or symbolic links) in its directory (assumed that both bridge and station are invoked from the same directory which shared across all the machines in the emulation) namely .lan-name.addr and .lan-name.port. Given a lan-name, a station, by reading these two files, knows the complete socket address of the bridge. Note that this is just one way of sharing information between a server and a client; you can implement this in other ways.

The fowarding table in a station or router is populated from some hard-coded configuration file.
