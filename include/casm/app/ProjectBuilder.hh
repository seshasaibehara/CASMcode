#ifndef CASM_ProjectBuilder
#define CASM_ProjectBuilder

#include <string>
#include <vector>
#include <map>
#include <boost/filesystem/path.hpp>
#include "casm/global/definitions.hh"
#include "casm/crystallography/BasicStructure.hh"
#include "casm/crystallography/Site.hh"

namespace CASM {
  namespace xtal {
    class Site;

    class BasicStructure;
  }


  /// \brief Sets up directories and files for a new CASM project
  ///
  /// \ingroup Project
  ///
  class ProjectBuilder {

  public:

    static bool valid_title(std::string const &_title);

    /// \brief Construct a CASM ProjectBuilder
    ///
    /// \param _prim The primitive structure defining the new CASM project
    /// \param _root The directory where a new CASM project should be created.
    /// \param _title The title of the CASM project. Should be a short title suitable for prepending to files.
    /// \param _property The name of the default cluster expansion property, i.e. "formation_energy"
    ///
    ProjectBuilder(xtal::BasicStructure const &_prim, fs::path _root, std::string _title, std::string _property);

    /// \brief Construct a CASM ProjectBuilder
    ///
    /// \param _root The directory where a new CASM project should be created.
    /// \param _title The title of the CASM project. Should be a short title suitable for prepending to files.
    /// \param _property The name of the default cluster expansion property, i.e. "formation_energy"
    ///
    ProjectBuilder(fs::path _root, std::string _title, std::string _property);


    ProjectBuilder &set_crystallography_tol(double _tol) {
      m_crystallography_tol = _tol;
      return *this;
    }

    ProjectBuilder &set_lin_alg_tol(double _tol) {
      m_lin_alg_tol = _tol;
      return *this;
    }

    /// \brief Builds a new CASM project
    void build() const;


  private:

    // require user initialization:
    xtal::BasicStructure m_prim;
    fs::path m_root;
    std::string m_title;
    std::string m_property;

    // allow default initialization:
    std::map<std::string, std::vector<std::string> > m_properties {
      {"Configuration", {"relaxed_energy"}},
      {"DiffTransConfiguration", {"kra"}}
    };
    double m_crystallography_tol = CASM::TOL;
    double m_lin_alg_tol = 1e-10;

  };

}

#endif
