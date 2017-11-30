## Service Smart Contract states and transitions described

#### 0. offer


Possible previous transaction types: no                              
Who can publish this type of transaction: Only operators (in the first version of the protocol)
Fields: transaction_time, null_sig, operator_id, service_ttl, service_data
Funds transfer: no (miner's fee only)


transaction_time: common field for all transactions, it must be sufficiently fine-grained to keep transactions order in the flow of about 10 ^ 4 transactions per second and be absolute for any geographical location.


null_sig: we specially show this empty field to emphasize that the "offer" is the genesig transaction for several particular SSCs. It initiates multiple possible smart-contracts in future, and does not contain the signature of the previous transaction. This behaviour can be changed in future versions.


operator_id: identity of the operator, who is offering service now. Identity of operator is needed to later create a targeted “request” transaction for it. This field contains at least a public key, which is then used to encrypt customer data transmitted to this targeted operator-assignee. 


service_ttl: offer time-to-live. “offer” with expired TTL is automatically considered to be inactive. This is a convenient mechanism to greatly limit the searches in blockchain and automatically clean the network of non-valid transactions. We plan to make an “offer” transaction to be paid by issuing operator, depending on the “service_ttl”, to prevent flood


service_fee: This is the amount of funds, that operator charge in every “charge” transaction. In future versions we plan to make this value set by consensus voting between operators


service_data: any other information about the service. For example, when operator places the offer, it can place its server addresses, closest to potential client geographically, or use this field for more flexible "per-user" confuguring its services



##### 1. request.


Possible previous transaction types: offer
Who can publish this type of transaction: Only operators (in the first version of the protocol)
Fields: transaction_time, last_transaction_sig, assignee_id, client_id, request_ttl, refund_sum, issuer_id, issuer_fee
Funds transfer: input


transaction_time: time to send a transaction to the network.


last_transaction_sig: signature of the previous transaction. In the case of “request, this is the “offer” transaction with actual TTL. By signing it operator-issuer confirms, that it accept “offer”'s data


assignee_id: identity of operator-assignee, who will provide service to client. Identity can contain some encrypted data for begin safe data transfer in transactions (for example - response on handshake, placed). In the first version of the protocol “request” is set to one specific operator to provide more strict control over transactions execution, but, in future versions we consider variants with auction-like transactions flow


client_id: identity of client in the operator's network. This identity must be sufficient to provide at least the identification of client in operator’s network and provide final, end-point service to client. It can be network address, one-time token or anything else, allowing client to receive the service. Information is encrypted by public key of the operator-issuer, so it’s safe to pass it on-chain


request_ttl:  transaction time-to-live. Like a similar field in “offer” transaction, this one sets the lifetime of “request”. If request_ttl is passed, the only possible next transaction is “refund”. Each time, when new “charge” transaction comes, the whole lifetime of SSC is increased, but still finite and restricted by BT global constants.


refund_sum: an amount of funds, that a transaction originator (the operator or client) counts as the maximum for single SSC. This sum is debited from originator’s account. SSC can repeatedly step into “charge” state, each time reducing refund_sum. In the event of a shortfall behavior is similar to the expiration of "request_ttl" - any participant can send a transaction “refund” and stop SSC


issuer_id: identity of operator, issuing a “request”. In the first version of the protocol it is planned that the SSC are made strictly by operators, not clients, to prevent misuse and uncontrolled circulation of tokens. To do this, in the first version of the protocol, "request" transaction contains issuer identity, allowing also to put some handshake data inside it to provide safe on-chain communication between client, assignee and issuer.


issuer_fee: the commission for issuing “request”. This parameter can be the object of consensus, and, together with other global BT constants, vary according to the participants voting 


In future versions we consider the variant with the "auto-renewal" SSC when a new “request” continues the previous transactions chain. In this case, when this “chain-of-chains” takes palce, issuer and the assignee may implement more efficient calculation circuit - for example, reducing and increasing fee or service_price, depending of overall SSC duration



##### 2. refund


Possible previous transaction types: request, charge
Who can publish this type of transaction: any participant of specific SSC
Fields: transaction_time, last_transaction_sig, refund_cause
Funds transfer: output


Fileds described:


transaction_time: (see above)


last_transaction_sig: (see above)


refund_cause: cause of “refund” call. Field added to differ “good refunds”, when all is good, and “bad” “refunds”, when client didn’t receive a service or was unhappy. Counts of “bad/good” refunds are useful metrics to estimate operator’s service quality and will be used for automatic penalty assignments or even exclusion of node from BT


As said above, transaction "refund" is the only way to transfer funds from the SSC. SSC can always be completed by any of participants sending the transaction “refund”. This transaction automatically changes the balance sheets of the parties involved, distributing funds in accordance with the internal state of SSC. “refund” can be called immediately after the request, if the user or operator changed his mind.


It seems a promising possibility to strictly restrict “refund” calls, to save network from abuse, but we believe it is reasonable not to apply strict restrictions, as refund is the only valid method to complete SSC and return funds. We believe that all protective mechanisms must be applied before the call to refund, to avoid a "stuck" SSC, caused by algorithmic or program error. Reasonable approach is also some kind of delayed execution of refunding, and we're researching this case.



##### 3. charge


Possible previous transaction types: request, charge
Who can publish this type of transaction:only the assignee
Fields: transaction_time, last_transaction_sig, charge_data
Funds transfer: no


transaction_time: time to send a transaction to the network (see above)


last_transaction_sig: previous digital signature transaction (see above)


charge_data  the data necessary for the client to get the service. The data is encrypted by the public key of operator-issuer or client


Requirement of encrypted “charge data” is important: client_id in the transaction “request”  and “charge_data” in “charge” must not disclose information about the client and provided service. This data can be decrypted only by participants for whom they are intended. If necessary, operators, and clients can deanonymize their transactions, but an internal engine by design should protect the participants from the information leaks. The internal format and the size of this field must be on the one hand to be strict and minimalistic to provide small size of blockchain and fast transaction processing, and on the other - must allow to store a sufficient amount of data to support BT evolution, allowing to add a variety of different 	services. Of course, it is possible to reduce the “charge_data” to a small buffer with a short cryptographic token to access arbitrary API of operators, but in this case, the information exchange will have to take place outside of BT, which, as stated above, is undesirable, because we are trying to minimize operator contacts outside of BT, reducing the number of points of failure. 






