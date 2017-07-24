#include "IoContext.h"

IoContext::IoContext()
{
	SecureZeroMemory(&overlapped_, sizeof(overlapped_));
	SecureZeroMemory(&buffer_, sizeof(buffer_));
	buffer_.len = 0;
	buffer_.buf = nullptr;
	send_buffer_ = new char[SEND_BUFFER_SIZE];
	recv_buffer_ = new char[RECV_BUFFER_SIZE];
	SecureZeroMemory(send_buffer_, sizeof(send_buffer_));
	SecureZeroMemory(recv_buffer_, sizeof(recv_buffer_));
}

IoContext::~IoContext()
{
	delete send_buffer_;
	delete recv_buffer_;
}
