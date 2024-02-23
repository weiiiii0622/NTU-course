#ifndef HTTP_HEADER_H
#define HTTP_HEADER_H

#include <string>

struct HTTPrequest {
    std::string Method = "";
    std::string Path = "";
    std::string Version = "";
    std::string Host = "";
    std::string User_Agent = "";
    std::string Connection = "";
    std::string Authorization = "";
    std::string Content_Type = "";
    long long Content_Len = 0;
    char* Body;
    std::string Boundary = "";
    std::string File_Name = "";
};

struct HTTPresponse {
    std::string Version = "";
    int Status_Code = 0;
    std::string Status_Text = "";
    std::string Server = "";
    std::string Allow = "";
    std::string WWW_Authenticate="";
    std::string Content_Type = "";
    std::string Connection = "";
    long long Content_Len = 0;
    char* Body;
};

#endif