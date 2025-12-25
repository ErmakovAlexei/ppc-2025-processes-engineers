#include "ermakov_a_quick_sort_betcher/seq/include/ops_seq.hpp"

#include <algorithm>
#include <cstddef>
#include <stack>
#include <vector>

#include "ermakov_a_quick_sort_betcher/common/include/common.hpp"

namespace ermakov_a_quick_sort_betcher {

static void BatcherCompare(std::vector<int> &data, int idx1, int idx2, int phase) {
  if ((idx1 / (phase * 2)) == (idx2 / (phase * 2))) {
    if (data[idx1] > data[idx2]) {
      std::swap(data[idx1], data[idx2]);
    }
  }
}

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
  stack.push({left, right});

  while (!stack.empty()) {
    std::pair<int, int> range = stack.top();
    stack.pop();

    int l_bound = range.first;
    int r_bound = range.second;
    if (l_bound >= r_bound) {
      continue;
    }

    int pivot = arr[l_bound + ((r_bound - l_bound) / 2)];
    int i_idx = l_bound;
    int j_idx = r_bound;

    while (i_idx <= j_idx) {
      while (arr[i_idx] < pivot) {
        i_idx++;
      }
      while (arr[j_idx] > pivot) {
        j_idx--;
      }
      if (i_idx <= j_idx) {
        std::swap(arr[i_idx], arr[j_idx]);
        i_idx++;
        j_idx--;
      }
    }
    if (l_bound < j_idx) {
      stack.push({l_bound, j_idx});
    }
    if (i_idx < r_bound) {
      stack.push({i_idx, r_bound});
    }
  }
}

void ErmakovAQuickSortBetcherTestTaskSEQ::DoBatcherSort() {
  auto &data = GetOutput();
  int n_size = static_cast<int>(data.size());

  for (int phase = 1; phase < n_size; phase <<= 1) {
    for (int step = phase; step > 0; step >>= 1) {
      for (int base_j = step % phase; base_j <= n_size - 1 - step; base_j += 2 * step) {
        for (int offset_i = 0; offset_i < step; ++offset_i) {
          int idx1 = base_j + offset_i;
          int idx2 = base_j + offset_i + step;
          BatcherCompare(data, idx1, idx2, phase);
        }
      }
    }
  }
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) {
    return true;
  }
  int n_size = static_cast<int>(GetOutput().size());

  if (IsPowerOfTwo(n_size)) {
    int mid = n_size / 2;
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
