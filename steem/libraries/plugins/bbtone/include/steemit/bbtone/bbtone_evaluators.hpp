#pragma once

#include <steemit/chain/evaluator.hpp>

#include <steemit/bbtone/bbtone_operations.hpp>
#include <steemit/bbtone/bbtone_plugin.hpp>

namespace steemit { namespace bbtone {

DEFINE_PLUGIN_EVALUATOR( bbtone_plugin, steemit::bbtone::bbtone_plugin_operation, bbtone )

} }
