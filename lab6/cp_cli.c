//
//  cp_cli.c
//  
//
//  Created by Victoria Mengqi LIU on 4/22/16.
//
//


main()
{
    int cli_sockfd;/*客户端SOCKET */
    int addrlen;
    char seraddr[14];
    struct sockaddr_in ser_addr,/* 服务器的地址*/
    cli_addr;/* 客户端的地址*/
    char msg[MAX_MSG_SIZE];/* 缓冲区*/
    GetServerAddr(seraddr);
    cli_sockfd=socket(AF_INET,SOCK_STREAM,0);/*创建连接的SOCKET */
    if(ser_sockfd<0)
    {/*创建失败 */
        fprintf(stderr,"socker Error:%s\n",strerror(errno));
        exit(1);
    }
    /* 初始化客户端地址*/
    addrlen=sizeof(struct sockaddr_in);
    bzero(&ser_addr,addrlen);
    cli_addr.sin_family=AF_INET;
    cli_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    cli_addr.sin_port=0;
    if(bind(cli_sockfd,(struct sockaddr*)&cli_addr,addrlen)<0)
    {
        /*棒定失败 */
        fprintf(stderr,"Bind Error:%s\n",strerror(errno));
        exit(1);
    }
    /* 初始化服务器地址*/
    addrlen=sizeof(struct sockaddr_in);
    bzero(&ser_addr,addrlen);
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=inet_addr(seraddr);
    ser_addr.sin_port=htons(SERVER_PORT);
    if(connect(cli_sockfd,(struct sockaddr*)&ser_addr,&addrlen)!=0)/*请求连接*/
    {
        /*连接失败 */
        fprintf(stderr,"Connect Error:%s\n",strerror(errno));
        close(cli_sockfd);
        exit(1);
    }
    strcpy(msg,"hi,I am client!");
    send(sockfd,msg,sizeof(msg),0);/*发送数据*/
    recv(sockfd,msg,MAX_MSG_SIZE,0); /* 接受数据*/
    printf("%s\n",msg);/*在屏幕上打印出来 */
    close(cli_sockfd);
}