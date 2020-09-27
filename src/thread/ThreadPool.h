/***************************************************************************
自己来实现并发版std::for_each
****************************************************************************/
#pragma once
#include <iterator>
#include <thread>
#include <algorithm>
#include <vector>
#include "JoinThreads.h"

namespace code047 {
	template<typename Iterator, typename Func>
	void parallel_for_each(Iterator first, Iterator last, Func f) {
		auto length = std::distance(first, last);
		if (length == 0) {
			return;
		}

		// 计算创建多少线程合适，最大不超过机器支持的线程数
		size_t min_per_thread = 25;
		const size_t max_threads = (length + min_per_thread - 1) / min_per_thread;
		const size_t hardware_threads = std::thread::hardware_concurrency();
		const size_t num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		//std::cout << "共有:" << num_threads << "个线程" << std::endl;
		const size_t block_size = length / num_threads;		// 每个线程分担的负载
		std::vector<std::thread> threads(num_threads - 1);	// 注意这里创建线程的线程也算一个
		JoinThreads joiner(threads);						// 线程资源管理

		// 分配负载
		Iterator block_start = first;
		for (size_t i = 0; i < num_threads - 1; i++) {
			Iterator block_end = block_start;
			std::advance(block_end, block_size);
			threads[i] = std::thread([=]()
				{
					std::for_each(block_start, block_end, f);
				});
			block_start = block_end;
		}
		std::for_each(block_start, last, f);
	}
}


