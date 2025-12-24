#include <gtest/gtest.h>
#include <mpi.h>

#include <algorithm>
#include <random>
#include <string>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"
#include "ermakov_a_quick_sort_betcher/mpi/include/ops_mpi.hpp"
#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_quick_sort_betcher {

class ErmakovAQuickSortBetcherRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int n = std::get<0>(params);
    std::string desc = std::get<1>(params);

    input_data_.resize(n);

    if (desc == "AlreadySorted") {
      for (int i = 0; i < n; ++i) {
        input_data_[i] = i;
      }
    } else if (desc == "ReverseSorted") {
      for (int i = 0; i < n; ++i) {
        input_data_[i] = n - i;
      }
    } else if (n > 0) {
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(-1000, 1000);
      for (int i = 0; i < n; ++i) {
        input_data_[i] = dis(gen);
      }
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    int rank = 0;
    int is_mpi_initialized = 0;

    // Проверяем, запущен ли MPI
    MPI_Initialized(&is_mpi_initialized);

    if (is_mpi_initialized) {
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    }

    // В MPI только Rank 0 делает проверку. В SEQ rank всегда 0.
    if (rank != 0) {
      return true;
    }

    OutType reference = input_data_;
    std::sort(reference.begin(), reference.end());
    return (reference == output_data);
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
};

namespace {

TEST_P(ErmakovAQuickSortBetcherRunFuncTests, TestSorting) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 9> kTestParam = {
    std::make_tuple(0, "EmptyArray"),      std::make_tuple(1, "OneElement"),      std::make_tuple(2, "TwoElements"),
    std::make_tuple(8, "PowerOfTwo"),      std::make_tuple(13, "RandomSize"),     std::make_tuple(100, "MediumSize"),
    std::make_tuple(128, "AlreadySorted"), std::make_tuple(127, "ReverseSorted"), std::make_tuple(512, "LargeSize")};

const auto kTestTasksList = std::tuple_cat(ppc::util::AddFuncTask<ErmakovAQuickSortBetcherTestTaskMPI, InType>(
                                               kTestParam, PPC_SETTINGS_ermakov_a_quick_sort_betcher),
                                           ppc::util::AddFuncTask<ErmakovAQuickSortBetcherTestTaskSEQ, InType>(
                                               kTestParam, PPC_SETTINGS_ermakov_a_quick_sort_betcher));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);
const auto kPerfTestName =
    ErmakovAQuickSortBetcherRunFuncTests::PrintFuncTestName<ErmakovAQuickSortBetcherRunFuncTests>;

INSTANTIATE_TEST_SUITE_P(BatcherSortTests, ErmakovAQuickSortBetcherRunFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace ermakov_a_quick_sort_betcher
