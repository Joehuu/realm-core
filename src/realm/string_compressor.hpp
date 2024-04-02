/*************************************************************************
 *
 * Copyright 2016 Realm Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **************************************************************************/

#ifndef REALM_STRING_COMPRESSOR_HPP
#define REALM_STRING_COMPRESSOR_HPP

#include <realm/utilities.hpp>

using CompressionSymbol = uint16_t;
using CompressedString = std::vector<CompressionSymbol>;

namespace realm {

class StringCompressor {
public:
    StringCompressor();
    ~StringCompressor();

    int compare(CompressedString& A, CompressedString& B);
    int compare(StringData sd, CompressedString& B);

    CompressedString compress(StringData, bool learn);
    std::string decompress(CompressedString& c_str);

private:
    struct SymbolDef {
        CompressionSymbol id;
        CompressionSymbol expansion_a;
        CompressionSymbol expansion_b;
    };

    std::vector<SymbolDef> m_symbols;         // map from symbol -> symbolpair, 2 elements pr entry
    std::vector<SymbolDef> m_compression_map; // perfect hash from symbolpair to its symbol
};

} // namespace realm

#endif
