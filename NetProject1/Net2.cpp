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
char head[10]="01111110";
char end[10]="01111110";
char data[10];
char check[10];
char Frame[MAX_BUFFER_SIZE];//֡ 
char RecFrame[MAX_BUFFER_SIZE];//����֡ 
char sendbuffer[MAX_BUFFER_SIZE];
char checkbuffer[MAX_BUFFER_SIZE];
char buffer[MAX_BUFFER_SIZE];  //������
char buf[MAX_BUFFER_SIZE];//������
char SourcePort[6];//Դ�˿ں�
int len;
char mmmmmmm[MAX_BUFFER_SIZE]="111111111111111111111111111111";
char Broadcast[MAX_BUFFER_SIZE]="000000000000000000000000000000";
uint Num1[MAX_BUFFER_SIZE];
//���롢����ģ��
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

//����Ƿ��������5��1�������س��ֵ�λ��
int FindTarget(char *str){
	int i,j,cunt=0,target=0;
	rep(i,0,strlen(str)){   
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
			break;//�ҵ�target���˳����� 
		}			
	} 		
	return target;
}

//Ѱ��֡
int FindFrame(char *str){
	int head=0;//��֡ͷ 
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
	int tail=head;//��֡β 
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
	rep(i,beginnum,endnum){//���ҳ�����֡����������֡��
		RecFrame[i]=str[i];
	}
} 

//֡��װ�����ģ��
namespace FRAME{
	int frame(char *str){//֡��װ
        int target=FindTarget(str);
        printf("target of strΪ��%d\n",target);
		char Str[10];
		memset(Str,0,sizeof(Str));
		if(target){
			rep(i,0,target){
				Str[i]=str[i];
			}
			Str[target+1]='0';//����5��'1'֮���� 
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
	int deframe(char *str){//���װ
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
        printf("target of dataΪ��%d\n",target1);
        int target2=FindTarget(check1);
        printf("target of checkΪ��%d\n",target2);
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
	printf("\n\n-----------------�����ģ��------------------\n\n");
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); 

	memset(sendbuffer, 0, sizeof(sendbuffer));
    memset(buffer, 0, sizeof(buffer));
	memset(buf, 0, sizeof(buf));
    memset(checkbuffer,0,sizeof(checkbuffer));

    
    srand((unsigned) time(NULL));
    int randnum=rand()%500+1; //��������� 
    
    //�����׽���
    SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    
    int PhyPort=14100,NetPort=14500,AppPort=14300;

    /*---------------------�ָ���-------------------*/

    //Ӧ�ò���Ϣ
    printf("������Ӧ�ò�˿ںţ�\n");
    printf("%d\n",AppPort);
    sockaddr_in AppAddr;
    memset(&AppAddr, 0, sizeof(AppAddr));
    int AppLen=sizeof(AppAddr);
    AppAddr.sin_family = PF_INET;
    AppAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    AppAddr.sin_port = htons(AppPort);

    /*---------------------�ָ���-------------------*/

    //�������Ϣ 
    printf("�����������˿ڣ�\n");
	printf("%d\n",NetPort);
    sockaddr_in NetAddr;
    memset(&NetAddr, 0, sizeof(NetAddr));
    NetAddr.sin_family = PF_INET;
    NetAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    NetAddr.sin_port = htons(NetPort);
    bind(sock, (SOCKADDR*)&NetAddr, sizeof(SOCKADDR)); //�󶨶˿� 

    /*---------------------�ָ���-------------------*/

    //�������Ϣ
    printf("�����������˿ںţ�\n");
    printf("%d\n",PhyPort);
    sockaddr_in PhyAddr;
    memset(&PhyAddr, 0, sizeof(PhyAddr));
    int PhyLen=sizeof(PhyAddr);
    PhyAddr.sin_family = PF_INET;
    PhyAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    PhyAddr.sin_port = htons(PhyPort);

	/*---------------------�ָ���-------------------*/

	//��������������ͨ��
	sendto(sock,"connect",8,0,(sockaddr*)&PhyAddr,sizeof(PhyAddr));
	int retval = recvfrom(sock,buffer,MAX_BUFFER_SIZE,0,(sockaddr*)&PhyAddr,&PhyLen);
	if(retval == SOCKET_ERROR){
		printf("�ӿ�API����ʧ��\n");
		return 0;
	}
	//�н��գ��Ϳ�����Ϊ�����ɹ�
	printf("�ӿ�API���ӳɹ���\n");
    memset(buffer, 0, sizeof(buffer));

    while(1){
    	uint num[32];
      	uint num1[32];
	    int len1=1;
    	int len2=0;
        char IsSend[2];
		int i,cunt=0,framenum=0,resendnum=0,rerecivenum=0,recivenum=0;
        memset(IsSend,0,sizeof(IsSend));
		printf("���ڵȴ�Ӧ�ò��״̬��Ϣ\n");
    	recvfrom(sock, IsSend, 2, 0, (SOCKADDR*)&AppAddr, &AppLen);

        if(IsSend[0]=='1'){
           	printf("\n\n------------���ڽ���Ӧ�ò����Ϣ------------\n\n") ; 

	        memset(buffer, 0, sizeof(buffer));
            recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&AppAddr, &AppLen);
            printf("��Ӧ�ò��յ�����ϢΪ��%s\n",buffer);

			memset(buf,0,sizeof(buf));
			rep(i,0,4){              //��ȡĿ�Ķ˿ں�
				buf[i]=buffer[i];
			}
			char mybuf[9]="00000";
			if(!strcmp(buf,mybuf)){//�Ա�Ŀ�Ķ˿ںţ��ж��Ƿ�ΪȺ����Ϣ
				printf("����ϢΪȺ����Ϣ��\n");
				itoa(PhyPort,SourcePort,10);//�����ض˿ں�ת��Ϊ�ַ�������
				memset(buf, 0, sizeof(buf));
			    strcat(buf,SourcePort);
			    strcat(buf,buffer);

			    printf("��������ϢΪ��%s\n�����뽻������������.......\n",buf);
				memset(buffer,0,sizeof(buffer));
			    uint Num[MAX_BUFFER_SIZE],Len=1;

                //��Դ�˿ںź�Ŀ�Ķ˿ںŷ�����������,�����Ŀ�Ķ˿ں���Ϊһ��Ⱥ����־
			    rep(i,0,9){
                    buffer[i]=buf[i];
			    }
			    printf("%s\n",buffer);

                //���˿ں���Ϣת����������
			    for(int i=0;i<strlen(buffer);i++){
	                Num[Len++]=buffer[i];
		        }
                memset(sendbuffer, 0, sizeof(sendbuffer));
		        CODE::code(Num,Len-1,sendbuffer);//ת�����
			    printf("%s\n",sendbuffer);

                //����������������֡��װ
			    char Buffer[MAX_BUFFER_SIZE];
			    memset(Buffer,0,sizeof(Buffer));
			    strcat(Buffer,head);
			    strcat(Buffer,sendbuffer);
			    strcat(Buffer,end);
			    sendto(sock, Buffer, strlen(Buffer), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));//
			    memset(buffer,0,sizeof(buffer));
                retval=recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
			    if(retval){
				    printf("%s\n",buffer);//���ӷ�����Ϣ
				    printf("���ӳɹ�����������ת�����......\n\n");
			    }
			    else{
				    printf("����ʧ�ܣ�\n");
			    }

				int number=1;
                //���ַ�תΪASCII�� 
                for(int i=0;i<strlen(buf);i++){
	                num[len1++]=buf[i];
		        }
		        memset(sendbuffer, 0, sizeof(sendbuffer));
		        CODE::code(num,len1-1,sendbuffer);//ת�����
				memset(buffer,0,sizeof(buffer));
				strcat(buffer,head);
				strcat(buffer,sendbuffer);
				strcat(buffer,end);
				sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
				printf("Ⱥ���ɹ���\n");
				printf("�Ͽ����ӳɹ���\n");
			}
			else{
				printf("����ϢΪ�㵽����Ϣ��\n");
			    memset(buf, 0, sizeof(buf));

                itoa(PhyPort,SourcePort,10);//���˿ں�ת��Ϊ�ַ�������
			    memset(buf, 0, sizeof(buf));
			    strcat(buf,SourcePort);
			    strcat(buf,buffer);
			    printf("��������ϢΪ��%s\n�����뽻������������.......\n",buf);
			    memset(buffer,0,sizeof(buffer));
			    uint Num[MAX_BUFFER_SIZE],Len=1;

                //��Դ�˿ںź�Ŀ�Ķ˿ںŷ�����������
			    rep(i,0,9){
                    buffer[i]=buf[i];
			    }
			    printf("%s\n",buffer);
			    for(int i=0;i<strlen(buffer);i++){
	                Num[Len++]=buffer[i];
		        }
                memset(sendbuffer, 0, sizeof(sendbuffer));
		        CODE::code(Num,Len-1,sendbuffer);//ת�����
			    printf("%s\n",sendbuffer);
			    char Buffer[MAX_BUFFER_SIZE];
			    memset(Buffer,0,sizeof(Buffer));
			    strcat(Buffer,head);
			    strcat(Buffer,sendbuffer);
			    strcat(Buffer,end);
			    sendto(sock, Buffer, strlen(Buffer), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
			    memset(buffer,0,sizeof(buffer));
                retval=recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
			    if(retval){
				    printf("%s\n",buffer);
				    printf("���ӳɹ�����������ת�����......\n\n");
			    }
			    else{
				    printf("����ʧ�ܣ�\n");
			    }
                int number=1;
                //���ַ�תΪASCII�� 
                for(int i=0;i<strlen(buf);i++){
	                num[len1++]=buf[i];
		        }
		        memset(sendbuffer, 0, sizeof(sendbuffer));
		        CODE::code(num,len1-1,sendbuffer);//ת�����
		        printf("ת��ɹ���������Ϊ:%s\n������֡���ͣ�.............\n",sendbuffer);

			    while(1){	//ÿ��0.5�뷢��һ��������Ϣ	
				    printf("����������.........\n"); 
                    sendto(sock, "11111111", 8, 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
                    memset(buffer, 0, sizeof(buffer));
                    recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
                    if(buffer[1]=='0'){
        	            printf("�Է�æ.....���Ժ���\n");
			            Sleep(500); 
		            }
				    else{	
			            printf("�ɷ�����Ϣ...\n");
					    break;
				    }
			    } 
			
			    //���ͳ�����Ϣ
			    char STRLEN[MAX_BUFFER_SIZE];
			    memset(STRLEN,0,sizeof(STRLEN));
			    itoa(strlen(sendbuffer)/8,STRLEN,10);
			    printf("%s\n",STRLEN);
			    Len=1;
			    for(int i=0;i<strlen(STRLEN);i++){
	                Num[Len++]=STRLEN[i];
		        }
			    memset(buffer, 0, sizeof(buffer));
		        CODE::code(Num,Len-1,buffer);//ת�����
                memset(buf,0,sizeof(buf));
			    strcat(buf,head);
			    strcat(buf,buffer);
			    strcat(buf,end);
			    printf("���͵ĳ�����ϢΪ��%s\n",buf);
			    printf("һ��Ҫ����%d֡\n",strlen(sendbuffer)/8);
			    sendto(sock, buf, strlen(buf), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
			    memset(buf,0,sizeof(buf));
			    recvfrom(sock, buf,MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
			    if(buf[0]=='1'){
				    printf("�Է����ճɹ���\n");
			    }
			    else{
				    printf("�Է�����ʧ�ܣ�\n");
			    }

                char str[9];
                memset(str, 0, sizeof(str));

                rep(i,0,strlen(sendbuffer)){//��֡����
        	        str[cunt++]=sendbuffer[i];
        	        if(cunt>=8){
        		        cunt=0;//���ü�����      

        	            FRAME::frame(str); 
        	            printf("��%d֡�ѷ�װ��ϣ���װ֡Ϊ��%s\n��������\n",++framenum,Frame);
        	            while(1){
							char TFrame[MAX_BUFFER_SIZE];
							memset(TFrame,0,sizeof(TFrame));
							strcpy(TFrame,Frame);				
							randnum=rand()%500+1;
						    if(randnum<10){
							    if(TFrame[16]=='0')TFrame[16]='1';
								else TFrame[16]='0';
						    }
        	                sendto(sock, TFrame, strlen(TFrame), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
        	                printf("������ϣ����ڵȴ�ȷ��.......\n");
        	                recvfrom(sock, checkbuffer,MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
        	                if(checkbuffer[5]=='1'){
        	    	            printf("ȷ�ϳɹ�����%d֡�������\n",framenum);
        	    	            break;
			    	        }
				            printf("�ڷ��͹����г�����һ��С���󣬼����ط���֡\n");
				            resendnum++;//�ط�����+1 
            	        }
			        }
		        }
		        printf("������ϣ�һ������%d֡���ط�%d��\n",framenum,resendnum); 
			    printf("�����뽻�����Ͽ�����\n");
			    sendto(sock, mmmmmmm, strlen(mmmmmmm), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
			    printf("�Ͽ����ӳɹ���\n");
            }
		}
        else{
			memset(sendbuffer, 0, sizeof(sendbuffer));
		    printf("\n\n---------���ڴ�����������Ϣ-----------\n\n");
			while(1){//����������Ϣ
			    memset(buf,0,sizeof(buf));
				recvfrom(sock, buf, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
				if(!strcmp(buf,Broadcast)){
					printf("��������һ��Ⱥ����Ϣ��\n");
					memset(buffer,0,sizeof(buffer));
                    recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
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
                    for(int j=1;j<=n;j++){
		               buffer[len2++]=char(Num1[j]);
	                }
					printf("ת��ɹ�����ϢΪ��%s\n���ڽ���Ϣ����Ӧ�ò㣡..............\n",buffer); 
                    sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&AppAddr, sizeof(AppAddr));
					break;
				}
				else{
					printf("���յ��˽�������...\n");		
		            randnum=rand()%500+1;
		            if(randnum%2==0){//ͨ������һ�����������Ȼ��ͨ����ż���������Ƿ���Խ�����Ϣ
		                sendto(sock, "00000000", strlen("00000000"), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
		                printf("��æ....�޷�������Ϣ\n");
		            }
		            else{
		                sendto(sock, "11111111", strlen("11111111"), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
				        printf("�ɽ�����Ϣ.......");
					    break;
		            }
				}    	   
			}
			if(!strcmp(buf,Broadcast)){//�յ�Ⱥ����Ϣ�����˳��ò�ѭ��
				continue;
			}
			uint Num1[MAX_BUFFER_SIZE];
			memset(Num1,0,sizeof(Num1));
            printf("���ڽ��ճ�����Ϣ\n");
			memset(buffer, 0, sizeof(buffer));
            recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
			sendto(sock, "111111111", 9, 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
			int j=0;
			memset(sendbuffer,0,sizeof(sendbuffer));
			rep(i,8,strlen(buffer)-9){
            sendbuffer[j++]=buffer[i];
            }
			printf("%s\n",sendbuffer);
			memset(buffer,0,sizeof(buffer));
            int ll=CODE::decode(Num1,sendbuffer,strlen(sendbuffer));//���
			memset(sendbuffer,0,sizeof(sendbuffer));
			len2=0;
			printf("%d\n",ll);
			for(int j=1;j<=ll;j++){
		    buffer[len2++]=char(Num1[j]);
	        }
			printf("%s\n",buffer);
			int num11=atoi(buffer);
			printf("��������֡����%d����Ϣ\n",num11);
			i=1;
            while(1){ 
				    printf("���ڽ��յ�%d֡\n",i++);
       	            recivenum++;
       	            while(1){
       	                memset(buffer, 0, sizeof(buffer));
                        recvfrom(sock, buffer, MAX_BUFFER_SIZE, 0, (SOCKADDR*)&PhyAddr, &PhyLen);
                        printf("���ճɹ����������н��......\n");
                        FindFrame(buffer);//Ѱ��֡ 
                        FRAME::deframe(RecFrame);//���֡ 
                        printf("���������Ϊ��%s  У������Ϊ��%s",data,check);
                        printf("���ɹ������ڽ���У��....\n");
                        if(!strcmp(data,check)){
                            printf("У��ɹ���");
                            sendto(sock, "111111111", strlen("111111111"), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
                            strcat(sendbuffer,data);
                            break;
			            }
			            printf("У��ʧ�ܣ��������ʹ�����Ϣ........\n");
			            sendto(sock, "000000000", strlen("000000000"), 0, (SOCKADDR*)&PhyAddr, sizeof(PhyAddr));
			            rerecivenum++;//���½�����Ϣ����+1 
			            printf("���ͳɹ����������½�����Ϣ\n");
                   }
				   if(i>num11){
					   break;
				   }
		    }
            printf("������%d֡�����½���%d��\n",recivenum,rerecivenum);
            printf("����bit��Ϊ��%s\n��������ת��",sendbuffer);
            //������Ϣ 
            memset(buffer, 0, sizeof(buffer));

            int n=CODE::decode(num1,sendbuffer,strlen(sendbuffer));//���
			len2=0;
            //��ASCII��תΪ�ַ����� 
	        for(int j=1;j<=n;j++){
		    buffer[len2++]=char(num1[j]);
	        }
			
	        printf("ת��ɹ�����ϢΪ��%s\n���ڽ���Ϣ����Ӧ�ò㣡..............\n",buffer); 
            sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR*)&AppAddr, sizeof(AppAddr));   
        }
	}
    closesocket(sock);
    WSACleanup();
    return 0;
}
