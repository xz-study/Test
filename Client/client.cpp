#include "client.h"
#include <iostream>
#include <winsock2.h>
#include <process.h>

#pragma comment(lib, "WS2_32.lib")
using namespace std;

//����
SOCKET	sClient;							//�׽���
HANDLE	hThreadSend;						//���������߳�
HANDLE	hThreadRecv;						//���������߳�
char    bufSend[MAX_NUM_BUF];				//�������ݻ�����
BOOL    bSend = FALSE;                      //���ͱ��λ
BOOL	bConnecting;						//�������������״̬
HANDLE	arrThread[2];						//���߳�����
CRITICAL_SECTION cs;					//�ٽ�����������bufSend

/**
 *	��ʼ��
 */
BOOL InitClient(void)
{
	//��ʼ��ȫ�ֱ���
	InitMember();

	//����SOCKET
	if (!InitSockt())
	{
		return FALSE;
	}

	return TRUE;
}
/**
 * ��ʼ��ȫ�ֱ���
 */
void InitMember(void)
{
	InitializeCriticalSection(&cs);

	sClient = INVALID_SOCKET;	//�׽���
    hThreadRecv = NULL;			//���������߳̾��
	hThreadSend = NULL;			//���������߳̾��
	bConnecting = FALSE;		//Ϊ����״̬

    //��ʼ�����ݻ�����
	memset(bufSend, 0, MAX_NUM_BUF);
	memset(arrThread, 0, 2);
}

/**
 * �����������׽���
 */
BOOL  InitSockt(void)
{
	int			reVal;	//����ֵ
	WSADATA		wsData;	//WSADATA����
	reVal = WSAStartup(MAKEWORD(2,2),&wsData);//��ʼ��Windows Sockets Dll

	//�����׽���
	sClient = socket(AF_INET, SOCK_STREAM, 0);
	if(INVALID_SOCKET == sClient)
		return FALSE;


	//�����׽��ַ�����ģʽ
	unsigned long ul = 1;
	reVal = ioctlsocket(sClient, FIONBIO, (unsigned long*)&ul);
	if (reVal == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

/**
 * ���ӷ�����
 */
BOOL ConnectServer(void)
{
	int reVal;			//����ֵ
	sockaddr_in serAddr;//��������ַ
	//����Ҫ���ӵ�������ַ
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(SERVERPORT);
    serAddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);

	while(true)
	{
		//���ӷ�����
		reVal = connect(sClient, (struct sockaddr*)&serAddr, sizeof(serAddr));
		//�������Ӵ���
		if(SOCKET_ERROR == reVal)
		{
			int nErrCode = WSAGetLastError();
			if( WSAEWOULDBLOCK == nErrCode || WSAEINVAL == nErrCode)    //���ӻ�û�����
			{
				continue;
			}
			else if (WSAEISCONN == nErrCode)//�����Ѿ����
			{
				break;
			}
			else//����ԭ������ʧ��
			{
				return FALSE;
			}
		}

		if ( reVal == 0 )//���ӳɹ�
			break;
	}

	bConnecting = TRUE;

	return TRUE;
}
/**
 * �������ͺͽ��������߳�
 */
BOOL	CreateSendAndRecvThread(void)
{
	//�����������ݵ��߳�
	unsigned long ulThreadId;
	hThreadRecv = CreateThread(NULL, 0, RecvDataThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadRecv)
		return FALSE;

	//�����������ݵ��߳�
	hThreadSend = CreateThread(NULL, 0, SendDataThread, NULL, 0, &ulThreadId);
	if (NULL == hThreadSend)
		return FALSE;

	//��ӵ��߳�����
	arrThread[0] = hThreadRecv;
	arrThread[1] = hThreadSend;
	return TRUE;
}
/**
 * ���������߳�
 */
DWORD __stdcall	RecvDataThread(void* pParam)
{
	int		reVal;				    //����ֵ
	char    bufRecv[MAX_NUM_BUF];   //�������ݻ�����

	while(bConnecting)			    //����״̬
	{
        memset(bufRecv, 0, MAX_NUM_BUF);
		reVal = recv(sClient, bufRecv, MAX_NUM_BUF, 0);//��������
		if (SOCKET_ERROR == reVal)
		{
			int nErrCode = WSAGetLastError();
			if (WSAEWOULDBLOCK == nErrCode)			//�������ݻ�����������
			{
				continue;							//������������
			}else{
				bConnecting = FALSE;
				return 0;							//�߳��˳�
			}
		}

		if ( reVal == 0)							//�������ر�������
		{
			ShowConnectMsg(FALSE);
            bConnecting = FALSE;
            bSend = FALSE;
            memset(bufRecv, 0, MAX_NUM_BUF);		//��ս��ջ�����
            ExitClient();
			return 0;								//�߳��˳�
		}
		if(reVal > 0)
        {

            if(('E'==bufRecv[0] || 'e'==bufRecv[0]))     //�ж��Ƿ��˳�
            {
                ShowConnectMsg(FALSE);
                bConnecting = FALSE;
                bSend = FALSE;
                memset(bufRecv, 0, MAX_NUM_BUF);		//��ս��ջ�����
                ExitClient();
            }
            //��ʾ����
            cout<<bufRecv<<endl;
        }
	}
	return 0;
}
/**
 * ���������߳�
 */
DWORD __stdcall	SendDataThread(void* pParam)
{
	while(bConnecting)						//����״̬
	{
		if (bSend)						//��������
		{
            EnterCriticalSection(&cs);	//�����ٽ���
			while(TRUE)
            {
                int val = send(sClient, bufSend, MAX_NUM_BUF,0);

                //�����ش���
                if (SOCKET_ERROR == val)
                {
                    int nErrCode = WSAGetLastError();
                    if(WSAEWOULDBLOCK == nErrCode)		//���ͻ�����������
                    {
                        continue;						//����ѭ��
                    }else
                    {
                        LeaveCriticalSection(&cs);	//�뿪�ٽ���
                        return 0;
                    }
                }

                    bSend = FALSE;			//����״̬
                    break;					//����for
            }
            LeaveCriticalSection(&cs);	//�뿪�ٽ���
		}
    }
	return 0;
}
/**
 * �������ݺ���ʾ���
 */
void	InputAndOutput(void)
{
    char cInput[MAX_NUM_BUF];	//�û����뻺����
    while(bConnecting)			//����״̬
	{
		memset(cInput, 0, MAX_NUM_BUF);
		cin >> cInput;			        //������ʽ
        EnterCriticalSection(&cs);		//�����ٽ���
		memcpy(bufSend, cInput, strlen(cInput));
		LeaveCriticalSection(&cs);		//�뿪�ٽ���
		bSend = TRUE;
	}
}

/**
 * �ͻ����˳�
 */
void ExitClient(void)
{
	DeleteCriticalSection(&cs);
    CloseHandle(hThreadRecv);
	CloseHandle(hThreadSend);
    memset(bufSend, 0, MAX_NUM_BUF);
	closesocket(sClient);
	WSACleanup();
}

/**
 * ��ʾ���ӷ�����ʧ����Ϣ
 */
void ShowConnectMsg(BOOL bSuc)
{
	if (bSuc)
	{
		cout << "* Succeed to connect server! *" << endl;
	}
	else
    {
		cout << "* Client has to exit! *" << endl;
	}
}
