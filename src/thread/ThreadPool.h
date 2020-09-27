/***************************************************************************
�Լ���ʵ�ֲ�����std::for_each
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

		// ���㴴�������̺߳��ʣ���󲻳�������֧�ֵ��߳���
		size_t min_per_thread = 25;
		const size_t max_threads = (length + min_per_thread - 1) / min_per_thread;
		const size_t hardware_threads = std::thread::hardware_concurrency();
		const size_t num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
		//std::cout << "����:" << num_threads << "���߳�" << std::endl;
		const size_t block_size = length / num_threads;		// ÿ���̷ֵ߳��ĸ���
		std::vector<std::thread> threads(num_threads - 1);	// ע�����ﴴ���̵߳��߳�Ҳ��һ��
		JoinThreads joiner(threads);						// �߳���Դ����

		// ���为��
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


