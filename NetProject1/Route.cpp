#include<winsock2.h> 
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define rep(i,x,y) for(int i=x;i<=y;++i) 
typedef unsigned int uint;
#define MAX_BUFFER_SIZE 50000
char buffer[MAX_BUFFER_SIZE];//������
char buf[MAX_BUFFER_SIZE];//������
char sendbuffer[MAX_ACL_REVISION];//������
char mmmmmmm[MAX_BUFFER_SIZE]="111111111111111111111111111111";
char Broadcast[MAX_BUFFER_SIZE]="000000000000000000000000000000";
char mybuf[9]="00000";
int RoutingList[20][20];//·�ɱ�,�潻�����˿ںźͽ������µ�������Ԫ�˿ں�
namespace CODE {
	int code(uint *num, int n, char *s)
	{
		if (n > MAX_BUFFER_SIZE) return -1;
		int pos = 0;
		rep(i, 1, n)
		{
			uint val = num[i];
			uint tmp = 1u << 7;
			rep(j, 1, 8)
			{
				s[pos++] = (tmp&val) ? '1' : '0';
				tmp >>= 1;
			}
		}
		s[pos + 1] = 0;
		return 0;
	}

	int decode(uint *num, char *s, int l)
	{
		int top = 0;
		for (int i = 0; i < l; i += 8)
		{
			++top; num[top] = 0;
			rep(j, 0, 7) {
				num[top] = (num[top] << 1) + (s[i + j] - '0');
			}
		}
		return top;
	}
}

int UpdateRoutingList(int Swicthnum,int Port){
    rep(i,1,20){
        if(RoutingList[Swicthnum][i]!=0){//���жϸö˿ں��Ƿ���ڣ����ھ��˳�
            if(RoutingList[Swicthnum][i]==Port){
                break;
            }
        }
        else{
            if(RoutingList[Swicthnum][i]==0){//��Ӷ˿ں�
                RoutingList[Swicthnum][i]=Port;
            }
        }
    }
}
int main(){
    printf("\n\n----------------·����ģ��---------------\n\n");
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); 

    SOCKET sock[2];
    rep(i,1,2){
        sock[i] = socket(PF_INET, SOCK_DGRAM, 0);
    }

    FD_SET rfds;
    timeval timeout;
    timeout.tv_sec=1;
    timeout.tv_usec = 0;



    /*-----------------��ʼ��·�ɱ�---------------*/
    RoutingList[1][0]=18100;
    RoutingList[1][1]=11100;
    RoutingList[1][2]=13100;
    RoutingList[1][3]=15100;
    RoutingList[2][0]=18102;
    RoutingList[2][1]=11102;
    RoutingList[2][2]=13102;


    memset(buffer,0,sizeof(buffer));
    int MyLen[4];
    int PhyLen[4];
    int MyPort[4],PhyPort[32],SourcePort=0,TargetPort=0,retval,len2=0;
    MyPort[1]=50100;
    MyPort[2]=50200;
    uint Num1[MAX_BUFFER_SIZE];
    PhyPort[1]=18100;
    PhyPort[2]=18102;   
    sockaddr_in MyAddr[5];
    sockaddr_in PhyAddr[5];
    rep(i,1,2){   
        printf("��󶨵�%d�˿ںţ�\n",i);
        printf("%d\n",MyPort[i]);

        memset(&MyAddr[i], 0, sizeof(MyAddr[i]));
        MyLen[i]=sizeof(MyAddr[i]);
        MyAddr[i].sin_family = PF_INET;
        MyAddr[i].sin_addr.s_addr = inet_addr("127.0.0.1"); 
        MyAddr[i].sin_port = htons(MyPort[i]);
        bind(sock[i], (SOCKADDR*)&MyAddr[i], sizeof(SOCKADDR)); //�󶨶˿� 

        memset(&PhyAddr[i], 0, sizeof(PhyAddr[i]));
        PhyLen[i]=sizeof(PhyAddr[i]);
        PhyAddr[i].sin_family = PF_INET;
        PhyAddr[i].sin_addr.s_addr = inet_addr("127.0.0.1"); 
        PhyAddr[i].sin_port = htons(PhyPort[i]);
       sendto(sock[i],"connect",8,0,(sockaddr*)&PhyAddr[i],sizeof(PhyAddr[i]));
        memset(buffer,0,sizeof(buffer));
	    retval=recvfrom(sock[i],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[i],&PhyLen[i]);
	    if(retval == SOCKET_ERROR){
		    printf("�ӿ�API%d����ʧ��\n",i);
	    }
	    //�н��գ��Ϳ�����Ϊ�����ɹ�
        printf("%s\n",buffer);
	    printf("�ӿ�API%d���ӳɹ���\n",i);
    }

    while(1){
        rep(i,1,2){//�ŶӼ��ÿ���˿��Ƿ���ܵ�����Ϣ
            FD_ZERO(&rfds);
			FD_SET(sock[i],&rfds);
            memset(buffer,0,sizeof(buffer));
            int ret=select(sock[i],&rfds,NULL,NULL,&timeout);
			if(ret==0){
			    continue;//�����ʱ���������˿ڼ����һ���˿�
			} 
            if(FD_ISSET(sock[i],&rfds)){//�����⵽�н���
                memset(buffer,0,sizeof(buffer));
                recvfrom(sock[i],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[i],&PhyLen[i]);
                if(!strcmp(buffer,Broadcast)){
                    printf("����ϢΪȺ����Ϣ\n");
                    memset(buffer,0,sizeof(buffer));
                    recvfrom(sock[i],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[i],&PhyLen[i]);

                    /*������ݹ㲥Դ�˿ںŸ��»����·�ɱ�����*/
                    memset(buf,0,sizeof(buf));
                    strcpy(buf,buffer);
                    memset(sendbuffer,0,sizeof(sendbuffer));
                    int j=0;
                    //��ȡ��Ч��Ϣ
                    rep(i,8,strlen(buffer)-9){
                        sendbuffer[j++]=buffer[i];
                    }

                    int n=CODE::decode(Num1,sendbuffer,strlen(sendbuffer));//���
                    len2=0;
                    memset(buffer,0,sizeof(buffer));
                    for(int j=1;j<=5;j++){
		                buffer[len2++]=char(Num1[j]);
	                }
                    SourcePort=atoi(buffer);
                    printf("SourcePort is:%d\n",SourcePort);
                    UpdateRoutingList(i,SourcePort);//����·�ɱ�
                    rep(j,1,2){//ѭ������Ϣ������ÿ���˿�
                        if(j==i)
                        continue;
                        else{
                            sendto(sock[j], Broadcast, strlen(Broadcast), 0, (SOCKADDR*)&PhyAddr[j], sizeof(PhyAddr[j]));//�ȷ���Ⱥ������
                            sendto(sock[j], buf, strlen(buf), 0, (SOCKADDR*)&PhyAddr[j], sizeof(PhyAddr[j]));//����Ⱥ����Ϣ
                        }
                    }
                    printf("�ѶϿ����ӣ�\n");
                    continue;
                }
                memset(buf,0,sizeof(buf));
                strcpy(buf,buffer);
                sendto(sock[i],"11111111",24,0,(sockaddr*)&PhyAddr[i],sizeof(PhyAddr[i]));//��������Դ��������
                printf("���յ��Ķ˿ں�bit��Ϊ��%s\n",buffer);
                memset(sendbuffer,0,sizeof(sendbuffer));
                int j=0;
                //��ȡ��Ч��Ϣ
                rep(i,8,strlen(buffer)-9){
                    sendbuffer[j++]=buffer[i];
                }
                printf("%s\n",sendbuffer);

                int n=CODE::decode(Num1,sendbuffer,strlen(sendbuffer));//���
                len2=0;
                memset(buffer,0,sizeof(buffer));
                for(int j=1;j<=5;j++){
		            buffer[len2++]=char(Num1[j]);
	            }
                SourcePort=atoi(buffer);
                printf("SourcePort is:%d\n",SourcePort);
                len2=0;
                memset(buffer,0,sizeof(buffer));
                for(int j=6;j<=10;j++){
		            buffer[len2++]=char(Num1[j]);
	            }
                TargetPort=atoi(buffer);
                printf("TargetPort is:%d\n",TargetPort);
                int SwitchNum=0;

                rep(i,0,20){//���Ѱ��Ŀ�Ķ˿ںŵĽ��������
                    rep(j,0,20){
                        if(RoutingList[i][j]==TargetPort){
                            SwitchNum=i;
                            break;
                        }
                    }
                    if(SwitchNum){
                        break;
                    }
                }
                //��Ŀ�Ľ�������������
                sendto(sock[SwitchNum], buf, strlen(buf), 0, (SOCKADDR*)&PhyAddr[SwitchNum], sizeof(PhyAddr[SwitchNum]));
                recvfrom(sock[SwitchNum],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[SwitchNum],&PhyLen[SwitchNum]);
                while(1){  
                    memset(buffer,0,sizeof(buffer));
                    recvfrom(sock[i],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[i],&PhyLen[i]);
                    printf("��Դ�˿ںŽ��յ�����ϢΪ��%s\n",buffer);
                    if(!strcmp(buffer,mmmmmmm)){
                        sendto(sock[SwitchNum],buffer,strlen(buffer),0,(sockaddr*)&PhyAddr[SwitchNum],sizeof(PhyAddr[SwitchNum]));
                        printf("�ѶϿ����ӣ�\n");
                        break;
                    }
                    sendto(sock[SwitchNum],buffer,strlen(buffer),0,(sockaddr*)&PhyAddr[SwitchNum],sizeof(PhyAddr[SwitchNum]));
                    memset(buffer,0,sizeof(buffer));
                    recvfrom(sock[SwitchNum],buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr[SwitchNum],&PhyLen[SwitchNum]);
                    printf("��Ŀ�Ķ˿ںŽ��յ�����ϢΪ��%s\n",buffer);
                    sendto(sock[i],buffer,strlen(buffer),0,(sockaddr*)&PhyAddr[i],sizeof(PhyAddr[i]));
                } 
            }           
        }
    }
    return 0;
}