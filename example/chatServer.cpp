#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"

int main()
{
	ServiceBase::GetInstancePtr()->StartNetWork(9999,1000);
	return 0;
}
