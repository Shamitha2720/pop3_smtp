

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX 100
#define DOMAIN_NAME "iitkgp.edu"

int is_right_un(char *userName)
{
   FILE *file;
    char line[256]; 
    char *fileun;
    int fount_flag = 0; 


    file = fopen("user.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; 
    }

    
    while (fgets(line, sizeof(line), file)) {
       
        fileun = strtok(line, " \t\n");
        if (fileun != NULL && strcmp(userName, fileun) == 0) {
            fount_flag = 1; 
            break;
        }
    }

  
    fclose(file);

    return fount_flag;
}



void mailread(char *usern, int mailno, char *mailtext) {
    char path[MAX];
    strcpy(path, usern);
    strcat(path, "/mymailbox");

    FILE *fptr = fopen(path, "r");
    if (fptr == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX];
    int curr_mail = 0;
    int mail_t = 0;

    while (fgets(line, MAX, fptr) != NULL) {
       

        if (strncmp(line, "From: ", 6) == 0) {
            curr_mail++;
            if(curr_mail== mailno+1){
                break;
            }

            if (curr_mail == mailno) {
                mail_t = 1; 
            }
        }

        if (mail_t) {
            strcat(mailtext, line);
        }

        if (mail_t && curr_mail > mailno) {
            break;
        }
    }

    fclose(fptr);
}


int is_right_pwd(char *username,char *password){
    FILE *file;
    char line[256]; 
    char *fileun;
    char *filepwd;
    int fount_flag = 0; 
    file = fopen("user.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1; 
    }

    while (fgets(line, sizeof(line), file)) {
        fileun = strtok(line, " \t\n");

       
        filepwd = strtok(NULL, " \t\n");


        if (fileun != NULL && strcmp(username, fileun) == 0) {
            if (filepwd != NULL && strcmp(password, filepwd) == 0) {
                fount_flag = 1;
                break;
            }
        }
    }

    fclose(file);

    return fount_flag;


}




int main(int argc, char *argv[])
{

    int j = 0;


    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int Port = atoi(argv[1]);

    char buff[MAX + 1];

    struct sockaddr_in serverAddr, clientAddr;
    int clientlen;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        printf("Cannot create socket\n");
        exit(EXIT_FAILURE);
    }

    printf("Server Socket is created.\n");

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(Port);

    // Binding the server
    if (bind(sock_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Error in binding\n");
        exit(EXIT_FAILURE);
    }



    if (listen(sock_fd, 5) == 0)
    {
        printf("Listening....\n");
    }
    else
    {
        printf("Error in Listening.\n");
        exit(EXIT_FAILURE);
    }

    printf("POP server listening on port %d...\n", Port);

    while (1)
    {

        int clientSocket = accept(sock_fd, (struct sockaddr *)&clientAddr, &clientlen);
        if (clientSocket < 0)
        {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }

        printf("Connection accepted\n");

        if (fork() == 0)
        {

            close(sock_fd);

            sprintf(buff,"+OK POP3 server ready\r\n");
            printf("Server: %s",buff);
            fflush(stdout);


            send(clientSocket, buff, sizeof(buff), 0);



            char username[MAX];
            char password[MAX];
            int auth = 0;

            int flag=0;

            while (1)
            {
                ssize_t bytes;
                memset(buff, '\0', sizeof(buff));
                bytes = recv(clientSocket, buff, sizeof(buff), 0);
                int n;
                flag=0; 

                if (strstr(buff,"USER") != NULL)
                {
                    n = 1;
                    char user[MAX];
                        j = 0;
                        for (int i = 5; i < strlen(buff); i++)
                        {
                            if (buff[i] == '\r')
                            {
                                break;
                            }
                            else
                                user[j] = buff[i];
                            j++;
                        }
                        user[j] = '\0';

                        strcpy(username, user);
                        // printf("username: %s",username);
                        memset(buff, '\0', sizeof(buff));

                        int check_user = is_right_un(username);
                        if(check_user == 0){
                            memset(buff, '\0', sizeof(buff));
                            strcpy(buff, "-ERR Invalid username \r\n");
                            bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                            if (bytes < 0)
                            {
                                perror("Error occured while writing to socket!");
                                return 1;
                            }
                            flag=1;
                            break;
                        }
                        strcpy(buff, "+OK Please enter the password\r\n");
                        bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                        if (bytes < 0)
                        {
                            perror("Error occured while writing to socket!");
                            return 1;
                        }
                }
                else if (strstr(buff, "PASS") != NULL)
                {
                    n = 2;
                   char pass[MAX];
                        j = 0;
                        for (int i = 5; i < strlen(buff); i++)
                        {
                            if (buff[i] == '\r')
                            {
                                break;
                            }
                            else
                                pass[j] = buff[i];
                            j++;
                        }
                        pass[j] = '\0';

                        strcpy(password, pass);
                        int exists = is_right_pwd(username,password);
                        
                        if (exists)
                        {
                            memset(buff, '\0', sizeof(buff));
                            strcpy(buff, "+OK maildrop locked and ready\r\n");
                            bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                            if (bytes < 0)
                            {
                                perror("Error occured while writing to socket!");
                                return 1;
                            }
                            // auth = 1;
                        }
                        else
                        {
                            flag=1;
                            memset(buff, '\0', sizeof(buff));
                            strcpy(buff, "-ERR Invalid password \r\n");
                            bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                            if (bytes < 0)
                            {
                                perror("Error occured while writing to socket!");
                                return 1;
                            }
                        }


                }
                else if (strstr(buff, "STAT") != NULL)
                { n=3;
                     int mailCount = 0;
                        char path[MAX];
                        strcpy(path, username);
                        strcat(path, "/mymailbox");

                        FILE *fptr = fopen(path, "r");
                        if (fptr == NULL)
                        {
                            perror("Error opening file");
                            exit(EXIT_FAILURE);
                        }

                        char line[MAX];
                        while (fgets(line, MAX, fptr) != NULL)
                        {
                            if (strncmp(line, "From: ", 6) == 0)
                            {
                                mailCount++;
                            }
                        }
                        memset(buff, '\0', sizeof(buff));
                        strcpy(buff, "+OK ");
                        char mailCountStr[MAX];
                        sprintf(mailCountStr, "%d", mailCount);
                        strcat(buff, mailCountStr);
                        strcat(buff, " ");
                        strcat(buff, "0\r\n");
                        bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                        if (bytes < 0)
                        {
                            perror("Error occured while writing to socket!");
                            return 1;
                        }
                }
                else if (strstr(buff, "LIST") != NULL)
                {
                    n = 4;
                }
                else if (strstr(buff, "RETR") != NULL)
                {
                    n = 5;
                    char mail_no[MAX];
                        j = 0;
                        for (int i = 5; i < strlen(buff); i++)
                        {
                            if (buff[i] == '\r')
                            {
                                break;
                            }
                            else
                                mail_no[j] = buff[i];
                            j++;
                        }
                        mail_no[j] = '\0';

                        int mailno = atoi(mail_no);
                        char mailtext[MAX * MAX];
                        char buff2[MAX * MAX];
                        memset(mailtext,'\0',sizeof(mailtext));
                        mailread(username, mailno, mailtext);
                        mailtext[strlen(mailtext)-1] = '\0';
                        memset(buff2, '\0', sizeof(buff2));
                        strcpy(buff2, "+OK \r\n");
                        strcat(buff2, mailtext);
                        // strcat(buff2, "\r\n");
                        strcat(buff2, ".\r\n");
                        bytes = send(clientSocket, buff2, strlen(buff2) + 1, 0);
                        if (bytes < 0)
                        {
                            perror("Error occured while writing to socket!");
                            return 1;
                        }
                }
                else if (strstr(buff, "DELE") != NULL)
                {
                    n = 6;
                }
                else if (strstr(buff, "QUIT") != NULL)
                {
                    n = 7;
                     memset(buff, '\0', sizeof(buff));
                        strcpy(buff, "+OK POP3 server signing off\r\n");
                        bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                        if (bytes < 0)
                        {
                            perror("Error occured while writing to socket!");
                            return 1;
                        }
                        flag=1;
                }

                else
                {
                    n = 8;
                     memset(buff, '\0', sizeof(buff));
                        strcpy(buff, "-ERR Unknown command \r\n");
                        bytes = send(clientSocket, buff, strlen(buff) + 1, 0);
                        if (bytes < 0)
                        {
                            perror("Error occured while writing to socket!");
                            return 1;
                        }
                        flag=1;
                }

                if (bytes < 0)
                {
                    printf("\nError occurred while reading from socket!\n");

                }
                

                    

                

            }
        
            close(clientSocket);
            exit(0);
        }

        close(clientSocket);
    }

    close(sock_fd);

     return 0 ;
}