#include <stdio.h>
#include <unistd.h>
#include <string>
#include <poll.h>

#include "socket.hh"
#include "controller.hh"

using namespace std;
using namespace Network;

int main( int argc, char *argv[] )
{
  /* check arguments */
  bool debug = false;
  if ( argc == 4 && string( argv[ 3 ] ) == "debug" ) {
    debug = true;
  } else if ( argc == 3 ) {
    /* do nothing */
  } else {
    fprintf( stderr, "Usage: %s IP PORT [debug]\n", argv[ 0 ] );
    exit( 1 );
  }

  try {
    /* Fill in destination address from command line arguments */
    Address destination( argv[ 1 ] /* ip */, argv[ 2 ] /* port */ );

    fprintf( stderr, "Sending packets to %s:%d.\n",
             destination.ip().c_str(), destination.port() );

    /* Create UDP socket for outgoing datagrams. */
    Network::Socket sock;

    /* Initialize packet counters */
    uint64_t sequence_number = 0;

    /* Initialize flow controller */
    Controller controller( debug );

/* Sending at constant bit rate (not considering window size) */
    while( 1 ){
  	struct pollfd fd = { sock.fd(), POLLIN, 0 };
  	int when_to_send = poll( &fd, 1, controller.timeout_ms() );
  	if (when_to_send == 0) { /* time T has elapsed, so send a packet to maintain constant bit rate */
    	Packet x( destination, sequence_number++ );
    	sock.send( x );
    	controller.packet_was_sent( x.sequence_number(),
                                    x.send_timestamp() );
        }
    }
}
  catch ( const string & exception ) {
    /* We got an exception, so quit. */
    fprintf( stderr, "Exiting on exception: %s\n", exception.c_str() );
    exit( 1 );
  }

  return 0;
}
        
