/*********************************************************************************
对象管理资源的思想
这里确保线程被加入
**********************************************************************************/
#pragma once
#include <thread>
#include <vector>

class JoinThreads {
private:
	std::vector<std::thread>& threads;
public:
	explicit JoinThreads(std::vector<std::thread>& threads_) :
		threads(threads_) {}
	JoinThreads(const JoinThreads& other) = delete;
	JoinThreads& operator=(const JoinThreads& other) = delete;

	~JoinThreads() {
		for (size_t i = 0; i < threads.size(); i++) {
			if (threads[i].joinable()) {
				threads[i].join();
			}
		}
	}
};