// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include <alutils/print.h>
#include <alutils/string.h>
#include <alutils/random.h>

#include <cmath>
#include <random>
#include <cassert>

#include <stdio.h>

using namespace alutils;

#define v2s(val) std::to_string(val).c_str()


int main(int argc, char** argv) {
	printf("\n\n");
	printf("=====================\n");
	printf("random-test:\n");
	log_level = LOG_DEBUG_OUT;

	const uint64_t n_items = 1000000;
	uint64_t items[n_items];
	for (uint64_t i=0; i<n_items; i++)
		items[i]=0;

	zipf_distribution zipf(n_items, 0.99);
	for (uint64_t i=1; i<100000000; i++) {
		auto r = zipf.next();
		items[r-1]++;
	}

	for (uint64_t i=1; i<=10; i++) {
		printf("items[%s] = %s\n", v2s(i), v2s(items[i-1]));
	}
	if (n_items >=20) for (uint64_t i=n_items-10; i<=n_items; i++) {
		printf("items[%s] = %s\n", v2s(i), v2s(items[i-1]));
	}

	/*for (uint64_t i=2; i<=n_items; i++) {
		if (items[i-2]*2.0 < items[i-1]*1.0) {
			printf("possible inconsistent distribution: items[%s]=%s, items[%s]=%s\n", v2s(i-2), v2s(items[i-2]), v2s(i-1), v2s(items[i-1]));
		}
	}*/


	printf("OK!!\n");
	return 0;
}
