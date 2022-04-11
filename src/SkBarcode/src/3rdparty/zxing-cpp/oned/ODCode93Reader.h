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

#include "ODRowReader.h"

namespace ZXing {
namespace OneD {

/**
* <p>Decodes Code 93 barcodes.</p>
*
* @author Sean Owen
* @see Code39Reader
*/
class Code93Reader : public RowReader
{
public:
	Result decodePattern(int rowNumber, const PatternView& row, std::unique_ptr<DecodingState>&) const override;
};

} // OneD
} // ZXing
