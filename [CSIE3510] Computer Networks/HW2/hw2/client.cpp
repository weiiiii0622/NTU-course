#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h> 
#include<sys/stat.h>
#include<sys/ioctl.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<net/if.h>
#include<unistd.h> 
#include<netdb.h>
#include<dirent.h>
#include<fcntl.h>

#include"HTTP_header.h"
#include"./utils/base64.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <cctype>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

#define DEBUG 0
#define LIMIT 1000

#define TEXT_T  0
#define FILE_T  1
#define VIDEO_T 2

#define BUFSIZE 1024*2
#define SEND_BUFSIZE 1024*1024
#define MAX_CLIENTS 200
#define MAX_AUTH_LENGTH 50
#define ERR_EXIT(a){ perror(a); exit(1); }

void printCharString(const char* str, ssize_t len, int mode, std::string msg){
    if(!DEBUG) return;
    printf("************* %s len: %ld ************\n", msg.c_str(), len);
    if(len > 0){
        for(int i=0; i<std::min(LIMIT,(int)len); i++){
            if(mode && str[i]>=0 && str[i]<256)  printf("%c", str[i]);
            else printf("0x%x ", str[i]);
        }
        if(len > LIMIT) printf("...");
        if(len > LIMIT || str[len-1]!='\n') printf("\n");
    }
    printf("**************************************\n");
}

std::string percentEncode(std::string str){
    std::ostringstream encoded;
    encoded.fill('0');
    encoded << std::hex << std::uppercase;
    if(DEBUG) printf("[SYS] Encoding %s ...\n", str.c_str());
    for(char c : str){
        if(std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') encoded << c;
        else{
            encoded << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
        }
    }
    return encoded.str();
}

std::string percentDecode(std::string str){
    // Credit: ChatGPT :)
    std::ostringstream decoded;
    std::istringstream inputStream(str);
    int c;
    while(true){
        c = inputStream.get();
        if(c == EOF) break;
        if(c == '%'){
            int digit1 = inputStream.get();
            int digit2 = inputStream.get();
            if(std::isxdigit(digit1) && std::isxdigit(digit2)){
                std::istringstream hexStream(std::string(1, digit1) + std::string(1, digit2));
                int decodedChar;
                hexStream >> std::hex >> decodedChar;
                decoded << static_cast<char>(decodedChar);
            } 
            else{
                decoded << '%' << static_cast<char>(digit1) << static_cast<char>(digit2);
            }
        } 
        else{
            decoded << static_cast<char>(c);
        }
    }
    return decoded.str();
}

std::string getFileFolder(const std::string& file_name, int op) {
    size_t pos = file_name.find_last_of('/');
    if(pos != std::string::npos && pos != file_name.size() - 1){
        if(op == 0) return file_name.substr(0, pos);
        if(op == 1 || op == 2) return file_name.substr(pos+1);
    }
    if(op == 2) return file_name;
    return "";
}

long long getFileSize(std::string filename){
    FILE* file = fopen(filename.c_str(), "r");
    fseek(file, 0, SEEK_END);
    long long sz = ftell(file);
    return sz;
}

int parseAuth(std::string& auth, std::string& username, std::string& password){
    int count = 0;
    for(char c : auth){
        if(c == ':') count++;
    }
    if(count != 1) return -1;
    size_t pos = auth.find_last_of(':');
    if(pos != std::string::npos && pos != auth.size() - 1){
        username = auth.substr(0, pos);
        password = auth.substr(pos + 1);
        return 0;
    }
    return -1;
}

std::string getEncodedAuth(std::string auth){
    size_t len;
    char* encoded_auth = base64_encode((unsigned char*)auth.c_str(), auth.length(), &len);
    std::string ret(encoded_auth, len);
    free(encoded_auth);
    return ret;
}

std::string getFileExtension(const std::string& file_name){
    size_t pos = file_name.find_last_of('.');
    if(pos != std::string::npos && pos != file_name.size() - 1){
        return file_name.substr(pos + 1);
    }
    return "";
}

std::string preProcessHeader(std::string str, bool toLower) {
    // skip Leading Space
    auto start = str.begin();
    int space_cnt = 0;
    while(start != str.end() && std::isspace(*start)) start++, space_cnt++;
    str.erase(0, space_cnt);
    // toLower
    if(toLower) std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

void splitHeaderLine(const std::string& line, std::string& name, std::string& value) {
    std::istringstream stream(line);
    std::getline(stream, name, ':');
    std::getline(stream, value, '\r');
}

// Return the remaining size of body left to read
long long parseResponse(const std::string& msg, HTTPresponse& res){
    std::vector<std::string> header_lines;
    std::string line;
    int header_cnt = 0;

    std::istringstream stream(msg);
    std::string status_code;
    std::getline(stream, res.Version, ' ');
    std::getline(stream, status_code, ' ');
    std::getline(stream, res.Status_Text, '\r');
    stream.ignore(); // Consume the newline character after the version

    res.Status_Code = std::stoi(status_code);

    if(DEBUG) printf("[SYS] Header: Version    , Value: %s\n", res.Version.c_str());
    if(DEBUG) printf("[SYS] Header: Status Code, Value: %d\n", res.Status_Code);
    if(DEBUG) printf("[SYS] Header: Status Text, Value: %s\n", res.Status_Text.c_str());

    while(std::getline(stream, line, '\r')){
        stream.ignore(); // Consume the newline character after each line
        if(line.empty()) break;
        header_lines.push_back(line);
        header_cnt++;
    }

    // Parsing headers
    for(int i=0; i < header_cnt; i++){
        std::string line = header_lines[i];
        std::string name, value;
        // if(DEBUG) printf("[SYS] %s\n", line.c_str());
        splitHeaderLine(line, name, value);
        std::string header_name = preProcessHeader(name, 1);
        std::string header_value = value;
        if(DEBUG) printf("[SYS] Header: %s, Value: %s\n", header_name.c_str(), header_value.c_str());
        if(header_name == "server"){
            res.Server = preProcessHeader(header_value, 0);
        }
        else if(header_name == "allow"){
            res.Allow = preProcessHeader(header_value, 0);
        }
        else if(header_name == "www-authenticate"){
            res.WWW_Authenticate = preProcessHeader(header_value, 0);
        }
        else if(header_name == "content-type"){
            res.Content_Type = preProcessHeader(header_value, 0);
        }
        else if(header_name == "content-length"){
            res.Content_Len = std::stoll(preProcessHeader(header_value, 0));
        }
        else if(header_name == "connection"){
            res.Connection = preProcessHeader(header_value, 1);
        }
    }

    // Malloc Body
    if(DEBUG) printf("[SYS] RES BODY Content Len: %lld\n", res.Content_Len);
    if(res.Content_Len != 0){
        res.Body = (char*) malloc(sizeof(char)*res.Content_Len);
        if(DEBUG) printf("[SYS] FINISH MALLOC BODY\n");
        //req.Body = (unsigned char*) malloc(sizeof(unsigned char)*req.Content_Len);
    }

    // Parse Body if any
    char c;
    long long body_len = 0;
    while(stream.get(c)){
        if(body_len > res.Content_Len) {
            return -1;
        } 
        res.Body[body_len] = c;
        body_len++;
    }
    return body_len;
}

int readServer(int fd, struct HTTPresponse& res, bool& needReconnect){
    std::string buf = "";
    char tempBuffer[1024]={"\0"};
    ssize_t bytesRead = 0;

    // Read from fd
    if(DEBUG) printf("[SYS] Start Reading Request....\n");
    bytesRead += read(fd, tempBuffer, sizeof(tempBuffer)-1);
    if(DEBUG) printf("[SYS] Finished Reading %ld\n", bytesRead);
    if(bytesRead <= 0) return -1;
    buf.append(tempBuffer, bytesRead);

    // Parse HTTP request
    long long body_len = parseResponse(buf, res);

    // Determine reconnect needed
    if(res.Connection == "close"){
        needReconnect = true;
    }

    if(DEBUG) printf("[SYS] BODY First Read: %lld\n", body_len);
    if(body_len == -1){
        // INTERNAL ERROR
        return -1;
    }
    // Print First Received body
    if(DEBUG) printCharString(res.Body, body_len, 1, "RES BODY");

    // Read remaining body
    ssize_t n=0, tmpB=0;
    if(DEBUG) printf("[SYS] Start Reading Reamining Body.... %ld\n", n);
    if(body_len < res.Content_Len){
        while(1){
            n = read(fd, res.Body+tmpB+body_len, 2000);
            tmpB += n;
            if(body_len+tmpB >= res.Content_Len) break;
        }
        if(DEBUG) printf("[SYS] readBytes: %ld\n", tmpB);
        if(tmpB+body_len != res.Content_Len){
            if(DEBUG) printf("[ERROR] readBytes: %lld != Content Length: %lld\n", tmpB+body_len, res.Content_Len);
            return -1;
        }
        // Print 2nd Received body
        if(DEBUG) printCharString(res.Body, res.Content_Len, 1, "RES BODY");
    }
    return bytesRead+body_len+tmpB;
}

int sendServer(int sockfd, struct HTTPrequest& req, std::string path, int body_type, std::string body_path, std::string front, std::string back){
    char buf[BUFSIZE] = "\0";
    long long body_sent_len = 0;

    // Send Header
    if(req.Method == "GET"){
        if(DEBUG) printf("[CLIENT] GET for %s\n", path.c_str());
        snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: %s\r\n\r\n", req.Method.c_str(), req.Path.c_str(), req.Host.c_str(), req.User_Agent.c_str(), req.Connection.c_str());
        if(send(sockfd, buf, strlen(buf), 0) < 0){
            return -1;
        }
    }
    else if(req.Method == "POST"){
        if(DEBUG) printf("[CLIENT] POST for %s\n", path.c_str());
        snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: %s\r\nContent-Type: %s\r\nContent-Length: %lld\r\nAuthorization: %s\r\n\r\n", req.Method.c_str(), req.Path.c_str(), req.Host.c_str(), req.User_Agent.c_str(), req.Connection.c_str(), req.Content_Type.c_str(), req.Content_Len, req.Authorization.c_str());
        if(send(sockfd, buf, strlen(buf), 0) < 0){
            return -1;
        }
    }
    else{
        // ERROR
        if(DEBUG) printf("[CLIENT ERROR] INVALID METHOD: %s Path:%s\n", req.Method.c_str(), path.c_str());
    }

    // Send Body if any
    ssize_t n;
    if(req.Content_Len > 0){
        if(body_type == TEXT_T){
            // Send directly
            if((n = send(sockfd, req.Body, req.Content_Len, 0)) < 0){
                return -1;
            }
            body_sent_len += n;
        }
        else if(body_type == FILE_T){
            // Send front
            if((n = send(sockfd, front.c_str(), front.length(), 0)) < 0){
                if(DEBUG) printf("[SYS ERROR] Failed to send front\n");
                //ERR_EXIT("send()");
                return -1;
            }
            body_sent_len += n;

            // Open file from path and send sequentially
            FILE* file = fopen(body_path.c_str(), "r");
            char buffer[SEND_BUFSIZE];
            
            while((n = fread(buffer, sizeof(char), SEND_BUFSIZE-1, file)) > 0){
                if(DEBUG) printf("[SYS] Read %ld.\n", n);
                if((n = send(sockfd, buffer, n, 0)) < 0){
                    return -1;
                }
                if(DEBUG) printf("[SYS] Send %ld.\n", n);
                body_sent_len += n;
            }
            fclose(file);

            // Send back
            if((n = send(sockfd, back.c_str(), back.length(), 0)) < 0){
                if(DEBUG) printf("[SYS ERROR] Failed to send back\n");
                return -1;
            }
            body_sent_len += n;
        }
    }


    //if(DEBUG) printf("[SYS] buf:\n%s", buf);
    //if(DEBUG) printf("[SYS] Content Body:\n%s\n", res.Body);
    if(DEBUG) printf("[SYS] buf len: %ld\n", strlen(buf));
    if(DEBUG) printf("[SYS] Content Length: %lld\n", req.Content_Len);
    if(DEBUG) printf("[SYS] SENT BODY Length: %lld\n", body_sent_len);
    if(DEBUG) printf("[SYS] Sent total %lld.\n", strlen(buf)+body_sent_len);
    if(body_type==TEXT_T && req.Content_Len != 0) free(req.Body);
    //free(msg);
    return 0;
}

int main(int argc , char *argv[]){

    if (argc != 3 && argc != 4) {
        fprintf(stderr, "Usage: %s [host] [port] [username:password]\n", argv[0]);
        exit(1);
    }

    int PORT = atoi(argv[2]);
    std::string auth;
    if(argc == 3){
        auth = "";
    }
    else{
        std::string tmp(argv[3]);
        auth = tmp;
    }
    std::string username, password;
    int ret;
    if((ret = parseAuth(auth, username, password)) < 0){
        fprintf(stderr, "Usage: %s [host] [port] [username:password]\n", argv[0]);
        exit(1);
    }

    if(DEBUG) printf("[SYS] Auth UserName: %s, PassWord: %s\n", username.c_str(), password.c_str());

    // Determin host is IP or Domain Name
    char *hostIP;
    struct in_addr ipv4_addr;
    struct in6_addr ipv6_addr;
    if(inet_pton(AF_INET, argv[1], &ipv4_addr) != 1 && inet_pton(AF_INET6, argv[1], &ipv6_addr) != 1) {
        if(DEBUG) printf("[SYS] %s is a hostname\n", argv[1]);
        struct hostent *host = gethostbyname(argv[1]);
        struct in_addr addr;
        memcpy(&addr, host->h_addr_list[0], sizeof(struct in_addr));
        hostIP = inet_ntoa(addr);
    }
    else{
        hostIP = argv[1];
    }
 
    // Get HOST header value
    std::string HOST = "";
    HOST += (hostIP);
    HOST += ":";
    HOST += std::to_string(PORT);
    if(DEBUG) printf("[SYS] HOST: %s\n", HOST.c_str());

    int sockfd;
    struct sockaddr_in addr;

    // Get socket file descriptor
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        ERR_EXIT("socket()");
    }

    // Set server address
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(hostIP);
    addr.sin_port = htons(PORT);

    // Connect to the server
    if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        ERR_EXIT("connect()");
    }
    if(DEBUG) printf("[SYS] Successfully connected to %s\n", HOST.c_str());

    mkdir("./files", 0777);

    bool needReconnect = false;

    while(1){

        // Connect to the server
        while(needReconnect){
            if(DEBUG) printf("[SYS] Try to connect to %s\n", HOST.c_str());
            if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                continue;
            }
            if(connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
                continue;
            }
            if(DEBUG) printf("[SYS] Successfully connected to %s\n", HOST.c_str());
            needReconnect = false;
        }

        char command[BUFSIZE] = {'\0'};
        char file_name[BUFSIZE] = {'\0'};

        printf("> "); // prompt
        fgets(command, sizeof(command), stdin);


        char *token = strtok(command, " \n");
        if(token != NULL && strcmp(token, "put") == 0){
            token = strtok(NULL, "\n");
            if(token != NULL) {
                strcpy(file_name, token);
                if(DEBUG) printf("[SYS] PUT file: %s\n", file_name);
                if(fopen(file_name, "r") != NULL) {

                    std::string file_path(file_name);
                    file_path = getFileFolder(file_path, 2);

                    if(DEBUG) printf("[SYS] file_path: %s\n", file_path.c_str());

                    struct HTTPrequest req;
                    req.Method = "POST";
                    req.Path = "/api/file";
                    req.Host = HOST;
                    req.User_Agent = "CN2023Client/1.0";
                    req.Connection = "keep-alive";
                    req.Content_Type = "multipart/form-data; boundary=cc62c5da2dfddd7683cecf9f256f1de9";
                    req.Authorization = "Basic " + getEncodedAuth(auth);

                    std::string front = "--cc62c5da2dfddd7683cecf9f256f1de9\r\nContent-Disposition: form-data; name=\"upfile\"; filename=\"" + file_path + "\"\r\n\r\n";
                    std::string back = "\r\n--cc62c5da2dfddd7683cecf9f256f1de9--\r\n";
                    req.Content_Len = getFileSize(file_path) + front.length() + back.length();

                    int ret = sendServer(sockfd, req, req.Path, FILE_T, file_path, front, back);

                    if(ret == -1){
                        // Error
                        if(DEBUG) printf("[SYS ERROR] Failed to put server when sendServer()\n");
                        fprintf(stderr, "Command failed.\n");
                    }
                    else{
                        // Read Server Response
                        struct HTTPresponse res;
                        ssize_t req_len = readServer(sockfd, res, needReconnect);

                        if(res.Status_Code != 200 || req_len <= 0) {
                            // Error
                            if(DEBUG) printf("[SYS ERROR] Status Code: %d, RES Body: %s\n", res.Status_Code, res.Body);
                            fprintf(stderr, "Command failed.\n");
                        }
                        else{
                            // Success
                            if(DEBUG) printf("[SYS] Successfully upload file %s\n", file_name);
                            fprintf(stdout, "Command succeeded.\n");
                        }
                    }
                }
                else{
                    // File Not Exist
                    fprintf(stderr, "Command failed.\n");
                }
            }
            else{
                // Wrong Usage
                fprintf(stderr, "Usage: put [file]\n");
            }
        }
        else if(token != NULL && strcmp(token, "putv") == 0){
            token = strtok(NULL, "\n");
            if(token != NULL) {
                strcpy(file_name, token);
                if(DEBUG) printf("[SYS] PUT VIDEO: %s\n", file_name);
                if(fopen(file_name, "r") != NULL) {

                    std::string file_path(file_name);
                    file_path = getFileFolder(file_path, 2);

                    if(DEBUG) printf("[SYS] file_path: %s\n", file_path.c_str());

                    struct HTTPrequest req;
                    req.Method = "POST";
                    req.Path = "/api/video";
                    req.Host = HOST;
                    req.User_Agent = "CN2023Client/1.0";
                    req.Connection = "keep-alive";
                    req.Content_Type = "multipart/form-data; boundary=cc62c5da2dfddd7683cecf9f256f1de9";
                    req.Authorization = "Basic " + getEncodedAuth(auth);

                    std::string front = "--cc62c5da2dfddd7683cecf9f256f1de9\r\nContent-Disposition: form-data; name=\"upfile\"; filename=\"" + file_path + "\"\r\n\r\n";
                    std::string back = "\r\n--cc62c5da2dfddd7683cecf9f256f1de9--\r\n";
                    req.Content_Len = getFileSize(file_path) + front.length() + back.length();

                    int ret = sendServer(sockfd, req, req.Path, FILE_T, file_path, front, back);


                    if(ret == -1){
                        // Error
                        if(DEBUG) printf("[SYS ERROR] Failed to putv server when sendServer()\n");
                        fprintf(stderr, "Command failed.\n");
                    }
                    else{
                        // Read Server Response
                        struct HTTPresponse res;
                        ssize_t req_len = readServer(sockfd, res, needReconnect);

                        if(res.Status_Code != 200 || req_len <= 0) {
                            // Error
                            if(DEBUG) printf("[SYS ERROR] Status Code: %d, RES Body: %s\n", res.Status_Code, res.Body);
                            fprintf(stderr, "Command failed.\n");
                        }
                        else{
                            // Success
                            if(DEBUG) printf("[SYS] Successfully upload video %s\n", file_name);
                            fprintf(stdout, "Command succeeded.\n");
                        }
                    }
                }
                else{
                    // File Not Exist
                    fprintf(stderr, "Command failed.\n");
                }
            }
            else{
                // Wrong Usage
                fprintf(stderr, "Usage: putv [file]\n");
            }
        }
        else if(token != NULL && strcmp(token, "get") == 0){
            token = strtok(NULL, "\n");
            if(DEBUG) printf("[SYS] GET FILE: %s\n", file_name);
            if(token != NULL) {
                strcpy(file_name, token);
                
                // Send GET request for file
                std::string file_name_str(file_name);

                struct HTTPrequest req;
                req.Method = "GET";
                req.Path = "/api/file/" + percentEncode(file_name_str);
                req.Host = HOST;
                req.User_Agent = "CN2023Client/1.0";
                req.Connection = "keep-alive";
                
                int ret = sendServer(sockfd, req, req.Path, TEXT_T, "", "", "");


                if(ret == -1){
                    // Error
                    if(DEBUG) printf("[SYS ERROR] Failed to get server when sendServer()\n");
                    fprintf(stderr, "Command failed.\n");
                }
                else{
                    // Read Server Response
                    struct HTTPresponse res;
                    ssize_t req_len = readServer(sockfd, res, needReconnect);

                    if(res.Status_Code != 200 || req_len <= 0) {
                        // Error
                        if(DEBUG) printf("[SYS ERROR] Status Code: %d, RES Body: %s\n", res.Status_Code, res.Body);
                        fprintf(stderr, "Command failed.\n");
                    }
                    else{
                        // Success Received File
                        if(DEBUG) printCharString(res.Body, res.Content_Len, 1, "RES BODY");

                        // Create or truncate the file and write
                        int fd;
                        int result = 0;
                        std::string file_path = "./files/" + getFileFolder(file_name_str, 2);

                        if((fd = open(file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1){
                            if(DEBUG) printf("[SYS ERROR] ERROR CREATING FILE\n");
                            result = -1;
                        }
                        else{
                            size_t bytesWritten = write(fd, res.Body, static_cast<size_t>(res.Content_Len));
                            if(bytesWritten != static_cast<size_t>(res.Content_Len)){
                                if(DEBUG) printf("[SYS ERROR] ERROR WRITING FILE/VIDEO\n");
                                result = -1;
                            }
                        }

                        if(result == 0){
                            // Success
                            if(DEBUG) printf("[SYS] Successfully download file %s\n", file_name);
                            fprintf(stdout, "Command succeeded.\n");
                        }
                        else{
                            // ERROR
                            fprintf(stderr, "Command failed.\n");
                        }
                        close(fd);
                    }
                }
            }
            else{
                fprintf(stderr, "Usage: get [file]\n");
            }
        }
        else if(token != NULL && strcmp(token, "quit") == 0){
            fprintf(stdout, "Bye.\n");
            // Send close HTTPrequest
            break;
        }
        else{
            // Unknown Command
            fprintf(stderr, "Command Not Found.\n");
        }
    }
    close(sockfd);

    return 0;
}