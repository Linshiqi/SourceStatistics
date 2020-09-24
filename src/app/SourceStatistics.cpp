#include "SourceStatistics.h"
#include "../fileHelper/FileHelper.h"
#include <thread>


void f() {
	const unsigned long hardware_threads = std::thread::hardware_concurrency();
	std::cout << hardware_threads << std::endl;
}

void printAllFilesInDir() {
	char dir[200];
	cout << "Enter a directory: ";
	cin.getline(dir, 200);
	vector<string>allFileList = getFilesList(dir);
	std::cout << "共有: " << allFileList.size() << "个文件" << std::endl;
	cout << "输出所有文件的路径：" << endl;
	for (size_t i = 0; i < allFileList.size(); i++)
	{
		string filePath = allFileList.at(i);
		cout << filePath << endl;
	}
}

int main()
{
	printAllFilesInDir();
	return 0;
}