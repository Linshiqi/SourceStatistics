#include "../../include/catch.hpp"
#include "../fileHelper/FileHelper.h"

using namespace code047;
TEST_CASE("fileHelper����") {
	SECTION("../src/appĿ¼������3���ļ�") {
		std::string dir = "../src/app";
		std::vector<std::string> allFileList = getFilesList(dir);
		size_t count = allFileList.size();
		REQUIRE(count == 3);
	}
}