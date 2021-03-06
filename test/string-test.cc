// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/string.h>
#include <alutils/print.h>

#include <stdio.h>
#include <cassert>
#include <iostream>
#include <sstream>

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
	log_level = LOG_DEBUG;
	bool fail = false;
	{
		debug_parse = true;
		std::string s;

		s = " v1, v2, v3 \t";
		printf("strip \"%s\": \"%s\"\n", s.c_str(), strip(s).c_str());
		assert( strip(s) == "v1, v2, v3" );

		s = "v1, v2 ,v3 ";
		printf("split_str \"%s\": %s\n", s.c_str(), vector_to_str(split_str(s, ",")).c_str());
		assert( vector_to_str(split_str(s, ",")) == "[\"v1\", \"v2\", \"v3\"]" );

		assert( parseBool("true") == true );
		assert( parseBool("t") == true );
		assert( parseBool("false") == false );
		assert( parseBool("0") == false );
		assert( parseBool("", false, true) == true );
		assert( parseBool(" ", false, false) == false );

		try {s=""; parseBool(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="32"; parseBool(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);

		assert( parseInt32("-3245") == -3245 );
		assert( parseInt64("-3245") == -3245 );
		assert( parseUint32("3245") == 3245 );
		assert( parseUint32("", false, 324) == 324 );
		assert( parseUint64("3245") == 3245 );

		try {s="2a"; parseUint32(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="-2"; parseUint32(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);

		assert( parseDouble("-324.5") == -324.5 );
		assert( parseDouble("-.5") == -.5 );
		assert( parseDouble(".5") == .5 );
		assert( parseDouble("5.") == 5. );
		assert( parseDouble(" ", false, 324.5) == 324.5 );
		try {s=""; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="-"; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="-."; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="-1.a"; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="true"; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);
		try {s="ad2"; parseDouble(s.c_str()); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", s.c_str(), e.what());}
		assert (!fail);

	}

	{ // sprintf
		std::string aux;
		for (int i = 0; i<200; i++) {
			aux += std::to_string(i);
			std::string aux2 = aux + " test %d %d";
			assert( sprintf(aux2.c_str(),123, i) == aux+std::string(" test 123 ")+std::to_string(i) );
		}
	}

	{ // parse*Suffix
		debug_parseSuffix = true;
		std::string str;
		std::map<std::string, uint32_t> u32suf { {"s",1}, {"m",60} };

		uint32_t u32v;
		u32v = parseUint32Suffix("10 ", u32suf);
		assert( u32v == 10 );
		u32v = parseUint32Suffix(" 20s ", u32suf);
		assert( u32v == 20 );
		u32v = parseUint32Suffix("10 m", u32suf);
		assert( u32v == 600 );
		u32v = parseUint32Suffix("0 m", u32suf);
		assert( u32v == 0 );

		try {str=""; u32v = parseUint32Suffix(str.c_str(), u32suf); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", str.c_str(), e.what());}
		assert (!fail);
		try {str=" m"; u32v = parseUint32Suffix(str.c_str(), u32suf); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", str.c_str(), e.what());}
		assert (!fail);
		try {str="m0"; u32v = parseUint32Suffix(str.c_str(), u32suf); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", str.c_str(), e.what());}
		assert (!fail);

		double dv;
		std::map<std::string, double> dsuf { {"K",1000}, {"M",1000000} };
		dv = parseDoubleSuffix("1.1K", dsuf);
		assert( dv==1100 );
		dv = parseDoubleSuffix("-1.2M", dsuf);
		assert( dv==-1200000 );
		dv = parseDoubleSuffix("-0M", dsuf);
		assert( dv==0 );
		dv = parseDoubleSuffix("0M", dsuf);
		assert( dv==0 );

		try {str="--0M"; dv = parseDoubleSuffix(str.c_str(), dsuf); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", str.c_str(), e.what());}
		assert (!fail);
		try {str="5.5s"; dv = parseDoubleSuffix(str.c_str(), dsuf); fail = true;} catch (std::exception& e) {printf("expected exception for \"%s\": %s\n", str.c_str(), e.what());}
		assert (!fail);
	}

	{
		std::stringstream stream("123; 324 ; abc");
		std::string s;
		while (std::getline(stream, s, ';')) {
			printf("%s\n", inplace_strip(s).c_str());
		}
	}

	{
#		define TEST_PARSE(str, pattern) { \
			ParseRE p(str, pattern); \
			printf("ParseRE(\"%s\", \"%s\"): valid=%s, value=%s\n", str, pattern, p.valid?"true":"false", p.value.c_str()); }

		TEST_PARSE("test ", "[0-9]+");
		TEST_PARSE("test 123", "[0-9]+");
		TEST_PARSE("test 123 456", "[0-9]+");
		TEST_PARSE("test 123", "([0-9]+)");
		TEST_PARSE("test 123 456", "([0-9]+)");
		TEST_PARSE("test 123", "test ([0-9]+)");

		ParseRE r;
		if ((r = ParseRE("test 123", "[0-9]+")).valid) {
			printf("ParseRE.sm.size() = %ld\n", r.sm.size());
		}
		if ((r = ParseRE("test 123", "([0-9]+)")).valid) {
			printf("ParseRE.sm.size() = %ld\n", r.sm.size());
		}
	}

	printf("OK!!\n");
	return 0;
}
