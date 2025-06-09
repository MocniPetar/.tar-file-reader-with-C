#include "../headers/server_structures.h"
#include "../headers/starting_header.h"
#include "../headers/important_header.h"

ssize_t parseRequest(struct Client*, char*);
int getReguestedRoute(char*, char*, char*);
int writingResponse(char *, char *, char **, int);
int sendingResponse(int, char*, int);
void* constructResponse(void*);

ssize_t parseRequest(struct Client* client, char* req_buffer) 
{
    ssize_t receivedBytes = 0;
    receivedBytes = recv(client->socket, req_buffer, MAX_REQUEST_SIZE - 1, 0);

    dprintf(client->fd, "Request:\n%s\n", req_buffer);
    sscanf(req_buffer, "%s %s", client->method, client->route);

    printf("(Log) Requested route: %s\n", client->route);

    client->method[strlen(client->method) + 1] = '\0';
    client->route[strlen(client->route) + 1] = '\0';
    
    ssize_t dirLength = strlen(client->dir);
    ssize_t routeLength = strlen(client->route);

    char* temp_route = (char *)malloc(routeLength + dirLength + 1);
    if (temp_route == NULL) {
        printf("(Log) Failed to allocate memory using malloc for temp_route...\n");
        return -1;
    }
    memset(temp_route, 0, routeLength + dirLength + 1);

    snprintf(temp_route, routeLength + dirLength, "%s", client->dir);
    strcat(temp_route, client->route);
    snprintf(client->route, sizeof(client->route), "%s", temp_route);

    free(temp_route);
    return receivedBytes;
}

int parsePostRequest(char* req_buffer, ssize_t recievedBytes) {

    // Find the Content-Length header, store the length in a variable,
    // read bytes from the socket until the read size is equel to the Content-Length
    // As a test print the full request with a file in the body

    return 0;
}

int getReguestedRoute(char *filePath, char *method, char *MIMEtype) 
{
    // Skip the first two dots from ../WebSite/dist/browser/{filePath}
    ssize_t filePathLength = strlen(filePath);
    for (int i = 2; i < (int)filePathLength; i++) {
        if (filePath[i] == '.') { 
            snprintf(MIMEtype, 16, "%s", &filePath[i]);
        }
    }
    
    // If MIMEtype is less then one then the client requetsed a page
    if(strlen(MIMEtype) < 1)
    {
        if (filePath[filePathLength-1] == '/')
            strcat(filePath, "index.html");
        else
            strcat(filePath, "/index.html");
        
        snprintf(MIMEtype, 16, ".html");
        filePath[strlen(filePath) + 1] = '\0';
    }
    
    if (access(filePath, F_OK) != 0) {
        printf("(Log) File does not exist. Re-routing client to not found...\n\n");
        snprintf(filePath, 256, "../WebSite/src/pages/not_found/index.html");
        snprintf(MIMEtype, 16, ".html");
    }

    return 0;
}

int writingResponse(char *fullPath, char *MIMEtype, char **response_buffer, int fd) 
{
    // Call findPageDataInList here
    int page_fd = findPageDataInList(fullPath);
    char *file_buffer = NULL;

    if (fd == 0)
    {
        size_t problem_response_size = strlen("HTTP/1.1 500 Problem\r\n\r\n") + 1;
        *response_buffer = (char *)malloc((problem_response_size)*sizeof(char));
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            return -1;
        }
        memset(*response_buffer, 0, problem_response_size);
        snprintf(*response_buffer, problem_response_size, "HTTP/1.1 500 Problem\r\n\r\n");
    }
    else 
    {
        char response_setup_string[256];
        memset(response_setup_string, 0, 256);

        // Find a way to better write this part
        if (strcmp(MIMEtype, ".html") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".css") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/css; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".js") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: text/javascript; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".svg") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/svg+xml; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".json") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".ico") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }
        else if (strcmp(MIMEtype, ".png") == 0) {
            snprintf(response_setup_string, sizeof(response_setup_string), "HTTP/1.1 200 OK\r\nContent-Type: image/png; charset=UTF-8\r\nConnection: keep-alive\r\n\r\n");
        }

        size_t response_setup_size = strlen(response_setup_string);
        ssize_t file_size = 0;
        check((file_size = lseek(page_fd, 0, SEEK_END)), "(Log) Failed to read the size of page_fd\n", 0);
        check(lseek(page_fd, 0, SEEK_SET), "(Log) Failed to reset the page_fd\n", 0);

        file_buffer = (char *)malloc(file_size + 1);
        if (file_buffer == NULL)
        {
            fprintf(stderr, "(Log) Failed to allocate memory...\n");
            dprintf(fd, "(Log) Failed to allocate memory...\n");
            return -1;
        }
        memset(file_buffer, 0, file_size + 1);

        if(read(page_fd, file_buffer, file_size) <= 0) {
            fprintf(stderr, "(Log) Failed to read data from file...\n");
            dprintf(fd, "(Log) Failed to read data from file...\n");
            free(file_buffer);
            return -1;
        }

        *response_buffer = (char *)malloc(response_setup_size + file_size + 1);
        if (*response_buffer == NULL) {
            fprintf(stderr, "Failed to allocate memory...\n");
            dprintf(fd, "Failed to allocate memory...\n");
            free(file_buffer);
            return -1;
        }
        memset(*response_buffer, 0, response_setup_size + file_size + 1);

        strncpy(*response_buffer, response_setup_string, response_setup_size + file_size);
        strcat(*response_buffer, file_buffer);

        free(file_buffer);
    }
    return 0;
}

int sendingResponse(int clientSocket, char* response_buffer, int fd)
{
    if (send(clientSocket, response_buffer, strlen(response_buffer), 0) < 0)
    {
        fprintf(stderr, "(Log) Failed to send response\n");
        dprintf(fd, "(Log) Failed to send response\n");
        return -1;
    }

    return 0;
}

void* constructResponse(void* arg) 
{
    struct Client client = *((struct Client*)arg);
    free(arg);

    ssize_t recievedBytes = 0;
    char* req_buffer = (char *)malloc((MAX_REQUEST_SIZE + 1) * sizeof(char));
    if (req_buffer == NULL) {
        dprintf(client.fd, "Failed to allocate memmory for req_buffer\n");
        close(client.socket);
        return NULL;
    }
    memset(req_buffer, 0, MAX_REQUEST_SIZE);

    // Take the request and process it
    if ((recievedBytes = parseRequest(&client, req_buffer)) == -1) {
        printf("(Log) Error occured while parsing the request. Check the log for more details\n");
        free(req_buffer);
        close(client.socket);
        return NULL;
    }
    
    if (strcmp(client.method, "POST") == 0) {
        printf("(Log) Recieved a POST request from client\n");

        if (strstr(client.route, "readFile") == NULL) {
            printf("(Log) Error: Request for route %s is not a supported route\n", client.route);
        } else {
            check(parsePostRequest(req_buffer, recievedBytes), "(Log) Failed to execute parsePostRequest", client.fd);
        }

        free(req_buffer);
        close(client.socket);
        return NULL;
    }

    char filePath[1024];
    char MIMEtype[16];
    memset(filePath, 0, 1024);
    memset(MIMEtype, 0, 16);

    snprintf(filePath, sizeof(filePath), "%s", client.route);

    if (strcmp(client.method, "GET") != 0) {
        printf("(Log) Error: The server does not accept %s method\n", client.method);
        free(req_buffer);
        close(client.socket);
        return NULL;
    }

    if (getReguestedRoute(filePath, client.method, MIMEtype) == -1) {
        printf("(Log) Error occured while creating the route. Check the log for more details\n");
        free(req_buffer);
        close(client.socket);
        return NULL;
    }

    // Create the response
    char* responseBuffer = NULL;
    if (writingResponse(filePath, MIMEtype, &responseBuffer, client.fd) == -1) {
        if (responseBuffer != NULL) { free(responseBuffer); }
        free(req_buffer);
        close(client.socket);
        return NULL;
    }

    if (responseBuffer == NULL) {
        printf("(Log) ERROR: Response buffer is NULL!\n");
        free(req_buffer);
        close(client.socket);
        return NULL;
    }

    // Send the response to the client
    if (sendingResponse(client.socket, responseBuffer, client.fd) == -1) {
        printf("(Log) Error occured while sending the response. Check the log for more details\n");
    }

    free(req_buffer);
    close(client.socket);
    free(responseBuffer);
    return NULL;
}