#include<stdio.h> 
#include<string.h> 
#include<sys/socket.h> 
#include<arpa/inet.h> 
#include<stdlib.h>
#include<regex.h>
#define POP3_SERVER_NAME "pop.example.com"
#define MAX_SIZE_BUFF 1024
int iswrong(const char* str, const char* regex_pattern)
{
 //return (sscanf(to, "^[[:alnum:]]+@127\\.0\\.0\\.1$") == 1);
     regex_t regex;
    int reti;

    reti = regcomp(&regex, regex_pattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return 0; 
    }

    reti = regexec(&regex, str, 0, NULL, 0);
    int i;
    
    regfree(&regex);

    return reti; // Return 1 if regex matches, 0 otherwise
}
int main(int argc , char *argv[])
{
    int flg,w,mail_sn;
    char no_of_msgs[100];
    char temp_buff[4];
    char del_flg;
    ssize_t n,no_of_bytes;
    FILE *filename;
    filename = fopen("user.txt", "r");
    
    if (filename == NULL) {
        perror("Error opening filename");
        return 1;
    }

    char **usernames = NULL; 
    char *username = (char *)malloc(50 * sizeof(char)); 
    char *password = (char *)malloc(50 * sizeof(char));  
    char **passwords=NULL;
    int count = 0; 
    while (fscanf(filename, "%49s %49s", username, password) == 2) {
        usernames = (char **)realloc(usernames, (count + 1) * sizeof(char *));
        passwords=(char**)realloc(passwords,(count+1)*sizeof(char *));
        usernames[count] = strdup(username);
        passwords[count]=strdup(password);
        count++;
    }
    
    printf("Usernames:\n");
    for (int i = 0; i < count; i++) {
        printf("%s\n", usernames[i]);
        w=i;
    }

    fclose(filename);
    char buff[MAX_SIZE_BUFF + 1];
    char buff2[MAX_SIZE_BUFF + 1];
while(1){
     printf("1. Manage Mail : Shows the stored mails of the logged in user only\n2. Send Mail : Allows the user to send a mail\n3. Quit : Quits the program.\n");
    int k;
    scanf("%d",&k);
    if(k==3)
    {
        break;
    }
    if(k==1)
    {memset(buff2, '\0', sizeof(buff2));

    int port_no = atoi(argv[1]); 
    int sock;
    struct sockaddr_in server;
    char message[1000] ;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port_no );
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("succesfully Connected\n");
            no_of_bytes = recv(sock, buff2, sizeof(buff2), 0);
            temp_buff[0] = buff2[0];
            temp_buff[1] = buff2[1];
            temp_buff[2] = buff2[2];
            temp_buff[3] = '\0';

            if (strcmp(temp_buff, "+OK") != 0)
            {
                printf("\n[-]Error in recieving from the Server\n");
                exit(EXIT_FAILURE);
            }

            printf("S: %s", buff2);

            // Sending the username
            char un[30];
            printf("enter username\n");
            scanf("%s",un);
            char pwd[40];
            printf("enter password\n");
            scanf("%s",pwd);

            memset(buff, '\0', sizeof(buff));
            strcpy(buff, "USER ");
            strcat(buff, un);
            strcat(buff, "\r\n");

            n = send(sock, buff, strlen(buff) + 1, 0);
            if (n < 0)
            {
                printf("\nError occured while writing to socket!\n");
            }
            printf("C: %s", buff); // USER username

            memset(buff, '\0', sizeof(buff));

            n = recv(sock, buff, MAX_SIZE_BUFF, 0);
            if (n < 0)
            {
                printf("\nError occured while reading from socket!\n");
            }

            // checking whether username correct or not
            temp_buff[0] = buff[0];
            temp_buff[1] = buff[1];
            temp_buff[2] = buff[2];
            temp_buff[3] = '\0';

            if (strcmp(temp_buff, "+OK") != 0)
            {
                printf("\nError occured in username!\n\n");
                return 1;
            }

            printf("S: %s", buff); // +OK

            fflush(stdin);
            fflush(stdout);

            // Sending the password

            memset(buff, '\0', sizeof(buff));
            strcpy(buff, "PASS ");
            strcat(buff, pwd);
            strcat(buff, "\r\n");

            send(sock, buff, strlen(buff) + 1, 0);
            if (n < 0)
            {
                printf("\nError occured while writing to socket!\n");
            }
            printf("C: %s", buff); // PASS password

            memset(buff, '\0', sizeof(buff));

            n = recv(sock, buff, MAX_SIZE_BUFF, 0);
            if (n < 0)
            {
                printf("\nError occured while reading from socket!\n");
            }

            // checking whether password correct or not
            temp_buff[0] = buff[0];
            temp_buff[1] = buff[1];
            temp_buff[2] = buff[2];
            temp_buff[3] = '\0';

            if (strcmp(temp_buff, "+OK") != 0)
            {
                printf("\nError occured in password!\n\n");
                return 1;
            }

            printf("S: %s", buff); // +OK

            fflush(stdin);
            fflush(stdout);

            // Displaying the usernames on the client side by first sending the STAT command to the server to get the number of messages

            memset(buff, '\0', sizeof(buff));
            strcpy(buff, "STAT");
            strcat(buff, "\r\n");

            n = send(sock, buff, strlen(buff) + 1, 0);
            if (n < 0)
            {
                printf("\nError occured while writing to socket!\n");
            }
            printf("C: %s", buff); // STAT

            // Recieving the number of messages from the server which will be of the form "+OK <number of messages> <size of messages>"

            memset(buff, '\0', sizeof(buff));
            n = recv(sock, buff, MAX_SIZE_BUFF, 0);
            if (n < 0)
            {
                printf("\nError occured while reading from socket!\n");
            }

            // chekcing +OK or not
            temp_buff[0] = buff[0];
            temp_buff[1] = buff[1];
            temp_buff[2] = buff[2];
            temp_buff[3] = '\0';

            if (strcmp(temp_buff, "+OK") != 0)
            {
                printf("\nError occured while receiving STAT\n\n");
                return 1;
            }

            // Extracting the number of messages from the server response and converting it to int

            // getting the position of the first space in the message
            char *sp1 = strchr(buff, ' ');
            // getting the position of the second space in the message
            char *sp2 = strchr(sp1 + 1, ' ');

            // calculating the msg_len of the number of messages
            int msg_len = sp2 - sp1 - 1;

            // copying the number of messages to the mesasges_count array
            strncpy(no_of_msgs, sp1 + 1, msg_len);
            no_of_msgs[msg_len] = '\0';

            // converting the number of messages to int
            int msgs_ct = atoi(no_of_msgs);

            printf("S: %s", buff); // +OK <number of messages> <size of messages>
         // printf("%d\n", msgs_ct);

            // Displaying the list of messages on the client side by recursively running the RETR command for each message

            for (int i = 1; i <= msgs_ct; i++)
            {
                memset(buff, '\0', sizeof(buff));
                strcpy(buff, "RETR ");
                char message_number[10];
                sprintf(message_number, "%d", i);
                strcat(buff, message_number);
                strcat(buff, "\r\n");

                // printf("sending message: %s\n", buff);

                n = send(sock, buff, strlen(buff) + 1, 0);
                if (n < 0)
                {
                    printf("\nError occured while writing to socket!\n");
                }
                printf("C: %s", buff); // RETR message_number

                // Recieving the message from the server

                memset(buff, '\0', sizeof(buff));

                n = recv(sock, buff, MAX_SIZE_BUFF, 0);
                if (n < 0)
                {
                    printf("\nError occured while reading from socket!\n");
                }

                // checking +OK or not
                temp_buff[0] = buff[0];
                temp_buff[1] = buff[1];
                temp_buff[2] = buff[2];
                temp_buff[3] = '\0';

                if (strcmp(temp_buff, "+OK") != 0)
                {
                    printf("\nError occured while receiving RETR\n\n");
                    return 1;
                }

                // printf("S: %s", buff); // +OK

                // Printing the message
                printf("Message %d: %s", i, buff);

                // Should change temp_buff here 
            }

            flg = 0;

            while(1)
            {
                // Now waiting for the user to ask to display a message :
                printf("Enter mail no. to see: ");
                scanf("%d", &mail_sn);
                getchar();

                // If the number entered is –1, the program goes back to the main menu (the three options)
                if (mail_sn == -1)
                {
                    flg = 1;
                    break;
                }
                
                //  If the number entered is out of range, an error message “Mail no. out of range, give again” is printed and the user is prompted to give the number of the mail to read again
                if (mail_sn < 1 || mail_sn > msgs_ct)
                {
                    printf("Mail no. out of range, give again\n");
                    continue;
                }

                // If the number entered is valid, the program displays the message and then goes back to the main menu (the three options)
                else
                {
                    memset(buff, '\0', sizeof(buff));
                    strcpy(buff, "RETR ");
                    char message_number[10];
                    sprintf(message_number, "%d", mail_sn);
                    strcat(buff, message_number);
                    strcat(buff, "\r\n");

                    n = send(sock, buff, strlen(buff) + 1, 0);
                    if (n < 0)
                    {
                        printf("\nError occured while writing to socket!\n");
                    }
                    printf("C: %s", buff); // RETR message_number

                    // Recieving the message from the server

                    memset(buff, '\0', sizeof(buff));
                    n = recv(sock, buff, MAX_SIZE_BUFF, 0);
                    if (n < 0)
                    {
                        printf("\nError occured while reading from socket!\n");
                    }

                    // checking +OK or not
                    temp_buff[0] = buff[0];
                    temp_buff[1] = buff[1];
                    temp_buff[2] = buff[2];
                    temp_buff[3] = '\0';

                    if (strcmp(temp_buff, "+OK") != 0)
                    {
                        printf("\nError occured while receiving RETR\n\n");
                        return 1;
                    }

                    printf("S: %s", buff); // +OK

                    // // Printing the message
                    // printf("Message %d: %s\n", mail_sn, buff);

                    // The program waits on a getchar() after showing the mail. If the character is ‘d’, the mail is deleted
                    del_flg = getchar();

                    if (del_flg == 'd')
                    {
                        // Send the DELE command to the server along with the mail number
                        memset(buff, '\0', sizeof(buff));
                        strcpy(buff, "DELE ");
                        char message_number[10];
                        sprintf(message_number, "%d", mail_sn);
                        strcat(buff, message_number);
                        strcat(buff, "\r\n");

                        n = send(sock, buff, strlen(buff) + 1, 0);
                        if (n < 0)
                        {
                            printf("\nError occured while writing to socket!\n");
                        }
                        printf("C: %s", buff); // DELE message_number

                        // Recieving the message from the server

                        memset(buff, '\0', sizeof(buff));
                        n = recv(sock, buff, MAX_SIZE_BUFF, 0);
                        if (n < 0)
                        {
                            printf("\nError occured while reading from socket!\n");
                        }

                        // checking +OK or not
                        temp_buff[0] = buff[0];
                        temp_buff[1] = buff[1];
                        temp_buff[2] = buff[2];
                        temp_buff[3] = '\0';

                        if (strcmp(temp_buff, "+OK") != 0)
                        {
                            printf("\nError occured while receiving DELE\n\n");
                            return 1;
                        }

                        printf("S: %s", buff); // +OK

                        flg=1;
                        break;

                    }
                    else
                    {
                        continue;
                    }
                    
                }
                
            }

            if(flg == 1)
            {
                break;
            }

    }

    if (k==2)
    {
      int port_no = atoi(argv[1]); 
    int sock;
    struct sockaddr_in server;
    char message[1000] ;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port_no );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    
    
     

   
     write(sock, &k, sizeof(int));
     char to[40];
     const char* re="^[[:alnum:]]+@127\\.0\\.0\\.1$";
     printf("MAIL FROM: <shamhash@127.0.0.1>\n");
    printf("From:shamhash@127.0.0.1\n");
    do {printf("To:");
    
    scanf("%s",to);}while(iswrong(to,re));
    printf(" RCPT TO: %s ",to);

    char msg[4000];  // Assuming the To line will not contain more than 80 characters
    char sub[80];
    printf("\n");
    printf("sub:");
     
        scanf("%s",sub);
        
     printf(" \nsubject is %s",sub);
 printf("\n");
 printf("msg:");
    // Read the To line
for(int i=0;i<4000;i++){
    scanf("%c",&msg[i]);
    if(msg[i-1]=='\n'&&msg[i]=='.')
    {break;}

    
}
printf("\n");
write(sock, to, sizeof(to)); 
write(sock, sub, sizeof(sub));
write(sock, msg, sizeof(msg)); 
 

    close(sock);
    }
}
    return 0;
}