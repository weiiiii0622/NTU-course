#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <zlib.h>

#include "def.h"

using namespace std;

#define DEBUG 0
#define LOGGING 1
#define ll long long

// *********************** STATE ***********************
#define SlowStart 0 
#define CongestionAvoidance 1

// *********************** Variables ***********************
int sock_fd;
struct sockaddr_in recv_addr;

double cwnd     = 0;
int thresh      = 16;
int dup_ack     = 0;
int cur_state;

// timer
timer_t timerid;

// queue
vector<segment> q;
int total_seg;
bool in_queue[99999];
bool has_send[99999];

// fin
bool finSent = false;

// *********************** Functions ***********************

// Return timer remaining `ns`
int getRemainingTime(timer_t timerid){
    struct itimerspec remaining;
    if(timer_gettime(timerid, &remaining) == -1){
        if(DEBUG) printf("[SYS] Timer get time failed.\n");
    }
    if(DEBUG) printf("[SYS] Timer remaining time: %ld ns.\n", remaining.it_value.tv_sec*1000000000 + remaining.it_value.tv_nsec);
    return (remaining.it_value.tv_sec*1000000000 + remaining.it_value.tv_nsec);
}

// Reset timer to a specific time (TIMEOUT_MILLISECONDS msec).
void resetTimer(timer_t timerid){
    struct itimerspec its;
    its.it_value.tv_sec = TIMEOUT_MILLISECONDS / 1000;
    its.it_value.tv_nsec = (TIMEOUT_MILLISECONDS % 1000) * 1000000; // 1 ms =  1000000 ns
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if(timer_settime(timerid, 0, &its, NULL) == -1){
        if(DEBUG) printf("[SYS] Timer reset failed.\n");
    }
}

// After you remove some segments in the window or the cwnd increases, 
// there will be more segments that is contained inside the window. 
// (Re)transmit those segments, no matter whether this segment has ever been sent before.
void transmitNew(){
    if(q[0].head.seqNumber == -1){
        // FIN
        if(finSent == true) return;
        segment fin_sgmt{};
        fin_sgmt.head.length = 0;
        fin_sgmt.head.seqNumber = 1 + total_seg;
        fin_sgmt.head.fin = 1;
        fin_sgmt.head.syn = 0;
        fin_sgmt.head.ack = 0;
        bzero(fin_sgmt.data, sizeof(char) * MAX_SEG_SIZE);
        fin_sgmt.head.checksum = crc32(0L, (const Bytef *)fin_sgmt.data, 0);

        sendto(sock_fd, &fin_sgmt, sizeof(fin_sgmt), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
        if(LOGGING) printf("send\tfin\n");
        finSent = true;
        return;
    }

    for(int idx=0; idx<int(cwnd); idx++){
        // Dummy Seg
        if(q[idx].head.seqNumber == -1 || idx >= q.size()) break;

        if(in_queue[q[idx].head.seqNumber] == false){
            sendto(sock_fd, &(q[idx]), sizeof(q[idx]), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
            if(has_send[q[idx].head.seqNumber] == false){ 
                if(LOGGING) printf("send\tdata\t#%d,\twinSize = %d\n", q[idx].head.seqNumber, int(cwnd));
            }
            else{
                if(LOGGING) printf("resnd\tdata\t#%d,\twinSize = %d\n", q[idx].head.seqNumber, int(cwnd));
            }
            in_queue[q[idx].head.seqNumber] = true;
            has_send[q[idx].head.seqNumber] = true;
        }
    }
}

// (Re)transmit the first segment in the window.
void transmitMissing(){
    if(q[0].head.seqNumber == -1) return;

    sendto(sock_fd, &(q[0]), sizeof(q[0]), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
    
    if(has_send[q[0].head.seqNumber] == false){ 
        if(LOGGING) printf("send\tdata\t#%d,\twinSize = %d\n", q[0].head.seqNumber, int(cwnd));
    }
    else{
        if(LOGGING) printf("resnd\tdata\t#%d,\twinSize = %d\n", q[0].head.seqNumber, int(cwnd));
    }
    has_send[q[0].head.seqNumber] = true;
}
// Go to a specific state.
void setState(int state){
    cur_state = state;
}

// True if the state is at `state` else False
bool isAtState(int state){
    return (cur_state == state);
}

// Remove the segment with sequence number seqNumber from the transmit queue
void markSACK(int seq_num){
    std::vector<segment>::iterator it;
    for(it=q.begin(); it!=q.end(); it++){
        if(it->head.seqNumber == seq_num) break;
    }
    if(it != q.end()) q.erase(it);
}

// Update base, transmit queue and window s.t. base > ackNumber
void updateBase(int ack_num){
    //q.erase(q.begin());
}

void updateCwnd(double value){
    cwnd = value;
    int sz = q.size();
    //for(int i=0; i<int(cwnd); i++)  in_queue[q[i].head.seqNumber] = true;
    for(int i=int(cwnd); i<sz; i++) in_queue[q[i].head.seqNumber] = false;
}

void setIP(char *dst, char *src){
    if(strcmp(src, "0.0.0.0") == 0 || strcmp(src, "local") == 0 || strcmp(src, "localhost") == 0){
        sscanf("127.0.0.1", "%s", dst);
    }
    else{
        sscanf(src, "%s", dst);
    }
    return;
}


// ./sender <send_ip> <send_port> <agent_ip> <agent_port> <src_filepath>
int main(int argc, char *argv[]) {
    // parse arguments
    if (argc != 6) {
        cerr << "Usage: " << argv[0] << " <send_ip> <send_port> <agent_ip> <agent_port> <src_filepath>" << endl;
        exit(1);
    }

    int send_port, agent_port;
    char send_ip[50], agent_ip[50];

    // read argument
    setIP(send_ip, argv[1]);
    sscanf(argv[2], "%d", &send_port);

    setIP(agent_ip, argv[3]);
    sscanf(argv[4], "%d", &agent_port);

    char *filepath = argv[5];

    // make socket related stuff
    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(agent_port);
    recv_addr.sin_addr.s_addr = inet_addr(agent_ip);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(send_port);
    addr.sin_addr.s_addr = inet_addr(send_ip);
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));    
    bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));

    fd_set main_set, work_set;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    FD_ZERO(&main_set);
    FD_SET(sock_fd, &main_set);
    //if(DEBUG) printf("socket fd: %ld\n", sock_fd);

    // Create packet
    int fd;
    ssize_t n;
    char buffer[MAX_SEG_SIZE];

    fd = open(filepath, O_RDONLY);

    while((n = read(fd, buffer, MAX_SEG_SIZE)) > 0){
        total_seg += 1;         // total # of segements

        segment sgmt{};
        sgmt.head.length = n;
        sgmt.head.seqNumber = total_seg;
        sgmt.head.fin = 0;
        sgmt.head.syn = 0;
        sgmt.head.ack = 0;
        bzero(sgmt.data, sizeof(char) * MAX_SEG_SIZE);
        memcpy(sgmt.data, buffer, n);
        sgmt.head.checksum = crc32(0L, (const Bytef *)sgmt.data, n);

        q.push_back(sgmt);
    }

    // Insert a Dummy Seg indicating the end
    segment nop_sgmt{};
    nop_sgmt.head.seqNumber = -1;
    q.push_back(nop_sgmt);

    if(DEBUG) printf("q size: %ld / total seq: %d\n", q.size(), total_seg);
    for(int i=0; i<total_seg; i++) in_queue[i] = false;
    for(int i=0; i<total_seg; i++) has_send[i] = false;

    // Create Timer
    struct sigevent sev;
    sev.sigev_notify = SIGEV_NONE;
    if(timer_create(CLOCK_REALTIME, &sev, &timerid) == -1){
        if(DEBUG) printf("[SYS] Timer create failed.\n");
    }

    if(DEBUG) printf("[SYS] Timer created\n");

    // Init
    updateCwnd(1);
    thresh = 16;
    dup_ack = 0;
    finSent = false;
    transmitNew();
    resetTimer(timerid);
    setState(SlowStart);

    // Start
    int ret;
    while(1){
        memcpy(&work_set, &main_set, sizeof(main_set));

        // Get timer
        ll remain_time = getRemainingTime(timerid);
        timeout.tv_sec = max((ll) 0, remain_time / 1000000000);
        timeout.tv_usec = max((ll) 0, (remain_time % 1000000000) / 1000);
        if(DEBUG) printf("[SYS] Clock remain %ld us\n", timeout.tv_usec + timeout.tv_sec*1000000);
        
        if(remain_time == 0) ret = 0;
        else ret = select(sock_fd + 1, &work_set, NULL, NULL, &timeout);

        if(ret == 0){
            // Timeout
            if(DEBUG) printf("[SYS] TIMEOUT !!! \n");
            thresh = max(1, int(cwnd/2));
            updateCwnd(1);
            dup_ack = 0;
            if(LOGGING) printf("time\tout,\tthreshold = %d,\twinSize = %d\n", thresh, int(cwnd));

            if(q[0].head.seqNumber == -1){
                // FIN
                if(finSent == true) continue;
                segment fin_sgmt{};
                fin_sgmt.head.length = 0;
                fin_sgmt.head.seqNumber = 1 + total_seg;
                fin_sgmt.head.fin = 1;
                fin_sgmt.head.syn = 0;
                fin_sgmt.head.ack = 0;
                bzero(fin_sgmt.data, sizeof(char) * MAX_SEG_SIZE);
                fin_sgmt.head.checksum = crc32(0L, (const Bytef *)fin_sgmt.data, 0);

                sendto(sock_fd, &fin_sgmt, sizeof(fin_sgmt), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
                if(LOGGING) printf("send\tfin\n");
                finSent = true;
            }
            else{
                transmitMissing();
            }
            resetTimer(timerid);
            setState(SlowStart);
        }
        else{
            // Read
            if(DEBUG) printf("[SYS] READ !!! \n");


            segment sgmt{};
            socklen_t recv_addr_sz;
            recvfrom(sock_fd, &sgmt, sizeof(sgmt), 0, (struct sockaddr *)&recv_addr, &recv_addr_sz);


            if(sgmt.head.fin == 1){
                // FIN-ACK
                if(LOGGING) printf("recv\tfinack\n");
                break;
            }
            if(LOGGING) printf("recv\tack\t#%d,\tsack\t#%d\n", sgmt.head.ackNumber, sgmt.head.sackNumber);

            if(q[0].head.seqNumber == -1){
                // FIN
                if(finSent == true) continue;
                segment fin_sgmt{};
                fin_sgmt.head.length = 0;
                fin_sgmt.head.seqNumber = 1 + total_seg;
                fin_sgmt.head.fin = 1;
                fin_sgmt.head.syn = 0;
                fin_sgmt.head.ack = 0;
                bzero(fin_sgmt.data, sizeof(char) * MAX_SEG_SIZE);
                fin_sgmt.head.checksum = crc32(0L, (const Bytef *)fin_sgmt.data, 0);

                sendto(sock_fd, &fin_sgmt, sizeof(fin_sgmt), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
                if(LOGGING) printf("send\tfin\n");
                finSent = true;
            }
            else{
                // Dup-Ack
                if(sgmt.head.ackNumber < q[0].head.seqNumber){
                    dup_ack += 1;
                    markSACK(sgmt.head.sackNumber);
                    transmitNew();
                    if(dup_ack == 3){
                        transmitMissing();
                    }
                }
                else{   // New-Ack
                    dup_ack = 0;
                    markSACK(sgmt.head.sackNumber);
                    updateBase(sgmt.head.ackNumber);
                    if(isAtState(SlowStart)){
                        updateCwnd(cwnd + 1);
                        if(cwnd >= thresh){
                            setState(CongestionAvoidance);
                        }
                    }
                    else if(isAtState(CongestionAvoidance)){
                        updateCwnd(cwnd + (double(1) / int(cwnd)));
                    }
                    transmitNew();
                    resetTimer(timerid);
                }

            }
        }
    }

    // Delete Timer
    if(timer_delete(timerid) == -1){
        if(DEBUG) printf("[SYS] Timer delete failed.\n");
    }
    return 0;
}