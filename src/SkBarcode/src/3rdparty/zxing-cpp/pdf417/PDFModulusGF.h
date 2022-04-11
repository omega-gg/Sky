#pragma once
/*
* Copyright 2016 Nu-book Inc.
* Copyright 2016 ZXing authors
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "PDFModulusPoly.h"
#include "ZXConfig.h"

#include <stdexcept>

namespace ZXing {
namespace Pdf417 {

/**
* <p>A field based on powers of a generator integer, modulo some modulus.< / p>
*
* @author Sean Owen
* @see com.google.zxing.common.reedsolomon.GenericGF
*/
class ModulusGF
{
	int _modulus;
	std::vector<short> _expTable;
	std::vector<short> _logTable;
	ModulusPoly _zero;
	ModulusPoly _one;

	// avoid using the '%' modulo operator => ReedSolomon computation is more than twice as fast
	// see also https://stackoverflow.com/a/33333636/2088798
	static int fast_mod(int a, int d) { return a < d ? a : a - d; }

public:
	ModulusGF(int modulus, int generator);

	const ModulusPoly& zero() const {
		return _zero;
	}

	const ModulusPoly& one() const {
		return _one;
	}

	ModulusPoly buildMonomial(int degree, int coefficient) const;

	int add(int a, int b) const {
		return fast_mod(a + b, _modulus);
	}

	int subtract(int a, int b) const {
		return fast_mod(_modulus + a - b, _modulus);
	}

	int exp(int a) const {
		return _expTable.at(a);
	}

	int log(int a) const {
		if (a == 0) {
			throw std::invalid_argument("a == 0");
		}
		return _logTable[a];
	}

	int inverse(int a) const {
		if (a == 0) {
			throw std::invalid_argument("a == 0");
		}
		return _expTable[_modulus - _logTable[a] - 1];
	}

	int multiply(int a, int b) const {
		if (a == 0 || b == 0) {
			return 0;
		}
#ifdef ZX_REED_SOLOMON_USE_MORE_MEMORY_FOR_SPEED
		return _expTable[_logTable[a] + _logTable[b]];
#else
		return _expTable[fast_mod(_logTable[a] + _logTable[b], _modulus - 1)];
#endif
	}

	int size() const {
		return _modulus;
	}
};

} // Pdf417
} // ZXing
