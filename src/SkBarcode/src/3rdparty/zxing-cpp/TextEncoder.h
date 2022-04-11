#pragma once
/*
* Copyright 2016 Nu-book Inc.
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

#include <string>

namespace ZXing {

enum class CharacterSet;

class TextEncoder
{
	static void GetBytes(const std::wstring& str, CharacterSet charset, std::string& bytes);
public:
	static std::string FromUnicode(const std::wstring& str, CharacterSet charset) {
		std::string r;
		GetBytes(str, charset, r);
		return r;
	}
};

} // ZXing
