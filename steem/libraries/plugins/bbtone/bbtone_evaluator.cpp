#include <steemit/chain/steem_evaluator.hpp>
#include <steemit/chain/database.hpp>
#include <steemit/chain/custom_operation_interpreter.hpp>
#include <steemit/chain/steem_objects.hpp>
#include <steemit/chain/witness_objects.hpp>
#include <steemit/chain/block_summary_object.hpp>

#include <steemit/chain/util/reward.hpp>

#include <steemit/protocol/steem_operations.hpp>
#include <steemit/protocol/operation_util.hpp>
#include <steemit/protocol/operation_util_impl.hpp>
#include <steemit/protocol/steem_virtual_operations.hpp>

#include <steemit/bbtone/bbtone_operations.hpp>
#include <steemit/bbtone/bbtone_objects.hpp>
#include <steemit/bbtone/bbtone_plugin.hpp>

#include <fc/uint128.hpp>
#include <fc/utf8.hpp>

#include <limits>

namespace steemit { namespace bbtone {

using namespace steemit::chain;
using namespace steemit::protocol;
using fc::uint128_t;
using fc::api;


void offer_create_evaluator::do_apply( const offer_create_operation& o )
{
    const auto& owner = _db.get_account( o.operator_name );

    _db.create<offer_object>( [&]( offer_object& obj )
    {
        obj.operator_name = owner.name;
        obj.offer_id    = o.offer_id;
        obj.tx_time     = (time_point)_db.head_block_time();
        obj.service_ttl = o.service_ttl;
        obj.service_id  = o.service_id;
        obj.service_fee = o.service_fee;
    });
}

void offer_cancel_evaluator::do_apply( const offer_cancel_operation& o )
{
   // _db.cancel_order( _db.get_limit_order( o.owner, o.orderid ) );
}


} } // steemit::chain
