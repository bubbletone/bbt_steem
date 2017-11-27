
#pragma once

#include <steemit/protocol/base.hpp>
#include <steemit/protocol/types.hpp>

#include <fc/reflect/reflect.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace steemit { namespace bbtone {

struct bbtone_operation : public steemit::protocol::base_operation
{
    protocol::account_name_type  from;
    protocol::account_name_type  to;
    protocol::public_key_type    from_memo_key;
    protocol::public_key_type    to_memo_key;
    uint64_t                     sent_time = 0; /// used as seed to secret generation
    uint32_t                     checksum = 0;
    std::vector<char>            encrypted_message;
};

typedef fc::static_variant< bbtone_operation > bbtone_plugin_operation;

} }

FC_REFLECT( steemit::bbtone::bbtone_operation, (from)(to)(from_memo_key)(to_memo_key)(sent_time)(checksum)(encrypted_message) )

DECLARE_OPERATION_TYPE( steemit::bbtone::bbtone_plugin_operation )
FC_REFLECT_TYPENAME( steemit::bbtone::bbtone_plugin_operation )
