#include<winsock2.h> 
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#pragma comment (lib, "ws2_32.lib")  //���� ws2_32.dll
#define MAX_BUFFER_SIZE 50000 //���������С

int main(){
	printf("------------------Ӧ�ò�ģ��-----------------\n\n"); 
	
    WSADATA wsaData;
    WSAStartup( MAKEWORD(2, 2), &wsaData);
    
    int AppPort=12300,NetPort=12500;
    
    char TargetPort[6];//Ŀ�Ķ˿ں�
    char buf[MAX_BUFFER_SIZE];
    char buffer[MAX_BUFFER_SIZE];
    memset(TargetPort, 0, sizeof(TargetPort));
    memset(buffer, 0, sizeof(buffer));
    memset(buf, 0, sizeof(buf));

    //����sock
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

    /*---------------------�ָ���-------------------*/

    //��Ӧ�ò�˿�
	printf("������Ӧ�ò�Ķ˿ںţ�\n") ;
	printf("%d\n",AppPort); 
    sockaddr_in AppAddr;
    memset(&AppAddr, 0, sizeof(AppAddr));
    AppAddr.sin_family = PF_INET;
    AppAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    AppAddr.sin_port = htons(AppPort);
    bind(sock, (SOCKADDR*)&AppAddr, sizeof(SOCKADDR)); 

    /*---------------------�ָ���-------------------*/

    //�������Ϣ 
	sockaddr_in NetAddr;
    memset(&NetAddr, 0, sizeof(NetAddr));
    int NetLen=sizeof(NetAddr);
    NetAddr.sin_family = PF_INET;
    NetAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	printf("�����������Ķ˿ںţ�\n") ;
    printf("%d\n",NetPort);
    NetAddr.sin_port = htons(NetPort);

    while(1){
        //���嵱ǰ״̬
        printf("�붨�嵱ǰ״̬��\n1.������Ϣ;2.������Ϣ\n\n");

        char IsSend[2];//״̬��Ϣ��־
        memset(IsSend,0,sizeof(IsSend));
        gets(IsSend);

        //ͬʱ��״̬��Ϣ���͵������
        sendto(sock, IsSend, strlen(IsSend), 0, (struct sockaddr*)&NetAddr, sizeof(NetAddr));
        
        if(IsSend[0]=='1'){   
            printf("��ѡ������ʽ��\n1.�㲥��Ϣ  2.��Ե㷢��\n");

            char IsPTP[2];//�㲥��־
            memset(IsPTP,0,sizeof(IsPTP));
            gets(IsPTP);

            if(IsPTP[0]=='1'){
                printf("\n\n------------------������Ϣ-------------------\n\n");
                memset(buffer, 0, sizeof(buffer));
                memset(buf, 0, sizeof(buf));
                printf("��������Ҫ���͵ĵ���Ϣ: \n");
                gets(buf); 
                char mybuf[9]="00000";
                strcat(buffer,mybuf);
                strcat(buffer,buf);
                printf("%s\n",buffer);
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&NetAddr, sizeof(NetAddr));
                printf("���ͳɹ����밴���������......\n");
                getchar();
            }
            else{
                memset(TargetPort, 0, sizeof(TargetPort));
                memset(buffer, 0, sizeof(buffer));
                memset(buf, 0, sizeof(buf));
                printf("\n\n------------------������Ϣ-------------------\n\n");
                printf("������Ŀ�Ķ˿ںţ�\n");//����ֻ��Ҫ����Ŀ�Ķ˵������˿ںż���
                gets(TargetPort);
                printf("��������Ҫ���͵ĵ���Ϣ: \n");
                gets(buf);

                //�ϲ���Ϣ
                strcat(buffer,TargetPort);
                strcat(buffer,buf);
                printf("%s\n",buffer);
                sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&NetAddr, sizeof(NetAddr));
                printf("���ͳɹ����밴���������......\n");
                getchar();
                }    
        }
        else{ 
            memset(TargetPort, 0, sizeof(TargetPort));
            memset(buffer, 0, sizeof(buffer));
            memset(buf, 0, sizeof(buf));
            printf("\n\n------------------������Ϣ--------------------\n\n");
            recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&NetAddr, &NetLen);
            for(int i=0;i<5;i++){//��ȡԴ�˿ں�
                TargetPort[i]=buffer[i];
            }
            printf("�˿ں�%s������һ����Ϣ��\n",TargetPort);
            int i=0;
            for(int j=10;j<strlen(buffer);j++){//��ȡ��Ϣ
                buf[i++]=buffer[j];
            }
            printf("�յ�����ϢΪ: %s\n", buf);
            printf("�밴�������������������\n");
            getchar();
        }
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}
