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

void create_service_offer_evaluator::do_apply( const create_service_offer_operation& o )
{
    const auto& owner = _db.get_account( o.operator_name );

    _db.create<offer_object>( [&]( offer_object& obj )
    {
        obj.tx_time         = (time_point)_db.head_block_time();
        obj.operator_name   = owner.name;
        obj.offer_ttl       = o.offer_ttl;
        obj.offer_local_id  = o.offer_local_id;
        obj.offer_data      = o.offer_data;
        obj.price           = o.price;
        obj.state           = offer_active;
    });

    dlog("operation: ${op_type} applied, data: ${data}",
        ("op_type", fc::get_typename<create_service_offer_operation>::name())
        ("data", o)
    );
}

void offer_cancel_evaluator::do_apply( const offer_cancel_operation& o )
{
    const auto & idx = _db.get_index<offer_index>().indices().get<offer_index_tag::by_id>();
    auto offer_it = idx.find(offer_id_type(o.offer_id));
    FC_ASSERT(offer_it != idx.end(), "offer not found");
    FC_ASSERT(offer_it->operator_name == o.operator_name ||
              o.operator_name == STEEMIT_INIT_MINER_NAME, "only offer owner can cancel offer");

    _db.modify<offer_object>(*offer_it, [&]( offer_object& obj )
    {
        obj.state           = offer_completed;
    });

    dlog("operation: ${op_type} applied, data: ${data}",
        ("op_type", fc::get_typename<offer_cancel_operation>::name())
        ("data", o)
    );
}

void attach_request_to_service_offer_evaluator::do_apply( const attach_request_to_service_offer_operation& o )
{
    const auto& issuer = _db.get_account( o.issuer_operator_name );
    FC_ASSERT(issuer.balance >= o.credits, "not enough balance");

    const auto & offer_idx = _db.get_index<offer_index>().indices().get<offer_index_tag::by_id>();
    auto offer_it = offer_idx.find(offer_id_type(o.target_offer_id));
    FC_ASSERT(offer_it != offer_idx.end(), "assignee offer not found");

    if (offer_it->tx_time + fc::seconds(offer_it->offer_ttl) < (time_point)_db.head_block_time()) {
        _db.modify<offer_object>(*offer_it, [&]( offer_object& obj )
        {
            obj.state           = offer_completed;
        });
        FC_THROW_EXCEPTION( fc::assert_exception, "assignee expired");
    }

    FC_ASSERT(offer_it->state == offer_active, "offer not active");
    FC_ASSERT(o.credits <= offer_it->price, "request credits > offer price");


    _db.adjust_balance( issuer, -o.credits );


	// [TODO] funds must be "locked" in request, and allowed to return only by "refund" transaction
    // const auto& assignee = _db.get_account( offer_it->operator_name );
    // _db.adjust_balance( assignee, o.credits );



    _db.create<request_object>( [&]( request_object& obj )
    {
        obj.tx_time = (time_point)_db.head_block_time();
        obj.issuer_operator_name = issuer.name;
        obj.assignee_offer_id = o.target_offer_id;
        obj.request_ttl = o.request_ttl;
        obj.max_credits = o.credits;
        obj.user_id = o.user_id;
        obj.user_pub_key = o.user_pub_key;
        obj.charge = asset(0, STEEM_SYMBOL);
        obj.state = request_attached;
        obj.error_code = 0;
    });

    dlog("operation: ${op_type} applied, data: ${data}",
        ("op_type", fc::get_typename<attach_request_to_service_offer_operation>::name())
        ("data", o)
    );
}

void attach_charge_to_service_request_evaluator::do_apply( const attach_charge_to_service_request_operation& o )
{
    const auto& request_idx = _db.get_index<request_index>().indices().get<request_index_tag::by_id>();
    const auto& request_it = request_idx.find(request_id_type(o.target_request_id));
    FC_ASSERT(request_it != request_idx.end(), "request not found");
    FC_ASSERT(request_it->state == request_attached, "incorrect request state");

    const auto & offer_idx = _db.get_index<offer_index>().indices().get<offer_index_tag::by_id>();
    auto offer_it = offer_idx.find(offer_id_type(request_it->assignee_offer_id));
    FC_ASSERT(offer_it != offer_idx.end(), "assignee offer not found");
    FC_ASSERT(offer_it->operator_name == o.operator_name, "only assignee offer owner can report request");

    FC_ASSERT(request_it->charge + o.charge <= request_it->max_credits, "charge > max_credits");

	// [VARIANT] - simplest version with direct balance modifications. ONLY FOR PROOF_OF_CONCEPT
	// send charge to charging address immediately. The "change" will be taken by requestor with "refund" tx

    _db.modify<request_object>(*request_it, [&]( request_object& obj )
    {
        obj.charge_data = o.charge_data;
        obj.charge += o.charge;
    });

    const auto& assignee = _db.get_account( o.operator_name );
    _db.adjust_balance( assignee, o.charge);

    dlog("operation: ${op_type} applied, data: ${data}",
        ("op_type", fc::get_typename<attach_charge_to_service_request_operation>::name())
        ("data", o)
    );
}

void attach_refund_to_service_request_evaluator::do_apply( const attach_refund_to_service_request_operation& o )
{

    const auto& request_idx = _db.get_index<request_index>().indices().get<request_index_tag::by_id>();
    const auto& request_it = request_idx.find(request_id_type(o.target_request_id));
    FC_ASSERT(request_it != request_idx.end(), "request not found");

    const auto & offer_idx = _db.get_index<offer_index>().indices().get<offer_index_tag::by_id>();
    auto offer_it = offer_idx.find(offer_id_type(request_it->assignee_offer_id));
    FC_ASSERT(offer_it != offer_idx.end(), "assignee offer not found");
    FC_ASSERT(offer_it->operator_name == o.operator_name, "only assignee offer owner can accept request");


    _db.modify<request_object>(*request_it, [&]( request_object& obj )
    {
        obj.state = request_refunded;
        obj.error_code = o.error_code;
    });

    // const auto& assignee = _db.get_account( o.operator_name );
    const auto& issuer = _db.get_account( request_it->issuer_operator_name );
    if (request_it->charge < request_it->max_credits) { // if request not fully charged return remain credits
        _db.adjust_balance( issuer, request_it->max_credits - request_it->charge );
    }

    dlog("operation: ${op_type} applied, data: ${data}",
        ("op_type", fc::get_typename<attach_refund_to_service_request_operation>::name())
        ("data", o)
    );
}

} } // steemit::chain
