#ifndef CASM_jsonIO_container
#define CASM_jsonIO_container

#include "casm/casm_io/jsonParser.hh"
#include "casm/CASM_global_definitions.hh"

//#include "casm/container/Array.hh"
//#include "casm/container/LinearAlgebra.hh"

namespace CASM {
/*  
  // --- Array<T> ------------------------------
  
  /// Converts to a JSON array
  template<typename T>
  jsonParser &to_json(const Array<T> &value, jsonParser &json) {
    return json.put_array(value.begin(), value.end());
  }

  /// This requires that 'T::T()' exists, if not, you must do this by hand
  template<typename T>
  void from_json(Array<T> &value, const jsonParser &json) {
    value.resize(json.size());
    for(int i = 0; i < json.size(); i++)
      from_json(value[i], json[i]);
  }
  
  // --- Matrix & Vector -----------------------
  
  /// Converts to a JSON array of arrays
  template <typename T, Index N>
  jsonParser &to_json(const Matrix<T, N> &value, jsonParser &json) {
    json.put_array();
    for(Index i = 0; i < value.num_rows(); i++) {
      jsonParser json_row;
      json_row.put_array();
      for(Index j = 0; j < value.num_cols(); j++) {
        json_row.push_back(value(i, j));
      }
      json.push_back(json_row);
    }
    return json;
  }

  template <typename T, Index N>
  void from_json(Matrix<T, N> &value, const jsonParser &json) {
    for(Index i = 0; i < value.num_rows(); i++) {
      for(Index j = 0; j < value.num_cols(); j++) {
        from_json(value.at(i, j), json[i][j]);
      }
    }
  }
  
  /// Converts to a JSON array
  template <typename T, Index N>
  jsonParser &to_json(const Vector<T, N> &value, jsonParser &json) {
    json.put_array();
    for(Index i = 0; i < value.size(); i++)
      json.push_back(value[i]);
    return json;
  }

  template <typename T, Index N>
  void from_json(Vector<T, N> &value, const jsonParser &json) {
    for(Index i = 0; i < value.size(); i++)
      from_json(value.at(i), json[i]);
  }
  
  
  // --- Permutation ---------------------------
  
  class Permutation;
  
  jsonParser &to_json(const Permutation &perm, jsonParser &json);
  
  template<> 
  Permutation from_json(const jsonParser &json);

  void from_json(Permutation &perm, const jsonParser &json);
*/

  
  // --- std::map<std::string, T> --------------
  
  /// Converts to a JSON object
  template<typename T>
  jsonParser& to_json(const std::map<std::string, T> &map, jsonParser& json) {
    return json.put_obj(map.begin(), map.end()); 
  }
  
  /// Read map from JSON
  ///
  /// Clears any previous contents
  template<typename T>
  void from_json(std::map<std::string, T> &map, const jsonParser& json) {
    map.clear();
    for(auto it=json.begin(); it!=json.end(); ++it) {
      from_json(map[it.name()], *it);
    }
  }
  
  // --- std::vector<T> --------------
  
  /// Converts to a JSON array
  template<typename T>
  jsonParser& to_json(const std::vector<T> &vec, jsonParser& json) {
    return json.put_array(vec.begin(), vec.end()); 
  }
  
  /// Read map from JSON
  ///
  /// Clears any previous contents
  template<typename T>
  void from_json(std::vector<T> &vec, const jsonParser& json) {
    vec.resize(json.size());
    int i=0;
    for(auto it=json.begin(); it!=json.end(); ++it, ++i) {
      from_json(vec[i], *it);
    }
  }
//}
//
//namespace Eigen {
  
  /// \brief Write Eigen Matrix/Vector to JSON
  template <typename Derived>
  CASM::jsonParser &to_json(const Eigen::MatrixBase<Derived> &value, CASM::jsonParser &json) {
    json.put_array();
    for(int i = 0; i < value.rows(); i++) {
      CASM::jsonParser json_row;
      json_row.put_array();
      for(int j = 0; j < value.cols(); j++) {
        json_row.push_back(value(i, j));
      }
      json.push_back(json_row);
    }
    return json;
  }
  
  /// \brief Read Eigen Matrix/Vector from JSON
  template <typename Derived>
  void from_json(Eigen::MatrixBase<Derived>  &value, const CASM::jsonParser &json) {
    value.derived().resize(json.size(), json[0].size());
    for(int i = 0; i < value.rows(); i++) {
      for(int j = 0; j < value.cols(); j++) {
        from_json(value(i, j), json[i][j]);
      }
    }
  }
  
}

#endif