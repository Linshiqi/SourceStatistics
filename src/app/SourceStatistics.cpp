#include "SourceStatistics.h"
#include "../fileHelper/FileHelper.h"
#include "../thread/ThreadPool.h"
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

struct SourceType {
	std::vector<std::string> suffix;		// ��Ҫͳ�Ƶ��ļ�����׺�������ж��ֺ�׺������C/C++(.h .hpp .hpp .c .cxx .cpp��)
	std::string singleLineComment;			// ����ע�ͷ���
	std::string multiLineCommentBegin;		// ����ע�Ϳ�ʼ����
	std::string multiLineCommentEnd;		// ����ע�ͽ�������
	SourceType(std::vector<std::string>& suffix_, std::string& singleLineComment_, std::string multiLineCommentBegin_, std::string& multiLineCommentEnd_)
		: suffix(suffix_), singleLineComment(singleLineComment_), multiLineCommentBegin(multiLineCommentBegin_), multiLineCommentEnd(multiLineCommentEnd_) {}
};


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
}


void testReadFile(std::string& file, SourceType& sourceType) {
	std::vector<std::string> lines;
	readLines(file, lines);

	size_t total = lines.size();		// ������
	size_t empty = 0;					// ������
	size_t effective = 0;				// ��Ч������

	size_t singleLineComments = 0;		// һ��ֻ�е���ע�͵�������            ���磺// ����ע��
	size_t singleCommentsWithCode = 0;	// �����к������ע�͵�����			 ����: int i = 0; // i
	size_t multiLineComments = 0;		// ����ע�ͷֲ��ڶ��е�����			 
	size_t multiCommentInOneLine = 0;	// ����ע����ͬһ�е�����				 ���磺/*����ע�ͷ��ڵ�����*/
	size_t multiCommentWithCode = 0;	// ����ע���������ͬһ�е�����		 ���磺int j /*j�ǲ��Ա���*/

	bool multiCommentStart = false;		// ����ע�Ϳ�ʼ
	int multiCommentStartRow = -1;		// ����ע�ʹ����п�ʼ��-1��ʾ��û��������ע��
	size_t curRow = 0;					// ��ǰ������кţ������ж϶���ע��д��һ���ϵ��ж�

	for (auto& s : lines) {
		curRow++;
		if (isEmptyLine(s)) {
			empty++;
			continue;
		}
		if (multiCommentStart == true) {
			multiLineComments++;	// �����ڶ���ע�� ע�����ע����Ŀ��в���
		}

		// ע���ַ����е�"//"����
		bool stringStart = false;	// �Ƿ����ַ�����ʼ
		bool codeStart = false;		// �����Ƿ��������
		size_t i = 0;
		while (i < s.size() && s[i] == ' ' || s[i] == '\t') i++;	// �ų�һ��ǰ��ո�
		size_t i_start = i;		// ��¼i��ʼλ��
		for (; i < s.size();) {

			if (s[i] == '"' && multiCommentStart == false && stringStart == false) {	// ע�����ע������ַ�������
				// ֮ǰû��"�����������ˣ�˵�����������ַ���
				i++;
				stringStart = true;
				continue;
			}
			else if (s[i] == '"' && multiCommentStart == false && s[i-1] != '\\' && stringStart == true) {	// ע���ַ����е�ת����� ����\"����Ͳ����ַ���������־
				// ֮ǰ��"�������������ˣ�˵�����ַ�������
				i++;
				stringStart = false;
				continue;
			}
			else if(stringStart == false){
				// ��������Ѿ������������ַ���
				if (multiCommentStart == false) {	// ������ƥ���Ƿ�ʼ
					int j = 0;
					int k = i;
					while (j < sourceType.multiLineCommentBegin.size() && k < s.size() && s[k] == sourceType.multiLineCommentBegin[j]) {
						j++;
						k++;
					}
					if (j == sourceType.multiLineCommentBegin.size()) {
						// ����ע�Ϳ�ʼ����ƥ��ɹ���˵������ע�Ϳ�ʼ��
						multiCommentStart = true;
						multiCommentStartRow = static_cast<int>(curRow);
						i += j;	// ��������ƥ�俪ͷ����
					}
					else {
						// ���Ƕ���ע�Ϳ�ʼ, ��ô����Ƿ��ǵ���ע�Ϳ�ʼ
						j = 0, k = i;
						while (k < s.size() && j < sourceType.singleLineComment.size() && s[k] == sourceType.singleLineComment[j]) {
							k++;
							j++;
						}
						if (multiCommentStart == false && j == sourceType.singleLineComment.size()) {
							// ƥ���˵���ע��
							if (!codeStart) {
								// ����ֻ�е���ע��
								singleLineComments++;
							}
							else {
								singleCommentsWithCode++;
							}
							break;		// ����ע�ͺ��涼��ע�����ݣ������������ж���
						}
						else {
							// �����˵����ǰ���Ǵ����ַ����ڣ�Ҳ���Ǵ��ڶ���ע���ڣ�Ҳ���ǵ���ע����,�Ǿ��Ǵ��뿪ʼ��
							i++;
							effective++;	// �ҵ�һ����Ч������
							break;
						}
					}
				}
				else {	// ֮ǰ�ж���ƥ���־��������ƥ���Ƿ����
					int j = 0;
					int k = i;
					while (j < sourceType.multiLineCommentEnd.size() && k < s.size() && s[k] == sourceType.multiLineCommentEnd[j]) {
						j++;
						k++;
					}
					if (j == sourceType.multiLineCommentEnd.size()) {
						// ����ע�ͽ�������ƥ��ɹ���˵������ע�ͽ�����
						multiCommentStart = false;
						if (multiCommentStartRow == curRow) {
							if (codeStart) {	// ����ע���������ͬһ��
								multiCommentWithCode++;
							}
							else {
								multiCommentInOneLine++; // ��һ��ֻ�ж���ע��
							}
						}
						else {
							multiLineComments++;	//���һ������ע��
						}
						i += j;
					}
					i++;
					continue;
				}
			}
			else {
				//�������ַ�����
				i++;
			}
		}
	}

	multiLineComments -= multiCommentInOneLine;
	size_t totalComments = total - effective;

	std::cout << "total: " << total << " empty: " << empty << " effective: " << effective<< " total comments: " << totalComments << std::endl;
	

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

void process(std::string& file, SourceType sourceType) {
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
	std::cout << "total: " << lines.size() << " empty: " << empty << " effective: " << effective << " comments: " << comments << std::endl;
}

int main(int argc, char* argv[])
{
	/*if (argc < 2) {
		printf("Usage: %s pathToSouce\n", argv[0]);
		return 0;
	}*/
	char* sourcePath;// = argv[1];
	//sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\SourceCounter\\test\\Config.cpp";//jsoncpp.cpp";
	//sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\SourceCounter\\test\\jsoncpp.cpp";
	sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\SourceCounter\\test\\imgui.cpp";

	//sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\Osiris\\jsoncpp.cpp";
	//sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\Osiris\\imgui\\imgui_impl_dx9.cpp";

	//sourcePath = "C:\\Users\\linshiqi\\Desktop\\ͳ��Դ��\\Osiris\\Hacks\\SkinChanger.cpp";


	std::cout << sourcePath << std::endl;
	//testThreadLib();
	//printAllFilesInDir(sourcePath);
	std::vector<std::string> suffix = {};
	std::string singleComments = "//";
	std::string multiCommentsBegin = "/*";
	std::string multiCommentEnd = "*/";
	SourceType sourceType = SourceType(suffix, singleComments, multiCommentsBegin, multiCommentEnd);
//	std::vector<std::string> fileList = getFilesList(sourcePath);
	//parallel_for_each<std::vector<std::string>::iterator, std::function<void(std::string)>>(fileList.begin(), fileList.end(), process);
//	testReadFile(std::string(sourcePath), sourceType);
	process(std::string(sourcePath), sourceType);
	return 0;
}