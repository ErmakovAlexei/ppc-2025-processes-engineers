#include <gtest/gtest.h>

#include <random>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"
#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

class ErmakovANumbViolElemVecPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 public:
  ErmakovANumbViolElemVecPerfTests() = default;

 protected:
  void SetUp() override {
    PrepareInput();
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data >= 0;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  static constexpr int CountElem = 200000000;
  InType input_data_;

  void PrepareInput() {
    input_data_.resize(CountElem);

    std::mt19937 gen(1337);
    std::uniform_int_distribution<int> low(0, 50);
    std::uniform_int_distribution<int> high(200, 500);

    for (int i = 0; i < CountElem; ++i) {
      if ((i / 1000) % 2 == 0) {
        input_data_[i] = low(gen);
      } else {
        input_data_[i] = high(gen);
      }
    }

    for (int j = 0; j < 100; ++j) {
      int pos = gen() % (CountElem - 1);
      input_data_[pos] = input_data_[pos + 1] + 100;
    }
  }
};

TEST_P(ErmakovANumbViolElemVecPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, ErmakovANumbViolElemVecMPI, ErmakovANumbViolElemVecSEQ>(
    PPC_SETTINGS_ermakov_a_numb_viol_elem_vec);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfName = ErmakovANumbViolElemVecPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(PerfRuns, ErmakovANumbViolElemVecPerfTests, kGtestValues, kPerfName);

}  // namespace ermakov_a_numb_viol_elem_vec
