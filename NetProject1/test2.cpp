#include<winsock2.h> 
#include<stdio.h>
#include<windows.h>
#include<string.h>
#include<time.h>
#include<unistd.h>
#include<stdlib.h>
#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll
#define MAX_BUFFER_SIZE 50000 //缓冲的最大大小
#define rep(i,x,y) for(int i=x;i<=y;i++)
using namespace std;
typedef unsigned int uint;
char head[10]="01111110";
char end[10]="01111110";
char data[10];
char check[10];
char Frame[MAX_BUFFER_SIZE];//帧 
char RecFrame[MAX_BUFFER_SIZE];//接收帧 

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
				s[pos++] = (tmp&val) ? 1 : 0;
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
				num[top] = (num[top] << 1) + (s[i + j] - 0);
			}
		}
		return top;
	}
}
int FindFrame(char *str){
	int head=0;//找帧头 
	while(str[head]=='1')head++;
	while(str[head++]=='0')
	{
		int one=0;
		while(str[head]=='1')one++,head++;
		if(one==6)
		{
			head++;
			break;
		}
	}
	int beginnum=head-8; 
	int tail=head;//找帧尾 
	while(str[tail]=='1')tail++;
	while(str[tail++]=='0')
	{
		int one=0;
		while(str[tail]=='1')one++,tail++;
		if(one==6)
		{
			tail-=8;
			break;
		}
	}
	int endnum=tail+8;
	int i;
	memset(RecFrame,0,sizeof(RecFrame));
	rep(i,beginnum,endnum){
		RecFrame[i]=str[i];
	}
} 

int FindTarget(char *str){
	int i,j,cunt=0,target=0;
	rep(i,0,3){//因为是寻找连续的5个'1'，这里只需遍历到3即可 
		if(str[i]=='1'){
			rep(j,i,7){
				if(str[j]=='1'){
					cunt++;
				}	
				else{
					cunt=0;
					break;
				}			
				if(cunt>=5){
					target=j;
					break;
				} 
			}
		}
		if(target){
			break;//找到target便退出遍历 
		}			
	} 		
	return target;
}
namespace FRAME{
	int frame(char *str){
        int target=FindTarget(str);
        printf("target为：%d\n",target);
		char Str[10];
		memset(Str,0,sizeof(Str));
		if(target){
			rep(i,0,target){
				Str[i]=str[i];
			}
			Str[target+1]='0';//出现5个'1'之后补零 
			if(target<7){
		    	rep(i,target+1,7) {
				    Str[i+1]=str[i];
		    	}
		    }
		}
		else{
			strcpy(Str,str);
		} 
		memset(Frame, 0, sizeof(Frame));
		strcat(Frame,head);
		strcat(Frame,Str);
		strcat(Frame,Str);
		strcat(Frame,end);
	}
	int deframe(char *str){
		int i,j=0;
		char data1[10];
		char check1[10];
		memset(data1, 0, sizeof(data1));
		memset(check1, 0, sizeof(check1));
		int len=strlen(str);
		rep(i,8,len/2-1){
			data1[j++]=str[i];
		}
		j=0;
		rep(i,len/2,len-9){
			check1[j++]=str[i];
		}
        int target1=FindTarget(data1);
        printf("target1为：%d\n",target1);
        int target2=FindTarget(check1);
        printf("target2为：%d\n",target2);
		memset(data, 0, sizeof(data));
		memset(check, 0, sizeof(check));
		if(target1){
			rep(i,0,target1){
			    data[i]=data1[i];
			}	
			if(target1<7){
			    rep(i,target1+2,len/2-9){
				    data[i-1]=data1[i];
			  }
			}	
		} 
		else{
			strcpy(data,data1);
		}
		if(target2){
		    rep(i,0,target2){
			    check[i]=check1[i];
			}	
			if(target2<7){
			    rep(i,target2+2,len/2-9){
				    check[i-1]=check1[i];
			   }
		    }	
		} 
		else{
			strcpy(check,check1);
		}
	}
}
int main(){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); 

    int len;
	char buffer[MAX_BUFFER_SIZE];  //缓冲区
	char sendbuffer[MAX_BUFFER_SIZE];
	memset(sendbuffer, 0, sizeof(sendbuffer));
    memset(buffer, 0, sizeof(buffer));
    
    //创建套接字
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    int PhyPort=12100,NetPort=12400;
    //网络层信息 
    printf("请设置网络层端口：\n");
	printf("%d\n",NetPort);
    sockaddr_in NetAddr;
    memset(&NetAddr, 0, sizeof(NetAddr));
    NetAddr.sin_family = PF_INET;
    NetAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    NetAddr.sin_port = htons(NetPort);
    bind(sock, (SOCKADDR*)&NetAddr, sizeof(SOCKADDR)); //绑定端口 
    
    //物理层信息
    printf("请输入物理层端口号：\n");
    printf("%d\n",PhyPort);
    sockaddr_in PhyAddr;
    memset(&PhyAddr, 0, sizeof(PhyAddr));
    int PhyLen=sizeof(PhyAddr);
    PhyAddr.sin_family = PF_INET;
    PhyAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    PhyAddr.sin_port = htons(PhyPort);
	
    /*sendto(sock,"connect",8,0,(sockaddr*)&PhyAddr,sizeof(PhyAddr));
	int retval = recvfrom(sock,buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr,&PhyLen);
	if(retval == SOCKET_ERROR){
		printf("接口API关联失败\n");
		return 0;
	}
	//有接收，就可以认为关联成功
	printf("接口API连接成功！\n");
    memset(buffer, 0, sizeof(buffer));*/
	while(1){
        memset(buffer,0,sizeof(buffer));
		gets(buffer);
        printf("%s\n",buffer);
		printf("%d\n",strlen(buffer));
		sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
	}
	return 0;
}
