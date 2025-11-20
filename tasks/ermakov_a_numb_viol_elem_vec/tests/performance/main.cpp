#include <gtest/gtest.h>

#include <numeric>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"
#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

class ErmakovANumbViolElemVecPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 10000000;
  InType input_data_;

  void SetUp() override {
    input_data_.resize(kCount_);
    std::iota(input_data_.begin(), input_data_.end(), 1);
    input_data_[kCount_ / 2] = -1;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    if (output_data >= 0) {
      return true;
    } else {
      return false;
    }
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ErmakovANumbViolElemVecPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ErmakovANumbViolElemVecMPI, ErmakovANumbViolElemVecSEQ>(
    PPC_SETTINGS_ermakov_a_numb_viol_elem_vec);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ErmakovANumbViolElemVecPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ErmakovANumbViolElemVecPerfTests, kGtestValues, kPerfTestName);

}  // namespace ermakov_a_numb_viol_elem_vec
