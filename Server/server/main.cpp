#include "server.h"

int main(int argc, char* argv[])
{
	//��ʼ��������
	if (!initSever())
	{
		exitServer();
    	return SERVER_SETUP_FAIL;
	}

	//��������
	if (!startService())
	{
	    showServerStartMsg(FALSE);
		exitServer();
    	return SERVER_SETUP_FAIL;
	}

	 //��������
    inputAndOutput();

    //�˳����̣߳�������Դ
	exitServer();

	return 0;
}
