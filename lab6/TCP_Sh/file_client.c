/*************************************************************************
	> File Name: file_server.c
	> Author: shihang
	> Mail: shihangTs@gmail.com
	> Created Time: 2016.04.11 MON
  > Description: 心情不好就写成这样吧23333
 ************************************************************************/

#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define HELLO_WORLD_SERVER_PORT       6666
#define BUFFER_SIZE                   1024
#define FILE_NAME_MAX_SIZE            512

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./%s ServerIPAddress\n", argv[0]);
        exit(1);
    }


    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htons(INADDR_ANY);
    client_addr.sin_port = htons(0);


    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        printf("Create Socket Failed!\n");
        exit(1);
    }


    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))
    {
        printf("Client Bind Port Failed!\n");
        exit(1);
    }


    struct sockaddr_in  server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;


    if (inet_aton(argv[1], &server_addr.sin_addr) == 0)
    {
        printf("Server IP Address Error!\n");
        exit(1);
    }

    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);
    socklen_t server_addr_length = sizeof(server_addr);


    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)
    {
        printf("Can Not Connect To %s!\n", argv[1]);
        exit(1);
    }

    char file_name[FILE_NAME_MAX_SIZE + 1];
    bzero(file_name, sizeof(file_name));
    printf("Please Input File Name On Server.\t");
    scanf("%s", file_name);

    char buffer[BUFFER_SIZE];
    bzero(buffer, sizeof(buffer));
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));

    send(client_socket, buffer, BUFFER_SIZE, 0);

    FILE *fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        printf("File:\t%s Can Not Open To Write!\n", file_name);
        exit(1);
    }

    // 从服务器端接收数据到buffer中
    bzero(buffer, sizeof(buffer));
    int length = 0;
    while(length = recv(client_socket, buffer, BUFFER_SIZE, 0))
    {
        if (length < 0)
        {
            printf("Recieve Data From Server %s Failed!\n", argv[1]);
            break;
        }

        int write_length = fwrite(buffer, sizeof(char), length, fp);
        if (write_length < length)
        {
            printf("File:\t%s Write Failed!\n", file_name);
            break;
        }
        bzero(buffer, BUFFER_SIZE);
    }

    printf("Recieve File:\t %s From Server[%s] Finished!\n", file_name, argv[1]);


    fclose(fp);
    close(client_socket);
    return 0;

}
