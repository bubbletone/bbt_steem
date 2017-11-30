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

#include <steemit/bbtone/bbtone_evaluator.hpp>
//#include <steemit/bbtone/bbtone_operations.hpp>
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
#include <steemit/chain/steem_objects.hpp>



//#include <steemit/wallet/wallet.hpp>

/************************************/
/*

#include <steemit/protocol/exceptions.hpp>

#include <steemit/chain/database.hpp>
#include <steemit/chain/database_exceptions.hpp>
#include <steemit/chain/hardfork.hpp>
#include <steemit/chain/steem_objects.hpp>

#include <steemit/chain/util/reward.hpp>

#include <steemit/witness/witness_objects.hpp>

#include <fc/crypto/digest.hpp>


#include <cmath>
#include <iostream>
#include <stdexcept>

sing fc::string;


*************************/


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

      bbtone_plugin&                                                             _self;
      // NEXT 
		// std::shared_ptr< generic_custom_operation_interpreter< steemit::bbtone::bbtone_offer_create_operation > >   _custom_operation_interpreter;
      flat_map<string,string>                                                             _tracked_accounts;
};

bbtone_plugin_impl::bbtone_plugin_impl( bbtone_plugin& _plugin )
   : _self( _plugin )
{
   // NEXT 
   // _custom_operation_interpreter = std::make_shared< generic_custom_operation_interpreter< steemit::bbtone::bbtone_offer_create_operation > >( database() );

   // NEXT 
   // _custom_operation_interpreter->register_evaluator< bbtone_offer_create_evaluator >( &_self );

   // NEXT 
   // database().set_custom_operation_interpreter( _self.plugin_name(), _custom_operation_interpreter );
   return;
}

bbtone_plugin_impl::~bbtone_plugin_impl()
{
   return;
}

} // end namespace detail


// NEXT
/* 
void bbtone_offer_create_evaluator::do_apply( const bbtone_offer_create_operation& pm )
{
   //database& d = db();
	FC_ASSERT( 1 == 1);
   
	const flat_map<string, string>& tracked_accounts = _plugin->my->_tracked_accounts;

   auto to_itr   = tracked_accounts.lower_bound(pm.to);
   auto from_itr = tracked_accounts.lower_bound(pm.from);

   FC_ASSERT( pm.from != pm.to );
   FC_ASSERT( pm.from_memo_key != pm.to_memo_key );
   FC_ASSERT( pm.sent_time != 0 );
   FC_ASSERT( pm.encrypted_message.size() >= 32 );

   if( !tracked_accounts.size() ||
       (to_itr != tracked_accounts.end() && pm.to >= to_itr->first && pm.to <= to_itr->second) ||
       (from_itr != tracked_accounts.end() && pm.from >= from_itr->first && pm.from <= from_itr->second) )
   {
      d.create<message_object>( [&]( message_object& pmo )
      {
         pmo.from               = pm.from;
         pmo.to                 = pm.to;
         pmo.from_memo_key      = pm.from_memo_key;
         pmo.to_memo_key        = pm.to_memo_key;
         pmo.checksum           = pm.checksum;
         pmo.sent_time          = pm.sent_time;
         pmo.receive_time       = d.head_block_time();
         pmo.encrypted_message.resize( pm.encrypted_message.size() );
         std::copy( pm.encrypted_message.begin(), pm.encrypted_message.end(), pmo.encrypted_message.begin() );
      } );
   }

}
*/

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
   add_plugin_index< message_index >(db);

   app().register_api_factory<bbtone_api>("bbtone_api");

   typedef pair<string,string> pairstring;
   LOAD_VALUE_SET(options, "pm-accounts", my->_tracked_accounts, pairstring);
}

vector< message_api_obj > bbtone_api::get_inbox( string to, time_point newest, uint16_t limit )const {
   FC_ASSERT( limit <= 100 );
   vector< message_api_obj > result;
   const auto& idx = _app->chain_database()->get_index< message_index >().indices().get< by_to_date >();
   auto itr = idx.lower_bound( std::make_tuple( to, newest ) );
   while( itr != idx.end() && limit && itr->to == to ) {
      result.push_back(*itr);
      ++itr;
      --limit;
   }

   return result;
}

vector< message_api_obj > bbtone_api::get_outbox( string from, time_point newest, uint16_t limit )const {
   FC_ASSERT( limit <= 100 );
   vector< message_api_obj > result;
   const auto& idx = _app->chain_database()->get_index< message_index >().indices().get< by_from_date >();

   auto itr = idx.lower_bound( std::make_tuple( from, newest ) );
   while( itr != idx.end() && limit && itr->from == from ) {
      result.push_back(*itr);
      ++itr;
      --limit;
   }
   return result;
}

std::map<string, string> bbtone_api::broadcast_service_offer(string operator_id)const {
	
	string OPERATOR_ASSIGNEE_ACC = STEEMIT_INIT_MINER_NAME;
	fc::ecc::private_key init_key = STEEMIT_INIT_PRIVATE_KEY;

	transfer_operation op;
	op.from = STEEMIT_INIT_MINER_NAME;
   op.to = "operator2";
	op.amount =  asset(1, STEEM_SYMBOL);

   signed_transaction tx;
   tx.set_expiration( _app->chain_database()->head_block_time() + STEEMIT_MAX_TIME_UNTIL_EXPIRATION );
   tx.operations.push_back( op );
	tx.sign( init_key, _app->chain_database()->get_chain_id() );
	_app->chain_database()->push_transaction(tx);

	std::map<string, string> res;
	res.insert(pair< string, string >("tx_id", "7233"));
	res.insert(pair< string, string >("MOCK_tx_sig", "efefefefefefefefefefefef"));
	res.insert(pair< string, string >("operator_id", operator_id));
	res.insert(pair< string, string >("ok", "1"));
   return res ;
}

vector< std::map<string, string> > bbtone_api::get_service_offers_of_given_operator_id(string offering_operator_id)const {
	vector< std::map<string, string> > res;
	res = {
		{{"tx_id","444"}, {"operator_id", offering_operator_id}, {"expires", "1609459200"}, {"price", "10.65"}, {"MOCK_tx_sig", "bababababababababababab"}},
		{{"tx_id","445"}, {"operator_id", offering_operator_id}, {"expires", "1609459200"}, {"price", "12.35"}, {"MOCK_tx_sig", "dededededededededededed"}},
		{{"tx_id","555"}, {"operator_id", offering_operator_id}, {"expires", "1609459200"}, {"price", "22.00"}, {"MOCK_tx_sig", "afafafafafafafafafafafa"}}
	};
   return res;
}

std::map<string, string> bbtone_api::attach_service_request_to_service_offer(string offer_tx_id)const {
	std::map<string, string> res;
	res.insert(pair<string,string>("ok", "1"));
	res.insert(pair<string,string>("tx_id", "3333"));
	res.insert(pair<string,string>("MOCK_tx_sig", "fefefefefefefefefefefefefef"));
   return res;
}

vector< std::map<string, string> > bbtone_api::get_active_service_requests_of_given_operator_id(string requesting_operator_id)const {
 	vector< std::map<string, string> > res;
	res = {
		{{"tx_id","9444"}, {"expires", "14576849332"}, {"reserve", "60.65"}, {"MOCK_tx_sig", "cececececececececececece"}},
		{{"tx_id","9945"}, {"expires", "14576849333"}, {"reserve", "62.35"}, {"MOCK_tx_sig", "dfdfdfdfdfdfdfdfdfdfdfdf"}},
		{{"tx_id","9555"}, {"expires", "14573349333"}, {"reserve", "62.00"}, {"MOCK_tx_sig", "dadadadadadadadadadadada"}}
	};
   return res;
}

std::map<string, string> bbtone_api::attach_charge_to_service_request(string service_tx_id)const {
	std::map<string, string> res;
	res.insert(pair<string,string>("ok", "1"));
	res.insert(pair<string,string>("tx_id", "5555"));
	res.insert(pair<string,string>("MOCK_tx_sig", "444123123"));
   return res;
}

std::map<string, string> bbtone_api::refund_and_close_request(string service_tx_id)const {
	std::map<string, string> res;
	res.insert(pair<string,string>("ok", "1"));
	res.insert(pair<string,string>("tx_id", "2222222"));
	res.insert(pair<string,string>("pay_to_assignee", "10.65"));
	res.insert(pair<string,string>("pay_back", "50.00"));
	res.insert(pair<string,string>("MOCK_tx_sig", "cacacacacacacacacacacacaca"));
   return res;
}


void bbtone_plugin::plugin_startup()
{
}

flat_map<string,string> bbtone_plugin::tracked_accounts() const
{
   return my->_tracked_accounts;
}

} }

STEEMIT_DEFINE_PLUGIN( bbtone, steemit::bbtone::bbtone_plugin )

// NEXT 
// DEFINE_OPERATION_TYPE( steemit::bbtone::bbtone_offer_create_operation )
