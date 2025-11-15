#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"

#include <numeric>
#include <random>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

ErmakovANumbViolElemVecSEQ::ErmakovANumbViolElemVecSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool ErmakovANumbViolElemVecSEQ::ValidationImpl() {
  return true;
}

bool ErmakovANumbViolElemVecSEQ::PreProcessingImpl() {
  return true;
}

bool ErmakovANumbViolElemVecSEQ::RunImpl() {
  const auto &vec = GetInput();
  int n = static_cast<int>(vec.size());
  if (n <= 0) {
    GetOutput() = 0;
    return true;
  }

  int viol = 0;
  for (int i = 0; i + 1 < n; ++i) {
    if (vec[i] > vec[i + 1]) {
      ++viol;
    }
  }

  GetOutput() = viol;
  return true;
}

bool ErmakovANumbViolElemVecSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
