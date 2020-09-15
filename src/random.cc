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

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "RandEnginesImpl::"

template <typename T>
class RandEngineImpl : public RandEngine {
	std::mt19937_64 reng;
	std::uniform_real_distribution<double> uniform_01_;    // both classes
	std::uniform_int_distribution<T>       uniform_keys_;  // class ScrambledZipfDistribution

	void setSeed() {
		auto seed = static_cast<uint64_t>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
		seed += seed * 1000000;
		PRINT_DEBUG("seed       = %s", v2s(seed));
		reng.seed(seed);
	}

	public:
	RandEngineImpl() { setSeed(); }
	RandEngineImpl(T n) {
		setSeed();
		uniform_keys_ = std::uniform_int_distribution<T>(1, n);
	}
	double uniform_01()   { return uniform_01_(reng); }
	double uniform_keys() { return uniform_keys_(reng); }
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
	auto rand_engine_impl = reinterpret_cast<RandEngineImpl<T>*>(default_rand_engine.get());
	if (rand_engine != nullptr)
		rand_engine_impl = reinterpret_cast<RandEngineImpl<T>*>(rand_engine);

	double u = rand_engine_impl->uniform_01();
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
	assert(sample_size > 0 && sample_size <= n);

	auto rand_engine_impl = new RandEngineImpl<T>(n);
	default_rand_engine.reset(rand_engine_impl);

	zipf.reset(new ZipfDistribution<T>(n, theta));

	for (T i=0; i<sample_size; i++)
		sample_list.push_back(rand_engine_impl->uniform_keys());

	PRINT_DEBUG("sample_list size: %s", v2s(sample_size * sizeof(sample_size)));
}

template <typename T>
T ScrambledZipfDistribution<T>::next(RandEngine* rand_engine) {
	auto rand_engine_impl = reinterpret_cast<RandEngineImpl<T>*>(default_rand_engine.get());
	if (rand_engine != nullptr)
		rand_engine_impl = reinterpret_cast<RandEngineImpl<T>*>(rand_engine);

	auto r = zipf->next(rand_engine);
	if (r <= sample_size)
		return sample_list[r-1];
	return rand_engine_impl->uniform_keys();
}

template <typename T>
RandEngine* ScrambledZipfDistribution<T>::newEngine() {
	return new RandEngineImpl<T>(n);
}

template class ScrambledZipfDistribution<int32_t>;
template class ScrambledZipfDistribution<int64_t>;

} // namespace alutils
