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

#include "PDFBarcodeValue.h"

#include <algorithm>

namespace ZXing {
namespace Pdf417 {

/**
* Add an occurrence of a value
*/
void
BarcodeValue::setValue(int value)
{
	_values[value] += 1;
}

/**
* Determines the maximum occurrence of a set value and returns all values which were set with this occurrence.
* @return an array of int, containing the values with the highest occurrence, or null, if no value was set
*/
std::vector<int>
BarcodeValue::value() const
{
	std::vector<int> result;
	if (!_values.empty()) {
		int maxConfidence = std::max_element(_values.begin(), _values.end(), [](auto& l, auto& r) { return l.second < r.second; })->second;
		for (auto [value, count] : _values)
			if (count == maxConfidence)
				result.push_back(value);
	}
	return result;
}

int
BarcodeValue::confidence(int value) const
{
	auto it = _values.find(value);
	return it != _values.end() ? it->second : 0;
}

} // Pdf417
} // ZXing
