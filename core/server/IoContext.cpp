#include "IoContext.h"

IoContext::IoContext()
{
	client_ = new Client();
	SecureZeroMemory(&overlapped_, sizeof(overlapped_));
	SecureZeroMemory(&buffer_, sizeof(buffer_));
	buffer_.len = 0;
	buffer_.buf = nullptr;
}

IoContext::~IoContext()
{
}
