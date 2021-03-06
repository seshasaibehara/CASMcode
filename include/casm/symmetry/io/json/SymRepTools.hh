#ifndef CASM_symmetry_io_json_SymRepTools
#define CASM_symmetry_io_json_SymRepTools

#include "casm/global/definitions.hh"

namespace CASM {

namespace SymRepTools {
struct IrrepInfo;
class SubWedge;
}  // namespace SymRepTools

struct VectorSpaceSymReport;
class jsonParser;

jsonParser &to_json(SymRepTools::IrrepInfo const &irrep, jsonParser &json);

jsonParser &to_json(SymRepTools::SubWedge const &wedge, jsonParser &json);

jsonParser &to_json(VectorSpaceSymReport const &obj, jsonParser &json);

namespace SymRepTools_v2 {
  struct IrrepInfo;
  class SubWedge;
  struct VectorSpaceSymReport;
}  // namespace SymRepTools_v2

jsonParser &to_json(SymRepTools_v2::IrrepInfo const &irrep, jsonParser &json);

jsonParser &to_json(SymRepTools_v2::SubWedge const &wedge, jsonParser &json);

jsonParser &to_json(SymRepTools_v2::VectorSpaceSymReport const &obj, jsonParser &json);

}  // namespace CASM

#endif
