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

#include "Reader.h"

#include <memory>
#include <vector>

namespace ZXing {

class DecodeHints;

namespace OneD {

class RowReader;

/**
* @author dswitkin@google.com (Daniel Switkin)
* @author Sean Owen
*/
class Reader : public ZXing::Reader
{
public:
	explicit Reader(const DecodeHints& hints);
    ~Reader() override;

	Result decode(const BinaryBitmap& image) const override;

private:
	std::vector<std::unique_ptr<RowReader>> _readers;
	bool _tryHarder;
	bool _tryRotate;
	bool _isPure;
};

} // OneD
} // ZXing
