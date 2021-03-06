#ifndef CASM_IntegralClusterSymCompareTraits_impl
#define CASM_IntegralClusterSymCompareTraits_impl

#include "casm/clusterography/IntegralCluster.hh"
#include "casm/clusterography/IntegralClusterSymCompareTraits.hh"
#include "casm/crystallography/SymTools.hh"
#include "casm/crystallography/UnitCellCoord.hh"

namespace CASM {

template <typename SymCompareType>
xtal::UnitCellCoord IntegralClusterSymCompareTraits<SymCompareType>::position(
    IntegralCluster const &clust, SymCompareType const &sym_compare) {
  return clust[0];
}

template <typename SymCompareType>
IntegralCluster IntegralClusterSymCompareTraits<SymCompareType>::copy_apply(
    SymOp const &op, IntegralCluster const &clust,
    SymCompareType const &sym_compare) {
  return CASM::sym::copy_apply(op, clust, sym_compare.prim());
}

template <typename SymCompareType>
ClusterInvariants
IntegralClusterSymCompareTraits<SymCompareType>::make_invariants(
    IntegralCluster const &clust, SymCompareType const &sym_compare) {
  return ClusterInvariants{clust};
}

}  // namespace CASM

#endif
