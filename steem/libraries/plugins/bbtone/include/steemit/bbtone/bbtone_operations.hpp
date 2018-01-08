#pragma once

#include <steemit/protocol/base.hpp>
#include <steemit/protocol/block_header.hpp>
#include <steemit/protocol/asset.hpp>

#include <fc/utf8.hpp>
#include <fc/crypto/equihash.hpp>
#include <fc/api.hpp>

#include <steemit/bbtone/bbtone_objects.hpp>
#include <steemit/bbtone/bbtone_plugin.hpp>

namespace steemit { namespace bbtone {

using namespace steemit::protocol;

struct offer_create_operation : base_operation
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

struct request_start_operation : base_operation
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

struct request_accept_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;
    string              encrypted_service_data;

    flat_set<account_name_type> required_posting_auths;

    void get_required_posting_authorities( flat_set<account_name_type>& a )const{  for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct request_ready_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{  for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct request_inwork_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct request_report_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;
    asset               charge;
    string              charge_data;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

struct request_end_operation : base_operation
{
    account_name_type   operator_name;
    uint64_t            target_request_id;
    uint32_t            error_code;

    flat_set<account_name_type> required_posting_auths;

    void  get_required_posting_authorities( flat_set<account_name_type>& a )const{ for( const auto& i : required_posting_auths ) a.insert(i); }
};

typedef fc::static_variant<

         offer_create_operation,
         offer_cancel_operation,

         request_start_operation,
         request_accept_operation,
         request_ready_operation,
         request_inwork_operation,
         request_report_operation,
         request_end_operation

      > bbtone_plugin_operation;

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, offer_create );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, offer_cancel );

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_start );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_accept );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_ready );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_inwork );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_report );
DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, bbtone_plugin_operation, request_end );

} } // steemit::bbtone

FC_REFLECT( steemit::bbtone::offer_create_operation,
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

FC_REFLECT( steemit::bbtone::request_start_operation,
    (issuer_operator_name)
    (target_offer_id)
    (request_ttl)
    (credits)
    (user_id)
    (user_pub_key)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::request_accept_operation,
    (operator_name)
    (target_request_id)
    (encrypted_service_data)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::request_ready_operation,
    (operator_name)
    (target_request_id)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::request_inwork_operation,
    (operator_name)
    (target_request_id)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::request_report_operation,
    (operator_name)
    (target_request_id)
    (charge)
    (required_posting_auths)
);

FC_REFLECT( steemit::bbtone::request_end_operation,
    (operator_name)
    (target_request_id)
    (error_code)
    (required_posting_auths)
);

DECLARE_OPERATION_TYPE( steemit::bbtone::bbtone_plugin_operation );

FC_REFLECT_TYPENAME( steemit::bbtone::bbtone_plugin_operation );
