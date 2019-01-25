#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"

int main()
{
	TestProto test;
	test.set_id(100);
	test.set_name("TEXT");
	ConnectionManager conn;
	conn.SetConnectionNum(1000);
	xstring ip = "0.0.0.0";
	if(!conn.CreteSocket(ip, 9999))
	{
		printf("create socket error \n");
		return -1;
	}

	if(!conn.CreateEpollEvent())
	{
		printf("create CreateEpollEvent error \n");
		return -1;
	}

	conn.Init();
	conn.Start();

	while (!conn.CanExit())
	{
		sleep(100);
	}
	printf(" LOOP EXIT!!! \n");
	conn.Close();
	return 0;
}
