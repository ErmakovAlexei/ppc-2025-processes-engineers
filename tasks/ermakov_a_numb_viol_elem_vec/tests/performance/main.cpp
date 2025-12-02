#include <gtest/gtest.h>

#include <random>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"
#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

class ErmakovANumbViolElemVecPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  InType input_{};
  int expected_{};

  void SetUp() override {
    const int N = 250000000;
    input_.resize(N);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> small(0, 50);
    std::uniform_int_distribution<int> big(100, 500);

    for (int i = 0; i < N; ++i) {
      input_[i] = (i % 2 == 0) ? big(gen) : small(gen);
    }

    expected_ = 0;
    for (int i = 0; i + 1 < N; ++i) {
      if (input_[i] > input_[i + 1]) {
        ++expected_;
      }
    }
  }

  bool CheckTestOutputData(OutType &out) final {
    return out == expected_;
  }

  InType GetTestInputData() final {
    return input_;
  }
};

TEST_P(ErmakovANumbViolElemVecPerfTests, PerfRun) {
  ExecuteTest(GetParam());
}

namespace {

const auto kName = ErmakovANumbViolElemVecPerfTests::CustomPerfTestName;

auto kPerfValues = ppc::util::TupleToGTestValues(
    ppc::util::MakeAllPerfTasks<InType, ErmakovANumbViolElemVecMPI, ErmakovANumbViolElemVecSEQ>(
        PPC_SETTINGS_ermakov_a_numb_viol_elem_vec));

using ParamType = ErmakovANumbViolElemVecPerfTests::ParamType;

::testing::internal::ParamGenerator<ParamType> PerfGen() {
  return kPerfValues;
}

std::string PerfGenName(const ::testing::TestParamInfo<ParamType> &info) {
  return kName(info);
}

const int kRegPerf = ::testing::UnitTest::GetInstance()
                         ->parameterized_test_registry()
                         .GetTestSuitePatternHolder<ErmakovANumbViolElemVecPerfTests>(
                             "ErmakovANumbViolElemVecPerfTests", ::testing::internal::CodeLocation(__FILE__, __LINE__))
                         ->AddTestSuiteInstantiation("PerfSuite", &PerfGen, &PerfGenName, __FILE__, __LINE__);

}  // namespace

}  // namespace ermakov_a_numb_viol_elem_vec
