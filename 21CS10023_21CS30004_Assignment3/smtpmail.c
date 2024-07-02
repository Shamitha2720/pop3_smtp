
/*
    C socket server example
*/

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<stdlib.h>
#include <time.h>

int main(int argc , char *argv[])
{   int sno[4]={0,0,0,0};
    while(1){
    
    
     int my_port = atoi(argv[1]);
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     int enable = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        return 1;
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( my_port );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("220 <iitkgp.edu> Service ready\n");

    //Receive a message from client
    int k;
    
    // Receive the value of k from the client
    read(client_sock, &k, sizeof(int));

    // Process the received value of k as needed
    printf("Received k from client: %d\n", k);
    char msg[4000];  // Assuming the To line will not contain more than 80 characters
    char sub[80];
    char to[40];
    read(client_sock, to, sizeof(to));
   
     printf("250 root... Recipient ok \n");
     printf("250 <shamhash@127.0.0.1>... Sender ok\n");
    read(client_sock, sub, sizeof(sub));
    
    printf("354 Enter mail, end with '.' on a line by itself\n");
    read(client_sock, msg, sizeof(msg));
    
    // Read the message until the end
    int i = 0;
    while (read(client_sock, &msg[i], 1) > 0 && (msg[i-1]=='\n'&&msg[i]=='.')) {
        i++;
    }
    
   
    
        // Process the received details (you can replace this with your logic)
        printf("To: %s\n", to);
        printf("Subject: %s\n", sub);
       
        printf("Message:\n%s\n", msg);
        printf("250 OK Message accepted for delivery\n ");


        
    const char *fileName = "mymailbox";

    // Construct the full path to the file
    char fullPath[256]; // Adjust the size as needed
    snprintf(fullPath, sizeof(fullPath), "%s/%s", to, fileName);

    // Open the file for writing
    FILE *file = fopen(fullPath, "a");

    // Check if the file was successfully opened
    if (file == NULL) {
        perror("Error opening file");
        return 1; // Exit with an error code
    }
time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
     

    
  fprintf(file, "From: shamhash@127.0.0.1\n");
    fprintf(file, "To: %s\n", to);
    fprintf(file, "Subject: %s\n", sub);
 fprintf(file, "Received at: %s", asctime(localTime));
  fprintf(file, "Message: %s\n", msg);


    // Close the file
    int a=fclose(file);
    if(a==0)
    {
        printf("file closed");
    }


    printf("File successfully written to %s\n", fullPath);   
    close(socket_desc);
}


    return 0;
}