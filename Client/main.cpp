#include <iostream>
#include "client.h"

using namespace std;

int main()
{
    	//��ʼ��
	if (!InitClient())
	{
		ExitClient();
	}

	//���ӷ�����
	if(ConnectServer())
	{
		ShowConnectMsg(TRUE);
	}
	else
    {
		ShowConnectMsg(FALSE);
		ExitClient();
	}

	//�������ͺͽ��������߳�
	if (!CreateSendAndRecvThread())
	{
		ExitClient();

	}

	//�û��������ݺ���ʾ���
	InputAndOutput();
//
//	//�˳�
//	ExitClient();
//
	return 0;
}
