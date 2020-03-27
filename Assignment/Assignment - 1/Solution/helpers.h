//==================================
// Computer Networks & Applications
// Student: Vandit Jyotindra Gajjar
// Student ID: a1779153
// Semester: 1
// Year: 2020
// Assignment: 1
//===================================

#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>

#include "httpreq.h"

/*----------------------------------------------------------
 * Function: bool Parse_HTTP_Request(int socket, struct http_request * request_values)
 *
 * Purpose:  Reads HTTP request from a socket and fills in a data structure
 *           with the request values (method and URI)
 *
 * Parameters:  socket         : the socket to read the request from
 *              request_value  : address of struct to write the values to
 *
 * Returns:  true if successfull, false otherwise
 *
 *-----------------------------------------------------------
 */

extern bool Parse_HTTP_Request(int, struct http_request *);

/*----------------------------------------------------------
 * Function: Is_Valid__Resource(char * URI)
 *
 * Purpose:  Checks if URI is a valid resource
 *
 * Parameters:  URI  : the URI of the requested resource, both absolute
 *                     and relative URIs are accepted
 *
 * Returns:  false : the URI does not refer to a resource on the server
 *           true  : the URI is available on the server
 *
 *-----------------------------------------------------------
 */

extern bool Is_Valid_Resource(char *);

/*----------------------------------------------------------
 * Function: void Send_Resource(int socket, char * URI) 
 *
 * Purpose:  Sends the contents of the file referred to in URI on the socket
 *
 * Parameters:  socket  : the socket to send the content on
 *                URI   : the Universal Resource Locator, both absolute and 
 *                        relative URIs are accepted
 *
 * Returns:  void - errors will cause exit with error printed to stderr
 *
 *-----------------------------------------------------------
 */

extern void Send_Resource(int, char *, char *);

#endif
