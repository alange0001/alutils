// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/random.h"

#include <set>
#include <algorithm>
#include <chrono>
#include <cassert>

namespace alutils {

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ ""

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "RandEnginesImpl::"

template <typename T>
class RandEngineImpl : public RandEngine {
	T n;
	std::uniform_real_distribution<double> uniform_01_;    // both classes
	std::uniform_int_distribution<T>       uniform_keys_;  // class ScrambledZipfDistribution

	void setSeed() {
		auto seed = static_cast<uint64_t>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
		seed += seed * 1000000;
		PRINT_DEBUG("seed       = %s", v2s(seed));
		reng.seed(seed);
	}

	public:
	std::mt19937_64 reng;
	RandEngineImpl() {
		n = 0;
		setSeed();
	}
	RandEngineImpl(T n): n(n) {
		assert( n > 1 );
		setSeed();
		uniform_keys_ = std::uniform_int_distribution<T>(1, n);
	}
	double uniform_01()   { return uniform_01_(reng); }
	double uniform_keys() {
		assert( n > 0 );
		return uniform_keys_(reng);
	}
};

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

	default_rand_engine.reset(new RandEngineImpl<T>);

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
T ZipfDistribution<T>::next(RandEngine* rand_engine) {
	T ret;
	double u;

	if (rand_engine != nullptr)
		u = reinterpret_cast<RandEngineImpl<T>*>(rand_engine)->uniform_01();
	else
		u = reinterpret_cast<RandEngineImpl<T>*>(default_rand_engine.get())->uniform_01();

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

template <typename T>
RandEngine* ZipfDistribution<T>::newEngine() {
	return new RandEngineImpl<T>();
}


template class ZipfDistribution<int32_t>;
template class ZipfDistribution<int64_t>;

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "ScrambledZipfDistribution::"

template <typename T>
ScrambledZipfDistribution<T>::ScrambledZipfDistribution(T n, T sample_size, double theta): n(n), sample_size(sample_size) {
	PRINT_DEBUG("n           = %s", v2s(n));
	PRINT_DEBUG("sample_size = %s", v2s(sample_size));
	PRINT_DEBUG("theta       = %s", v2s(theta));
	assert(sample_size > 0 && sample_size <= n);

	auto rand_engine_impl = new RandEngineImpl<T>(n);
	default_rand_engine.reset(rand_engine_impl);

	zipf.reset(new ZipfDistribution<T>(n, theta));

	if (sample_size <= (T)((double)n*0.8)) {
		std::set<T> used_keys;
		while (used_keys.size() <= sample_size) {
			auto key = rand_engine_impl->uniform_keys();
			if (used_keys.count(key) == 0) {
				used_keys.insert(key);
				sample_list.push_back(key);
			}
		}
	} else {
		for ( T i = 1; i<=n; i++)
			sample_list.push_back(i);
		std::shuffle(sample_list.begin(), sample_list.end(), rand_engine_impl->reng);
	}

	PRINT_DEBUG("sample_list size: %s", v2s(sample_size * sizeof(sample_size)));
}

template <typename T>
T ScrambledZipfDistribution<T>::next(RandEngine* rand_engine) {
	auto r = zipf->next(rand_engine);
	if (r <= sample_size)
		return sample_list[r-1];

	if (rand_engine != nullptr)
		return reinterpret_cast<RandEngineImpl<T>*>(rand_engine)->uniform_keys();
	else
		return reinterpret_cast<RandEngineImpl<T>*>(default_rand_engine.get())->uniform_keys();
}

template <typename T>
RandEngine* ScrambledZipfDistribution<T>::newEngine() {
	return new RandEngineImpl<T>(n);
}

template class ScrambledZipfDistribution<int32_t>;
template class ScrambledZipfDistribution<int64_t>;

} // namespace alutils
