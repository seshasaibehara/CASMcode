#include "casm/app/HamiltonianModules.hh"
#include "casm/app/HamiltonianModules_impl.hh"
#include "casm/misc/ParsingDictionary.hh"
#include "casm/basis_set/DoFTraits.hh"
#include "casm/basis_set/DoF.hh"
#include "casm/crystallography/SpeciesAttribute.hh"

namespace CASM {

  HamiltonianModules::HamiltonianModules(ProjectSettings const *_set) :
    m_dof_dict(make_parsing_dictionary<DoFDictionary::value_type>().clone()),
    m_species_attribute_dict(make_parsing_dictionary<SpeciesAttributeDictionary::value_type>().clone()) {
    //std::cout << "Inside HamiltonianModules Constructor\n";
    // add DoF plugins
    if(_set) {
      load_dof_plugins(
        *_set,
        std::inserter(*m_dof_dict, m_dof_dict->end()),
        std::inserter(m_dof_lib, m_dof_lib.end()));

      // add attribute plugins
      load_species_attribute_plugins(
        *_set,
        std::inserter(*m_species_attribute_dict, m_species_attribute_dict->end()),
        std::inserter(m_species_attribute_lib, m_species_attribute_lib.end()));
    }


  }

  HamiltonianModules::~HamiltonianModules() {
    // order of deletion matters
    m_dof_dict->clear();
    m_dof_lib.clear();
    // order of deletion matters
    m_species_attribute_dict->clear();
    m_species_attribute_lib.clear();
  }

  //std::unique_ptr<HamiltonianModules > HamiltonianModules::clone() const {
  //return std::unique_ptr<HamiltonianModules >(new HamiltonianModules(*this));
  //}

  HamiltonianModules::DoFDictionary &HamiltonianModules::dof_dict() {
    return *m_dof_dict;
  }

  HamiltonianModules::DoFDictionary const &HamiltonianModules::dof_dict()const {
    return *m_dof_dict;
  }

  HamiltonianModules::SpeciesAttributeDictionary &HamiltonianModules::species_attribute_dict() {
    return *m_species_attribute_dict;
  }

  HamiltonianModules::SpeciesAttributeDictionary const &HamiltonianModules::species_attribute_dict()const {
    return *m_species_attribute_dict;
  }


}
