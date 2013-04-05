#include <stdio.h>
#include <unordered_map>
#include "controller.hh"
#include "timestamp.hh"

using namespace Network;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), the_window_size(1)
{
  debug_ = true;
}
std::unordered_map<uint64_t, uint64_t> inflight;
std::unordered_map<uint64_t, uint64_t>::const_iterator itr;
/* Get current window size, in packets */
unsigned int Controller::window_size( void )
{
  /* Default: fixed window size of one outstanding packet */
 // int the_window_size = 1;

  if ( debug_ ) {
    fprintf( stderr, "At time %lu, return window_size = %d.\n",
             timestamp(), the_window_size );
  }

  return the_window_size;
}
int t = 0;
//int q = 0;
int w = 0;
float cwnd = 1.0;
uint64_t lowest = 10000000000000000;
/* A packet was sent */
void Controller::packet_was_sent( const uint64_t sequence_number,
                                  /* of the sent packet */
                                  const uint64_t send_timestamp )
                                  /* in milliseconds */
{
  /* Default: take no action */
  //store sequence number and timestamp
 /**
AIMD
 inflight[sequence_number] = send_timestamp; 
 for(itr = inflight.begin(); itr != inflight.end(); ++itr){
   if(itr->first < lowest){
     lowest = itr->first;
   }
 }
 if(timestamp() - inflight[lowest] >= timeout_ms() && q == 1){
   //the_window_size = the_window_size * 0.5;
     cwnd = cwnd * 0.8;
     the_window_size = int(cwnd);
     inflight.erase(lowest);
     q = 0;
 }
**/
/**
inflight[sequence_number] = send_timestamp; 
for(itr = inflight.begin(); itr != inflight.end(); ++itr){
  if(itr->first < lowest){
    lowest = itr->first;
   }
 }
 if(timestamp() - inflight[lowest] >= timeout_ms() && t == 1){
   //the_window_size = the_window_size * 0.5;
     cwnd = cwnd * 0.6;
     the_window_size = int(cwnd);
     inflight.erase(lowest);
     t = 0;
 }
**/
  if ( debug_ ) {
    fprintf( stderr, "At time %lu, sent packet %lu.\n",
             send_timestamp, sequence_number );
  }
}
/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
                               /* what sequence number was acknowledged */
                               const uint64_t send_timestamp_acked,
                               /* when the acknowledged packet was sent */
                               const uint64_t recv_timestamp_acked,
                               /* when the acknowledged packet was received */
                               const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /* Default: take no action */
/**
//AIMD- remove entry from list for this sequence number
cwnd = cwnd + 10/cwnd;
//the_window_size = the_window_size + 10;
the_window_size = int(cwnd);
inflight.erase(sequence_number_acked);
q = 1;
//t = 1;
**/
/**
  //delay-triggered
  if(timestamp_ack_received - send_timestamp_acked > 55 && t == 1){
      cwnd = cwnd * 0.5;
      //the_window_size = the_window_size * 0.5;
      the_window_size = int(cwnd);
      t = 0;
  }
  else if(timestamp_ack_received - send_timestamp_acked < 55){
      cwnd = cwnd + 10/cwnd;
      //the_window_size = the_window_size + 1;
      the_window_size = int(cwnd);
      t = 1;
 }
**/
//Contest (delay-triggered variation)
//best so far- 0.6, 75, w==4, 10/cwnd, to = 55
if(timestamp_ack_received - send_timestamp_acked > 75 && t == 1){
      cwnd = cwnd * 0.6;
      the_window_size = int(cwnd);
      t = 0;
  }
else if(timestamp_ack_received - send_timestamp_acked < 75){
  if(w == 4){
  cwnd = cwnd + 10/cwnd;
  the_window_size = int(cwnd);
  t = 1;
  w = 0;
}
  else{
    w++;
}
}
  if ( debug_ ) {
    fprintf( stderr, "At time %lu, received ACK for packet %lu",
             timestamp_ack_received, sequence_number_acked );

    fprintf( stderr, " (sent %lu, received %lu by receiver's clock).\n",
             send_timestamp_acked, recv_timestamp_acked );
  }
}

/* How long to wait if there are no acks before sending one more packet */
unsigned int Controller::timeout_ms( void )
{
  return 1; /* timeout of one second */
}

