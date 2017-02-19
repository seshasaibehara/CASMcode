#ifndef CASM_ConfigDatabase
#define CASM_ConfigDatabase

namespace {

  namespace DB {

    /// Derived ConfigDatabase must implement public methods:
    /// - std::pair<iterator, bool> rename(const name_type& old_name, const name_type& new_name)
    /// - std::pair<iterator, bool> update(const Configuration &config)
    /// - boost::iterator_range<iterator> scel_range(const name_type& scelname) const
    ///
    class ConfigDatabase : public Database<Configuration> {

    public:

      /// Set calc properties
      virtual iterator set_calc_properties(const Configuration &config);

      /// Range of Configuration in a particular supecell
      virtual boost::iterator_range<iterator> scel_range(const name_type &scelname) const;

    };

  }
}

#endif