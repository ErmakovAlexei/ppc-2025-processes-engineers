#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
#include <random>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "ermakov_a_quick_sort_betcher/mpi/include/ops_mpi.hpp"
#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace ermakov_a_quick_sort_betcher {

class ErmakovAQuickSortBetcherRunPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  const int kCount_ = 65536;
  InType input_data_;

  void SetUp() override {
    input_data_.resize(kCount_);
    std::mt19937 gen((std::random_device()()));

    for (int i = 0; i < kCount_; ++i) {
      input_data_[i] = static_cast<int>(gen() % 10000);
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank != 0) {
      return true;
    }

    return std::is_sorted(output_data.begin(), output_data.end());
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ErmakovAQuickSortBetcherRunPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, ErmakovAQuickSortBetcherTestTaskMPI, ErmakovAQuickSortBetcherTestTaskSEQ>(
        PPC_SETTINGS_ermakov_a_quick_sort_betcher);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ErmakovAQuickSortBetcherRunPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ErmakovAQuickSortBetcherRunPerfTests, kGtestValues, kPerfTestName);

}  // namespace ermakov_a_quick_sort_betcher
