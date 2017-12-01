#ifndef CLEXBASIS_HH
#define CLEXBASIS_HH

#include <string>
#include "casm/basis_set/BasisSet.hh"
#include "casm/CASM_global_enum.hh"


namespace CASM {
  class Site;
  template<typename CoordType>
  class BasicStructure;
  class Structure;
  class UnitCell;


  class PrimNeighborList;

  class BasisBuilder;

  // This is a hack to forward-declare IntegralCluster.  Forward declarations
  // of typedefs should probably get their own *.hh files, without any dependencies
  template<typename CoordType>
  class CoordCluster;
  class UnitCellCoord;
  typedef CoordCluster<UnitCellCoord> IntegralCluster;

  class ClexBasis {
  public:
    typedef std::vector<BasisSet> BSetOrbit;
    typedef std::string DoFKey;
    typedef std::vector<BSetOrbit>::const_iterator BSetOrbitIterator;

    /// \brief Initialize from Structure, in order to get Site DoF and global DoF info
    ClexBasis(Structure const &_prim);

    Structure const &prim() const;

    /// \brief Total number of basis sites in primitive cell
    Index n_sublat() const;

    /// \brief Total number of BasisSet orbits
    Index n_orbits() const;

    /// \brief Total number of basis functions
    Index n_functions() const;

    /// \brief Const access of clust basis of orbit @param orbit_ind and equivalent cluster @param equiv_ind
    BasisSet const &clust_basis(Index orbit_ind,
                                Index equiv_ind) const;

    /// \brief Const access of BSetOrbit of orbit @param orbit_ind
    BSetOrbit const &bset_orbit(Index orbit_ind) const;

    /// \brief Const iterator to first BasisSet orbit
    BSetOrbitIterator begin() const {
      return m_bset_tree.cbegin();
    }

    /// \brief Const iterator to first BasisSet orbit
    BSetOrbitIterator cbegin() const {
      return m_bset_tree.cbegin();
    }

    /// \brief Const past-the-end iterator for BasisSet orbits
    BSetOrbitIterator end() const {
      return m_bset_tree.cbegin();
    }

    /// \brief Const past-the-end iterator for BasisSet orbits
    BSetOrbitIterator cend() const {
      return m_bset_tree.cbegin();
    }

    jsonParser const &bspecs() const {
      return m_bspecs;
    }

    /// \brief Const access to dictionary of all site BasisSets
    std::map<DoFKey, std::vector<BasisSet> > const &site_bases()const {
      return m_site_bases;
    }

    /// \brief Const access to dictionary of all global BasisSets
    std::map<DoFKey, BasisSet> const &global_bases()const {
      return m_global_bases;
    }

    /// \brief generate clust_basis for all equivalent clusters in @param _orbitree
    template<typename OrbitIterType>
    void generate(OrbitIterType _begin,
                  OrbitIterType _end,
                  jsonParser const &_bspecs,
                  Index max_poly_order = -1);

  private:
    template<typename OrbitType>
    BasisSet _construct_prototype_basis(OrbitType const &_orbit,
                                        std::vector<DoFKey> const &local_keys,
                                        std::vector<DoFKey> const &global_keys,
                                        Index max_poly_order) const;

    /// \brief Performs heavy lifting for populating site bases in m_site_bases
    void _populate_site_bases(Structure const &_prim);

    Structure const *m_prim_ptr;

    notstd::cloneable_ptr<BasisBuilder> m_basis_builder;

    /// \brief Collection of all cluster orbits (are we keeping this?)
    //std::vector<OrbitType> m_orbitree

    /// \brief Collection of all cluster BasisSets, one per cluster orbit
    std::vector<BSetOrbit> m_bset_tree;

    /// \brief Dictionary of all site BasisSets, initialized on construction
    std::map<DoFKey, std::vector<BasisSet> > m_site_bases;

    /// \brief Dictionary of all global BasisSets, initialized
    std::map<DoFKey, BasisSet> m_global_bases;

    jsonParser m_bspecs;

  };


  class BasisBuilder {
  public:
    virtual ~BasisBuilder() {}

    virtual void prepare(Structure const &_prim) {

    }

    virtual std::vector<ClexBasis::DoFKey> filter_dof_types(std::vector<ClexBasis::DoFKey> const &_dof_types) {
      return _dof_types;
    }

    virtual void pre_generate() {

    }

    virtual BasisSet build(IntegralCluster const &_prototype,
                           std::vector<BasisSet const *> const &_arg_bases,
                           Index max_poly_order,
                           Index min_poly_order) = 0;

    std::unique_ptr<BasisBuilder> clone()const {
      return std::unique_ptr<BasisBuilder>(_clone());
    }

  private:
    virtual BasisBuilder *_clone()const = 0;

  };

  /// Print cluster with basis_index and nlist_index (from 0 to size()-1), followed by cluster basis functions
  /// Functions are labeled \Phi_{i}, starting from i = @param begin_ind
  /// Returns the number of functions that were printed
  Index print_clust_basis(std::ostream &stream,
                          BasisSet _clust_basis,
                          IntegralCluster const &_prototype,
                          Index func_ind = 0,
                          int space = 18,
                          char delim = '\n');


  template<typename OrbitType>
  void print_proto_clust_funcs(ClexBasis const &_clex_basis,
                               std::ostream &out,
                               BasicStructure<Site> const &_prim,
                               std::vector<OrbitType > const &_tree);


  namespace ClexBasis_impl {
    std::vector<ClexBasis::DoFKey> extract_dof_types(Structure const &_prim);

    BasisSet construct_clust_dof_basis(IntegralCluster const &_clust,
                                       std::vector<BasisSet const *> const &site_dof_sets);

    template<typename UCCIterType, typename IntegralClusterSymCompareType>
    std::map<UnitCellCoord, std::set<UnitCellCoord> > unique_ucc(UCCIterType begin,
                                                                 UCCIterType end,
                                                                 IntegralClusterSymCompareType const &sym_compare);
  }
}
#include "casm/clex/ClexBasis_impl.hh"
#endif