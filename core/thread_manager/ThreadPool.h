#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <future>
#include "Spinlock.h"

namespace core
{
class ThreadPool
{
public:
	ThreadPool(size_t thread_amount);
	~ThreadPool();
	template<class F, class... Args>
	auto Enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;
private:
	std::queue<std::function<void()>> tasks_;
	std::vector<std::thread> workers_;
	core::Spinlock spinlock_;
	bool stop;
};
}