#include <gtest/gtest.h>

#include <random>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"
#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

class ErmakovANumbViolElemVecPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  void SetUp() override {
    const int kInputSize = 200000000;

    input_data_.resize(kInputSize);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> small_dist(0, 50);
    std::uniform_int_distribution<int> large_dist(100, 500);

    for (int i = 0; i < kInputSize; i++) {
      if ((i % 2) == 0) {
        input_data_[i] = large_dist(gen);
      } else {
        input_data_[i] = small_dist(gen);
      }
    }

    expected_count_ = 0;
    for (int i = 0; i + 1 < kInputSize; i++) {
      if (input_data_[i] > input_data_[i + 1]) {
        expected_count_++;
      }
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_count_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  int expected_count_{0};
};

TEST_P(ErmakovANumbViolElemVecPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kPerfTasksErmakov =
    ppc::util::MakeAllPerfTasks<InType, ErmakovANumbViolElemVecMPI, ErmakovANumbViolElemVecSEQ>(
        PPC_SETTINGS_ermakov_a_numb_viol_elem_vec);

const auto kPerfValuesErmakov = ppc::util::TupleToGTestValues(kPerfTasksErmakov);

const auto kPerfNameErmakov = ErmakovANumbViolElemVecPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(PerfTestsErmakov, ErmakovANumbViolElemVecPerfTests, kPerfValuesErmakov, kPerfNameErmakov);

}  // namespace ermakov_a_numb_viol_elem_vec
