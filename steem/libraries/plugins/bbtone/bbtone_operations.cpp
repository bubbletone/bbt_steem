#include <steemit/protocol/operation_util_impl.hpp>

#include <steemit/bbtone/bbtone_operations.hpp>

namespace steemit { namespace bbtone {

   /* bool inline is_asset_type( asset asset, asset_symbol_type symbol )
   {
      return asset.symbol == symbol;
   }*/

/*
   void bbtone_offer_create_operation::validate()const
   {
      validate_account_name( owner );
      // FC_ASSERT( ( is_asset_type( amount_to_sell, STEEM_SYMBOL ) && is_asset_type( min_to_receive, SBD_SYMBOL ) )
      //   || ( is_asset_type( amount_to_sell, SBD_SYMBOL ) && is_asset_type( min_to_receive, STEEM_SYMBOL ) ),
      //   "Limit offer must be for the STEEM:SBD market" );
      //(amount_to_sell / min_to_receive).validate();
   }

   void bbtone_offer_cancel_operation::validate()const
   {
      validate_account_name( owner );
   }

*/
} } // steemit::bbtone

DEFINE_OPERATION_TYPE( steemit::bbtone::bbtone_plugin_operation )
