#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <zlib.h>

#include "def.h"

using namespace std;

#define DEBUG 0
#define LOGGING 1
#define ll long long

// *********************** Variables ***********************
int sock_fd;
struct sockaddr_in recv_addr;

bool is_ack[MAX_SEG_BUF_SIZE + 1];
segment seg_buffer[MAX_SEG_BUF_SIZE + 1];
int flush_cnt = 0;
int base = 1;

// sha256
EVP_MD_CTX *sha256;
int n_bytes = 0;

// *********************** Functions ***********************

// to hex string
string hexDigest(const void *buf, int len) {
    const unsigned char *cbuf = static_cast<const unsigned char *>(buf);
    ostringstream hx{};

    for (int i = 0; i != len; ++i)
        hx << hex << setfill('0') << setw(2) << (unsigned int)cbuf[i];

    return hx.str();
}

// Flush buffer and deliver to application (i.e. hash and store)
void flush(char* path){
    if(LOGGING) printf("flush\n");
    flush_cnt += 1;
    for(int i=0; i<MAX_SEG_BUF_SIZE; i++) is_ack[i] = false;

    int cnt;
    if((base-1)%MAX_SEG_BUF_SIZE == 0) cnt = MAX_SEG_BUF_SIZE;
    else cnt = (base-1)%MAX_SEG_BUF_SIZE;

    int fd = open(path, O_CREAT | O_WRONLY | O_APPEND, 0777);
    for(int i=0; i<cnt; i++){
        n_bytes += seg_buffer[i].head.length;
        EVP_DigestUpdate(sha256, seg_buffer[i].data, seg_buffer[i].head.length);
        write(fd, seg_buffer[i].data, seg_buffer[i].head.length);
    }
    close(fd);

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX *tmp_sha256 = EVP_MD_CTX_new();
    EVP_MD_CTX_copy_ex(tmp_sha256, sha256);
    EVP_DigestFinal_ex(tmp_sha256, hash, &hash_len);
    EVP_MD_CTX_free(tmp_sha256);

    if(LOGGING) printf("sha256\t%d\t%s\n", n_bytes, hexDigest(hash, hash_len).c_str());
}

// True if every packet (i.e. packet before AND INCLUDING fin) is received.
// This actually should happen when you receive FIN, no matter what.
bool isAllReceived(int fin){
    return fin == 1;
}

// Indicate that this connection is finished
void endReceive(){
    // Output finsha
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;
    EVP_MD_CTX *tmp_sha256 = EVP_MD_CTX_new();
    EVP_MD_CTX_copy_ex(tmp_sha256, sha256);
    EVP_DigestFinal_ex(tmp_sha256, hash, &hash_len);
    EVP_MD_CTX_free(tmp_sha256);

    if(LOGGING) printf("finsha\t%s\n", hexDigest(hash, hash_len).c_str());
}

// True if the buffer is full else False
bool isBufferFull(){
    for(int i=0; i<MAX_SEG_BUF_SIZE; i++){
        if(is_ack[i] == false) return false;
    }
    return true;
}

// True if the packet is corrupted else False
bool isCorrupt(segment& seg){
    unsigned int checksum = crc32(0L, (const Bytef *)seg.data, seg.head.length);
    if(DEBUG) printf("[SYS] Checksum:%u, should be %u\n", checksum, seg.head.checksum);
    return checksum != seg.head.checksum;
}

// Send a SACK packet
void sendSACK(int ack_seq_num, int sack_seq_num, bool is_fin){
    segment sgmt{};
    sgmt.head.length = 0;
    sgmt.head.ackNumber = ack_seq_num;
    sgmt.head.sackNumber = sack_seq_num;
    sgmt.head.fin = (is_fin ? 1 : 0);
    sgmt.head.syn = 0;
    sgmt.head.ack = 1;

    sendto(sock_fd, &sgmt, sizeof(sgmt), 0, (struct sockaddr *)&recv_addr, sizeof(sockaddr));
    if(LOGGING){
        if(is_fin==false) printf("send\tack\t#%d,\tsack\t#%d\n", ack_seq_num, sack_seq_num);
        else printf("send\tfinack\n");
    }
}


// Mark and put segment with sequence number seq_num in buffer
// (only if it is in current buffer range, if it is over buffer range then 
// you should've dropped this packet.)
void markSACK(segment& seg){
    if(seg.head.seqNumber < (MAX_SEG_BUF_SIZE * flush_cnt + 1)){
        // under buffer range
        return;
    }
    int idx = (seg.head.seqNumber - 1) % MAX_SEG_BUF_SIZE;
    is_ack[idx] = true;
    if(DEBUG) printf("[SYS] Mark idx:%d sacked. (#%d)\n", idx, seg.head.seqNumber);
    seg_buffer[idx].head.length = seg.head.length;
    seg_buffer[idx].head.seqNumber = seg.head.seqNumber;
    seg_buffer[idx].head.fin = seg.head.fin;
    seg_buffer[idx].head.syn = seg.head.syn;
    seg_buffer[idx].head.ack = seg.head.ack;
    seg_buffer[idx].head.checksum = seg.head.checksum;
    bzero(seg_buffer[idx].data, sizeof(char) * MAX_SEG_SIZE);
    memcpy(seg_buffer[idx].data, seg.data, seg.head.length);
}


// Update base and buffer s.t. base is the first unsacked packet
void updateBase(){
    int idx;
    for(idx=(base-1)%MAX_SEG_BUF_SIZE; idx<MAX_SEG_BUF_SIZE; idx++){
        if(is_ack[idx] == false) break;
    }
    base = (idx+1) + MAX_SEG_BUF_SIZE * flush_cnt;
    if(DEBUG) printf("[SYS] Base becomes %d\n", base);
}

// True if the sequence number is above buffer range
// e.g. if the buffer stores sequence number in range [1, 257) and receives
// a segment with seqNumber 257 (or above 257), return True
bool isOverBuffer(int seq_num){
    if(seq_num > (MAX_SEG_BUF_SIZE * (flush_cnt + 1))){
        return true;
    }
    return false;
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


// ./receiver <recv_ip> <recv_port> <agent_ip> <agent_port> <dst_filepath>
int main(int argc, char *argv[]) {
    // parse arguments
    if (argc != 6) {
        cerr << "Usage: " << argv[0] << " <recv_ip> <recv_port> <agent_ip> <agent_port> <dst_filepath>" << endl;
        exit(1);
    }

    int recv_port, agent_port;
    char recv_ip[50], agent_ip[50];

    // read argument
    setIP(recv_ip, argv[1]);
    sscanf(argv[2], "%d", &recv_port);

    setIP(agent_ip, argv[3]);
    sscanf(argv[4], "%d", &agent_port);

    char *filepath = argv[5];
    
    // clear file
    open(filepath, O_CREAT | O_WRONLY | O_TRUNC, 0777);

    // make socket related stuff
    sock_fd = socket(PF_INET, SOCK_DGRAM, 0);

    recv_addr.sin_family = AF_INET;
    recv_addr.sin_port = htons(agent_port);
    recv_addr.sin_addr.s_addr = inet_addr(agent_ip);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(recv_port);
    addr.sin_addr.s_addr = inet_addr(recv_ip);
    memset(addr.sin_zero, '\0', sizeof(addr.sin_zero));    
    bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr));


    // Init
    sha256 = EVP_MD_CTX_new();
    EVP_DigestInit_ex(sha256, EVP_sha256(), NULL);

    while(1){
        // receive a segment! (do the logging on your own)
        socklen_t recv_addr_sz;
        segment recv_sgmt{};
        recvfrom(sock_fd, &recv_sgmt, sizeof(recv_sgmt), 0, (struct sockaddr *)&recv_addr, &recv_addr_sz);


        if(isCorrupt(recv_sgmt)){
            // Drop, sack = cum_ack
            if(LOGGING) printf("drop\tdata\t#%d\t(corrupted)\n", recv_sgmt.head.seqNumber);
            sendSACK(base-1, base-1, false);
        }
        else if(recv_sgmt.head.seqNumber == base){
            // In-order
            if(LOGGING){
                if(recv_sgmt.head.fin) printf("recv\tfin\n");
                else printf("recv\tdata\t#%d\t(in order)\n", recv_sgmt.head.seqNumber);
            }

            markSACK(recv_sgmt);
            updateBase();
            sendSACK(base-1, recv_sgmt.head.seqNumber, recv_sgmt.head.fin);
            if(isAllReceived(recv_sgmt.head.fin)){
                flush(filepath);
                endReceive();
                break;
            }
            else if(isBufferFull()){
                flush(filepath);
            }
        }
        else{
            // Out-of-Order
            if(isOverBuffer(recv_sgmt.head.seqNumber)){
                // Drop, sack = cum_ack
                if(LOGGING) printf("drop\tdata\t#%d\t(buffer overflow)\n", recv_sgmt.head.seqNumber);
                sendSACK(base-1, base-1, false);
            }
            else{
                if(LOGGING) printf("recv\tdata\t#%d\t(out of order, sack-ed)\n", recv_sgmt.head.seqNumber);
                markSACK(recv_sgmt);
                sendSACK(base-1, recv_sgmt.head.seqNumber, recv_sgmt.head.fin);
            }
        }
    }

    EVP_MD_CTX_free(sha256);
    return 0;
}