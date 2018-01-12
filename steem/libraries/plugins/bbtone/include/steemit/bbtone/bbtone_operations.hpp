#pragma once

#include <steemit/protocol/base.hpp>
#include <steemit/protocol/block_header.hpp>
#include <steemit/protocol/asset.hpp>

#include <fc/utf8.hpp>
#include <fc/crypto/equihash.hpp>
#include <fc/api.hpp>

namespace steemit { namespace bbtone {

using namespace steemit::protocol;

struct create_service_offer_operation : base_operation
{
    account_name_type   operator_name;
    uint32_t            offer_ttl; // TTL of offer in seconds
    uint64_t            offer_local_id; /// an ID assigned by owner, must be unique
    string              offer_data;
    asset               price;

    flat_set<account_name_type> required_posting_auths;

    void get_required_posting_authorities( flat_set<account_name_type>& a )const{  for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct offer_cancel_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            offer_id;

    flat_set<account_name_type> required_posting_auths;

    void get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct attach_request_to_service_offer_operation : base_operation
{
    account_name_type   issuer_operator_name;
    uint64_t            target_offer_id;
    uint32_t            request_ttl;
    asset               credits;
    string              user_id;
    fc::ecc::public_key user_pub_key;

    flat_set<account_name_type> required_posting_auths;

    void get_required_posting_authorities( flat_set<account_name_type>& a )const{  for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct attach_charge_to_service_request_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;
    asset               charge;
    string              charge_data;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct attach_refund_to_service_request_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;
    uint32_t            error_code;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

typedef fc::static_variant<

         create_service_offer_operation,
         offer_cancel_operation,

         attach_request_to_service_offer_operation,
         attach_charge_to_service_request_operation,
         attach_refund_to_service_request_operation

      > bbtone_plugin_operation;

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, create_service_offer );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, offer_cancel );

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, attach_request_to_service_offer );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, attach_charge_to_service_request );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, attach_refund_to_service_request );

} } // steemit::bbtone

FC_REFLECT( steemit::bbtone::create_service_offer_operation,
    (operator_name)
    (offer_ttl)
    (offer_local_id)
    (offer_data)
    (price)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::offer_cancel_operation,
    (operator_name)
    (offer_id)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::attach_request_to_service_offer_operation,
    (issuer_operator_name)
    (target_offer_id)
    (request_ttl)
    (credits)
    (user_id)
    (user_pub_key)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::attach_charge_to_service_request_operation,
    (operator_name)
    (target_request_id)
    (charge)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::attach_refund_to_service_request_operation,
    (operator_name)
    (target_request_id)
    (error_code)
    (required_posting_auths)
);

DECLARE_OPERATION_TYPE( steemit::bbtone::bbtone_plugin_operation );

FC_REFLECT_TYPENAME( steemit::bbtone::bbtone_plugin_operation );
