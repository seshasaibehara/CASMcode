#include "casm/app/ProjectSettings.hh"
#include "casm/clex/PrimClex.hh"
#include "casm/database/DatabaseHandler_impl.hh"
#include "casm/database/json/jsonDatabase.hh"

namespace CASM {

namespace DB {

/// Constructor
///
/// - Uses PrimClex for constructing Supercell, etc.
/// - Uses PrimClex.settings().db_type() to determine default database type
DatabaseHandler::DatabaseHandler(const PrimClex &_primclex)
    : m_primclex(&_primclex),
      m_default_db_name(m_primclex->settings().default_database_name()) {
  jsonDB::insert(*this);
}

DatabaseHandler::~DatabaseHandler() { close(); }

const PrimClex &DatabaseHandler::primclex() const { return *m_primclex; }

/// Close all databases
void DatabaseHandler::close() {
  for (auto &db : m_db) {
    db.second->close();
  }
}

}  // namespace DB

}  // namespace CASM
