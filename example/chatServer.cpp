#include "../lib/ConnectionManager.h"
#include "../lib/ThreadPool.h"
#include "../protoFiles/test.pb.h"
#include "../lib/serviceBase.h"
#include "../lib/type_define.h"


int main()
{
	ServiceBase::GetInstancePtr()->StartNetWork(PORT,100000);
	return 0;
}
