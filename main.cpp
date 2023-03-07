#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>
#include<bits/stdc++.h>

using namespace std;


#define PORT 8080
#define MAX_CONNECTION 10000
const int MAX_BUFFER_LENGTH = 300000;
void sendFile(int, string);

string okay_header = "HTTP/1.1 200 OK";



int main()
{
    int fd; 
    long valread;
    sockaddr_in address;
    int addrlen = sizeof(address);
    string header = "HTTP/1.1 200 OK\n\n";
    
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socker error");
        exit(EXIT_FAILURE);
    }
    

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);
    
    
    if (bind(fd, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, MAX_CONNECTION) < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    while(true)
    {
        int new_socket;
        cout<<"Waiting for new connection\n";
        if ((new_socket = accept(fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }

        int pid = fork();
        
        if(pid < 0){
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if(pid==0){
            char buffer[MAX_BUFFER_LENGTH] = {0};
            valread = read( new_socket , buffer, MAX_BUFFER_LENGTH);
            string head = "HTTP/1.1 200 Ok\nContent-Type: text/html\n\n";
            write(new_socket, head.c_str(), head.size());
            cout<<buffer<<endl;
  //          write(new_socket , header.c_str() , header.size());
            sendFile(new_socket,"./index.html");
        }
        close(new_socket);
    }
    return 0;
}

void sendFile(int fd, string filePath)
{
    struct stat stat_buf;  /* hold information about input file */
    int fileFD = open(filePath.c_str(), O_RDONLY);
    
    if(fileFD < 0){
        printf("error while opening file: %d\n", fileFD); 
    }
     
    fstat(fileFD, &stat_buf);
    int fileSize = stat_buf.st_size;
    int blockSize = stat_buf.st_blksize;

    cout<<"fileDetails: "<<fileSize<<" "<<blockSize<<endl;
    
    if(fileFD >= 0){
        ssize_t sent_size;

        while(fileSize > 0){
              int send_bytes = ((fileSize < blockSize) ? fileSize : blockSize);
              int done_bytes = sendfile(fd, fileFD, NULL, blockSize);
              fileSize = fileSize - done_bytes;
        }
        if(sent_size >= 0){
            cout<<filePath<<endl;
        }
        close(fileFD);
    }
}
