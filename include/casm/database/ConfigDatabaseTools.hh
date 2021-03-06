#ifndef CASM_ConfigDatabaseTools
#define CASM_ConfigDatabaseTools

#include "casm/database/ConfigDatabase.hh"
#include "casm/database/ScelDatabase.hh"

namespace CASM {

struct ConfigInsertResult;
class PrimClex;

namespace DB {

// Note: Supercell is being transitioned to no longer have a `PrimClex const *`.
// While this is occuring there are overloads of these functions with and
// without a `PrimClex const *`. Currently, the `PrimClex const *` is required
// for Configuration calculated properties, but not for enumeration without
// calculated properties.

// /// Returns the canonical form Configuration in the canonical Supercell
// Configuration in_canonical_supercell(
//   Configuration const &configuration,
//   PrimClex const *primclex,
//   Database<Supercell> &supercell_db);
//
// /// Insert this configuration (in primitive & canonical form) in the database
// ConfigInsertResult make_canonical_and_insert(
//   Configuration const &configuration,
//   PrimClex const *primclex,
//   Database<Supercell> &supercell_db,
//   Database<Configuration> &configuration_db,
//   bool primitive_only);

/// Returns the canonical form Configuration in the canonical Supercell
Configuration in_canonical_supercell(Configuration const &configuration,
                                     Database<Supercell> &supercell_db);

/// Insert this configuration (in primitive & canonical form) in the database
ConfigInsertResult make_canonical_and_insert(
    Configuration const &configuration, Database<Supercell> &supercell_db,
    Database<Configuration> &configuration_db, bool primitive_only);

/// Insert this configuration (in primitive & canonical form) in the database
///
/// - This version checks `is_guaranteed_for_database_insert(enumerator)` and
/// either inserts
///   directly or makes canonical and then inserts
template <typename EnumeratorType>
ConfigInsertResult make_canonical_and_insert(
    EnumeratorType const &enumerator, Configuration const &configuration,
    Database<Supercell> &supercell_db,
    Database<Configuration> &configuration_db, bool primitive_only);
}  // namespace DB
}  // namespace CASM

#endif
