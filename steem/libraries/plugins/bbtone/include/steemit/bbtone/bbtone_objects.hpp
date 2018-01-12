#pragma once

#include <steemit/app/plugin.hpp>
#include <steemit/chain/database.hpp>

#include <boost/multi_index/composite_key.hpp>

#include <fc/thread/future.hpp>
#include <fc/api.hpp>

namespace steemit { namespace bbtone {

using namespace boost::multi_index;
using namespace chain;
//
// Plugins should #define their SPACE_ID's so plugins with
// conflicting SPACE_ID assignments can be compiled into the
// same binary (by simply re-assigning some of the conflicting #defined
// SPACE_ID's in a build script).
//
// Assignment of SPACE_ID's cannot be done at run-time because
// various template automagic depends on them being known at compile
// time.
//
#ifndef BBTONE_SPACE_ID
#define BBTONE_SPACE_ID 13
#endif

enum bbtone_object_type
{
    offer_object_type   = ( BBTONE_SPACE_ID << 8 ),
    request_object_type = ( BBTONE_SPACE_ID << 8 ) + 1,
};


/**** Offer Object ****/
enum offer_state
{
    offer_active        = 1,
    offer_completed     = 2
};

class offer_object : public object< offer_object_type, offer_object >
{
public:
    template< typename Constructor, typename Allocator >
    offer_object( Constructor&& c, allocator< Allocator > a)
    {
        c( *this );
    }

    id_type             id;

    time_point          tx_time;
    account_name_type   operator_name;
    uint32_t            offer_ttl;
    uint64_t            offer_local_id;
    string              offer_data;
    asset               price;
    uint32_t            state;
};

typedef offer_object::id_type offer_id_type;

namespace offer_index_tag {
    struct by_id;
    struct by_operator_name;
    struct by_operator_name_offer_local_id;
}

typedef multi_index_container<
    offer_object,
    indexed_by<
        ordered_unique< tag< offer_index_tag::by_id >, member< offer_object, offer_id_type, &offer_object::id > >,
        ordered_non_unique< tag< offer_index_tag::by_operator_name >, member< offer_object, account_name_type, &offer_object::operator_name > >,
        ordered_unique< tag< offer_index_tag::by_operator_name_offer_local_id >,
            composite_key< offer_object,
                member< offer_object, account_name_type, &offer_object::operator_name >,
                member< offer_object, uint64_t, &offer_object::offer_local_id >
            >,
            composite_key_compare< std::less< string >, std::greater< uint64_t > >
        >
    >,
    allocator< offer_object >
> offer_index;


/**** Request Object ****/
enum request_states
{
    request_attached   = 1,
    request_charged    = 2,
    request_refunded   = 3,
};

class request_object : public object< request_object_type, request_object >
{
public:
    template< typename Constructor, typename Allocator >
    request_object( Constructor&& c, allocator< Allocator > a)
    {
        c( *this );
    }

    id_type             id;

    time_point          tx_time;
    account_name_type   issuer_operator_name;
    uint64_t            assignee_offer_id;
    uint32_t            request_ttl;
    string              user_id;
    fc::ecc::public_key user_pub_key;
    asset               max_credits;
    asset               charge;         // actual charge count
    string              charge_data;    // data provided from last attach_charge_to_service_request
    uint32_t            state;
    uint32_t            error_code;     // ending code, 0 is ok;
};

typedef request_object::id_type request_id_type;

namespace request_index_tag {
    struct by_id;
    struct by_issuer_operator_name;
    struct by_state_issuer_operator_name;
    struct by_assignee_offer_id;
    struct by_state_assignee_offer_id;
}

typedef multi_index_container<
    request_object,
    indexed_by<
        ordered_unique< tag< request_index_tag::by_id >, member< request_object, request_id_type, &request_object::id > >,
        ordered_non_unique< tag< request_index_tag::by_issuer_operator_name >, member< request_object, account_name_type, &request_object::issuer_operator_name > >,
        ordered_non_unique< tag< request_index_tag::by_assignee_offer_id >, member< request_object, uint64_t, &request_object::assignee_offer_id > >,
        ordered_non_unique< tag< request_index_tag::by_state_issuer_operator_name >,
            composite_key< request_object,
                member< request_object, uint32_t, &request_object::state >,
                member< request_object, account_name_type, &request_object::issuer_operator_name >
            >,
            composite_key_compare< std::greater< uint32_t >, std::less< string > >
        >,
        ordered_non_unique< tag< request_index_tag::by_state_assignee_offer_id >,
            composite_key< request_object,
                member< request_object, uint32_t, &request_object::state >,
                member< request_object, uint64_t, &request_object::assignee_offer_id >
            >,
            composite_key_compare< std::greater< uint32_t >, std::greater< uint64_t> >
        >
    >,
    allocator< request_object >
> request_index;

}} // namespace steemit::bbtone



FC_REFLECT( steemit::bbtone::offer_object,
    (id)
    (tx_time)
    (operator_name)
    (offer_ttl)
    (offer_local_id)
    (offer_data)
    (price)
    (state)
);
CHAINBASE_SET_INDEX_TYPE( steemit::bbtone::offer_object, steemit::bbtone::offer_index );


FC_REFLECT( steemit::bbtone::request_object,
    (id)
    (tx_time)
    (issuer_operator_name)
    (assignee_offer_id)
    (request_ttl)
    (user_id)
    (user_pub_key)
    (max_credits)
    (charge)
    (charge_data)
    (state)
    (error_code)
);
CHAINBASE_SET_INDEX_TYPE( steemit::bbtone::request_object, steemit::bbtone::request_index );

