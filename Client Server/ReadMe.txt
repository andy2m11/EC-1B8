To compile the code, just type make in the current directory.


Typing in ./server in the command line will run the server.
To run a client, type ./client followed by the server address (which should be the ip address of the device).

The server is set up to handle no more than 5 clients at a time. If a 6th client attempts to connect, the server will inform that client to try again later before disconnecting it.

Upon connection, the server sends the client information regarding the uptime of the server as well as the number of clients it has served.

Messages sent from the clients are echoed back to them by the server as well as the current sum total. The sum total is incremented whenever a client sends a number to the server. 






