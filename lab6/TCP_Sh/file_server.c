/*************************************************************************
	> File Name: file_server.c
	> Author: shihang
	> Mail: shihangTs@gmail.com
	> Created Time: 2016.04.11 MON
  > Description: Commercial use is forbidden ~shihang
 ************************************************************************/

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define HELLO_WORLD_SERVER_PORT    6666
#define LENGTH_OF_LISTEN_QUEUE     20
#define BUFFER_SIZE                1024
#define FILE_NAME_MAX_SIZE         512

int main(int argc, char **argv)
{

    struct sockaddr_in   server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);


    int server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }


    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
    {
        printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);
        exit(1);
    }


    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
    {
        printf("Server Listen Failed!\n");
        exit(1);
    }


    while(1)
    {

        struct sockaddr_in client_addr;
        socklen_t          length = sizeof(client_addr);


        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
        if (new_server_socket < 0)
        {
            printf("Server Accept Failed!\n");
            break;
        }

        char buffer[BUFFER_SIZE];
        bzero(buffer, sizeof(buffer));
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);
        if (length < 0)
        {
            printf("Server Recieve Data Failed!\n");
            break;
        }

        char file_name[FILE_NAME_MAX_SIZE + 1];
        bzero(file_name, sizeof(file_name));
        strncpy(file_name, buffer,
                strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));

        FILE *fp = fopen(file_name, "r");
        if (fp == NULL)
        {
            printf("File:\t%s Not Found!\n", file_name);
        }
        else
        {
            bzero(buffer, BUFFER_SIZE);
            int file_block_length = 0;
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                printf("file_block_length = %d\n", file_block_length);


                if (send(new_server_socket, buffer, file_block_length, 0) < 0)
                {
                    printf("Send File:\t%s Failed!\n", file_name);
                    break;
                }

                bzero(buffer, sizeof(buffer));
            }
            fclose(fp);
            printf("File:\t%s Transfer Finished!\n", file_name);
        }

        close(new_server_socket);
    }

    close(server_socket);

    return 0;
}
