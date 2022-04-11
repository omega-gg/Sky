#pragma once
/*
* Copyright 2016 Huy Cuong Nguyen
* Copyright 2006 Jeremias Maerki.
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

namespace ZXing {

class ByteArray;

namespace DataMatrix {

class SymbolInfo;

/**
 * Creates and interleaves the ECC200 error correction for an encoded message.
 *
 * @param codewords  the codewords
 * @param symbolInfo information about the symbol to be encoded
 * @return the codewords with interleaved error correction.
 */
void EncodeECC200(ByteArray& codewords, const SymbolInfo& symbolInfo);

} // DataMatrix
} // ZXing
