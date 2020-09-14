// Copyright (c) 2020-present, Adriano Lange.  All rights reserved.
// This source code is licensed under both the GPLv2 (found in the
// LICENSE.GPLv2 file in the root directory) and Apache 2.0 License
// (found in the LICENSE.Apache file in the root directory).

#pragma once

namespace alutils {

// Implementation based on:
// J. Gray, et al. “Quickly generating billion-record synthetic databases,”
// in Proceedings of ACM SIGMOD 1994.
class zipf_distribution {
	uint64_t n;
	double theta;
	double alpha;
	double zeta_n;
	double zeta_theta;
	double eta;
	double zeta(double n, double theta);

	public:
	zipf_distribution(uint64_t n, double theta);
	uint64_t next();
};

} // namespace alutils
