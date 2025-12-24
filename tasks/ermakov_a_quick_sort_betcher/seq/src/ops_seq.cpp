#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"

namespace ermakov_a_quick_sort_betcher {

namespace {

void CompareAndSwap(std::vector<int> &data, int idx1, int idx2, int block) {
  if ((idx1 / block) == (idx2 / block)) {
    if (data[idx1] > data[idx2]) {
      std::swap(data[idx1], data[idx2]);
    }
  }
}

}  // namespace

ErmakovAQuickSortBetcherTestTaskSEQ::ErmakovAQuickSortBetcherTestTaskSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::ValidationImpl() {
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PreProcessingImpl() {
  input_size_ = static_cast<int>(GetInput().size());
  GetOutput() = GetInput();
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::IsPowerOfTwo(std::size_t n) {
  return (n > 0) && ((n & (n - 1)) == 0);
}

void ErmakovAQuickSortBetcherTestTaskSEQ::DoBatcherSort() {
  auto &data = GetOutput();
  int n = static_cast<int>(data.size());

  for (int phase = 1; phase < n; phase <<= 1) {
    for (int step = phase; step > 0; step >>= 1) {
      for (int j = step % phase; j <= n - 1 - step; j += 2 * step) {
        for (int i = 0; i < step; ++i) {
          int idx1 = j + i;
          int idx2 = j + i + step;
          CompareAndSwap(data, idx1, idx2, phase * 2);
        }
      }
    }
  }
}

void ErmakovAQuickSortBetcherTestTaskSEQ::DoStdSort() {
  std::ranges::sort(GetOutput());
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }

  if (IsPowerOfTwo(GetOutput().size()) && GetOutput().size() < 100000) {
    DoBatcherSort();
  } else {
    DoStdSort();
  }

  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
