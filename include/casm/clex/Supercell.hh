#ifndef SUPERCELL_HH
#define SUPERCELL_HH

#include "casm/misc/cloneable_ptr.hh"
#include "casm/crystallography/PrimGrid.hh"
#include "casm/crystallography/Structure.hh"
#include "casm/crystallography/UnitCellCoord.hh"
#include "casm/clex/Configuration.hh"
#include "casm/clex/ConfigDoF.hh"
#include "casm/clex/NeighborList.hh"
#include "casm/database/Named.hh"

namespace CASM {


  template<typename T, typename U> class ConfigIterator;
  class PermuteIterator;
  class PrimClex;
  class Clexulator;

  struct ConfigMapCompare {

    bool operator()(const Configuration *A, const Configuration *B) const {
      return *A < *B;
    }

  };

  namespace DB {
    template<typename T> class DatabaseIterator;
  }

  template<>
  struct QueryTraits<Supercell> {
    static const std::string name;
  };

  /** \defgroup Supercell
   *  \ingroup Clex
   *  \brief Represents a supercell of the primitive parent crystal structure
   *  @{
   */

  /// \brief Represents a supercell of the primitive parent crystal structure
  ///
  class Supercell : public Comparisons<Supercell>, public DB::Named<Supercell> {

  public:

    typedef PermuteIterator permute_const_iterator;

    // **** Constructors ****

    Supercell(const Supercell &RHS);
    Supercell(const PrimClex *_prim, const Lattice &superlattice);
    Supercell(const PrimClex *_prim, const Eigen::Ref<const Eigen::Matrix3i> &superlattice_matrix);


    // **** Coordinates ****

    /// \brief Return the sublattice index for a linear index
    ///
    /// Linear indices are grouped by sublattice, then ordered as determined by
    /// PrimGrid. This function is equivalent to:
    /// \code
    /// linear_index / volume();
    /// \endcode
    Index sublat(Index linear_index) const {
      return linear_index / volume();
    }

    /// \brief Given a Coordinate and tolerance, return linear index into Configuration
    ///
    ///   This may be slow, first converts Coordinate -> UnitCellCoord,
    ///   then gets linear_index from UnitCellCoord
    ///
    /// Implementation:
    /// \code
    /// Coordinate tcoord(coord);
    /// tcoord.within();
    /// return linear_index(UnitCellCoord(prim(), coord, tol));
    /// \endcode
    Index linear_index(const Coordinate &coord, double tol = TOL) const {
      Coordinate tcoord(coord);
      tcoord.within();
      return linear_index(UnitCellCoord(prim(), coord, tol));
    };

    /// \brief Return the linear index corresponding to integral coordinates
    ///
    /// Linear indices are grouped by sublattice, then ordered as determined by
    /// PrimGrid. This function is equivalent to:
    /// \code
    /// bijk[0] * volume() + m_prim_grid.find(bijk.unitcell());
    /// \endcode
    Index linear_index(const UnitCellCoord &bijk) const {
      return bijk[0] * volume() + m_prim_grid.find(bijk.unitcell());
    }

    /// \brief Return the linear index corresponding to integral coordinates
    ///
    /// Equivalent to:
    /// \code
    /// uccoord(linear_index).coordinate()
    /// \endcode
    Coordinate coord(Index linear_index) const {
      return uccoord(linear_index).coordinate();
    }

    /// \brief Return the integral coordinates corresponding to a linear index
    ///
    /// Linear indices are grouped by sublattice, then ordered as determined by
    /// PrimGrid. This function is equivalent to:
    /// \code
    /// UnitCellCoord(prim(), sublat(linear_index), m_prim_grid.unitcell(linear_index % volume()))
    /// \endcode
    UnitCellCoord uccoord(Index linear_index) const {
      return UnitCellCoord(prim(), sublat(linear_index), m_prim_grid.unitcell(linear_index % volume()));
    };


    // returns maximum allowed occupation bitstring -- used for initializing enumeration counters
    std::vector<int> max_allowed_occupation() const;

    Configuration configuration(const BasicStructure<Site> &structure_to_config, double tol = TOL) const;

    /// return Structure corresponding to this supercell
    ///    w/ basis site occupation as per primclex.prim
    Structure superstructure() const;

    /// return Structure corresponding to this supercell
    ///    w/ basis site occupation as per config
    Structure superstructure(const Configuration &config) const;

    // **** Accessors ****

    const PrimClex &primclex() const {
      return *m_primclex;
    }

    /// \brief Get the PrimClex crystallography_tol
    double crystallography_tol() const;

    const PrimGrid &prim_grid() const {
      return m_prim_grid;
    }

    const Structure &prim() const;

    ///Return number of primitive cells that fit inside of *this
    Index volume() const {
      return m_prim_grid.size();
    };

    Index basis_size() const {
      return prim().basis.size();
    }

    Index num_sites() const {
      return volume() * basis_size();
    };

    // the permutation_symrep is the SymGroupRep of prim().factor_group() that describes how
    // operations of m_factor_group permute sites of the Supercell.
    // NOTE: The permutation representation is for (*this).prim().factor_group(), which may contain
    //       more operations than m_factor_group, so the Permutation SymGroupRep may have 'gaps' at the
    //       operations that aren't in m_factor_group. You should access elements of the SymGroupRep using
    //       SymGroupRep::get_representation(m_factor_group[i]) or SymGroupRep::get_permutation(m_factor_group[i]),
    //       so that you don't encounter the gaps (i.e., the representation can be indexed using the
    //       SymOps of m_factor_group
    SymGroupRepID permutation_symrep_ID() const {
      if(m_perm_symrep_ID.empty())
        generate_permutations();
      return m_perm_symrep_ID;
    }

    SymGroupRep const &permutation_symrep() const {
      return prim().factor_group().representation(permutation_symrep_ID());
    }

    const Eigen::Matrix3i &transf_mat() const {
      return m_transf_mat;
    };

    const Lattice &real_super_lattice() const {
      return m_real_super_lattice;
    };

    /// \brief Returns the SuperNeighborList
    const SuperNeighborList &nlist() const;


    // Populates m_factor_group (if necessary) and returns it.
    const SymGroup &factor_group() const;

    // Returns the permutation representation of the i'th element of m_factor_group
    const Permutation &factor_group_permute(Index i) const;

    // Returns the i'th element of m_trans_permute
    // Populates m_trans_permute if needed
    const Permutation &translation_permute(Index i) const;

    // Const access of m_trans_permute
    // Populates m_trans_permute if needed
    const std::vector<Permutation> &translation_permute() const;

    /// \brief Begin iterator over pure translational permutations
    permute_const_iterator translate_begin() const;

    /// \brief End iterator over pure translational permutations
    permute_const_iterator translate_end() const;

    // begin and end iterators for iterating over translation and factor group permutations
    permute_const_iterator permute_begin() const;
    permute_const_iterator permute_end() const;
    permute_const_iterator permute_it(Index fg_index, Index trans_index) const;


    bool is_canonical() const {
      return real_super_lattice().is_canonical();
    }

    SymOp to_canonical() const {
      return real_super_lattice().to_canonical();
    }

    SymOp from_canonical() const {
      return real_super_lattice().from_canonical();
    }

    const Supercell &canonical_form() const;

    bool is_equivalent(const Supercell &B) const;

    bool operator<(const Supercell &B) const;

    /// \brief Insert the canonical form of this into the database
    std::pair<DB::DatabaseIterator<Supercell>, bool> insert() const;

    // **** Other ****

    bool is_supercell_of(const Structure &structure) const;
    bool is_supercell_of(const Structure &structure, Eigen::Matrix3d &multimat) const;
    std::vector<int> vacant()const;

  private:

    friend Comparisons<Supercell>;
    friend Named<Supercell>;

    bool _eq(const Supercell &B) const;

    // **** Generating functions ****

    // Populate m_factor_group -- probably should be private
    void generate_factor_group() const;

    // Populate m_trans_permute -- probably should be private
    void generate_permutations() const;

    std::string generate_name() const;


    // pointer to Primcell containing all the cluster expansion data
    const PrimClex *m_primclex;

    // lattice of supercell in real space
    Lattice m_real_super_lattice;

    // superlattice arithmetic
    PrimGrid m_prim_grid;

    // m_perm_symrep_ID is the ID of the SymGroupRep of prim().factor_group() that describes how
    // operations of m_factor_group permute sites of the Supercell.
    // NOTE: The permutation representation is for (*this).prim().factor_group(), which may contain
    //       more operations than m_factor_group, so the Permutation SymGroupRep may have 'gaps' at the
    //       operations that aren't in m_factor_group. You should access elements of the SymGroupRep using
    //       the the Supercel::factor_group_permute(int) method, so that you don't encounter the gaps
    //       OR, see note for Supercell::permutation_symrep() below.
    mutable SymGroupRepID m_perm_symrep_ID;

    // m_factor_group is factor group of the super cell, found by identifying the subgroup of
    // (*this).prim().factor_group() that leaves the supercell lattice vectors unchanged
    // if (*this).prim() is actually primitive, then m_factor_group.size() <= 48
    // NOTE: This is different from the SymGroup found by doing (*this).superstruc().factor_group()
    //       if Tprim is the translation group formed by the primitive cell lattice vectors, then
    //       m_factor_group is the group formed by the cosets of Tprim in the supercell space group
    //       if Tsuper is the translation group formed by the supercell lattice vectors, then,
    //       m_occupation(init_config.occupation()),
    //       m_displacement(init_config.displacement()),
    //       m_strain(init_config.supercell().strain
    //       (*this).superstruc().factor_group() is the group formed by the cosets of Tsuper in the supercell space group
    mutable SymGroup m_factor_group;

    /// SuperNeighborList, mutable for lazy construction
    mutable notstd::cloneable_ptr<SuperNeighborList> m_nlist;

    /// Store size of PrimNeighborList at time of construction of SuperNeighborList
    /// to enable checking if SuperNeighborList should be re-constructed
    mutable Index m_nlist_size_at_construction;

    /// Store a pointer to the canonical equivalent Supercell
    mutable const Supercell *m_canonical;

    Eigen::Matrix3i m_transf_mat;

  };

  Supercell &apply(const SymOp &op, Supercell &scel);

  Supercell copy_apply(const SymOp &op, const Supercell &scel);

  std::string generate_name(const Eigen::Matrix3i &transf_mat);

  /** @} */
}
#endif
