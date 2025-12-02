#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <string>
#include <tuple>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"
#include "ermakov_a_numb_viol_elem_vec/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

class ErmakovRunFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    const auto &vec = std::get<0>(test_param);
    int expected = std::get<1>(test_param);
    return "size_" + std::to_string(vec.size()) + "_exp_" + std::to_string(expected);
  }

 protected:
  InType input_data_;
  OutType expected_output_;

  void SetUp() override {
    const TestType params = std::get<TestType>(ErmakovRunFuncTests::GetParam());

    input_data_ = std::get<0>(params);
    expected_output_ = std::get<1>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == expected_output_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ErmakovRunFuncTests, Correctness) {
  ExecuteTest(GetParam());
}

namespace {

const std::array<TestType, 9> kTestParam = {
    std::make_tuple(std::vector<int>{1, 2, 3, 4, 5}, 0),
    std::make_tuple(std::vector<int>{5, 4, 3, 2, 1}, 4),
    std::make_tuple(std::vector<int>{1, 3, 2, 5, 4}, 2),
    std::make_tuple(std::vector<int>{7, 7, 7, 7}, 0),
    std::make_tuple(std::vector<int>{42}, 0),
    std::make_tuple(std::vector<int>{}, 0),
    std::make_tuple(std::vector<int>{1, 3, 2, 4, 3, 5, 4}, 3),
    std::make_tuple(std::vector<int>{2, 1, 3}, 1),
    std::make_tuple(std::vector<int>{1, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6, 8, 7, 9, 8}, 8),
};

const auto kTestTasks = std::tuple_cat(
    ppc::util::AddFuncTask<ErmakovANumbViolElemVecMPI, InType>(kTestParam, PPC_SETTINGS_ermakov_a_numb_viol_elem_vec),
    ppc::util::AddFuncTask<ErmakovANumbViolElemVecSEQ, InType>(kTestParam, PPC_SETTINGS_ermakov_a_numb_viol_elem_vec));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasks);

const auto kTestNameGen = ErmakovRunFuncTests::PrintFuncTestName<ErmakovRunFuncTests>;

using ParamType = ErmakovRunFuncTests::ParamType;

::testing::internal::ParamGenerator<ParamType> ErmakovGenerator() {
  return kGtestValues;
}

std::string ErmakovNameGen(const ::testing::TestParamInfo<ParamType> &info) {
  return kTestNameGen(info);
}

const int kErmakovDummy =
    ::testing::UnitTest::GetInstance()
        ->parameterized_test_registry()
        .GetTestSuitePatternHolder<ErmakovRunFuncTests>("ErmakovRunFuncTests",
                                                        ::testing::internal::CodeLocation(__FILE__, __LINE__))
        ->AddTestSuiteInstantiation("FuncTests", &ErmakovGenerator, &ErmakovNameGen, __FILE__, __LINE__);

}  // namespace
}  // namespace ermakov_a_numb_viol_elem_vec
