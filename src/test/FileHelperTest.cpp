#include "../../include/catch.hpp"
#include "../fileHelper/FileHelper.h"

using namespace code047;
TEST_CASE("fileHelper测试") {
	SECTION("../src/app目录下现有3个文件") {
		std::string dir = "../src/app";
		std::vector<std::string> allFileList = getFilesList(dir);
		size_t count = allFileList.size();
		REQUIRE(count == 3);
	}
}