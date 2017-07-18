#pragma once
#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <future>
#include "ThreadPool.h"

namespace core
{	
	ThreadPool::ThreadPool(size_t thread_amount) : stop(false)
	{
		for (size_t i = 0; i < thread_amount; ++i) {
			workers_.emplace_back(
				[this]()
			{
				for (;;) {
					std::function<void()> task = nullptr;
					spinlock_.Lock();
					if (stop && tasks_.empty()) {
						spinlock_.Unlock();
						return;
					}
					else if (!tasks_.empty()) {
						task = std::move(tasks_.front());
						tasks_.pop();
						spinlock_.Unlock();
					}
					else {
						spinlock_.Unlock();
					}
	
	
					if (task != nullptr)
						task();
				}
	
			});
		}
	}
	ThreadPool::~ThreadPool()
	{
		spinlock_.Lock();
		stop = true;
		spinlock_.Unlock();
		for (auto &w : workers_)
			w.join();
	}
	
	template<class F, class... Args>
	auto ThreadPool::Enqueue(F&& f, Args&&... args)
		-> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;
	
		auto task = std::make_shared< std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
			);
	
		std::future<return_type> res = task->get_future();
		{
			spinlock_.Lock();
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");
	
			tasks_.emplace([task]() { (*task)(); });
			spinlock_.Unlock();
		}
		return res;
	}
}
