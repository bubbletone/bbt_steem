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

#include <steemit/bbtone/bbtone_evaluator.hpp>
#include <steemit/bbtone/bbtone_operations.hpp>

#ifndef IS_LOW_MEM
#include <diff_match_patch.h>
#include <boost/locale/encoding_utf.hpp>

using boost::locale::conv::utf_to_utf;

std::wstring utf8_to_wstring(const std::string& str)
{
    return utf_to_utf<wchar_t>(str.c_str(), str.c_str() + str.size());
}

std::string wstring_to_utf8(const std::wstring& str)
{
    return utf_to_utf<char>(str.c_str(), str.c_str() + str.size());
}

#endif

#include <fc/uint128.hpp>
#include <fc/utf8.hpp>

#include <limits>

namespace steemit { namespace bbtone {
	using namespace steemit::chain;
	using namespace steemit::protocol;
   using fc::uint128_t;
	using fc::api;

void bbtone_offer_create_evaluator::do_apply( const bbtone_offer_create_operation& o )
{
	FC_ASSERT( o.expiration > _db.head_block_time(), "Limit order has to expire after head block time." );

   const auto& owner = _db.get_account( o.owner );

   FC_ASSERT( _db.get_balance( owner, o.amount_to_sell.symbol ) >= o.amount_to_sell, "Account does not have sufficient funds for limit order." );

   _db.adjust_balance( owner, -o.amount_to_sell );

   const auto& order = _db.create<limit_order_object>( [&]( limit_order_object& obj )
   {
       obj.created    = _db.head_block_time();
       obj.seller     = o.owner;
       obj.orderid    = o.orderid;
       obj.for_sale   = o.amount_to_sell.amount;
       obj.sell_price = o.get_price();
       obj.expiration = o.expiration;
   });

   bool filled = _db.apply_order( order );

   if( o.fill_or_kill ) FC_ASSERT( filled, "Cancelling order because it was not filled." );
}

void bbtone_offer_cancel_evaluator::do_apply( const bbtone_offer_cancel_operation& o )
{
   // _db.cancel_order( _db.get_limit_order( o.owner, o.orderid ) );
}


} } // steemit::chain
