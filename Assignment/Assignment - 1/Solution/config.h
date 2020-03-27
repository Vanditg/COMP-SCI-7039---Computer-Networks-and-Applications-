//==================================
// Computer Networks & Applications
// Student: Vandit Jyotindra Gajjar
// Student ID: a1779153
// Semester: 1
// Year: 2020
// Assignment: 1
//===================================

#define QLEN 6               /* size of request queue */
#define MAX_HTTP_REQ_SIZE 2048 /* handle HTTP request up to 2KB */
#define MAX_HTTP_RESPONSE_SIZE 2048 /* handle HTTP responses up to 2KB */
#define MAX_HEADER_LENGTH 50  /* maximum length for HTTP headers */

#define DEFAULT_PORT 8080    /* default port for server listening socket */

#define RESOURCE_PATH "/public"  /* location of http files */