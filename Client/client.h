#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED
#include <iostream>
#include <winsock2.h>
#include <process.h>

#pragma comment(lib, "WS2_32.lib")
using namespace std;

//�궨��
#define	SERVERIP			"192.168.1.103"		//������IP
#define	SERVERPORT			5555			//������TCP�˿�
#define	MAX_NUM_BUF			60				//����������󳤶�

//��������
BOOL InitClient(void);              //��ʼ��
void InitMember(void);              //��ʼ��ȫ�ֱ���
BOOL InitSockt(void);               //�������׽���
BOOL ConnectServer(void);           //���ӷ�����
bool RecvLine(SOCKET s, char* buf); //��ȡһ������
bool sendData(SOCKET s, char* str); //��������
void recvAndSend(void);             //���ݴ�����
bool recvData(SOCKET s, char* buf);
void ShowConnectMsg(BOOL bSuc);     //���Ӵ�ӡ����
void ExitClient(void);              //�˳�������
DWORD __stdcall	RecvDataThread(void* pParam);
DWORD __stdcall	SendDataThread(void* pParam);
BOOL	CreateSendAndRecvThread(void);
void	InputAndOutput(void);


#endif
