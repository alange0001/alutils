// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#include "alutils/print.h"
#include "alutils/internal.h"
#include "alutils/random.h"

#include <random>
#include <cassert>

namespace alutils {

static std::uniform_real_distribution<double> rand_uniform;
static std::default_random_engine reng;

////////////////////////////////////////////////////////////////////////////////////
#undef __CLASS__
#define __CLASS__ "zipf_distribution::"

double zipf_distribution::zeta(double n, double theta) {
	double ans = 0;
	for (double i=1; i<=n; i++)
		ans += std::pow(1.0/i, theta);
	return ans;
}

zipf_distribution::zipf_distribution(uint64_t n, double theta): n(n), theta(theta) {
	PRINT_DEBUG("n          = %s", v2s(n));
	PRINT_DEBUG("theta      = %s", v2s(theta));

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

uint64_t zipf_distribution::next() {
	uint64_t ret;
	double u = rand_uniform(reng);
	double uz = u * zeta_n;
	if (uz < 1.0)
		ret = 1;
	else if (uz < 1.0 + std::pow(0.5, theta))
		ret = 2;
	else
		ret = 1 + static_cast<uint64_t>(static_cast<double>(n) * std::pow(eta*u - eta +1.0, alpha));

	assert( ret >= 1 && ret <= n );
	return ret;
}

} // namespace alutils
