#include "SourceStatistics.h"
#include "../fileHelper/FileHelper.h"
#include "../thread/ThreadPool.h"
#include "FileInfo.h"
#include <thread>
#include <functional>
#include <stdio.h>
#include <unordered_map>

using namespace code047;
void printHardwareThreads() {
	const unsigned long hardware_threads = std::thread::hardware_concurrency();
	std::cout << hardware_threads << std::endl;
}

void print(int e) {
	//std::cout << std::this_thread::get_id() << ": ";
	std::cout << e << " ";
}

void testThreadLib() {
	const size_t N = 1000;
	std::vector<int> nums;
	for (int i = 0; i < N; i++) {
		nums.push_back(i);
	}
	parallel_for_each<std::vector<int>::iterator, std::function<void(int)>>(nums.begin(), nums.end(), print);
}

void printAllFilesInDir(const char* dir) {
	/*char dir[200];
	cout << "Enter a directory: ";
	cin.getline(dir, 200);*/
	vector<string>allFileList = getFilesList(dir);
	std::cout << "共有: " << allFileList.size() << "个文件" << std::endl;
	cout << "输出所有文件的路径：" << endl;
	for (size_t i = 0; i < allFileList.size(); i++)
	{
		string filePath = allFileList.at(i);
		cout << filePath << endl;
	}
}




bool isEmptyLine(std::string& s) {
	if (s.empty()) {
		return true;
	}
	for (size_t i = 0; i < s.size(); i++) {
		if ( (s[i] == ' ') || (s[i] == '\t') || (s[i] == '\r') || (s[i] == '\n')) {
			continue;
		}
		else {
			return false;
		}
	}
	return true;
}

void strTrim(std::string& s) {
	size_t i = 0;
	size_t end = s.size() - 1;
	while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n'))
	{
		i++;	// 排除一行前面空格和无效符号
	}
	while (end > 0 && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
		end--;	// 去除一行后面空格和无效符号
	}
	s = s.substr(i, end - i+1);
}



void process(std::string& file) {
	string suffixStr = file.substr(file.find_last_of('.') + 1);//获取文件后缀
	if (sourceType.suffix.find(suffixStr) == sourceType.suffix.end()) {
		return;		// 不是要处理的源代码文件类型
	}
	std::vector<std::string> lines;
	readLines(file, lines);

	bool stringStart = false;	// 字符串开始标记 需要特殊处理
	bool multiCommentStart = false;	// 多行注释开始 需要特殊处理
	bool codeStart = false;		// 代码开始
	bool commentStart = false;	// 注释开始，包含多行与单行注释

	size_t total = 0;			// 文件总行数
	size_t empty = 0;			// 空行数
	size_t effective = 0;		// 有效代码行数
	size_t comments = 0;		// 注释行数

	for (auto& s : lines) {
		codeStart = false;	//  复位 开始没有代码
		commentStart = false;	// 开始没有注释
		if (multiCommentStart) {
			// 如果前面多行注释还未结束,那么本行还处于注释状态
			commentStart = true;
		}
		if (isEmptyLine(s)) {
			// 统计空行数
			empty++;
			continue;
		}

		strTrim(s);	// 去除字符串s两边空格无效字符
		//size_t end = s.size() - 1;
		//while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n'))
		//{
		//	i++;	// 排除一行前面空格和无效符号
		//}
		//while (end > 0 && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
		//	end--;	// 去除一行后面空格和无效符号
		//}
		//s = s.substr(i, end - i);
		size_t stringSize = 0;		// 记录字符串是否独占一行
		size_t i = 0;
		unordered_map<size_t, bool> isEscape;	// 字符串中连续的转移符号是否代表转义还是反斜杠
		while (true) {
			if (!stringStart && (s[i] == '\"') && ((i > 0 && (s[i - 1] != '\\' && s[i-1] != '\'')) || (i == 0))) {
				// 之前没有遇到字符串开始标志，现在遇到了
				stringStart = true;
				i++;
				continue;
			}
			if (stringStart) {	// 处于字符串中
				if ( (i == 0 || (i > 0 && !isEscape[i - 1])) && s[i] == '\\' ) {
					// 第一个或者i的前一个不是转义符号，则当前反斜杠是转义符
					isEscape[i] = true;
				}
				else {
					isEscape[i] = false;
				}
				stringSize++;	// 单独的一行字符串也是代码行 这里记录字符串是不是独占一行
				if ((s[i] == '\"') && ((i > 0 && (s[i - 1] != '\\' || (s[i-1] == '\\' && !isEscape[i-1]))) || (i == 0))) {
					// 之前有字符串开始标志，现在遇到结束标志
					stringStart = false;
				}
				else if ( s[i] == '\0') {
					// 到达行末尾
					if (commentStart) {
						comments++;
					}
					if (codeStart) {
						effective++;
					}
					break;	// 行结束
				}
				i++;
				continue;
			}

			if (!multiCommentStart) {	// 检查单行注释匹配
				int k = 0;
				int j = i;
				while (k < sourceType.singleLineComment.size() && j < s.size() && s[j] == sourceType.singleLineComment[k]) {
					j++;
					k++;
				}
				if (k == sourceType.singleLineComment.size()) {
					// 单行注释标志匹配
					if (codeStart) {
						// 前面代码标志，说明是代码注释行
						comments++;
						effective++;
					}
					else {
						// 只有注释
						comments++;
					}
					break;	// 单行注释后面全是注释内容，不用考虑了
				}
			}
			if (!multiCommentStart) {	// 检查多行注释
				int k = 0;
				int j = i;
				while (k < sourceType.multiLineCommentBegin.size() && j < s.size() && s[j] == sourceType.multiLineCommentBegin[k]) {
					k++;
					j++;
				}
				if (k == sourceType.multiLineCommentBegin.size()) {
					i = j;	// 更新i
					multiCommentStart = true;	// 多行注释标志开始
					commentStart = true;		// 发现注释
					continue;
				}
			}
			if (multiCommentStart) {
				// 正在多行注释中
				int k = 0;
				int j = i;
				while (k < sourceType.multiLineCommentEnd.size() && j < s.size() && s[j] == sourceType.multiLineCommentEnd[k]) {
					k++;
					j++;
				}
				if (k == sourceType.multiLineCommentEnd.size()) {
					// 多行注释结束标志匹配
					multiCommentStart = false;
					//commentStart = false;
					i = j;
					continue;
				}
				else if(s[i] == '\0'){
					// 行结束
					if (codeStart) {
						// 前面有代码 说明是代码注释行
						effective++;
						comments++;
					}
					else {
						// 只有注释
						comments++;
					}
					break;	// 行结束
				}
				else {
					// 还在多行注释中
					i++;
					continue;
				}
			}
			if (s[i] == '\0') {
				if (commentStart) {
					comments++;
				}
				if (codeStart) {
					effective++;
				}
				if (!codeStart && stringSize == s.size()-1) {
					// 独占一行的字符串
					effective++;
				}
				break;	// 行结束
			}

			// 到这里说明是有效代码行了
			codeStart = true;
			i++;
		}
	}
	total = lines.size();
	std::cout << file << ": total: " << lines.size() << " empty: " << empty << " effective: " << effective << " comments: " << comments << std::endl;
}

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Usage: %s pathToSouce\n", argv[0]);
		return 0;
	}
	char* sourcePath = argv[1];

	std::vector<std::string> fileList = getFilesList(sourcePath);
	parallel_for_each<std::vector<std::string>::iterator, std::function<void(std::string)>>(fileList.begin(), fileList.end(), process);
	//process(std::string(sourcePath));
	return 0;
}