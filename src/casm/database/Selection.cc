#include "casm/app/DirectoryStructure.hh"
#include "casm/casm_io/Log.hh"
#include "casm/casm_io/SafeOfstream.hh"
#include "casm/casm_io/container/stream_io.hh"
#include "casm/casm_io/dataformatter/DataFormatter_impl.hh"
#include "casm/clex/PrimClex_impl.hh"
#include "casm/database/DatabaseTypes_impl.hh"
#include "casm/database/Selected_impl.hh"
#include "casm/database/Selection_impl.hh"
#include "casm/global/errors.hh"

namespace CASM {

const std::string traits<DB::SELECTION_TYPE>::name = "selection";

const std::multimap<DB::SELECTION_TYPE, std::vector<std::string> >
    traits<DB::SELECTION_TYPE>::strval = {
        {DB::SELECTION_TYPE::MASTER, {"MASTER", "Master", "master"}},
        {DB::SELECTION_TYPE::ALL, {"ALL", "All", "all"}},
        {DB::SELECTION_TYPE::NONE, {"NONE", "None", "none"}},
        {DB::SELECTION_TYPE::EMPTY, {"EMPTY", "Empty", "empty"}},
        {DB::SELECTION_TYPE::CALCULATED,
         {"CALCULATED", "Calculated", "calculated"}}};

namespace Monte {
ENUM_IO_DEF(CASM::DB::SELECTION_TYPE)
ENUM_JSON_IO_DEF(CASM::DB::SELECTION_TYPE)
}  // namespace Monte
}  // namespace CASM

// explicit template instantiations
#define INST_Selection(r, data, type)                                     \
  template class SelectionIterator<type, Selection<type>::base_iterator>; \
  template class SelectionIterator<type,                                  \
                                   Selection<type>::base_const_iterator>; \
  template class Selection<type>;

namespace CASM {
namespace DB {

namespace {

// Initialization for "CALCULATED" depends on type:

/// Use SFINAE to implement if ObjType is a calculable config type:
///
/// - Set selected to value of 'is_calculated(const ObjType&)'
template <typename ObjType, typename IfConfigType<ObjType>::type * = nullptr>
void init_calculated(typename Selection<ObjType>::map_type &m_data,
                     Database<ObjType> &db) {
  for (const auto &obj : db) {
    m_data.insert(std::make_pair(obj.name(), is_calculated(obj)));
  }
}

/// Use SFINAE to implement if ObjType is not a calculable config type
///
/// - throw runtime_error
template <typename ObjType, typename IfNotConfigType<ObjType>::type * = nullptr>
void init_calculated(typename Selection<ObjType>::map_type &m_data,
                     Database<ObjType> &db) {
  std::stringstream msg;
  msg << "Selection 'CALCULATED' is not allowed for type: "
      << traits<ObjType>::short_name;
  throw std::runtime_error(msg.str());
}
}  // namespace

// --- template<typename ObjType, typename BaseIterator> class SelectionIterator
// ---

/// \brief Name of object the iterator points at
template <typename ObjType, typename BaseIterator>
std::string SelectionIterator<ObjType, BaseIterator>::name() const {
  return m_it->first;
}

/// \brief Reference to value 'is_selected'
template <typename ObjType, typename BaseIterator>
typename SelectionIterator<ObjType, BaseIterator>::bool_type &
SelectionIterator<ObjType, BaseIterator>::is_selected() {
  return m_it->second;
}

/// \brief Reference to value 'is_selected'
template <typename ObjType, typename BaseIterator>
bool SelectionIterator<ObjType, BaseIterator>::is_selected() const {
  return m_it->second;
}

/// Construct iterator
template <typename ObjType, typename BaseIterator>
SelectionIterator<ObjType, BaseIterator>::SelectionIterator(
    const Selection<ObjType> &_list, BaseIterator _it, bool _selected_only)
    : m_list(&_list), m_it(_it), m_selected_only(_selected_only) {
  if (m_selected_only && m_it != m_list->data().end() &&
      m_it->second == false) {
    increment();
  }
}

/// boost::iterator_facade implementation
template <typename ObjType, typename BaseIterator>
void SelectionIterator<ObjType, BaseIterator>::increment() {
  ++m_it;
  while (m_selected_only && m_it != m_list->data().end() &&
         m_it->second == false) {
    ++m_it;
  }
}

/// boost::iterator_facade implementation
template <typename ObjType, typename BaseIterator>
void SelectionIterator<ObjType, BaseIterator>::decrement() {
  --m_it;
  while (m_selected_only && m_it != m_list->data().begin() &&
         m_it->second == false) {
    --m_it;
  }
}

/// boost::iterator_facade implementation
template <typename ObjType, typename BaseIterator>
const ObjType &SelectionIterator<ObjType, BaseIterator>::dereference() const {
  return *(m_list->db().find(m_it->first));
}

/// boost::iterator_facade implementation
template <typename ObjType, typename BaseIterator>
bool SelectionIterator<ObjType, BaseIterator>::equal(
    const SelectionIterator &B) const {
  return m_it == B.m_it;
}

// --- template<typename ObjType> class Selection ---

/// \brief Default construct into invalid state
template <typename ObjType>
Selection<ObjType>::Selection() : m_db(nullptr), m_primclex(nullptr){};

/// \brief Use default ObjType database
template <typename ObjType>
Selection<ObjType>::Selection(const PrimClex &_primclex,
                              fs::path selection_path)
    : Selection(_primclex.db<ObjType>(), selection_path) {}

template <typename ObjType>
Selection<ObjType>::Selection(Database<ObjType> &_db, fs::path selection_path)
    : m_db(&_db),
      m_primclex(&m_db->primclex()),
      m_name(selection_path.string()) {
  auto _match = matches<DB::SELECTION_TYPE>(selection_path.string());

  if (_match.size() == 1 || selection_path.empty()) {
    DB::SELECTION_TYPE sel =
        selection_path.empty() ? DB::SELECTION_TYPE::MASTER : *_match.begin();

    if (sel == DB::SELECTION_TYPE::MASTER) {
      fs::path master_selection_path;
      if (primclex().has_dir()) {
        master_selection_path =
            primclex().dir().template master_selection<ObjType>();
      }
      if (!master_selection_path.empty() && fs::exists(master_selection_path)) {
        fs::ifstream select_file(master_selection_path);
        read(select_file);
        select_file.close();
      } else {
        for (const auto &obj : db()) {
          m_data.insert(std::make_pair(obj.name(), false));
        }
      }
    } else if (sel == DB::SELECTION_TYPE::NONE) {
      for (const auto &obj : db()) {
        m_data.insert(std::make_pair(obj.name(), false));
      }
    } else if (sel == DB::SELECTION_TYPE::EMPTY) {
    } else if (sel == DB::SELECTION_TYPE::ALL) {
      for (const auto &obj : db()) {
        m_data.insert(std::make_pair(obj.name(), true));
      }
    } else if (sel == DB::SELECTION_TYPE::CALCULATED) {
      init_calculated(m_data, db());
    }
  } else {
    if (!fs::exists(selection_path)) {
      std::stringstream ss;
      ss << "ERROR in parsing configuration selection name. \n"
         << "  " << singleline_help<DB::SELECTION_TYPE>() << "\n"
         << "  Received: '" << selection_path << "'\n"
         << "  No file named '" << selection_path << "'.";
      throw std::runtime_error(ss.str());
    }
    m_name = fs::absolute(selection_path).string();
    if (selection_path.extension() == ".json" ||
        selection_path.extension() == ".JSON") {
      from_json(jsonParser(selection_path));
    } else {
      fs::ifstream select_file(selection_path);
      read(select_file);
      select_file.close();
    }
  }
}

template <typename ObjType>
const PrimClex &Selection<ObjType>::primclex() const {
  return *m_primclex;
}

template <typename ObjType>
Database<ObjType> &Selection<ObjType>::db() const {
  if (!m_db) {
    throw std::runtime_error(
        "Error in Selection<ObjType>::db(): Database pointer invalid");
  }
  return *m_db;
}

template <typename ObjType>
boost::iterator_range<typename Selection<ObjType>::iterator>
Selection<ObjType>::all() {
  return boost::make_iterator_range(iterator(*this, m_data.begin(), false),
                                    iterator(*this, m_data.end(), false));
}

template <typename ObjType>
boost::iterator_range<typename Selection<ObjType>::const_iterator>
Selection<ObjType>::all() const {
  return boost::make_iterator_range(
      const_iterator(*this, m_data.begin(), false),
      const_iterator(*this, m_data.end(), false));
}

template <typename ObjType>
boost::iterator_range<typename Selection<ObjType>::iterator>
Selection<ObjType>::selected() {
  return boost::make_iterator_range(iterator(*this, m_data.begin(), true),
                                    iterator(*this, m_data.end(), true));
}

template <typename ObjType>
boost::iterator_range<typename Selection<ObjType>::const_iterator>
Selection<ObjType>::selected() const {
  return boost::make_iterator_range(const_iterator(*this, m_data.begin(), true),
                                    const_iterator(*this, m_data.end(), true));
}

template <typename ObjType>
typename Selection<ObjType>::map_type &Selection<ObjType>::data() {
  return m_data;
}

template <typename ObjType>
const typename Selection<ObjType>::map_type &Selection<ObjType>::data() const {
  return m_data;
}

template <typename ObjType>
Index Selection<ObjType>::size() const {
  return m_data.size();
}

template <typename ObjType>
const std::vector<std::string> &Selection<ObjType>::col_headers() const {
  return m_col_headers;
}

template <typename ObjType>
const std::string &Selection<ObjType>::name() const {
  return m_name;
}

template <typename ObjType>
Index Selection<ObjType>::selected_size() const {
  return boost::distance(selected());
}

/// \brief True if obj is in Selection and is selected; false otherwise
template <typename ObjType>
bool Selection<ObjType>::is_selected(const std::string &name_or_alias) const {
  auto it = m_data.find(db().name(name_or_alias));
  if (it == m_data.end()) {
    return false;
  }
  return it->second;
}

/// \brief Set selected objects to value of criteria
template <typename ObjType>
void Selection<ObjType>::set(const DataFormatterDictionary<ObjType> &dict,
                             const std::string &criteria) {
  try {
    if (criteria.size()) {
      DataFormatter<ObjType> tformat(dict.parse(criteria));
      auto it = all().begin();
      auto end = all().end();
      for (; it != end; ++it) {
        ValueDataStream<bool> select_stream;
        select_stream << tformat(*it);
        if (select_stream.fail()) {
          err_log() << "Warning: Unable to apply criteria \"" << criteria
                    << "\" to " << traits<ObjType>::name << " " << it.name()
                    << "\n";
          continue;
        }
        it.is_selected() = select_stream.value();
      }
    }
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string("Failure to select using criteria \"") + criteria +
        "\" for " + traits<ObjType>::name +
        "\n"
        "    Reason:  " +
        e.what());
  }
}

/// \brief Set selected objects to value, if criteria true
template <typename ObjType>
void Selection<ObjType>::set(const DataFormatterDictionary<ObjType> &dict,
                             const std::string &criteria, bool value) {
  try {
    if (criteria.size()) {
      DataFormatter<ObjType> tformat(dict.parse(criteria));
      auto it = all().begin();
      auto end = all().end();
      for (; it != end; ++it) {
        if (it.is_selected() == value) {
          continue;
        }
        ValueDataStream<bool> select_stream;
        if (select_stream.fail()) {
          err_log() << "Warning: Unable to apply criteria \"" << criteria
                    << "\" to " << traits<ObjType>::name << " " << it.name()
                    << "\n";
          continue;
        }

        select_stream << tformat(*it);
        if (select_stream.value()) {
          it.is_selected() = value;
        }
      }
    } else {
      auto it = all().begin();
      auto end = all().end();
      for (; it != end; ++it) {
        it.is_selected() = value;
      }
    }
  } catch (std::exception &e) {
    throw std::runtime_error(
        std::string("Failure to select using criteria \"") + criteria +
        "\" for " + traits<ObjType>::name +
        "\n"
        "    Reason:  " +
        e.what());
  }
}

/// \brief Read csv selection from stream
template <typename ObjType>
void Selection<ObjType>::read(std::istream &_input) {
  std::string tname_or_alias;
  bool tselect;
  _input >> std::ws;
  if (_input.peek() == '#') {
    _input.get();
    // discard first two columns (name_or_alias, selected)
    _input >> tname_or_alias;
    _input >> tname_or_alias;
    std::getline(_input, tname_or_alias, '\n');
    m_col_headers.clear();
    boost::split(m_col_headers, tname_or_alias, boost::is_any_of(" \t"),
                 boost::token_compress_on);
    //_input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  while (_input >> tname_or_alias >> tselect) {
    // skip unknown quietly (not sure what is best)
    // this typically arises in cases an object was deleted
    if (db().find(db().name(tname_or_alias)) == db().end()) {
      continue;
    }
    m_data[db().name(tname_or_alias)] = tselect;
    _input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

/// \brief Read selection from JSON
template <typename ObjType>
const jsonParser &Selection<ObjType>::from_json(const jsonParser &_json) {
  if (!_json.is_array()) {
    std::cerr << "CRITICAL ERROR: Unable to initialize a Selection from passed "
                 "JSON input."
              << std::endl
              << "                JSON input must be an array of records."
              << std::endl
              << "                Exiting..." << std::endl;
    exit(1);
  }

  std::string tname;
  bool tselected;
  bool contains_name;

  std::set<std::string> prop_set;
  m_data.clear();

  for (Index i = 0; i < _json.size(); i++) {
    auto it(_json[i].cbegin()), it_end(_json[i].cend());

    contains_name = false;
    tselected = true;  // default to selected

    for (; it != it_end; ++it) {
      // for compatibility, include "configname" also
      if (it.name() == "name" || it.name() == "alias" ||
          it.name() == "alias_or_name" || it.name() == "configname") {
        tname = db().name(it->get<std::string>());
        contains_name = true;
      } else if (it.name() == "selected") {
        tselected = it->get<bool>();
      } else {
        prop_set.insert(it.name());
      }
    }

    if (!contains_name) {
      throw std::runtime_error(
          std::string("CRITICAL ERROR: Field 'name' is missing from ") +
          std::to_string(i) +
          " of json Array passed to Selection::from_json()." +
          " This field is required.");
    }

    m_data[tname] = tselected;
  }

  m_col_headers = std::vector<std::string>(prop_set.begin(), prop_set.end());

  return _json;
}

/// \brief Write selection to JSON
template <typename ObjType>
jsonParser &Selection<ObjType>::to_json(
    const DataFormatterDictionary<ObjType> &_dict, jsonParser &_json,
    bool only_selected) const {
  _json.put_array();

  DataFormatter<ObjType> tformat(
      alias_or_name<ObjType>(),
      datum_formatter_alias("selected", Selected<ObjType>(*this)));

  tformat.append(_dict.parse(m_col_headers));

  if (only_selected) {
    _json = tformat(selected().begin(), selected().end());
  } else {
    _json = tformat(all().begin(), all().end());
  }

  return _json;
}

/// \brief Print csv selection to stream
template <typename ObjType>
void Selection<ObjType>::print(const DataFormatterDictionary<ObjType> &_dict,
                               std::ostream &_out, bool only_selected) const {
  DataFormatter<ObjType> tformat(
      alias_or_name<ObjType>(),
      datum_formatter_alias("selected", Selected<ObjType>(*this)));

  tformat.append(_dict.parse(m_col_headers));

  if (only_selected) {
    _out << tformat(selected().begin(), selected().end());
  } else {
    _out << tformat(all().begin(), all().end());
  }
}

/// \brief Write selection to file -- will overwrite
template <typename ObjType>
void Selection<ObjType>::write(const DataFormatterDictionary<ObjType> &dict,
                               const fs::path &_out_path, bool write_json,
                               bool only_selected) const {
  fs::path out_path(_out_path);
  auto _matches = matches<DB::SELECTION_TYPE>(out_path.string());
  if (_matches.size() == 1 && *_matches.begin() == DB::SELECTION_TYPE::MASTER) {
    out_path = primclex().dir().template master_selection<ObjType>();
  }

  if (write_json || out_path.extension() == ".json" ||
      out_path.extension() == ".JSON") {
    jsonParser json;
    this->to_json(dict, json, only_selected);
    SafeOfstream sout;
    sout.open(out_path);
    json.print(sout.ofstream());
    sout.close();
  } else {
    SafeOfstream sout;
    sout.open(out_path);
    this->print(dict, sout.ofstream(), only_selected);
    sout.close();
  }
}

BOOST_PP_SEQ_FOR_EACH(INST_Selection, _, CASM_DB_TYPES)
}  // namespace DB
}  // namespace CASM
