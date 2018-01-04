#pragma once

#include <steemit/protocol/base.hpp>
#include <steemit/protocol/block_header.hpp>
#include <steemit/protocol/asset.hpp>

#include <fc/utf8.hpp>
#include <fc/crypto/equihash.hpp>
#include <fc/api.hpp>

#include <steemit/bbtone/bbtone_plugin.hpp>

namespace steemit { namespace bbtone {

using namespace steemit::protocol;

   /*inline void validate_account_name( const string& name )
   {
      FC_ASSERT( is_valid_account_name( name ), "Account name ${n} is invalid", ("n", name) );
   }

   inline void validate_permlink( const string& permlink )
   {
      FC_ASSERT( permlink.size() < STEEMIT_MAX_PERMLINK_LENGTH, "permlink is too long" );
      FC_ASSERT( fc::is_utf8( permlink ), "permlink not formatted in UTF8" );
   }*/


struct offer_create_operation : base_operation
{
    account_name_type   operator_name;
    uint32_t            offer_id; /// an ID assigned by owner, must be unique
    uint32_t            service_id; // ID of service (tariff information, metadata, etc)
    uint32_t            service_ttl; // TTL of offer in seconds
    asset               service_fee;

//    void  validate()const;
//    void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
};

struct offer_cancel_operation : base_operation
{
    account_name_type   operator_name;
    uint32_t            offer_id;

//    void  validate()const;
//    void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
};

typedef fc::static_variant<
         offer_create_operation,
         offer_cancel_operation
      > bbtone_plugin_operation;

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, offer_create );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, offer_cancel );

} } // steemit::bbtone

FC_REFLECT( steemit::bbtone::offer_create_operation, (operator_name)(offer_id)(service_id)(service_ttl)(service_fee) )
FC_REFLECT( steemit::bbtone::offer_cancel_operation, (operator_name)(offer_id) )

DECLARE_OPERATION_TYPE( steemit::bbtone::bbtone_plugin_operation )

FC_REFLECT_TYPENAME( steemit::bbtone::bbtone_plugin_operation )
