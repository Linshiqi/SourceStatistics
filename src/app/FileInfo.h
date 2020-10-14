#pragma once

#include <set>
#include <string>

struct SourceType {
	std::set<std::string> suffix;		// 需要统计的文件名后缀，可能有多种后缀，比如C/C++(.h .hpp .hpp .c .cxx .cpp等)
	std::string singleLineComment;			// 单行注释符号
	std::string multiLineCommentBegin;		// 多行注释开始符号
	std::string multiLineCommentEnd;		// 多行注释结束符号
	SourceType(std::set<std::string>& suffix_, std::string& singleLineComment_, std::string multiLineCommentBegin_, std::string& multiLineCommentEnd_)
		: suffix(suffix_), singleLineComment(singleLineComment_), multiLineCommentBegin(multiLineCommentBegin_), multiLineCommentEnd(multiLineCommentEnd_) {}
};

// 默认定义C类型文件
std::set<std::string> suffix = {"c", "cpp", "h", "cxx", "hpp"};	// 文件后缀
std::string singleComments = "//";		// 单行注释
std::string multiCommentsBegin = "/*";	// 多行注释开头
std::string multiCommentEnd = "*/";		// 多行注释结尾

// rb
//std::set<std::string> suffix = { "c", "cpp", "h", "cxx", "hpp" };	// 文件后缀
//std::string singleComments = "//";		// 单行注释
//std::string multiCommentsBegin = "/*";	// 多行注释开头
//std::string multiCommentEnd = "*/";		// 多行注释结尾

SourceType sourceType = SourceType(suffix, singleComments, multiCommentsBegin, multiCommentEnd);