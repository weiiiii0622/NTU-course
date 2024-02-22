#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define ERR_EXIT(a) do { perror(a); exit(1); } while(0)

#define OBJ_NUM 3

#define FOOD_INDEX 0
#define CONCERT_INDEX 1
#define ELECS_INDEX 2
#define RECORD_PATH "./bookingRecord"

#define DEBUG 0

static char* obj_names[OBJ_NUM] = {"Food", "Concert", "Electronics"};

typedef struct {
    int id;          // 902001-902020
    int bookingState[OBJ_NUM]; // 1 means booked, 0 means not.
}record;

static int readingState[20]; // Determine the # of requests reading bookingRecord[id]
static int writingState[20]; // Determine the # of requests reading bookingRecord[id]

typedef struct {
    char hostname[512];  // server's hostname
    unsigned short port;  // port to listen
    int listen_fd;  // fd to wait for a new connection
} server;

typedef struct {
    char host[512];  // client's host
    int conn_fd;  // fd to talk with client
    char buf[512];  // data sent by/to client
    size_t buf_len;  // bytes used by buf
    int id;
    int wait_for_write;  // used by handle_read to know if the header is read or not.
    int wait_for_exit; // to know whether waiting for "Exit"
    int updateState[OBJ_NUM];

} request;

server svr;  // server
request* requestP = NULL;  // point to a list of requests
int maxfd;  // size of open file descriptor table, size of request list

const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";
const unsigned char IAC_IP[3] = "\xff\xf4";

static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static void free_request(request* reqP);
// free resources used by a request instance

// Handle Read/Write
int handle_read(request* reqP) {
    /*  Return value:
     *      1: read successfully
     *      0: read EOF (client down)
     *     -1: read failed
     */
    int r;
    char buf[512] = "";

    // Read in request from client
    r = read(reqP->conn_fd, buf, sizeof(buf));
    if (r < 0) return -1;
    if (r == 0) return 0;
    char* p1 = strstr(buf, "\015\012");
    int newline_len = 2;
    if (p1 == NULL) {
       p1 = strstr(buf, "\012");
        if (p1 == NULL) {
            if (!strncmp(buf, IAC_IP, 2)) {
                // Client presses ctrl+C, regard as disconnection
                if(DEBUG) fprintf(stderr, "Client presses ctrl+C....\n");
                return 0;
            }
            ERR_EXIT("this really should not happen...");
        }
    }
    size_t len = p1 - buf + 1;
    memmove(reqP->buf, buf, len);
    reqP->buf[len - 1] = '\0';
    reqP->buf_len = len-1;
    return 1;
}
void handle_write(request* reqP, char *buf){
    write(reqP->conn_fd, buf, strlen(buf));
}

// Parse update value

int check_only_digits(char *str){
    int len = strlen(str);
    for(int i=0; i<len; i++){
        if(str[i] == '-' && i != 0){
            if(DEBUG) fprintf(stderr, "Invalid '-' occured at i=%d\n", i);
            return -1;
        }
        else if(str[i] != '-' && (str[i] < '0' || str[i] > '9')){
            if(DEBUG) fprintf(stderr, "Invalid alphabet occured at i=%d\n", i);
            return -1;
        }
    }
    return 1;
}

int handle_update_value(request* reqP){
    char str[512]; strcpy(str, reqP->buf);
    const char* d = " ";
    char *p;
    int i=0;
    
    p = strtok(str, d);
    while(p != NULL){

        // input check
        if(check_only_digits(p) == 1){
            reqP->updateState[i] = atoi(p);
            // fprintf(stderr, "%d: %d\n", i, reqP->updateState[i]);
        }
        else{
            if(DEBUG) fprintf(stderr, "%d: %s is invalid\n", i, p);
            return -1;
        }
        p = strtok(NULL, d);
        i++;
    }

    return 1;
}

int handle_id_input(request* reqP){
    char str[512]; strcpy(str, reqP->buf);

    // input check
    if(check_only_digits(str) == 1){
        return 1;
    }

    return -1;    
}


// Set Lock
int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len){
    struct flock lock;
    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;
    return(fcntl(fd, cmd, &lock));
}

int setReadLock(int fd, int id){
    return lock_reg(fd, F_SETLK, F_RDLCK, id*sizeof(record), SEEK_SET, sizeof(record));
}

int setWriteLock(int fd, int id){
    return lock_reg(fd, F_SETLK, F_WRLCK, id*sizeof(record), SEEK_SET, sizeof(record));
}

int unlock(int fd, int id){
    return lock_reg(fd, F_SETLK, F_UNLCK, id*sizeof(record), SEEK_SET, sizeof(record));  
}

// Close a connection
void close_connection(request *reqP, fd_set *s){
    if(DEBUG) fprintf(stderr, "Request: %d disconnected\n", reqP->conn_fd);
    FD_CLR(reqP->conn_fd, s);
    close(reqP->conn_fd);
    free_request(reqP);    
}

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in cliaddr;  // used by accept()
    int clilen;

    int conn_fd;  // fd for a new connection with client
    int file_fd;  // fd for file that we open for reading
    char buf[512] = "";
    int buf_len;

    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Loop for handling connections
    fprintf(stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);

    // Descriptor Set
    fd_set main_set, work_set;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100;

    FD_ZERO(&main_set);
    FD_SET(svr.listen_fd, &main_set);

    // Open bookingRecord
#ifdef READ_SERVER
    file_fd = open(RECORD_PATH, O_RDONLY);
#elif defined WRITE_SERVER
    file_fd = open(RECORD_PATH, O_RDWR);
#endif

    while (1) {
        // TODO: Add IO multiplexing
        // Reset descriptor set
        memcpy(&work_set, &main_set, sizeof(main_set));
        

        if(select(maxfd+1, &work_set, NULL, NULL, &timeout) <= 0){
            continue;
        }
        
        if(FD_ISSET(svr.listen_fd, &work_set)){
            
            // Check new connection
            clilen = sizeof(cliaddr);
            conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
            if (conn_fd < 0) {
                if (errno == EINTR || errno == EAGAIN) continue;  // try again
                if (errno == ENFILE) {
                    (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
                    continue;
                }
                ERR_EXIT("accept");
            }
            requestP[conn_fd].conn_fd = conn_fd;
            strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
            fprintf(stderr, "getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);
            
            FD_SET(conn_fd, &main_set);
            
            // Welcome message
            handle_write(&requestP[conn_fd], "Please enter your id (to check your booking state):\n");
            continue;
        }
        else{
            
            for(int i=0; i<maxfd; i++){
                if(i!=svr.listen_fd && FD_ISSET(i, &work_set)){

                    // TODO: handle requests from clients
#ifdef READ_SERVER
                    int ret = handle_read(&requestP[i]); // parse data from client to requestP[conn_fd].buf
                    if(DEBUG) fprintf(stderr, "Request: %d for read typed: %s\n", requestP[i].conn_fd, requestP[i].buf);
                    if (ret < 0) {
                        fprintf(stderr, "bad request from %s\n", requestP[i].host);
                        continue;
                    }
                    
                    if(requestP[i].wait_for_exit == 0){
                        int ret = handle_id_input(&requestP[i]);
                        if(ret < 0){
                            handle_write(&requestP[i], "[Error] Operation failed. Please try again.\n");
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }

                        int id = atoi(requestP[i].buf) - 902001;
                        requestP[i].id = id;
                        if(DEBUG) fprintf(stderr, "Request: %d asked to read for id: %d\n", requestP[i].conn_fd, id);
                        if(id < 0 || id >= 20){
                            handle_write(&requestP[i], "[Error] Operation failed. Please try again.\n");
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }
                        
                        if(setReadLock(file_fd, id) < 0){
                            handle_write(&requestP[i], "Locked.\n");
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }
                        else{
                            readingState[id] += 1;
                            if(DEBUG) fprintf(stderr, "# of request reading id %d: %d\n", id, readingState[id]);
                            record rec;
                            char buf[512];
                            lseek(file_fd, sizeof(record)*id, SEEK_SET);
                            read(file_fd, &rec, sizeof(record));

                            sprintf(buf, 
                            "Food: %d booked\nConcert: %d booked\nElectronics: %d booked\n\n(Type Exit to leave...)\n"
                            , rec.bookingState[0], rec.bookingState[1], rec.bookingState[2]);

                            handle_write(&requestP[i], buf);
                        }

                        // fprintf(stderr, "%s", "Successfully open bookingRecord");
                        requestP[i].wait_for_exit = 1;
                    }
                    else{
                        if(strcmp(requestP[i].buf, "Exit") == 0 || ret == 0){
                            readingState[requestP[i].id] -= 1;
                            if(readingState[requestP[i].id] == 0){
                                if(DEBUG) fprintf(stderr, "Request: %d unlocked id: %d\n", requestP[i].conn_fd, requestP[i].id);
                                unlock(file_fd, requestP[i].id);
                            }
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }
                    }  

#elif defined WRITE_SERVER
                    int ret = handle_read(&requestP[i]); // parse data from client to requestP[conn_fd].buf
                    if(DEBUG) fprintf(stderr, "Request: %d for write typed: %s\n", requestP[i].conn_fd, requestP[i].buf);
                    if (ret < 0) {
                        fprintf(stderr, "bad request from %s\n", requestP[i].host);
                        continue;
                    }
                    else if(ret == 0){
                        if(writingState[requestP[i].id]>0){
                            if(DEBUG) fprintf(stderr, "Request: %d unlocked id: %d\n", requestP[i].conn_fd, requestP[i].id);
                            unlock(file_fd, requestP[i].id);
                            writingState[requestP[i].id] = 0;
                        }
                        close_connection(&requestP[i], &main_set);
                        continue;
                    }
                
                    if(requestP[i].wait_for_write == 0){
                        int ret = handle_id_input(&requestP[i]);
                        if(ret < 0){
                            handle_write(&requestP[i], "[Error] Operation failed. Please try again.\n");
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }

                        int id = atoi(requestP[i].buf) - 902001;
                        requestP[i].id = id;
                        if(DEBUG) fprintf(stderr, "Request: %d asked to write for id: %d\n", requestP[i].conn_fd, id);
                        if(id < 0 || id >= 20){
                            handle_write(&requestP[i], "[Error] Operation failed. Please try again.\n");
                            close_connection(&requestP[i], &main_set);
                            continue;
                        }

                        
                        if(writingState[id]>0 || setWriteLock(file_fd, id) < 0){
                            handle_write(&requestP[i], "Locked.\n");
                            close_connection(&requestP[i], &main_set);
                        }
                        else{
                            if(DEBUG) fprintf(stderr, "Request: %d accessed writing mode\n", requestP[i].conn_fd);
                            writingState[id] = 1;
                            record rec;
                            char buf[512];
                            lseek(file_fd, sizeof(record)*id, SEEK_SET);
                            read(file_fd, &rec, sizeof(record));

                            sprintf(buf, 
                            "Food: %d booked\nConcert: %d booked\nElectronics: %d booked\n\nPlease input your booking command. (Food, Concert, Electronics. Positive/negative value increases/decreases the booking amount.):\n"
                            , rec.bookingState[0], rec.bookingState[1], rec.bookingState[2]);
                            handle_write(&requestP[i], buf);

                            requestP[i].wait_for_write = 1;
                        } 
                    }
                    else{
                        record rec;
                        char buf[512];
                        lseek(file_fd, sizeof(record)*requestP[i].id, SEEK_SET);
                        read(file_fd, &rec, sizeof(record));

                        int ret = handle_update_value(&requestP[i]);
                        if(ret < 0){
                            handle_write(&requestP[i], "[Error] Operation failed. Please try again.\n");
                        }
                        else{
                            
                            int flag = 1;
                            // Upper bound
                            if(rec.bookingState[0]+rec.bookingState[1]+rec.bookingState[2]+requestP[i].updateState[0]+requestP[i].updateState[1]+requestP[i].updateState[2]>15){
                                handle_write(&requestP[i], "[Error] Sorry, but you cannot book more than 15 items in total.\n");
                                flag = 0;
                            }
                            // Lower bound
                            if(rec.bookingState[0]+requestP[i].updateState[0] < 0 || rec.bookingState[1]+requestP[i].updateState[1] < 0 || rec.bookingState[2]+requestP[i].updateState[2] < 0){
                                handle_write(&requestP[i], "[Error] Sorry, but you cannot book less than 0 items.\n");
                                flag = 0;
                            }

                            if(flag){
                                for(int j=0; j<3; j++){
                                    rec.bookingState[j] = rec.bookingState[j]+requestP[i].updateState[j];
                                }
                                lseek(file_fd, sizeof(record)*requestP[i].id, SEEK_SET);
                                write(file_fd, &rec, sizeof(record));

                                sprintf(buf, 
                                "Bookings for user %d are updated, the new booking state is:\nFood: %d booked\nConcert: %d booked\nElectronics: %d booked\n"
                                , rec.id, rec.bookingState[0], rec.bookingState[1], rec.bookingState[2]);
                                handle_write(&requestP[i], buf);                                
                            }
                        }

                        writingState[requestP[i].id] = 0;
                        if(DEBUG) fprintf(stderr, "Request: %d unlocked id: %d\n", requestP[i].conn_fd, requestP[i].id);
                        unlock(file_fd, requestP[i].id);

                        close_connection(&requestP[i], &main_set);
                        continue;
                    }
  
#endif
                }
            }
        }
    }

    close(file_fd);
    free(requestP);
    return 0;
}

// ======================================================================================================
// You don't need to know how the following codes are working
#include <fcntl.h>

static void init_request(request* reqP) {
    reqP->conn_fd = -1;
    reqP->buf_len = 0;
    reqP->id = 0;
    reqP->wait_for_exit = 0;
    reqP->wait_for_write = 0;
}

static void free_request(request* reqP) {
    /*if (reqP->filename != NULL) {
        free(reqP->filename);
        reqP->filename = NULL;
    }*/
    init_request(reqP);
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    tmp = 1;
    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        ERR_EXIT("bind");
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }

    // Get file descripter table size and initialize request table
    maxfd = getdtablesize();
    requestP = (request*) malloc(sizeof(request) * maxfd);
    if (requestP == NULL) {
        ERR_EXIT("out of memory allocating all requests");
    }
    for (int i = 0; i < maxfd; i++) {
        init_request(&requestP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);

    return;
}
