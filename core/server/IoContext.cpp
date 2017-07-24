#include "IoContext.h"

IoContext::IoContext()
{
	SecureZeroMemory(&overlapped, sizeof(overlapped));
	SecureZeroMemory(&buffer, sizeof(buffer));
}