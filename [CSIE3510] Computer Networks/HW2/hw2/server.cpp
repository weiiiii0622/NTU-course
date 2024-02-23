#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<net/if.h>
#include<unistd.h> 
#include<poll.h>
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
#include <filesystem>


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

std::string ROUTE[] = {"/", "/file/upload/", "/video/upload/", "/file/", "/video/", "/api/file/", "/api/video/"};

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

bool checkValidFilename(std::string file_name, int type){
    std::string file_path;
    std::filesystem::path canonical_path;
    std::filesystem::path valid_path;

    if(file_name[0] == '/') file_path = file_name;                          // if begin with "/" its an absoulte path
    else{
        if(type == FILE_T) file_path = "./web/files/" + file_name;          // else is relative path
        else if(type == VIDEO_T) file_path = "./web/videos/" + file_name; 
    }

    try{
        canonical_path = std::filesystem::canonical(file_path);
        if(type == FILE_T) valid_path = std::filesystem::canonical("./web/files");
        else if(type == VIDEO_T) valid_path = std::filesystem::canonical("./web/videos");

        if(DEBUG) printf("[SYS] Given Path : %s\n", canonical_path.c_str());
        if(DEBUG) printf("[SYS] Given Path parent: %s\n", canonical_path.parent_path().c_str());
        if(DEBUG) printf("[SYS] Valid Path : %s\n", valid_path.c_str());
        if(DEBUG) printf("[SYS] Valid Path parent: %s\n", valid_path.parent_path().c_str());
    } catch(const std::filesystem::filesystem_error& e){
        return false;
    }

    if(file_name != "") return canonical_path.parent_path() == valid_path;
    else                return canonical_path == valid_path;
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

std::string getFileName(const std::string& file_name){
    size_t pos = file_name.find_last_of('.');
    if(pos != std::string::npos && pos != file_name.size() - 1){
        return file_name.substr(0, pos);
    }
    return "";
}

std::string getFileExtension(const std::string& file_name){
    size_t pos = file_name.find_last_of('.');
    if(pos != std::string::npos && pos != file_name.size() - 1){
        return file_name.substr(pos + 1);
    }
    return "";
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

int endpointExist(const std::string& path){
    for(int i=0; i<7; i++){
        if(path == ROUTE[i]){
            return 1;   // Endpoint Found
        }
    }
    return 0;
}

long long getFileSize(std::string filename){
    FILE* file = fopen(filename.c_str(), "r");
    fseek(file, 0, SEEK_END);
    long long sz = ftell(file);
    return sz;
}

char* readFileContent_char(std::string filename, long long& sz){
    
    FILE* file = fopen(filename.c_str(), "r");

    // Get file size
    fseek(file, 0, SEEK_END);
    sz = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file to buffer
    char* buffer = (char*) malloc(sizeof(char)*(sz+1));
    fread(buffer, sizeof(char), sz, file);
    buffer[sz] = '\0';
    fclose(file);

    return buffer;
}

std::string readFileContent_str(std::string filename, long long& sz){
    
    FILE* file = fopen(filename.c_str(), "r");

    // Get file size
    fseek(file, 0, SEEK_END);
    sz = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read file to buffer
    char* buffer = (char*) malloc(sizeof(char)*(sz+1));
    fread(buffer, sizeof(char), sz, file);
    buffer[sz] = '\0';
    fclose(file);

    std::string content(buffer);
    free(buffer);
    return content;
}

std::vector<std::string> getFileFromFolder(const std::string& folderPath, int type){
    std::vector<std::string> v;
    DIR* dir = opendir(folderPath.c_str());
    dirent* entry;
    while((entry = readdir(dir)) != nullptr){
        if(type == FILE_T && entry->d_type == DT_REG){
            v.push_back(entry->d_name);
            // if(DEBUG) printf("\t%s\n", entry->d_name);
        }
        else if(type == VIDEO_T && entry->d_type == DT_DIR){
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) v.push_back(entry->d_name);
            // if(DEBUG) printf("\t%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return v;
}

std::string appendFile(const std::string& body, const std::string& tag, int type, const std::string text){
    // Get Available Files
    std::vector<std::string> v;
    if(type == FILE_T) v = getFileFromFolder("./web/files", FILE_T);
    else if(type == VIDEO_T) v = getFileFromFolder("./web/videos", VIDEO_T); 
    if(type!=TEXT_T && DEBUG) printf("[SERVER] Current Files: \n");
    for(auto &f : v){
        if(DEBUG) printf("\t%s\n", f.c_str());
    }

    // Replace Pseudo Tag
    size_t pos = body.find(tag);
    std::string str1 = body.substr(0, pos);
    std::string str2 = body.substr(pos + tag.length());
    if(type == FILE_T){
        for(auto &f : v){
            str1 += ("<tr><td><a href=\"/api/file/" + percentEncode(f) +"\">" + f + "</a></td></tr>");
        }
    }
    else if(type == VIDEO_T){
        for(auto &f : v){
            str1 += ("<tr><td><a href=\"/video/" + percentEncode(f) + "\">" + f + "</a></td></tr>");
        }
    }
    else if(type == TEXT_T){
        str1 += text;
    }
    str1 += str2;
    return str1;
}

char* str_to_char(std::string str){
    char* buf = (char*) malloc(sizeof(char)*(str.length()+1));
    std::copy(str.begin(), str.end(), buf);
    buf[str.size()] = '\0';
    return buf;
}

std::vector<std::string> getSecret(){
    std::vector<std::string> v;
    FILE *file = fopen("./secret", "r");

    char buffer[MAX_AUTH_LENGTH] = "";
    while(fgets(buffer, sizeof(buffer), file) != NULL){
        while(buffer[strlen(buffer)-1] == '\r' || buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1] = '\0';
        }
        std::string secret(buffer, strlen(buffer));
        v.push_back(secret);
        for(int i=0; i<MAX_AUTH_LENGTH; i++) buffer[i] = '\0';
    }
    return v;
}

bool checkAuth(std::string auth){
    size_t len;
    unsigned char* decoded_auth = base64_decode(auth.c_str(), auth.length(),&len);
    if(DEBUG) printf("[SYS] Given Auth is %s -> %s\n", auth.c_str(), decoded_auth);
    // for(int i=0; i<len; i++){
    //     if(DEBUG) printf("%u", decoded_auth[i]);
    // }
    // if(DEBUG) printf("\n");
    //std::string d_auth((char*) decoded_auth);
    std::vector<std::string> secrets = getSecret();
    for(auto &secret: secrets){
        if(DEBUG) printf("[SYS] Secret is %s\n", secret.c_str());
        char* s_tmp = base64_encode((unsigned char*)secret.c_str(), secret.length(), &len);
        std::string s(s_tmp, len);
        free(s_tmp);
        if(DEBUG) std::cout << s << ' ' << auth << '\n';
        if(s == auth){
            return true;
        }
    }
    return false;
}

// ./NTU Marathon (2023).mp4" "./web/videos/NTU Marathon (2023)/dash.mpd"
int convertVideoToDash(std::string file_name, std::string file_path){
    pid_t pid = fork();
    if(pid < 0){
        if(DEBUG) printf("[SYS] 1st Fork failed\n");
        return -1;
    }
    else if(pid == 0){
        pid_t gpid = fork();
        if(gpid < 0){
            if(DEBUG) printf("[SYS] 2nd Fork failed\n");
            return -1;
        } 
        else if(gpid == 0){
            char* args[] = {"bash", "./convert.sh", (char*) file_name.c_str(), (char*) file_path.c_str(), NULL};
            execvp("bash", args);
            if(DEBUG) printf("[SYS] EXECVP failed\n");
            exit(EXIT_FAILURE);
        } 
        else{
            exit(EXIT_SUCCESS);
        }
    } 
    else{
        // Parent process
        // printf("Child process started with PID: %d\n", pid);
        waitpid(pid, NULL, 0);
    }
    return 0;
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

std::string preProcessAuthenication(std::string auth){
    std::istringstream stream(auth);
    std::string str;
    std::getline(stream, str, ' ');
    std::getline(stream, str, ' ');
    return str;
}

std::string parseBoundary(std::string str){
    std::string flag = "boundary=";
    size_t pos = str.find(flag);
    pos += flag.length();
    size_t endPos = str.find(';', pos);
    std::string boundary = str.substr(pos, endPos - pos);
    boundary = "--"+boundary;
    return boundary; 
}

char* findBoundary(char* body, long long body_len, const char* boundary, size_t boundary_len){
    char* current_pos = body;
    //if(DEBUG) printf("body_len:%lld / eval: %ld\n", body_len, current_pos-body);

    while(current_pos < body + body_len){
        //if(DEBUG) printf("body_len:%lld / eval: %ld\n", body_len, current_pos-body);
        //if(DEBUG) printf("body left:%ld boundary_len:%ld\n", body+body_len-current_pos, boundary_len);
        char* match = (char*) memmem(current_pos, body+body_len-current_pos, boundary, boundary_len);
        if(match == NULL) {
            return NULL;
        }
        // Ensure the match is at the beginning of a line
        if(match != body && *(match - 1) != '\n'){
            current_pos = match + 1;
            continue;
        }
        return match;
    }
    return NULL;
}

int extractContent(char* body, long long body_len, std::string b, int type, std::string name, std::string& file){
    std::string b_end = b + "--";
    const char* boundary = b.c_str();
    const char* end_boundary = b_end.c_str();
    size_t boundary_len = strlen(boundary);
    size_t end_boundary_len = strlen(end_boundary);
    
    char* current_position = body;
    char* end_position = findBoundary(current_position, body_len, end_boundary, end_boundary_len);
    char* current_end;
    char* help_start;
    char* help_end;

    int fd;
    int ret = 0;

    while(1){
        // Boundary
        current_position = findBoundary(current_position, body_len-(int)(current_position-body), boundary, boundary_len);
        // Boundary Not Found / End but target not found 
        if(current_position == NULL || current_position == end_position){
            return -1;
        }

        current_end = (char*) memmem(current_position, body_len-(int)(current_position-body), "\r\n", 2);
        if(DEBUG){
            printf("[SYS] Boundary: \n");
            for(char* ptr = current_position; ptr < current_end; ++ptr) {
                printf("%c", *ptr);
            }
            printf("\n");
        }

        // Content-Disposition
        current_position = current_end + 2;
        current_end = (char*) memmem(current_position, body_len-(int)(current_position-body), "\r\n", 2);
        if(DEBUG){
            printf("[SYS] Content-Disposition:\n");
            for(char* ptr = current_position; ptr < current_end; ++ptr) {
                printf("%c", *ptr);
            }
            printf("\n");
        }

        // Check name="upfile"
        help_start = (char*) memmem(current_position, body_len-(int)(current_position-body), "name=", 5);
        help_end = (char*) memmem(help_start, body_len-(int)(help_start-body), ";", 1);
        if(help_end == NULL) help_end = current_end;
        std::string tmp(help_start+6, (int)(help_end-help_start-6-1));
        if(DEBUG) printf("[SYS] Current Form Name: %s\n", tmp.c_str());
        if(tmp != name){
            continue;
        }
        if(DEBUG) printf("[SYS] Form Name: %s matched, upload file started...\n", tmp.c_str());

        // Get file name
        help_start = (char*) memmem(current_position, body_len-(int)(current_position-body), "filename=", 9);
        help_end = (char*) memmem(help_start, body_len-(int)(help_start-body), ";", 1);
        if(help_end >= current_end || help_end == NULL) help_end = current_end;
        std::string file_name(help_start+9, (int)(help_end-help_start-9));
        if(DEBUG) printf("[SYS] 1st Current File Name: %s\n", file_name.c_str());
        if(file_name[0] == '"' && file_name[file_name.length()-1] == '"') file_name = file_name.substr(1, file_name.length()-2);
        if(DEBUG) printf("[SYS] 2nd Current File Name: %s\n", file_name.c_str());
        file = file_name;

        // Skip to file body
        current_position = current_end;
        current_position = (char*) memmem(current_position, body_len-(int)(current_position-body), "\r\n\r\n", 4);
        current_position += 4;

        // Cannot use /r/n to determine the end of content
        current_end = findBoundary(current_position, body_len-(int)(current_position-body), boundary, boundary_len) - 2;
        if(DEBUG){
            printf("[SYS] Content-Body with len: %lld:\n", (long long)(current_end - current_position));
            printCharString(current_position, (current_end - current_position), 1, "CONTENT BODY");
            printf("\n");
        }

        // Create or truncate the file and write
        if(type == FILE_T) file_name = "./web/files/"+file_name;
        else file_name = "./web/tmp/"+file_name;

        if((fd = open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1){
            if(DEBUG) printf("[SYS ERROR] ERROR CREATING FILE/VIDEO\n");
            return -1;
        }
        size_t bytesWritten = write(fd, current_position, static_cast<size_t>(current_end - current_position));
        if(bytesWritten != (size_t)(current_end - current_position)){
            if(DEBUG) printf("[SYS ERROR] ERROR WRITING FILE/VIDEO\n");
            close(fd);
            return -1;
        }

        // Success
        if(DEBUG) printf("[SYS] Successfully upload file.\n");
        close(fd);
        break;
    }
    return ret;
}

// Return the remaining size of body left to read
long long parseReqeust(const std::string& msg, HTTPrequest& req){
    std::vector<std::string> header_lines;
    std::string line;
    int header_cnt = 0;

    std::istringstream stream(msg);
    std::getline(stream, req.Method, ' ');
    std::getline(stream, req.Path, ' ');
    std::getline(stream, req.Version, '\r');
    stream.ignore(); // Consume the newline character after the version

    if(DEBUG) printf("[SYS] Header: Method , Value: %s\n", req.Method.c_str());
    if(DEBUG) printf("[SYS] Header: Path   , Value: %s\n", req.Path.c_str());
    if(DEBUG) printf("[SYS] Header: Version, Value: %s\n", req.Version.c_str());

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
        if(header_name == "connection"){
            req.Connection = preProcessHeader(header_value, 1);
        }
        else if(header_name == "user-agent"){
            req.User_Agent = preProcessHeader(header_value, 0);
        }
        else if(header_name == "host"){
            req.Host = preProcessHeader(header_value, 0);
        }
        else if(header_name == "content-type"){
            req.Content_Type = preProcessHeader(header_value, 0);
            // Check if need to parse boundary
            if(req.Content_Type.substr(0, 19) == "multipart/form-data"){
                req.Boundary = parseBoundary(req.Content_Type.substr(20));
                if(DEBUG) printf("[SYS] Located boundary: %s\n", req.Boundary.c_str());
            }
        }
        else if(header_name == "content-length"){
            req.Content_Len = std::stoll(preProcessHeader(header_value, 0));
        }
        else if(header_name == "authorization"){
            req.Authorization = preProcessAuthenication(preProcessHeader(header_value, 0));
        }
    }

    // Malloc Body
    if(DEBUG) printf("[SYS] REQ BODY Content Len: %lld\n", req.Content_Len);
    if(req.Content_Len != 0){
        req.Body = (char*) malloc(sizeof(char)*req.Content_Len);
        if(DEBUG) printf("[SYS] FINISH MALLOC BODY\n");
        //req.Body = (unsigned char*) malloc(sizeof(unsigned char)*req.Content_Len);
    }

    // Parse Body if any
    char c;
    long long body_len = 0;
    while(stream.get(c)){
        if(body_len > req.Content_Len) {
            return -1;
        } 
        req.Body[body_len] = c;
        body_len++;
    }
    return body_len;
}

int readClient(int fd, struct HTTPrequest& req){
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
    long long body_len = parseReqeust(buf, req);
    if(DEBUG) printf("[SYS] BODY First Read: %lld\n", body_len);
    if(body_len == -1){
        // INTERNAL ERROR
        return -1;
    }
    // Print First Received body
    if(DEBUG) printCharString(req.Body, body_len, 1, "REQ BODY");

    // Read remaining body
    ssize_t n=0, tmpB=0;
    if(DEBUG) printf("[SYS] Start Reading Reamining Body.... %ld\n", n);
    if(body_len < req.Content_Len){
        while(1){
            n = read(fd, req.Body+tmpB+body_len, 2000);
            tmpB += n;
            if(body_len+tmpB >= req.Content_Len) break;
        }
        if(DEBUG) printf("[SYS] readBytes: %ld\n", tmpB);
        if(tmpB+body_len != req.Content_Len){
            if(DEBUG) printf("[ERROR] readBytes: %lld != Content Length: %lld\n", tmpB+body_len, req.Content_Len);
            return -1;
        }
        // Print 2nd Received body
        if(DEBUG) printCharString(req.Body, req.Content_Len, 1, "REQ BODY");
    }
    return bytesRead+body_len+tmpB;
}

void sendClient(int sockfd, struct HTTPresponse& res, std::string path, int body_type, std::string body_path){
    char buf[BUFSIZE] = "\0";
    long long body_sent_len = 0;
    //char* msg;
    if(path == ""){
        if(DEBUG) printf("[SERVER] Connection Ended\n");
        return;
    }
    // Send Header
    if(res.Status_Code == 200){
        if(DEBUG) printf("[SERVER] 200 OK for %s\n", path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.Content_Type.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            //ERR_EXIT("send()");
        }
    }
    else if(res.Status_Code == 401){
        if(DEBUG) printf("[SERVER] 401 Unauthorized for %s\n", path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nWWW-Authenticate: %s\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.WWW_Authenticate.c_str(), res.Content_Type.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            //ERR_EXIT("send()");
        }
    }
    else if(res.Status_Code == 404){
        if(DEBUG) printf("[SERVER] 404 NOT FOUND for %s\n", path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nContent-Type: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.Content_Type.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            //ERR_EXIT("send()");
        }
    }
    else if(res.Status_Code == 405){
        if(DEBUG) printf("[SERVER] 405 Method Not Allowed for %s\n", path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nAllow: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.Allow.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            //ERR_EXIT("send()");
        }
    }
    else if(res.Status_Code == 500){
        if(DEBUG) printf("[SERVER] 500 SERVER INTERNAL ERROR for %s\n", path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            //ERR_EXIT("send()");
        }
    }
    else{
        // ERROR
        if(DEBUG) printf("[SERVER ERROR] Status Code:%d Path:%s\n", res.Status_Code, path.c_str());
        snprintf(buf, sizeof(char)*(BUFSIZE), "HTTP/1.1 %d %s\r\nServer: %s\r\nContent-Length: %lld\r\n\r\n", res.Status_Code, res.Status_Text.c_str(), res.Server.c_str(), res.Content_Len);
        if(send(sockfd, buf, strlen(buf), MSG_NOSIGNAL) < 0){
            return;
            ERR_EXIT("send()");
        }
    }

    // Send Body if any
    ssize_t n;
    ssize_t send_n;
    if(res.Content_Len > 0){
        if(body_type == TEXT_T){
            // Send directly
            if((send_n = send(sockfd, res.Body, res.Content_Len, 0)) < 0){
                ERR_EXIT("send()");
            }
            body_sent_len += send_n;
        }
        else if(body_type == FILE_T){
            // Open file from path and send sequentially
            FILE* file = fopen(body_path.c_str(), "r");
            char buffer[SEND_BUFSIZE];
            
            int FAIL_COUNT = 0;

            while((n = fread(buffer, sizeof(char), SEND_BUFSIZE-1, file)) > 0){
                if(DEBUG) printf("[SYS] Read %ld.\n", n);
                // if((send_n = send(sockfd, buffer, n, MSG_NOSIGNAL)) < 0){
                //     if(DEBUG) printf("[SYS ERROR] Sending %ld.\n", n);
                //     FAIL_COUNT += 1;
                //     continue;
                // }
                do{
                    send_n = send(sockfd, buffer, n, MSG_NOSIGNAL);
                    FAIL_COUNT += 1;
                }while(send_n == -1 && FAIL_COUNT <= 1);

                if(send_n != -1){
                    if(DEBUG) printf("[SYS] Send %ld.\n", send_n);
                    body_sent_len += send_n;
                }
                else{
                    if(DEBUG) printf("[SYS ERROR] Failed to Send %ld.\n", n);
                    break;
                }
            }
            fclose(file);
        }
    }


    //if(DEBUG) printf("[SYS] buf:\n%s", buf);
    //if(DEBUG) printf("[SYS] Content Body:\n%s\n", res.Body);
    if(DEBUG) printf("[SYS] buf len: %ld\n", strlen(buf));
    if(DEBUG) printf("[SYS] Content Length: %lld\n", res.Content_Len);
    if(DEBUG) printf("[SYS] SENT BODY Length: %lld\n", body_sent_len);
    if(DEBUG) printf("[SYS] Sent total %lld.\n", strlen(buf)+body_sent_len);
    if(body_type==TEXT_T && res.Content_Len != 0) free(res.Body);
    //free(msg);
    return;
}

int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }

    // Create ./web and ./web/files and ./web/videos if not exist
    mkdir("./web", 0777);
    mkdir("./web/files", 0777);
    mkdir("./web/videos", 0777);
    mkdir("./web/tmp", 0777);

    int listenfd, connfd;
    int PORT = atoi(argv[1]);
    struct sockaddr_in server_addr;

    // Get socket file descriptor
    if((listenfd = socket(AF_INET , SOCK_STREAM , 0)) < 0){
        ERR_EXIT("socket()");
    }

    // Set server address information
    bzero(&server_addr, sizeof(server_addr)); // erase the data
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // Bind the server file descriptor to the server address
    if(bind(listenfd, (struct sockaddr *)&server_addr , sizeof(server_addr)) < 0){
        ERR_EXIT("bind()");
    }

    // Listen on the server file descriptor
    if(listen(listenfd , 3) < 0){
        ERR_EXIT("listen()");
    }


    int client_cnt = 0;
    struct pollfd clients[MAX_CLIENTS+1];

    for(int i=0; i<=MAX_CLIENTS; i++){
        clients[i].fd = -1;
    }
    clients[client_cnt].fd = listenfd;  // Add the listening socket to the poll set
    clients[client_cnt].events = POLLIN;
    client_cnt++;

    while(1){
        int res = poll(clients, client_cnt+1, 0);
        if(res <= 0) continue;

        // Check new connections
        if(clients[0].revents & POLLIN){ // Incoming connection on listening socket
            struct sockaddr_in client_addr;
            int client_addr_len = sizeof(client_addr);
            // Accept the client and get client file descriptor
            if((connfd = accept(listenfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addr_len)) < 0){
                ERR_EXIT("accept()");
            }
            if(DEBUG) printf("Accepted %s\n", inet_ntoa(client_addr.sin_addr));

            if(client_cnt < MAX_CLIENTS){
                for(int i=1; i<=MAX_CLIENTS; i++){
                    // Get Empty socket
                    if(clients[i].fd == -1){
                        clients[i].fd = connfd;
                        clients[i].events = POLLIN;
                        client_cnt++;
                        break;
                    }
                }
                if(DEBUG) printf("Currently %d clients.\n", client_cnt-1);
            }
            else{
                ERR_EXIT("Too many clients.");
            }
        }

        // Check client activity
        for(int i=1; i<=MAX_CLIENTS; i++){
            if(clients[i].fd != -1 && clients[i].revents & POLLIN){
                printf("[SYS] Client %d actioning on fd: %d ....\n", i, clients[i].fd);
                // Parse HTTP request from client
                struct HTTPrequest req;
                ssize_t req_len = readClient(clients[i].fd, req);
                //std::string buf = readClient(clients[i].fd);
                //if(buf.length() == 0) continue;
                //if(DEBUG) printf("[SYS] msg size: %ld\n", buf.length());
                //if(DEBUG) printf("[SYS] From Client %d:\n%s\n", i, buf.c_str());
                // if(DEBUG) printf("req: %s, %s, %s, %s, %s\n", req.Method.c_str(), req.Path.c_str(), req.Host.c_str(), req.User_Agent.c_str(), req.Connection.c_str());
                
                // ERROR
                if(req_len <= 0){
                    if(req_len == -1){
                        // INTERNAL SERVER ERROR
                        struct HTTPresponse res;
                        res.Status_Code = 500;
                        res.Status_Text = "Internal Server Error";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    close(clients[i].fd);
                    clients[i].fd = -1;
                    client_cnt -= 1;
                    if(DEBUG) printf("[SYS] CLOSE from %d\n", clients[i].fd);
                    continue;
                }
                
                // Routing
                // Root Page
                if(req.Path == "/"){
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else{
                        //std::string body = readFileContent("./web/index.html");
                        // if(DEBUG) printf("%s\n", body);
                        long long len;
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Body = readFileContent_char("./web/index.html", len);
                        res.Content_Len = len;

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path == "/file" || req.Path == "/file/"){
                    if(DEBUG) printf("[SERVER] REQEST LISTF\n");
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else{
                        long long len;
                        std::string body = readFileContent_str("./web/listf.rhtml", len);
                        body = appendFile(body, "<?FILE_LIST?>", FILE_T, "");
                        // if(DEBUG) printf("%s\n", body);
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Content_Len = body.length();
                        res.Body = str_to_char(body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path == "/upload/file" || req.Path == "/upload/file/"){
                    if(DEBUG) printf("[SERVER] REQEST UPLOADF\n");
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkAuth(req.Authorization) == false){
                        // Invalid Credential
                        std::string body = "Unauthorized\n";

                        struct HTTPresponse res;
                        res.Status_Code = 401;
                        res.Status_Text = "Unauthorized";
                        res.Server = "CN2023Server/1.0";
                        res.WWW_Authenticate = "Basic realm=\"B10902138\"";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        long long len;
                        char* body = readFileContent_char("./web/uploadf.html", len);
                        // if(DEBUG) printf("%s\n", body);
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Content_Len = len;
                        res.Body = body;

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path == "/api/file" || req.Path == "/api/file/"){
                    if(DEBUG) printf("[SERVER] UPLOADING FILE\n");
                    if(req.Method != "POST"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "POST";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkAuth(req.Authorization) == false){
                        // Invalid Credential
                        std::string body = "Unauthorized\n";
                        // std::vector<std::string> s = getSecret();
                        // std::string body = "\nGOT: "+req.Authorization + '\n';
                        // size_t len;
                        // body += "Available secret:\n";
                        // for(auto ss : s){
                        //     char* s_tmp = base64_encode((const unsigned char*)ss.c_str(), ss.length(), &len);
                        //     std::string sss(s_tmp, len);
                        //     free(s_tmp);
                        //     body += ss;
                        //     body += " -> ";
                        //     body += sss;
                        //     body += " len:";
                        //     body += std::to_string(len);
                        //     body += "\n";
                        // }

                        struct HTTPresponse res;
                        res.Status_Code = 401;
                        res.Status_Text = "Unauthorized";
                        res.Server = "CN2023Server/1.0";
                        res.WWW_Authenticate = "Basic realm=\"B10902138\"";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        // Extract Body if any                 
                        int ret;
                        if(req.Content_Len > 0){
                            ret = extractContent(req.Body, req.Content_Len, req.Boundary, FILE_T, "upfile", req.File_Name);
                            free(req.Body);
                        }

                        struct HTTPresponse res;
                        if(ret == -1){
                            res.Status_Code = 500;
                            res.Status_Text = "Internal Server Error";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Len = 0;

                            sendClient(clients[i].fd, res, req.Path, -1, "");
                            continue;
                        }
                        else{
                            std::string body = "OK\n";
                            
                            res.Status_Code = 200;
                            res.Status_Text = "OK";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Type = "text/plain";
                            res.Body = str_to_char(body);
                            res.Content_Len = strlen(res.Body);
                            sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                        }
                    }
                }
                else if(req.Path.substr(0, 10) == "/api/file/"){
                    std::string file_name = percentDecode(req.Path.substr(10));
                    if(DEBUG) printf("[SYS] Request to download file: %s\n", file_name.c_str());
                    std::vector<std::string> v = getFileFromFolder("./web/files", FILE_T);

                    std::string file_last_name = getFileFolder(file_name, 2);

                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkValidFilename(file_name, FILE_T) == false){
                        // Invalid file
                        if(DEBUG) printf("[SERVER] FILE INVALID\n");
                        // Invalid Endpoint
                        std::string body = "Not Found\n";
                        struct HTTPresponse res;
                        res.Status_Code = 404;
                        res.Status_Text = "Not Found";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");

                    }
                    else if((std::find(v.begin(), v.end(), file_last_name)) == v.end()){     // Check if file exist
                        // Not exist
                        if(DEBUG) printf("[SERVER] FILE NOT FOUND\n");
                        // Invalid Endpoint
                        std::string body = "Not Found\n";
                        struct HTTPresponse res;
                        res.Status_Code = 404;
                        res.Status_Text = "Not Found";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        // Exist !
                        struct HTTPresponse res;
                        file_name = "./web/files/"+file_name;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Len = getFileSize(file_name);

                        // Check Content Type
                        std::string file_ext = getFileExtension(file_name);
                        if(file_ext == "html" || file_ext == "rhtml")   res.Content_Type = "text/html";
                        else if(file_ext == "mp4" || file_ext == "mpv") res.Content_Type = "video/mp4";
                        else if(file_ext == "m4s")                      res.Content_Type = "video/iso.segment";
                        else if(file_ext == "m4a")                      res.Content_Type = "audio/mp4";
                        else if(file_ext == "mpd")                      res.Content_Type = "application/dash+xml";
                        else                                            res.Content_Type = "text/plain";

                        sendClient(clients[i].fd, res, req.Path, FILE_T, file_name);
                    }
                }
                else if(req.Path == "/video" || req.Path == "/video/"){
                    if(DEBUG) printf("[SERVER] REQEST LISTV\n");
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else{
                        long long len;
                        std::string body = readFileContent_str("./web/listv.rhtml", len);
                        body = appendFile(body, "<?VIDEO_LIST?>", VIDEO_T, "");
                        // if(DEBUG) printf("%s\n", body);
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Content_Len = body.length();
                        res.Body = str_to_char(body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path.substr(0, 7) == "/video/"){
                    std::string file_name = percentDecode(req.Path.substr(7));
                    if(DEBUG) printf("[SYS] Request to play video: %s\n", file_name.c_str());
                    std::vector<std::string> v = getFileFromFolder("./web/videos", VIDEO_T);

                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if((std::find(v.begin(), v.end(), file_name)) == v.end()){     // Check if video folder exist
                        // Not exist
                        if(DEBUG) printf("[SERVER] VIDEO FOLDER NOT FOUND\n");
                        // Invalid Endpoint
                        std::string body = "Not Found\n";
                        struct HTTPresponse res;
                        res.Status_Code = 404;
                        res.Status_Text = "Not Found";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        long long len;
                        //std::string mpd_path = "\"./web/videos/" + file_name +"/dash.mpd\"";
                        std::string mpd_path = "\"/api/video/"+ percentEncode(file_name) +"/dash.mpd\"";
                        if(DEBUG) printf("[SYS] Encode result: %s ...\n", mpd_path.c_str());
                        std::string body = readFileContent_str("./web/player.rhtml", len);
                        body = appendFile(body, "<?VIDEO_NAME?>", TEXT_T, file_name);
                        body = appendFile(body, "<?MPD_PATH?>", TEXT_T, mpd_path);
                        // if(DEBUG) printf("%s\n", body);
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Content_Len = body.length();
                        res.Body = str_to_char(body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path == "/upload/video" || req.Path == "/upload/video/"){
                    if(DEBUG) printf("[SERVER] REQEST UPLOADV\n");
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkAuth(req.Authorization) == false){
                        // Invalid Credential
                        std::string body = "Unauthorized\n";
                        struct HTTPresponse res;
                        res.Status_Code = 401;
                        res.Status_Text = "Unauthorized";
                        res.Server = "CN2023Server/1.0";
                        res.WWW_Authenticate = "Basic realm=\"B10902138\"";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        long long len;
                        char* body = readFileContent_char("./web/uploadv.html", len);
                        // if(DEBUG) printf("%s\n", body);
                        struct HTTPresponse res;
                        res.Status_Code = 200;
                        res.Status_Text = "OK";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/html";
                        res.Content_Len = len;
                        res.Body = body;

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                }
                else if(req.Path == "/api/video" || req.Path == "/api/video/"){
                    if(DEBUG) printf("[SERVER] UPLOADING VIDEO\n");
                    if(req.Method != "POST"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "POST";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkAuth(req.Authorization) == false){
                        // Invalid Credential
                        std::string body = "Unauthorized\n";
                        struct HTTPresponse res;
                        res.Status_Code = 401;
                        res.Status_Text = "Unauthorized";
                        res.Server = "CN2023Server/1.0";
                        res.WWW_Authenticate = "Basic realm=\"B10902138\"";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        // Extract Body if any                  
                        int ret;
                        if(req.Content_Len > 0){
                            ret = extractContent(req.Body, req.Content_Len, req.Boundary, VIDEO_T, "upfile", req.File_Name);
                            free(req.Body);
                        }

                        // Convert video to dash
                        if(ret != -1){
                            // Check if folder exist
                            std::string folder_path = "./web/videos/" + getFileName(req.File_Name);
                            DIR *dir = opendir(folder_path.c_str());
                            struct dirent *entry;
                            if(dir == NULL){
                                // Create it
                                mkdir(folder_path.c_str(), 0777);
                                if(DEBUG) printf("[SYS] Video folder created at: %s\n", folder_path.c_str());
                            }
                            else{
                                // Delete all files in it
                                while((entry = readdir(dir)) != NULL){
                                    std::string f(entry->d_name);
                                    f = folder_path + "/" + f;

                                    if(entry->d_type == DT_REG){
                                        if(remove(f.c_str()) == -1){
                                            if(DEBUG) printf("[SYS ERROR] ERROR DELETING %s\n", f.c_str());
                                        }
                                        else if(DEBUG) printf("[SYS] %s DELETED\n", f.c_str());
                                    }
                                }
                                closedir(dir);
                            }
                            std::string arg1 = "./web/tmp/" + req.File_Name;
                            std::string arg2 = folder_path + "/dash.mpd";
                            ret = convertVideoToDash(arg1, arg2);
                        }

                        struct HTTPresponse res;
                        if(ret == -1){
                            res.Status_Code = 500;
                            res.Status_Text = "Internal Server Error";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Len = 0;

                            sendClient(clients[i].fd, res, req.Path, -1, "");
                            continue;
                        }
                        else{
                            std::string body = "Video Uploaded\n";
                            
                            res.Status_Code = 200;
                            res.Status_Text = "OK";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Type = "text/plain";
                            res.Body = str_to_char(body);
                            res.Content_Len = strlen(res.Body);
                            sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                        }
                    }
                }
                else if(req.Path.substr(0, 11) == "/api/video/"){
                    std::string file_name = percentDecode(req.Path.substr(11));
                    std::string folder_name = getFileFolder(file_name, 0);
                    std::string file_last_name = getFileFolder(file_name, 1);
                    if(DEBUG) printf("[SYS] Request video file: %s\n", file_name.c_str());
                    if(DEBUG) printf("[SYS] Request video folder: %s\n", folder_name.c_str());
                    if(DEBUG) printf("[SYS] Request video file last name: %s\n", file_last_name.c_str());
                    std::vector<std::string> v = getFileFromFolder("./web/videos", VIDEO_T);

                    //if(DEBUG) printf("[SYS] Request /api/video/ method: %s\n", req.Method.c_str());
                    if(req.Method != "GET"){
                        // Invalid method
                        struct HTTPresponse res;
                        res.Status_Code = 405;
                        res.Status_Text = "Method Not Allowed";
                        res.Server = "CN2023Server/1.0";
                        res.Allow = "GET";
                        res.Content_Len = 0;

                        sendClient(clients[i].fd, res, req.Path, -1, "");
                    }
                    else if(checkValidFilename(folder_name, VIDEO_T) == false){
                        // FilePath Invalid
                        if(DEBUG) printf("[SERVER] VIDEO folder Invalid\n");
                        // Invalid Endpoint
                        std::string body = "Not Found\n";
                        struct HTTPresponse res;
                        res.Status_Code = 404;
                        res.Status_Text = "Not Found";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else if((std::find(v.begin(), v.end(), folder_name)) == v.end()){     // Check if video folder exist
                        // Not exist
                        if(DEBUG) printf("[SERVER] VIDEO folder NOT FOUND\n");
                        // Invalid Endpoint
                        std::string body = "Not Found\n";
                        struct HTTPresponse res;
                        res.Status_Code = 404;
                        res.Status_Text = "Not Found";
                        res.Server = "CN2023Server/1.0";
                        res.Content_Type = "text/plain";
                        res.Body = str_to_char(body);
                        res.Content_Len = strlen(res.Body);

                        sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                    }
                    else{
                        std::string file_path = "./web/videos/" + folder_name;
                        v = getFileFromFolder(file_path, FILE_T);
                        if((std::find(v.begin(), v.end(), file_last_name)) == v.end()){     // Check if video file exist
                            // Not exist
                            if(DEBUG) printf("[SERVER] VIDEO file NOT FOUND\n");
                            // Invalid Endpoint
                            std::string body = "Not Found\n";
                            struct HTTPresponse res;
                            res.Status_Code = 404;
                            res.Status_Text = "Not Found";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Type = "text/plain";
                            res.Body = str_to_char(body);
                            res.Content_Len = strlen(res.Body);

                            sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                        }
                        else{
                            // Exist !
                            if(DEBUG) printf("[SYS] VIDEO file exist\n");

                            struct HTTPresponse res;
                            file_name = "./web/videos/" + file_name;
                            res.Status_Code = 200;
                            res.Status_Text = "OK";
                            res.Server = "CN2023Server/1.0";
                            res.Content_Len = getFileSize(file_name);
                            
                            if(DEBUG) printf("[SYS] VIDEO file size: %lld\n", res.Content_Len);
                            // Check Content Type
                            std::string file_ext = getFileExtension(file_name);
                            if(file_ext == "html" || file_ext == "rhtml")   res.Content_Type = "text/html";
                            else if(file_ext == "mp4" || file_ext == "mpv") res.Content_Type = "video/mp4";
                            else if(file_ext == "m4s")                      res.Content_Type = "video/iso.segment";
                            else if(file_ext == "m4a")                      res.Content_Type = "audio/mp4";
                            else if(file_ext == "mpd")                      res.Content_Type = "application/dash+xml";
                            else                                            res.Content_Type = "text/plain";

                            if(DEBUG) printf("[SYS] Sending video file...\n");
                            sendClient(clients[i].fd, res, req.Path, FILE_T, file_name);
                        }
                    }
                }
                else{
                    if(DEBUG) printf("[SERVER] ENDPOINT NOT FOUND\n");
                    // Invalid Endpoint
                    std::string body = "Not Found\n";
                    struct HTTPresponse res;
                    res.Status_Code = 404;
                    res.Status_Text = "Not Found";
                    res.Server = "CN2023Server/1.0";
                    res.Content_Type = "text/plain";
                    res.Body = str_to_char(body);
                    res.Content_Len = strlen(res.Body);

                    sendClient(clients[i].fd, res, req.Path, TEXT_T, "");
                }

                // Close without keep-alive
                if(req.Connection == "close"){
                    if(DEBUG) printf("[SYS] 'connection: close' for %d\n", clients[i].fd);
                    close(clients[i].fd);
                    clients[i].fd = -1;
                    client_cnt -= 1;
                }
            }
        }
    }

    base64_cleanup();
    close(connfd);
    close(listenfd);
}