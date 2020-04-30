//==================================
// Computer Networks & Applications
// Student: Vandit Jyotindra Gajjar
// Student ID: a1779153
// Semester: 1
// Year: 2020
// Assignment: 2
//===================================

extern int TRACE;

/* statistics updated by GBN */
extern int total_ACKs_received;
extern int packets_resent;       /* count of the number of packets resent  */
extern int new_ACKs;      /* count of the number of acks correctly received */
extern int packets_received;  /* count of the packets received by receiver */
extern int window_full; /* count of the number of messages dropped due to full window */

#define   A    0
#define   B    1

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg {
  char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt {
  int seqnum;
  int acknum;
  int checksum;
  char payload[20];
};

/* send to A or B (int), packet to send */
extern void tolayer3(int, struct pkt);  

/* deliver to A or B (int), data to deliver */
extern void tolayer5(int, char[20]); 

/* start timer at A or B (int), increment */
extern void starttimer(int, double);       

/* stop timer at A or B (int) */
extern void stoptimer(int);               
