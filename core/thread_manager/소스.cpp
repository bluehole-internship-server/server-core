#include "ThreadManager.h"
#include <Windows.h>
#include <process.h>
#include <chrono>
#include <vector>
#include <atomic>
#include <random>
#include <iostream>
#include "ThreadPool.h"

#define TASK_AMOUNT 1'000
#define THREAD_MAX_AMOUNT 64
#define SLEEP_SECONDS 10

std::atomic<unsigned> gv;
std::default_random_engine generator;
std::uniform_int_distribution<int> distribution(0, 1000000);

VOID CALLBACK HelloCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
)
{
	gv = distribution(generator);
	Sleep(SLEEP_SECONDS);
}

unsigned Hello(PVOID p)
{
	gv = distribution(generator);
	Sleep(SLEEP_SECONDS);
	return 0;
}

void TestNoThreadpool(int thread_amount, int task_amount)
{
	std::vector<HANDLE> tasks(task_amount);
	for (auto t : tasks) {
		t = (HANDLE)_beginthreadex(NULL, 0, Hello, NULL, 0, NULL);
	}
	WaitForMultipleObjects(task_amount, tasks.data(), TRUE, INFINITE);
}

void TestThreadManager(int thread_amount, int task_amount)
{
	core::ThreadManager tm;
	tm.SetMaximumWorker(thread_amount);
	tm.AddWork(HelloCallback, NULL, task_amount, NULL);
	tm.JoinAll();
}

void TestOtherThreadPool(int thread_amount, int task_amount)
{
	ThreadPool tp(thread_amount);
	std::vector< std::future<int> > results;
	for (int i = 0; i < task_amount; ++i) {
		results.emplace_back(
			tp.enqueue([i] {
			gv = distribution(generator); 
			Sleep(SLEEP_SECONDS);
			return 0;
		})
		);
	}
	for (auto && result : results)
		result.get();
}

void test(int thread_amount)
{
	gv = 0;
	//auto testTarget = TestNoThreadpool;
	//auto testTarget = TestThreadManager;
	auto testTarget = TestOtherThreadPool;
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();
	testTarget(thread_amount, TASK_AMOUNT);
	end = std::chrono::system_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

	std::cout << elapsed_time.count() << std::endl;
}

int main()
{
	for (int thread_amount = 1; thread_amount <= THREAD_MAX_AMOUNT ;  thread_amount *= 2) {
		std::cout << thread_amount << " Threads " << TASK_AMOUNT << " Tasks" << std::endl;
		test(thread_amount);
	}
	return 0;
}