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
#pragma once

#include <steemit/app/plugin.hpp>
#include <steemit/chain/database.hpp>

#include <boost/multi_index/composite_key.hpp>

#include <fc/thread/future.hpp>
#include <fc/api.hpp>

#include <steemit/bbtone/bbtone_objects.hpp>


namespace steemit { namespace bbtone {
using namespace chain;
using app::application;
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

namespace detail
{
    class bbtone_plugin_impl;
}

class bbtone_plugin : public steemit::app::plugin
{
public:
    bbtone_plugin( application* app );
    virtual ~bbtone_plugin();

    std::string plugin_name()const override;
    virtual void plugin_set_program_options(
        boost::program_options::options_description& cli,
        boost::program_options::options_description& cfg) override;
    virtual void plugin_initialize(const boost::program_options::variables_map& options) override;
    virtual void plugin_startup() override;

    flat_map<string,string> tracked_accounts()const; /// map start_range to end_range

    friend class detail::bbtone_plugin_impl;
    std::unique_ptr<detail::bbtone_plugin_impl> my;
};


class bbtone_api : public std::enable_shared_from_this<bbtone_api>
{
public:
    bbtone_api(){};
    bbtone_api(const app::api_context& ctx):_app(&ctx.app){
        ilog( "creating bbtone api" );
    }
    void on_api_startup(){
        wlog( "on bbtone api startup" );
    }

	 // [TODO] rename "operator_name" parameter in each function to exact role, for example "offering_operator" and "requesting_operator",
	 // It will be useful to grep code, looking for all operations of needed role

    map<string, string> create_service_offer(string operator_name, uint64_t offer_local_id,
        string offer_data, uint32_t offer_ttl, asset price)const;
    vector< offer_object > get_service_offers_by_operator_name(string operator_name, uint32_t limit)const;

    std::map<string, string> attach_request_to_service_offer(string offering_operator_name, uint64_t target_offer_id, uint32_t request_ttl,
        asset credits, string user_id, fc::ecc::public_key user_pub_key)const;
    vector< request_object > get_active_service_requests_attached_to_offers_of_given_operator_name(string offering_operator_name, uint32_t limit)const;
    vector< request_object > get_service_requests_by_offer_id(uint64_t assignee_offer_id, uint32_t limit)const;
    vector< request_object > get_service_requests_by_state_and_assignee_offer_id(uint32_t state, uint64_t assignee_offer_id, uint32_t limit)const;
    map <string, string> attach_charge_to_service_request(string operator_name, uint64_t target_request_id, asset charge, string charge_data)const;
	map <string, string> attach_refund_to_service_request(string operator_name, uint64_t target_request_id, uint32_t error_code)const;

private:
    app::application* _app = nullptr;
};

} } //steemit::bbtone

FC_API( steemit::bbtone::bbtone_api,
    (create_service_offer)
    (get_service_offers_by_operator_name)
    (attach_request_to_service_offer)
    (get_active_service_requests_attached_to_offers_of_given_operator_name)
    (get_service_requests_by_offer_id)
    (attach_charge_to_service_request)
	(attach_refund_to_service_request)
);

