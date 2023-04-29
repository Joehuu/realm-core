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

#ifndef REALM_IMPL_INPUT_STREAM_HPP
#define REALM_IMPL_INPUT_STREAM_HPP

#include <realm/util/input_stream.hpp>

#include <realm/column_binary.hpp>
#include <realm/impl/cont_transact_hist.hpp>

namespace realm::_impl {
class ChangesetInputStream : public util::InputStream {
public:
    using version_type = History::version_type;

    ChangesetInputStream(History& hist, version_type begin_version, version_type end_version)
        : m_history(hist)
        , m_begin_version(begin_version)
        , m_end_version(end_version)
    {
    }

    util::Span<const char> next_block() override
    {
        while (get_changeset()) {
            if (BinaryData actual = m_changesets_begin->get_next(); actual.size() > 0) {
                return actual;
            }
            m_changesets_begin++;
        }
        return {nullptr, nullptr}; // End of input
    }

private:
    static constexpr version_type NB_BUFFERS = 8;
    History& m_history;
    version_type m_begin_version, m_end_version;
    BinaryIterator m_changesets[NB_BUFFERS]; // Buffer
    BinaryIterator* m_changesets_begin = nullptr;
    BinaryIterator* m_changesets_end = nullptr;

    bool get_changeset() noexcept
    {
        if (m_changesets_begin != m_changesets_end) {
            return true;
        }
        auto versions_to_get = std::min(m_end_version - m_begin_version, NB_BUFFERS);
        if (versions_to_get == 0) {
            return false;
        }

        version_type end_version = m_begin_version + versions_to_get;
        m_history.get_changesets(m_begin_version, end_version, m_changesets);
        m_begin_version = end_version;
        m_changesets_begin = m_changesets;
        m_changesets_end = m_changesets_begin + versions_to_get;
        return true;
    }
};

} // namespace realm::_impl

#endif // REALM_IMPL_INPUT_STREAM_HPP
