// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

#include <memory>
#include <vector>
#include <random>

namespace alutils {

class RandEngine {};

// Implementation based on:
// J. Gray, et al. “Quickly generating billion-record synthetic databases,”
// in Proceedings of ACM SIGMOD 1994.
// and
// https://github.com/brianfrankcooper/YCSB/blob/master/core/src/main/java/site/ycsb/generator/ZipfianGenerator.java
template <typename T>
class ZipfDistribution {
	T      n;
	double theta;
	double alpha;
	double zeta_n;
	double zeta_theta;
	double eta;
	double zeta(T n, double theta);

	std::unique_ptr<RandEngine> default_rand_engine;

	public:
	ZipfDistribution(T n, double theta);
	T next(RandEngine* rand_engine=nullptr);
	RandEngine* newEngine(); // use one engine per thread
};

typedef ZipfDistribution<int32_t> ZipfDistributionUint32;
typedef ZipfDistribution<int64_t> ZipfDistributionUint64;

template <typename T>
class ScrambledZipfDistribution {
	T              n;
	T              sample_size;
	std::vector<T> sample_list;

	std::unique_ptr<ZipfDistribution<T>> zipf;
	std::unique_ptr<RandEngine>          default_rand_engine;

	public:
	ScrambledZipfDistribution(T n, T sample_size, double theta);
	T next(RandEngine* rand_engine=nullptr);
	RandEngine* newEngine(); // use one engine per thread
};

typedef ScrambledZipfDistribution<int32_t> ScrambledZipfDistributionUint32;
typedef ScrambledZipfDistribution<int64_t> ScrambledZipfDistributionUint64;

} // namespace alutils
