// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/random.h"

#include <cassert>
#include <unistd.h>
#include <chrono>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

static std::uniform_real_distribution<double> rand_uniform;

std::mt19937_64 reng;

static void setSeed() {
	static bool set = false;
	if (not set) {
		std::chrono::system_clock::time_point beginning;
		auto seed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginning).count();
		seed += seed * 1000000;
		PRINT_DEBUG("seed       = %s", v2s(seed));
		reng.seed(seed);
		set = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "ZipfDistribution::"

template <typename T>
double ZipfDistribution<T>::zeta(T n, double theta) {
	double ans = 0;
	for (double i=1; i<=n; i++)
		ans += std::pow(1.0/i, theta);
	return ans;
}

template <typename T>
ZipfDistribution<T>::ZipfDistribution(T n, double theta): n(n), theta(theta) {
	PRINT_DEBUG("n          = %s", v2s(n));
	PRINT_DEBUG("theta      = %s", v2s(theta));
	assert(n > 1);
	assert(theta > 0);

	setSeed();

	alpha = 1.0 / (1.0 - theta);
	PRINT_DEBUG("alpha      = %s", v2s(alpha));

	zeta_n = zeta(n, theta);
	PRINT_DEBUG("zeta_n     = %s", v2s(zeta_n));

	zeta_theta = zeta(2, theta);
	PRINT_DEBUG("zeta_theta = %s", v2s(zeta_theta));

	eta = (1.0 - std::pow(2.0 / static_cast<double>(n), 1.0 - theta))
	    / (1.0 - zeta_theta / zeta_n);
	PRINT_DEBUG("eta        = %s", v2s(eta));
}

template <typename T>
T ZipfDistribution<T>::next() {
	T ret;
	double u = rand_uniform(reng);
	double uz = u * zeta_n;
	if (uz < 1.0)
		ret = 1;
	else if (uz < 1.0 + std::pow(0.5, theta))
		ret = 2;
	else
		ret = 1 + static_cast<T>(static_cast<double>(n) * std::pow(eta*u - eta +1.0, alpha));

	assert( ret >= 1 && ret <= n );
	return ret;
}

template class ZipfDistribution<int32_t>;
template class ZipfDistribution<int64_t>;

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "ScrambledZipfDistribution::"

template <typename T>
ScrambledZipfDistribution<T>::ScrambledZipfDistribution(T n, T sample_size, double theta): n(n), sample_size(sample_size) {
	assert(sample_size > 0 && sample_size <= n);

	setSeed();

	zipf.reset(new ZipfDistribution<T>(n, theta));

	rand_uniform_keys = std::uniform_int_distribution<T>(1, n);
	for (T i=0; i<sample_size; i++)
		sample_list.push_back(rand_uniform_keys(reng));

	PRINT_DEBUG("sample_list size: %s", v2s(sample_size * sizeof(sample_size)));
}

template <typename T>
T ScrambledZipfDistribution<T>::next() {
	auto r = zipf->next();
	if (r <= sample_size)
		return sample_list[r-1];
	return rand_uniform_keys(reng);
}

template class ScrambledZipfDistribution<int32_t>;
template class ScrambledZipfDistribution<int64_t>;

} // namespace alutils
