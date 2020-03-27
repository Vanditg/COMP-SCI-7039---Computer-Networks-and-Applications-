//==================================
// Computer Networks & Applications
// Student: Vandit Jyotindra Gajjar
// Student ID: a1779153
// Semester: 1
// Year: 2020
// Assignment: 1
//===================================

#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "config.h"
#include "httpreq.h"

/*----------------------------------------------------------
 * Function: Parse_HTTP_Request
 *
 * Purpose:  Reads HTTP request from a socket and fills in a data structure
 *           (see httpreq.h with the request values (method and URI)
 *
 * Parameters:  socket         : the socket to read the request from
 *              request_value  : address of struct to write the values to
 *
 * Returns:  true if successfull, false otherwise
 *
 *-----------------------------------------------------------
 */

bool Parse_HTTP_Request(int socket, struct http_request * request_values) {

  char buffer[MAX_HTTP_REQ_SIZE];
  char request[MAX_HTTP_REQ_SIZE];
  ssize_t recvdBytes;

  // read request
  request[0] = '\0';
  do {
    recvdBytes = recv(socket, buffer, sizeof(buffer), 0);
    if (recvdBytes > 0) {
      strncat(request, buffer, recvdBytes);
    }
  } while (recvdBytes > 0 && (strstr(request, "\r\n\r\n") == NULL));
  printf("received request: %s\n", request);
  
  // parse request 
  char *line, *method;
  char *line_ptr;

  line = strtok_r(request, "\r\n", &line_ptr);

  method = strtok(line, " ");
  request_values->method = malloc (strlen(method) + 1);
  printf("Method is: %s\n", method);
  if (method == NULL)
    return false;
  strcpy(request_values->method, method);
  
  // parse the requested URI
  char * request_URI = strtok(NULL, " ");
  printf("URI is: %s\n", request_URI);
  if (request_URI == NULL)
    return false;
  request_values->URI = malloc (strlen(request_URI)+1);
  strcpy(request_values->URI, request_URI);

  char * version =  strtok(NULL, " ");
  if (version == NULL)
    return false;
  printf("version is: %s\n", version);
  
  
    
  // we can ignore headers, so just check that the blank line exists
  if ((strstr(request, "\r\n\r\n") == NULL))
    return true;
  else 
    return false;
}

/*----------------------------------------------------------
 * Function: Is_Valid_Resource
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

bool Is_Valid_Resource(char * URI) {

  char * server_directory, * location;
  char * resource;

  /* set the root server directory */

  if ( (server_directory = (char *) malloc(PATH_MAX)) != NULL)
    getcwd(server_directory, PATH_MAX);

  /* remove http://domain/ from URI */

  resource = strstr(URI, "http://");
  if (resource == NULL) {
    /* no http:// check if first character is /, if not add it */
    if (URI[0] != '/')
      resource = strcat("/", URI);
    else 
      resource = URI;
  }
  else
    /* if http:// resource must start with '/' */
    resource = strchr(resource, '/');

  if (resource == NULL)
    /* invalid resource format */
    return false;

  /* append root server directory *
   * for example if request is for /images/myphoto.jpg          *
   * and directory for server resources is /var/www/            *
   * then the resource location is /var/www/images/myphoto.jpg  */

  strcat(server_directory, RESOURCE_PATH);
  location = strcat(server_directory, resource);
  printf("server resource location: %s\n", location);

  /* check file access */

  if (!(access(location, R_OK))) {
    puts("access OK\n");
    free(server_directory);
    return true;
  } else {
    puts("access failed\n");
    free(server_directory);
    return false;
  }
}


/*----------------------------------------------------------
 * Function: Send_Resource
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

void Send_Resource(int socket, char * URI, char * method) {

  char * server_directory,  * resource;
  char * location;

  /* set the root server directory */

  if ( (server_directory = (char *) malloc(PATH_MAX)) != NULL)
    getcwd(server_directory, PATH_MAX);

  /* remove http://domain/ from URI */
  resource = strstr(URI, "http://");
  if (resource == NULL) {
    /* no http:// check if first character is /, if not add it */
    if (URI[0] != '/')
      resource = strcat("/", URI);
    else 
      resource = URI;
  }
  else
    /* if http:// resource must start with '/' */
    resource = strchr(resource, '/');

  /* append root server directory *
   * for example if request is for /images/myphoto.jpg          *
   * and directory for server resources is /var/www/            *
   * then the resource location is /var/www/images/myphoto.jpg  */

  strcat(server_directory, RESOURCE_PATH);
  location = strcat(server_directory, resource);
  /* open file and send contents on socket */

  FILE * file = fopen(location, "r");

  if (file < 0) {
    fprintf(stderr, "Error opening file.\n");
    exit(EXIT_FAILURE);
  }

  char c;
  long sz;
  char content_header[MAX_HEADER_LENGTH];
  
  /* get size of file for content_length header */
  fseek(file, 0L, SEEK_END);
  sz = ftell(file);
  rewind(file);

  sprintf(content_header, "Content-Length: %ld\r\n\r\n", sz);
  printf("Sending headers: %s\n", content_header);
  send(socket, content_header, strlen(content_header), 0);
  if (strcmp(method, "GET") == 0) 
  {
  printf("Sending file contents of %s\n", location);
  free(server_directory);

  while ( (c = fgetc(file)) != EOF ) {
    if ( send(socket, &c, 1, 0) < 1 ) {
      fprintf(stderr, "Error sending file.");
      exit(EXIT_FAILURE);
    }
    printf("%c", c);
  }
  puts("\nfinished reading file\n");
  }
  fclose(file);
}
