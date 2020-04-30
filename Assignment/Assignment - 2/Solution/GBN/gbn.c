#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulator.h"
#include "altbit.h"

/* ******************************************************************
   Unfinished Alternating bit protocol.  Adapted from
   ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   Network properties:
   - one way network delay averages five time units (longer if there
   are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
   or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
   (although some can be lost).

   Modifications (6/6/2008 - CLP):
   - removed bidirectional code and other code not used by prac.
   - fixed C style to adhere to current programming style
   (7/8/2009 - CLP)
   - converted to Alt Bit
**********************************************************************/

#define RTT  15.0       /* round trip time.  MUST BE SET TO 15.0 when submitting assignment */
#define WINDOWSIZE 6    /* alternating bit only allows one unacked packet */
#define NOTINUSE (-1)   /* used to fill header fields that are not being used */

/* generic procedure to compute the checksum of a packet.  Used by both sender and receiver
   the simulator will overwrite part of your packet with 'z's.  It will not overwrite your
   original checksum.  This procedure must generate a different checksum to the original if
   the packet is corrupted.
*/

int ComputeChecksum(struct pkt packet)
{
  int checksum = 0;

  /****** 4. FILL IN CODE to calculate the checksum of packet *****/
  for (int i = 0; i < 20; i++) {
    checksum += packet.payload[i];
  }
  checksum += packet.acknum;
  checksum += packet.seqnum;
  checksum = ~checksum;

  return checksum;
}

bool IsCorrupted(struct pkt packet)
{
  if (packet.checksum == ComputeChecksum(packet))
    return (false);
  else
    return (true);
}

/********* Sender (A) variables and functions ************/

static struct pkt buffer[WINDOWSIZE];  /* array for storing packets waiting for ACK */
static int windowfirst, windowlast;    /* array indexes of the first/last packet awaiting ACK */
static int windowcount;                /* the number of packets currently awaiting an ACK */
static int A_nextseqnum;               /* the next sequence number to be used by the sender */

/* called from layer 5 (application layer), passed the message to be sent to other side */

void A_output(struct msg message)
{
  struct pkt sendpkt;
  int i;

  /* if not blocked waiting on ACK */
  if ( windowcount < WINDOWSIZE) {
    if (TRACE > 1)
      printf("----A: New message arrives, send window is not full, send new messge to layer3!\n");

    /* create packet */
    sendpkt.seqnum = A_nextseqnum;
    sendpkt.acknum = NOTINUSE;
    for ( i=0; i<20 ; i++ )
      sendpkt.payload[i] = message.data[i];
    sendpkt.checksum = ComputeChecksum(sendpkt);

    /* put packet in window buffer */
    /* windowlast will always be 0 for alternating bit; but not for GoBackN */
    windowlast = (windowlast + 1) % WINDOWSIZE;
    buffer[windowlast] = sendpkt;
    for (i=0; i<20; i++)
      buffer[windowlast].payload[i]=sendpkt.payload[i];  /* copy the array */
    windowcount++;

    /* send out packet */
    if (TRACE > 0)
      printf("Sending packet %d to layer 3\n", sendpkt.seqnum);
    tolayer3 (A, sendpkt);
    /**** 1. FILL IN CODE There's something else A needs to do when it sends a packet. *****/
    if (windowcount == 1) {
      starttimer(A, RTT);
    }

    A_nextseqnum = (A_nextseqnum + 1) % (WINDOWSIZE + 1);  /* we only have seqnum 0 and 1 */
  }
  /* if blocked,  window is full */
  else {
    if (TRACE > 0)
      printf("----A: New message arrives, send window is full\n");
    window_full++;
  }
}


/* called from layer 3, when a packet arrives for layer 4
   In this practical this will always be an ACK as B never sends data.
*/
void A_input(struct pkt packet)
{
  size_t ACKs = 0;
  int i = 0;
  bool dup = true;
  /* if received ACK is not corrupted */
  if (!IsCorrupted(packet)) {
    if (TRACE > 0)
      printf("----A: uncorrupted ACK %d is received\n",packet.acknum);
    total_ACKs_received++;
	
	int i = 0;
	for (i = windowfirst; i < windowfirst + windowcount; i++) {
		if (packet.acknum == buffer [i % WINDOWSIZE].seqnum) {
			dup = false;
		}
	}
    
    /* check if new ACK or duplicate */
    if (!dup) { /****(packet.acknum!= A_nextseqnum && !windowcount == 0) { ****/   /****!(windowcount == 0) ****/ /**** 2. FILL IN CODE replace TRUE with test whether this is a new ACK ***/
      /* packet is a new ACK */
      if (TRACE > 0)
        printf("----A: ACK %d is not a duplicate\n",packet.acknum);
      new_ACKs++;

      /* delete the acked packets from window buffer */
      for (i = windowfirst; (i % WINDOWSIZE) != windowlast && packet.acknum != buffer[i % WINDOWSIZE].seqnum; i++) {
        ACKs++;
      }
      ACKs++;

      windowfirst = (windowfirst + ACKs) % WINDOWSIZE;
      windowcount = windowcount - ACKs;


      /***** 1. FILL IN CODE  What else needs to be done when an ACK arrives
       besides removing the packet from the window?  ****/
      stoptimer(A);
	  if (windowcount > 0) {
        starttimer(A, RTT);
	  }

    }
    else
      if (TRACE > 0)
        printf ("----A: duplicate ACK received, do nothing!\n");
  }
  else
    if (TRACE > 0)
      printf ("----A: corrupted ACK is received, do nothing!\n");
}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
  int i = 0;
  if (TRACE > 0)
    printf("----A: time out,resend packets!\n");

  if (TRACE > 0) {
	for (i = windowfirst; (i % WINDOWSIZE) != windowlast; i++) {
	  printf ("---A: resending packet %d\n", (buffer[i]).seqnum);
	  tolayer3(A, buffer[i]);
	  
	  if (i == windowfirst) {
		starttimer(A, RTT);
	  }
	  packets_resent++;
	}
	
  printf("---A: resending packet %d\n", (buffer[windowlast]).seqnum);
  tolayer3(A,buffer[windowlast]);
  
  /**** 1. FILL IN CODE What state should the timer be in at this point? *****/
  
  if (windowfirst == windowlast) {
    starttimer(A, RTT);
  }
  packets_resent++;
  }
  
}



/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
  /* initialise A's window, buffer and sequence number */
  A_nextseqnum = 0;  /* A starts with seq num 0, do not change this */
  windowfirst = 0;
  windowlast = -1;   /* windowlast is where the last packet sent is stored.
		     new packets are placed in winlast + 1
		     so initially this is set to -1		   */
  windowcount = 0;
}



/********* Receiver (B)  variables and procedures ************/

static int expectedseqnum; /* the sequence number expected next by the receiver */
static int B_nextseqnum;   /* the sequence number for the next packets sent by B */


/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  struct pkt sendpkt;
  int i;

  /* if not corrupted and received packet is in order */
  if  ( (!IsCorrupted(packet))  && (packet.seqnum == expectedseqnum) ) {
    if (TRACE > 0)
      printf("----B: packet %d is correctly received, send ACK!\n",packet.seqnum);
    packets_received++;

    /* deliver to receiving application */
    tolayer5(B, packet.payload);

    /* send an ACK for the received packet */
    sendpkt.acknum = expectedseqnum;

    /* update state variables */
    expectedseqnum = (expectedseqnum + 1) % (WINDOWSIZE + 1);
  }
  else {
    /* packet is corrupted or out of order */
    if (TRACE > 0)
      printf("----B: packet corrupted or not expected sequence number, resend ACK!\n");
    /***** 3. FILL IN CODE  What ACK number should be sent if the packet
	   was corrupted or out of order? *******/ 
    if(expectedseqnum != 0) {
	  sendpkt.acknum = expectedseqnum - 1;
    }
	else {
	  sendpkt.acknum = WINDOWSIZE;
    }
  }
  /* create packet */
  sendpkt.seqnum = B_nextseqnum;
  B_nextseqnum = (B_nextseqnum + 1) % (WINDOWSIZE + 1);

  /* we don't have any data to send.  fill payload with 0's */
  for ( i=0; i<20 ; i++ )
    sendpkt.payload[i] = '0';

  /* computer checksum */
  sendpkt.checksum = ComputeChecksum(sendpkt);

  /* send out packet */
  tolayer3 (B, sendpkt);
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
  expectedseqnum = 0;
  B_nextseqnum = 1;
}

/******************************************************************************
 * The following functions need be completed only for bi-directional messages *
 * They do not need to be completed for this practical                        *
 *****************************************************************************/

/* Note that with simplex transfer from a-to-B, there is no B_output() */
void B_output(struct msg message)
{
}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
}
