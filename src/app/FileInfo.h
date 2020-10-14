#pragma once

#include <set>
#include <string>

struct SourceType {
	std::set<std::string> suffix;		// ��Ҫͳ�Ƶ��ļ�����׺�������ж��ֺ�׺������C/C++(.h .hpp .hpp .c .cxx .cpp��)
	std::string singleLineComment;			// ����ע�ͷ���
	std::string multiLineCommentBegin;		// ����ע�Ϳ�ʼ����
	std::string multiLineCommentEnd;		// ����ע�ͽ�������
	SourceType(std::set<std::string>& suffix_, std::string& singleLineComment_, std::string multiLineCommentBegin_, std::string& multiLineCommentEnd_)
		: suffix(suffix_), singleLineComment(singleLineComment_), multiLineCommentBegin(multiLineCommentBegin_), multiLineCommentEnd(multiLineCommentEnd_) {}
};

// Ĭ�϶���C�����ļ�
std::set<std::string> suffix = {"c", "cpp", "h", "cxx", "hpp"};	// �ļ���׺
std::string singleComments = "//";		// ����ע��
std::string multiCommentsBegin = "/*";	// ����ע�Ϳ�ͷ
std::string multiCommentEnd = "*/";		// ����ע�ͽ�β

// rb
//std::set<std::string> suffix = { "c", "cpp", "h", "cxx", "hpp" };	// �ļ���׺
//std::string singleComments = "//";		// ����ע��
//std::string multiCommentsBegin = "/*";	// ����ע�Ϳ�ͷ
//std::string multiCommentEnd = "*/";		// ����ע�ͽ�β

SourceType sourceType = SourceType(suffix, singleComments, multiCommentsBegin, multiCommentEnd);