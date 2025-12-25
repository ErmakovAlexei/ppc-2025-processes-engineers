#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <stack>
#include <utility>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"

namespace ermakov_a_quick_sort_betcher {

namespace {

void BatcherCompare(std::vector<int> &data, int idx1, int idx2, int phase) {
  if ((idx1 / (phase * 2)) == (idx2 / (phase * 2))) {
    if (data[idx1] > data[idx2]) {
      std::swap(data[idx1], data[idx2]);
    }
  }
}

void Partition(std::vector<int> &arr, int left, int right, int pivot, int &i, int &j) {
  i = left;
  j = right;

  while (i <= j) {
    while (arr[i] < pivot) {
      ++i;
    }
    while (arr[j] > pivot) {
      --j;
    }
    if (i <= j) {
      std::swap(arr[i], arr[j]);
      ++i;
      --j;
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
  GetOutput() = GetInput();
  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::IsPowerOfTwo(std::size_t n) {
  return (n > 0) && ((n & (n - 1)) == 0);
}

void ErmakovAQuickSortBetcherTestTaskSEQ::QuickSort(std::vector<int> &arr, int left, int right) {
  if (left >= right) {
    return;
  }

  std::stack<std::pair<int, int>> stack;
  stack.emplace(left, right);

  while (!stack.empty()) {
    const auto [l_bound, r_bound] = stack.top();
    stack.pop();

    if (l_bound >= r_bound) {
      continue;
    }

    const int pivot = arr[l_bound + ((r_bound - l_bound) / 2)];
    int i_idx = 0;
    int j_idx = 0;

    Partition(arr, l_bound, r_bound, pivot, i_idx, j_idx);

    if (l_bound < j_idx) {
      stack.emplace(l_bound, j_idx);
    }
    if (i_idx < r_bound) {
      stack.emplace(i_idx, r_bound);
    }
  }
}

void ErmakovAQuickSortBetcherTestTaskSEQ::DoBatcherSort() {
  auto &data = GetOutput();
  const int n_size = static_cast<int>(data.size());

  for (int phase = 1; phase < n_size; phase <<= 1) {
    for (int step = phase; step > 0; step >>= 1) {
      for (int base = step % phase; base <= n_size - 1 - step; base += 2 * step) {
        for (int offset = 0; offset < step; ++offset) {
          BatcherCompare(data, base + offset, base + offset + step, phase);
        }
      }
    }
  }
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }

  const int n_size = static_cast<int>(GetOutput().size());

  if (IsPowerOfTwo(n_size)) {
    const int mid = n_size / 2;
    if (mid > 0) {
      QuickSort(GetOutput(), 0, mid - 1);
      QuickSort(GetOutput(), mid, n_size - 1);
    }
    DoBatcherSort();
  } else {
    QuickSort(GetOutput(), 0, n_size - 1);
  }

  return true;
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
