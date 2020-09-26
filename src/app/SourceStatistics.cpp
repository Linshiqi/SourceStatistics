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
	std::cout << "����: " << allFileList.size() << "���ļ�" << std::endl;
	cout << "��������ļ���·����" << endl;
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
		i++;	// �ų�һ��ǰ��ո����Ч����
	}
	while (end > 0 && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
		end--;	// ȥ��һ�к���ո����Ч����
	}
	s = s.substr(i, end - i+1);
}



void process(std::string& file) {
	string suffixStr = file.substr(file.find_last_of('.') + 1);//��ȡ�ļ���׺
	if (sourceType.suffix.find(suffixStr) == sourceType.suffix.end()) {
		return;		// ����Ҫ�����Դ�����ļ�����
	}
	std::vector<std::string> lines;
	readLines(file, lines);

	bool stringStart = false;	// �ַ�����ʼ��� ��Ҫ���⴦��
	bool multiCommentStart = false;	// ����ע�Ϳ�ʼ ��Ҫ���⴦��
	bool codeStart = false;		// ���뿪ʼ
	bool commentStart = false;	// ע�Ϳ�ʼ�����������뵥��ע��

	size_t total = 0;			// �ļ�������
	size_t empty = 0;			// ������
	size_t effective = 0;		// ��Ч��������
	size_t comments = 0;		// ע������

	for (auto& s : lines) {
		codeStart = false;	//  ��λ ��ʼû�д���
		commentStart = false;	// ��ʼû��ע��
		if (multiCommentStart) {
			// ���ǰ�����ע�ͻ�δ����,��ô���л�����ע��״̬
			commentStart = true;
		}
		if (isEmptyLine(s)) {
			// ͳ�ƿ�����
			empty++;
			continue;
		}

		strTrim(s);	// ȥ���ַ���s���߿ո���Ч�ַ�
		//size_t end = s.size() - 1;
		//while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n'))
		//{
		//	i++;	// �ų�һ��ǰ��ո����Ч����
		//}
		//while (end > 0 && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) {
		//	end--;	// ȥ��һ�к���ո����Ч����
		//}
		//s = s.substr(i, end - i);
		size_t stringSize = 0;		// ��¼�ַ����Ƿ��ռһ��
		size_t i = 0;
		unordered_map<size_t, bool> isEscape;	// �ַ�����������ת�Ʒ����Ƿ����ת�廹�Ƿ�б��
		while (true) {
			if (!stringStart && (s[i] == '\"') && ((i > 0 && (s[i - 1] != '\\' && s[i-1] != '\'')) || (i == 0))) {
				// ֮ǰû�������ַ�����ʼ��־������������
				stringStart = true;
				i++;
				continue;
			}
			if (stringStart) {	// �����ַ�����
				if ( (i == 0 || (i > 0 && !isEscape[i - 1])) && s[i] == '\\' ) {
					// ��һ������i��ǰһ������ת����ţ���ǰ��б����ת���
					isEscape[i] = true;
				}
				else {
					isEscape[i] = false;
				}
				stringSize++;	// ������һ���ַ���Ҳ�Ǵ����� �����¼�ַ����ǲ��Ƕ�ռһ��
				if ((s[i] == '\"') && ((i > 0 && (s[i - 1] != '\\' || (s[i-1] == '\\' && !isEscape[i-1]))) || (i == 0))) {
					// ֮ǰ���ַ�����ʼ��־����������������־
					stringStart = false;
				}
				else if ( s[i] == '\0') {
					// ������ĩβ
					if (commentStart) {
						comments++;
					}
					if (codeStart) {
						effective++;
					}
					break;	// �н���
				}
				i++;
				continue;
			}

			if (!multiCommentStart) {	// ��鵥��ע��ƥ��
				int k = 0;
				int j = i;
				while (k < sourceType.singleLineComment.size() && j < s.size() && s[j] == sourceType.singleLineComment[k]) {
					j++;
					k++;
				}
				if (k == sourceType.singleLineComment.size()) {
					// ����ע�ͱ�־ƥ��
					if (codeStart) {
						// ǰ������־��˵���Ǵ���ע����
						comments++;
						effective++;
					}
					else {
						// ֻ��ע��
						comments++;
					}
					break;	// ����ע�ͺ���ȫ��ע�����ݣ����ÿ�����
				}
			}
			if (!multiCommentStart) {	// ������ע��
				int k = 0;
				int j = i;
				while (k < sourceType.multiLineCommentBegin.size() && j < s.size() && s[j] == sourceType.multiLineCommentBegin[k]) {
					k++;
					j++;
				}
				if (k == sourceType.multiLineCommentBegin.size()) {
					i = j;	// ����i
					multiCommentStart = true;	// ����ע�ͱ�־��ʼ
					commentStart = true;		// ����ע��
					continue;
				}
			}
			if (multiCommentStart) {
				// ���ڶ���ע����
				int k = 0;
				int j = i;
				while (k < sourceType.multiLineCommentEnd.size() && j < s.size() && s[j] == sourceType.multiLineCommentEnd[k]) {
					k++;
					j++;
				}
				if (k == sourceType.multiLineCommentEnd.size()) {
					// ����ע�ͽ�����־ƥ��
					multiCommentStart = false;
					//commentStart = false;
					i = j;
					continue;
				}
				else if(s[i] == '\0'){
					// �н���
					if (codeStart) {
						// ǰ���д��� ˵���Ǵ���ע����
						effective++;
						comments++;
					}
					else {
						// ֻ��ע��
						comments++;
					}
					break;	// �н���
				}
				else {
					// ���ڶ���ע����
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
					// ��ռһ�е��ַ���
					effective++;
				}
				break;	// �н���
			}

			// ������˵������Ч��������
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