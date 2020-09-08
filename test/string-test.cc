// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/string.h>

#include <stdio.h>
#include <cassert>

using namespace alutils;

std::string vector_to_str(const std::vector<std::string>& v) {
	std::string ret = "[";
	bool delimiter = false;
	for (auto i: v) {
		if (delimiter) {
			ret += ", ";
		} else {
			delimiter = true;
		}
		ret += "\"";
		ret += i;
		ret += "\"";
	}
	ret += "]";
	return ret;
}

int main(int argc, char** argv) {
	printf("\n\n=====================\nstring-test:\n");
	std::string s;

	s = " v1, v2, v3 \t";
	printf("strip \"%s\": \"%s\"\n", s.c_str(), strip(s).c_str());
	assert( strip(s) == "v1, v2, v3" );

	s = "v1, v2 ,v3 ";
	printf("split_str \"%s\": %s\n", s.c_str(), vector_to_str(split_str(s, ",")).c_str());
	assert( vector_to_str(split_str(s, ",")) == "[\"v1\", \"v2\", \"v3\"]" );

	assert( parseBool("true") == true );
	assert( parseUint32("3245") == (uint32_t)3245 );
	assert( parseUint64("3245") == (uint64_t)3245 );
	assert( parseDouble("324.5") == (double)324.5 );

	std::string aux;
	for (int i = 0; i<200; i++) {
		aux += std::to_string(i);
		std::string aux2 = aux + " test %d %d";
		assert( sprintf(aux2.c_str(),123, i) == aux+std::string(" test 123 ")+std::to_string(i) );
	}

	printf("OK!!\n");
	return 0;
}
