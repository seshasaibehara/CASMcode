#ifndef CASM_RemoveSupercell
#define CASM_RemoveSupercell

#include <string>

#include "casm/database/Remove.hh"
#include "casm/global/definitions.hh"

namespace CASM {

class PrimClex;
class Supercell;
class Log;

namespace Completer {
class RmOption;
}

namespace DB {

template <>
class Remove<Supercell> {
 public:
  Remove(const PrimClex &primclex, std::string report_dir);

  /// \brief Erase all enumerated configs that have no data; if no data, erase
  /// supercell
  void erase(const DB::Selection<Supercell> &selection, bool dry_run);

  /// \brief Erase data and files (permanently), but not configs / supercells
  void erase_data(const DB::Selection<Supercell> &selection, bool dry_run);

  /// \brief Removes supercell, including all configurations, data, and files
  /// (permanently)
  void erase_all(const DB::Selection<Supercell> &selection, bool dry_run);

  static std::string desc();
  static int run(const PrimClex &primclex, const Completer::RmOption &opt);

  const PrimClex &primclex() const;

  std::string report_dir() const;

 private:
  const PrimClex &m_primclex;
  std::string m_report_dir;
};

}  // namespace DB
}  // namespace CASM

#endif
