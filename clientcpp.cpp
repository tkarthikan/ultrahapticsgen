// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
//#include "rapidjson/document.h"
#include <fstream>
#include <sstream>
#define PORT 12345   
//using namespace rapidjson;
//Function to receive exactly "len" bytes.
//Returns number of bytes received, or -1 on EOF or error
int main(int argc, char const *argv[])
{   float arr[256][3];
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    int socket_desc, client_sock, c, read_size;
    int message[10], i;
    
    //Document document;
    char hello[]= "[[], [0, 0, 0.46697012], [1, 0, 0.91195464], [2, 0, 0.97631764], [3, 0, 0.995767], [4, 0, 0.9987637], [5, 0, 0.9997923], [6, 0, 0.9998909], [7, 0, 0.9999368], [8, 0, 0.9995988], [9, 0, 0.9917522], [10, 0, 0.92990494]]";
    char buffer[1024] = {0};
    //sscanf(hello,"%f",&arr);
    //printf("%s\n", hello[1] );
    // if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    // {
    //     printf("\n Socket creation error \n");
    //     return -1;
    // }
    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_port = htons(PORT);
       
    // // Convert IPv4 and IPv6 addresses from text to binary form
    // if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    // {
    //     printf("\nInvalid address/ Address not supported \n");
    //     return -1;
    // }
   
    // if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    // {
    //     printf("\nConnection Failed \n");
    //     return -1;
    // }
    float data[5][3];
    std::ifstream file("sheetgenhap.csv");
    printf("Hello message sent\n");
    for(int row = 0; row < 6; row++)
    {
        std::string line;
        std::getline(file, line);
        if ( !file.good() )
            break;
        std::stringstream iss(line);
        for (int col = 0; col < 4; col++)
        {
            std::string val;
            std::getline(iss, val, ',');
            std::stringstream convertor(val);
            convertor >> data[row][col];
            printf("Row: %d  ",row);
            printf("Col: %d  ",col);
            printf("%f\n",data[row][col]);
            if ( !iss.good() )
                break;
        }
    }
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read(sock , buffer, 1024);
    // printf("%c\n",buffer[1017]);
    //   printf("%c\n",buffer[1018]);
    //  printf("%c\n",buffer[1019]);
    //   printf("%c\n",buffer[1020]);
    //    printf("%c\n",buffer[1021]);
    //     printf("%c\n",buffer[1022]);
    // printf("%c\n",buffer[1023]);
    // printf("%c\n",buffer[1024]);
    // printf("Strlen: %d\n",strlen(buffer));
    // for (int i=0; i<strlen(buffer); i++){
    //   printf("%c\n",buffer[i]);  
    // }
    
    //document.Parse(buffer);
    // sscanf(buffer,"%f",&arr);
    // printf("%f\n",arr[0][2] );
    // printf("%f\n",buffer[5] );
    return 0;
}