#pragma once
#include <iostream>
#include <string>  
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace code047 {
	// �Ӹ���Ŀ¼��ȡ�����ļ�·��(����·�����ļ���)
	vector<string> getFilesList(string dir);

	// �Ӹ����ļ��ж�ȡ������
	void readLines(const std::string& file, std::vector<std::string>& res) {
		//std::vector<std::string> res;
		if (file.empty()) {
			return;
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
	}

#ifdef LINUX
#include <memory.h>
#include <dirent.h>
	vector<string> getFilesList(string dirpath) {
		vector<string> allPath;
		DIR* dir = opendir(dirpath.c_str());
		if (dir == NULL)
		{
			cout << "opendir error" << endl;
			return allPath;
		}
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			if (entry->d_type == DT_DIR) {//It's dir
				if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
					continue;
				string dirNew = dirpath + "/" + entry->d_name;
				vector<string> tempPath = getFilesList(dirNew);
				allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());

			}
			else {
				//cout << "name = " << entry->d_name << ", len = " << entry->d_reclen << ", entry->d_type = " << (int)entry->d_type << endl;
				string name = entry->d_name;
				string imgdir = dirpath + "/" + name;
				//sprintf("%s",imgdir.c_str());
				allPath.push_back(imgdir);
			}

		}
		closedir(dir);
		//system("pause");
		return allPath;
	}
#endif

#ifdef WINDOWS 
#include <io.h>  
	vector<string> getFilesList(string dir)
	{
		vector<string> allPath;
		// ��Ŀ¼�������"\\*.*"���е�һ������
		string dir2 = dir + "\\*.*";

		intptr_t handle;
		_finddata_t findData;

		handle = _findfirst(dir2.c_str(), &findData);
		if (handle == -1) {// ����Ƿ�ɹ�
			cout << "can not found the file ... " << endl;
			return allPath;
		}
		while (_findnext(handle, &findData) == 0)
		{
			if (findData.attrib & _A_SUBDIR) //�Ƿ�����Ŀ¼
			{
				//������Ŀ¼Ϊ"."��".."���������һ��ѭ�������������Ŀ¼������������һ������
				if (strcmp(findData.name, ".") == 0 || strcmp(findData.name, "..") == 0)
					continue;
				// ��Ŀ¼�������"\\"����������Ŀ¼��������һ������
				string dirNew = dir + "\\" + findData.name;
				vector<string> tempPath = getFilesList(dirNew);
				allPath.insert(allPath.end(), tempPath.begin(), tempPath.end());
			}
			else //������Ŀ¼�������ļ���������ļ������ļ��Ĵ�С
			{
				string filePath = dir + "\\" + findData.name;
				allPath.push_back(filePath);
			}
		}
		_findclose(handle);    // �ر��������
		return allPath;
	}
#endif
}

