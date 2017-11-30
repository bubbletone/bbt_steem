#pragma once
#include <steemit/protocol/base.hpp>
#include <steemit/protocol/block_header.hpp>
#include <steemit/protocol/asset.hpp>

#include <fc/utf8.hpp>
#include <fc/crypto/equihash.hpp>
#include <fc/api.hpp>

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

	
   // NEXT 
	/*
   struct bbtone_offer_create_operation : public base_operation
   {
		// operation struct was spizded from order creation operation
      account_name_type owner;
      uint32_t          offerid = 0; /// an ID assigned by owner, must be unique
		uint32_t				service_id = 0; // ID of service (tariff information, metadata, etc)
		uint32_t				ttl; // TTL of offer in seconds
		price					service_price; 
     
		// asset             amount_to_sell;
      // asset             min_to_receive;
      // bool              fill_or_kill = false;
      // time_point_sec    expiration = time_point_sec::maximum();

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };
	*/

   /**
    *  Cancels an offer and returns the balance to owner.
    */
   // NEXT 
	/*
   struct bbtone_offer_cancel_operation : public base_operation
   {
      account_name_type owner;
      uint32_t          offerid = 0;

      void  validate()const;
      void  get_required_active_authorities( flat_set<account_name_type>& a )const{ a.insert(owner); }
   };
	*/

} } // steemit::protocol


// NEXT 
// FC_REFLECT( steemit::bbtone::bbtone_offer_create_operation, (owner)(offerid)(service_id)(ttl)(service_price) )
// FC_REFLECT( steemit::bbtone::bbtone_offer_cancel_operation, (owner)(offerid) )
