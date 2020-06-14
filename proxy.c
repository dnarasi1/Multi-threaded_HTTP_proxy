#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

int hooktoserver(char *, ushort);
char *recvtext(int);
int sendlen(char *);
int strLen(char *);
char* removeString(char *,const char *);
int startserver();
void * threadHandle(void *);

int main(){
    int sock;
    int size;
    int csd;
    int f;

    fd_set livesdset; /* set of live client sockets */
    fd_set rdset;
    int livesdmax; /* largest live client socket descriptor */

    sock = startserver();

    if (sock == -1) {
        perror("Error on starting server: ");
        exit(1);
    }
	

    FD_ZERO(&livesdset);
    FD_SET(sock,&livesdset);
    livesdmax = sock;
    pthread_t t;
    while(1){
        rdset=livesdset;
        f = select(livesdmax+1, &rdset, NULL,  NULL, NULL);
        if (FD_ISSET(sock, &rdset)){
            struct sockaddr_in address;
            size = sizeof (address);
            csd = accept(sock, (struct sockaddr *)&address,(socklen_t*)&size);
            struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&address;
            struct in_addr ipAddr = pV4Addr->sin_addr;
            char str[INET_ADDRSTRLEN];
	    inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

            printf("%s|", str);
            if(csd>0){	
	        pthread_create(&t,NULL,threadHandle,&csd);
	    }         
	}
            
    }
        
}

int startserver(){
    int sock;
    struct sockaddr_in server_addr, server_p;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    char * servhost;
    servhost = (char*)malloc(sizeof(char)*50);
    ushort servport;
    struct hostent * host_name;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9090);
    server_addr.sin_addr.s_addr = INADDR_ANY;


    bind(sock, (struct sockaddr *)&server_addr,sizeof(server_addr));

    int j=  listen(sock, 5);
    gethostname(servhost, sizeof(servhost));
    host_name = gethostbyname(servhost);
    strcpy(servhost, host_name->h_name);
    int len = sizeof(server_p);
    getsockname(sock, (struct sockaddr *)&server_p, &len);
    servport = ntohs(server_p.sin_port);

    printf("admin: started server on '%s' at '%hu'\n", servhost, servport);
    free(servhost);
    return(sock);
}


int hooktoserver(char *servhost, ushort servport) {
    int sd; /* socket descriptor */
    struct sockaddr_in address, client_p;
    ushort clientport; /* port assigned to this client */

    struct hostent * host_name;

    sd = socket(PF_INET, SOCK_STREAM, 0);

    host_name = gethostbyname(servhost);

    address.sin_family = AF_INET;
    address.sin_port = htons(servport);
    address.sin_addr = *((struct in_addr *)host_name->h_addr);
    //memcpy (&address.sin_addr.s_addr, host_name->h_addr, host_name->h_length);

    int q = connect(sd, (struct sockaddr *)&address, sizeof(address));
    
    int len = sizeof(client_p);
    getsockname(sd, (struct sockaddr *)&client_p, &len);
    clientport = ntohs(client_p.sin_port);  
     
    fflush(stdout);
    return (sd);
}

void * threadHandle(void *csd){
        int hook;
  	int length;

        int s2 =0;
        char * msg;
        char * rmsg;
        char *resp;
        char *temp;
        char *copymsg;
        ushort port = 80;
        msg = (char*)malloc(sizeof(char)*1000);
        rmsg = (char*)malloc(sizeof(char)*100000);
        resp = (char*)malloc(sizeof(char)*1000);
        temp = (char*)malloc(sizeof(char)*1000);

	struct timeval start, stop;
        double secs = 0;	

        gettimeofday(&start, NULL);

        int csdd= *((int *)csd);
        
        pthread_t tid = pthread_self();

        pthread_detach(tid);

        fd_set readfds1;
        

        char *substring;
        substring = (char*)malloc(sizeof(char)*200);
        char * url;
        url = (char*)malloc(sizeof(char)*200);
        char *substring2;
        substring2 = (char*)malloc(sizeof(char)*200);
        char *pport = (char*)malloc(sizeof(char)*200);

        int imsg;
        imsg = read (csdd, msg, 65535);


        strcpy(temp, msg);
        substring = strtok(temp, " ");
        substring = strtok(NULL, " ");
        
	strcpy(url, substring);

        substring2 = strtok(substring, "//");
        substring2 = strtok(NULL, "/");
        //printf("substring %s\n", substring2);
        char *tem;
        tem = (char*)malloc(sizeof(char)*200);
        strcpy(tem, substring2);
        pport = strtok(substring2, ":");
       
        pport = strtok(NULL, ":");
        
        if (pport){
	    port = atoi(pport);
	} 
        
        
        
        char *remmsg;
        remmsg = removeString(msg, tem);

        char *sub1;
        sub1  = (char*)malloc(sizeof(char)*200);
        sub1 = strtok(remmsg, "\r\n");
        char *subb;
        subb = "\r\n";
        char *sub2;
        sub2 = "Host: ";

        char *sub3;
        sub3 = "\r\n";

        char *sub4;
        sub4 = "User-Agent: Wget/1.20.1 (linux-gnu)\r\nAccept: */*\r\nAccept-Encoding: identity\r\nConnection: keep-alive\r\nProxy-Connection: Keep-Alive\r\n\r\n";
        char *sub5;

        sub5  = (char*)malloc(sizeof(char)*200);
        strcpy(sub5,sub2 );
        strcat(sub5, substring2);
        char * sub7;

        char *sub6;
        sub6  = (char*)malloc(sizeof(char)*1000);
        strcpy(sub6, sub1);
        strcat(sub6, subb);
        strcat(sub6, sub5);
        strcat(sub6, sub3);
        strcat(sub6, sub4);
        hook = hooktoserver(substring2, port);
        int wrmsg;
        wrmsg = write(hook, sub6, strlen(sub6)+1);
        FD_ZERO(&readfds1);
        FD_SET(hook, &readfds1);

        int sel =select(hook+1,&readfds1,NULL,NULL,NULL);

        int irmsg;
        if (FD_ISSET(hook, &readfds1)){
            while(1){

                irmsg = read(hook, rmsg, 65535);
    
            if(!irmsg)
                    break;

                if(s2==0){
                    //char *copymsg;
                    copymsg = (char*)malloc(sizeof(char)*65535);
                    strncpy(copymsg, rmsg, irmsg);

                    char *stripmsg;
                    char *ss = "\r\n\r\n";
                    char *p = strstr(copymsg, ss);
                    int l = p-copymsg;
                    stripmsg = malloc(l+1);
                    memcpy(stripmsg, copymsg, l);
                    stripmsg[l] = '\0';

                    char * p1 = strstr(stripmsg, "Content-Length: ");
                    char * tok1;
                    tok1 = malloc(200);

                    tok1 = strtok(p1, " ");
                    tok1 = strtok(NULL, " ");

                    length = atoi(tok1);
                    s2=-1;
                }
                int icmsg;
                void *p = rmsg;
                while(irmsg>0){
                    icmsg = write(csdd, p, irmsg);
                    irmsg-=icmsg;
                    p+=icmsg;
                }
         }
     }
    memset(msg, 0, sizeof(*msg));
    memset(rmsg, 0, sizeof(*rmsg));
    memset(substring, 0, sizeof(*substring));
    memset(substring2, 0, sizeof(*substring2));
    memset(temp, 0, sizeof(*temp));
    memset(tem, 0, sizeof(*tem));
    memset(copymsg, 0, sizeof(*copymsg));
    gettimeofday(&stop, NULL);
    secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
    
    printf("%s|%d|%f\n",url,length,secs);
    
    free(msg);
    free(rmsg);
    close(csdd);
    close(hook);
}

int strLen(char *p)
{
    int c=0;
    while(*p!='\0')
    {
        c++;
        *p++;
    }
    return(c);
}

char* removeString(char *s,const char *sub)
{
    char * str1;
    str1 = "http://";

    char * str3 = (char *) malloc(10 + strlen(str1)+ strlen(sub) );

    strcpy(str3, str1);

    strcat(str3, sub);
    
    size_t len = strlen(str3);
    
     
    if (len > 0) {
        char *p = s;
        while ((p = strstr(p, str3)) != NULL) {
            memmove(p, p + len, strlen(p + len) + 1);
        }
    }
    return s;
}
