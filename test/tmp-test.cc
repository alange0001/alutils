// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).


#include <alutils/print.h>
#include <alutils/internal.h>

#include <vector>
#include <map>
#include <regex>

//#define TEST_JSON
#ifdef TEST_JSON
#include "nlohmann/json.hpp"
#endif

using namespace alutils;

int main(int argc, char** argv) {
	printf("\n\n=====================\ntmp-test:\n");
	log_level = LOG_DEBUG;


#	ifdef TEST_JSON
	std::string v1 = "testv1";
	std::vector<int> v2 = {1,2,3};
	std::map<std::string, std::string> v3 = {{"a","va"}, {"b","vb"}};
	nlohmann::json j;
	j["v1"] = v1;
	j["v2"] = v2;
	j["v3"] = v3;

	PRINT_DEBUG("JSON j: %s", j.dump().c_str() );
	PRINT_DEBUG("JSON j2: %s", nlohmann::json({{"a",1},{"b","bb"},{"c",{1,2,3}}}).dump().c_str() );
#	endif


	auto flags = std::regex_constants::match_any;
	auto re = std::regex("report(  *cf *= *(\\w+)){0,1}");
	std::cmatch cm;

	regex_search("ldkfjlakjdf", cm, re, flags);
	PRINT_DEBUG("cm.size() = %s", v2s(cm.size()));
	for (int i = 0; i < cm.size(); i++) { PRINT_DEBUG("\t%s", cm.str(i).c_str()); }
	regex_search("report", cm, re, flags);
	PRINT_DEBUG("cm.size() = %s", v2s(cm.size()));
	for (int i = 0; i < cm.size(); i++) { PRINT_DEBUG("\t%s", cm.str(i).c_str()); }
	regex_search("report cf= defaultak_kjad141\nlkdjflkajd", cm, re, flags);
	PRINT_DEBUG("cm.size() = %s", v2s(cm.size()));
	for (int i = 0; i < cm.size(); i++) { PRINT_DEBUG("\t%s", cm.str(i).c_str()); }


	printf("OK!!\n");
	return 0;
}
