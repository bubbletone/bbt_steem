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
class offer_object : public object< offer_object_type, offer_object >
{
public:
    template< typename Constructor, typename Allocator >
    offer_object( Constructor&& c, allocator< Allocator > a)
    {
        c( *this );
    }

    id_type             id;

    account_name_type   operator_name;
    uint32_t            offer_id;
    uint32_t            null_sig;
    time_point          tx_time;
    uint32_t            service_ttl;
    uint32_t            service_id;
    asset               service_fee;
};

typedef offer_object::id_type offer_id_type;

namespace offer_index_tag {
    struct by_id;
    struct by_operator_name;
    struct by_operator_name_offer_id;
}

typedef multi_index_container<
    offer_object,
    indexed_by<
        ordered_unique< tag< offer_index_tag::by_id >, member< offer_object, offer_id_type, &offer_object::id > >,
        ordered_non_unique< tag< offer_index_tag::by_operator_name >, member< offer_object, account_name_type, &offer_object::operator_name > >,
        ordered_unique< tag< offer_index_tag::by_operator_name_offer_id >,
            composite_key< offer_object,
                member< offer_object, account_name_type, &offer_object::operator_name >,
                member< offer_object, uint32_t, &offer_object::offer_id >
            >,
            composite_key_compare< std::less< string >, std::greater< uint32_t > >
        >
    >,
    allocator< offer_object >
> offer_index;



/**** Request Object ****/


}}

FC_REFLECT( steemit::bbtone::offer_object, (id)(operator_name)(offer_id)(null_sig)(tx_time)(service_ttl)(service_id)(service_fee) );
CHAINBASE_SET_INDEX_TYPE( steemit::bbtone::offer_object, steemit::bbtone::offer_index );
