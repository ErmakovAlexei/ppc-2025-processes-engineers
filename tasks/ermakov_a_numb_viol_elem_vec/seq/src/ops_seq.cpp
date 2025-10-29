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
  return GetInput() > 0;
}

bool ErmakovANumbViolElemVecSEQ::PreProcessingImpl() {
  return true;
}

bool ErmakovANumbViolElemVecSEQ::RunImpl() {
  int n = GetInput();
  if (n <= 0){
    GetOutput() = 0;
    return true;
  }
  std::vector<int> vec;
  vec.reserve(n);
  std::mt19937 rng(static_cast<unsigned int>(GetInput()));
  std::uniform_int_distribution<int> dist(0, 1000000);
  for(int i = 0; i < n; ++i){
    vec.push_back(dist(rng));
  }

  int viol = 0;
  for(int i = 0; i + 1 < n; ++i){
    if (vec[i] > vec[i + 1]) {
      viol += 1;
    }
  }

  
  GetOutput() = viol;
  return true;

}

bool ErmakovANumbViolElemVecSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
