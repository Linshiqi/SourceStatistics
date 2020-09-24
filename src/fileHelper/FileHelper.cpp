#include "FileHelper.h"

// 从给定文件中读取所有行
std::vector<std::string> readLines(std::string file) {
	std::vector<std::string> res;
	if (file.empty()) {
		return  res;
	}
	std::ifstream ifst(file);
	try {
		if (ifst.is_open() && ifst.good()) {
			std::string line;
			while (std::getline(ifst, line)) {
				res.push_back(line);
			}
		}
		else {
			std::cout << "Open file " << file << "failed" << std::endl;
		}
		ifst.close();
	}
	catch (const std::exception e) {
		std::cout << e.what() << std::endl;
		if (ifst.is_open()) {
			ifst.close();
		}
	}
	return res;
}