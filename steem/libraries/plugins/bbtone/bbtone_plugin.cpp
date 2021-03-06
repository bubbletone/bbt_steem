/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <steemit/bbtone/bbtone_plugin.hpp>

#include <steemit/app/impacted.hpp>

#include <steemit/protocol/config.hpp>

#include <steemit/chain/database.hpp>
#include <steemit/chain/index.hpp>
#include <steemit/chain/generic_custom_operation_interpreter.hpp>

#include <fc/smart_ref_impl.hpp>
#include <fc/thread/thread.hpp>


#include <steemit/app/application.hpp>

#include <steemit/chain/database.hpp>
#include <steemit/chain/database_exceptions.hpp>

#include <graphene/utilities/key_conversion.hpp>


using namespace steemit;
using namespace steemit::chain;
using namespace steemit::protocol;


namespace steemit { namespace bbtone {

namespace detail
{

class bbtone_plugin_impl
{
public:
    bbtone_plugin_impl(bbtone_plugin& _plugin);
    virtual ~bbtone_plugin_impl();

    steemit::chain::database& database()
    {
        return _self.database();
    }

    bbtone_plugin& _self;
    std::shared_ptr< generic_custom_operation_interpreter< steemit::bbtone::bbtone_plugin_operation > > _custom_operation_interpreter;

    flat_map<string,string> _tracked_accounts;
};

bbtone_plugin_impl::bbtone_plugin_impl( bbtone_plugin& _plugin )
   : _self( _plugin )
{
    _custom_operation_interpreter = std::make_shared< generic_custom_operation_interpreter< steemit::bbtone::bbtone_plugin_operation > >( database() );

    _custom_operation_interpreter->register_evaluator< create_service_offer_evaluator >( &_self );
    _custom_operation_interpreter->register_evaluator< offer_cancel_evaluator >( &_self );
    _custom_operation_interpreter->register_evaluator< attach_request_to_service_offer_evaluator >( &_self );
    _custom_operation_interpreter->register_evaluator< attach_charge_to_service_request_evaluator >( &_self );
    _custom_operation_interpreter->register_evaluator< attach_refund_to_service_request_evaluator >( &_self );

    database().set_custom_operation_interpreter( _self.plugin_name(), _custom_operation_interpreter );
    return;
}

bbtone_plugin_impl::~bbtone_plugin_impl()
{
   return;
}

} // end namespace detail

bbtone_plugin::bbtone_plugin( application* app )
   : plugin( app ), my( new detail::bbtone_plugin_impl(*this) )
{
}

bbtone_plugin::~bbtone_plugin()
{
}

std::string bbtone_plugin::plugin_name()const
{
    return "bbtone";
}

void bbtone_plugin::plugin_set_program_options(
    boost::program_options::options_description& cli,
    boost::program_options::options_description& cfg
    )
{
    cli.add_options()
            ("pm-account-range", boost::program_options::value<std::vector<std::string>>()->composing()->multitoken(), "Defines a range of accounts to private messages to/from as a json pair [\"from\",\"to\"] [from,to)")
            ;
    cfg.add(cli);
}

void bbtone_plugin::plugin_initialize(const boost::program_options::variables_map& options)
{
    ilog("Intializing bbtone plugin" );
    chain::database& db = database();
    add_plugin_index< offer_index >(db);
    add_plugin_index< request_index >(db);

    app().register_api_factory<bbtone_api>("bbtone_api");

    typedef pair<string,string> pairstring;
    LOAD_VALUE_SET(options, "pm-accounts", my->_tracked_accounts, pairstring);
}


map<string, string> bbtone_api::create_service_offer(string offering_operator_name, uint64_t offer_local_id,
                    string offer_data, uint32_t offer_ttl, asset price)const
{
    create_service_offer_operation op;
    op.operator_name = offering_operator_name;
    op.offer_ttl = offer_ttl;
    op.offer_local_id = offer_local_id;
    op.offer_data = offer_data;
    op.price = price;

    auto tx_id = get_plugin().broadcast_op(op);

    map<string, string> res;
    res.insert(pair<string,string>("tx_id", tx_id.str()));

    return res;
}

vector< offer_object > bbtone_api::get_service_offers_by_operator_name(string offering_operator_name, uint32_t limit)const
{
    vector< offer_object > res;
    const auto & idx = _app->chain_database()->get_index<offer_index>().indices().get<offer_index_tag::by_state_operator_name>();
    auto startIt = idx.lower_bound(std::make_tuple(offer_active, offering_operator_name));
    auto endIt = idx.upper_bound(std::make_tuple(offer_active, offering_operator_name));

    for (auto it = startIt; res.size() < limit && it != endIt; ++it) {
        if (it->tx_time + fc::seconds(it->offer_ttl) >= fc::time_point(fc::seconds(std::time(nullptr))))
            res.push_back(*it);
    }

    return res;
}

std::map<string, string> bbtone_api::attach_request_to_service_offer(string requesting_operator_name, uint64_t target_offer_id, uint32_t request_ttl,
    asset credits, string user_id, fc::ecc::public_key user_pub_key)const
{
    attach_request_to_service_offer_operation op;
    op.issuer_operator_name = requesting_operator_name;
    op.target_offer_id = target_offer_id;
    op.request_ttl = request_ttl;
    op.credits = credits;
    op.user_id = user_id;
    op.user_pub_key = user_pub_key;

    auto tx_id = get_plugin().broadcast_op(op);

    std::map<string, string> res;
    res.insert(pair<string,string>("tx_id", tx_id.str()));

    return res;
}

vector< request_object > bbtone_api::get_active_service_requests_attached_to_offers_of_given_operator_name(string offering_operator_name, uint32_t limit)const
{
    vector< request_object > res;

    const auto & offer_idx = _app->chain_database()->get_index<offer_index>().indices().get<offer_index_tag::by_operator_name>();
    auto offerStartIt = offer_idx.lower_bound(offering_operator_name);
    auto offerEndIt = offer_idx.upper_bound(offering_operator_name);

    const auto & request_idx = _app->chain_database()->get_index<request_index>().indices().get<request_index_tag::by_state_assignee_offer_id>();

    for (auto offerIt = offerStartIt; offerIt != offerEndIt && res.size() < limit; ++offerIt) {
        auto requestStartIt = request_idx.lower_bound(std::make_tuple(request_attached, offerIt->id._id));
        auto requestEndIt = request_idx.upper_bound(std::make_tuple(request_attached, offerIt->id._id));

        for (auto requestIt = requestStartIt; requestIt != requestEndIt && res.size() < limit; ++requestIt) {
            res.push_back(*requestIt);
        }
    }

    return res;
}

vector< request_object > bbtone_api::get_service_requests_by_offer_id(uint64_t assignee_offer_id, uint32_t limit)const
{
    vector< request_object > res;

    const auto & idx = _app->chain_database()->get_index<request_index>().indices().get<request_index_tag::by_assignee_offer_id>();
    auto startIt = idx.lower_bound(assignee_offer_id);
    auto endIt = idx.upper_bound(assignee_offer_id);

    for (auto it = startIt; res.size() < limit && it != endIt; ++it)
        res.push_back(*it);

    return res;
}


map <string, string> bbtone_api::attach_charge_to_service_request(string offering_operator_name, uint64_t target_request_id, asset charge, string charge_data)const
{
    attach_charge_to_service_request_operation op;
    op.operator_name = offering_operator_name;
    op.target_request_id = target_request_id;
    op.charge = charge;
    op.charge_data = charge_data;

    auto tx_id = get_plugin().broadcast_op(op);

    std::map<string, string> res;
    res.insert(pair<string,string>("tx_id", tx_id.str()));
    return res;
}

map <string, string> bbtone_api::attach_refund_to_service_request(string operator_name, uint64_t target_request_id, uint32_t error_code)const
{
    string OPERATOR_ASSIGNEE_ACC = STEEMIT_INIT_MINER_NAME;
    fc::ecc::private_key init_key = STEEMIT_INIT_PRIVATE_KEY;

    attach_refund_to_service_request_operation op;
    op.operator_name = operator_name;
    op.target_request_id = target_request_id;
    op.error_code = error_code;

    auto tx_id = get_plugin().broadcast_op(op);

    std::map<string, string> res;
    res.insert(pair<string,string>("tx_id", tx_id.str()));
    return res;
}

void bbtone_plugin::cancel_expired_offers()
{
    const auto & idx = app().chain_database()->get_index<offer_index>().indices().get<offer_index_tag::by_expiration>();

    auto offerIt = idx.begin();
    while ( offerIt != idx.end() &&
            offerIt->state == offer_active &&
            offerIt->tx_time + fc::seconds(offerIt->offer_ttl) < fc::time_point(fc::time_point(fc::seconds(std::time(nullptr)))))
    {
        offer_cancel_operation op;
        op.operator_name = STEEMIT_INIT_MINER_NAME;
        op.offer_id = offerIt->id._id;
        offerIt++;

        try {
            broadcast_op(op);
        }
        catch (const std::exception & ex) {
            wlog("auto expire offer: ${msg}", ("msg", ex.what()));
        }
    }
}

void bbtone_plugin::mainloop()
{
    while (true) {
        fc::usleep( fc::microseconds( 296645 ) );  // wake up a little over 3Hz

        cancel_expired_offers();
    }
}

void bbtone_plugin::plugin_startup()
{
    fc::async([this]()
    {
        mainloop();
    });
}

flat_map<string,string> bbtone_plugin::tracked_accounts() const
{
    return my->_tracked_accounts;
}

} }

STEEMIT_DEFINE_PLUGIN( bbtone, steemit::bbtone::bbtone_plugin )

