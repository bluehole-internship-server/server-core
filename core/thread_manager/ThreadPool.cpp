#pragma once
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
}