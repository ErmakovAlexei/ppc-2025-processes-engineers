#include "ermakov_a_quick_sort_betcher/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <limits>
#include <ranges>
#include <vector>

namespace ermakov_a_quick_sort_betcher {

ErmakovAQuickSortBetcherTestTaskMPI::ErmakovAQuickSortBetcherTestTaskMPI(const InType &in)
    : world_size_(0), world_rank_(0) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::ValidationImpl() {
  int is_init = 0;
  MPI_Initialized(&is_init);
  if (is_init == 0) {
    return true;
  }

  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  return size > 0;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::PreProcessingImpl() {
  return true;
}

void ErmakovAQuickSortBetcherTestTaskMPI::CompareSplitLow(int partner) {
  const int size = static_cast<int>(local_vec_.size());

  MPI_Sendrecv(local_vec_.data(), size, MPI_INT, partner, 0, remote_data_.data(), size, MPI_INT, partner, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  int i = 0;
  int j = 0;

  for (int k = 0; k < size; ++k) {
    if (j >= size || (i < size && local_vec_[i] <= remote_data_[j])) {
      temp_data_[k] = local_vec_[i];
      ++i;
    } else {
      temp_data_[k] = remote_data_[j];
      ++j;
    }
  }

  local_vec_ = temp_data_;
}

void ErmakovAQuickSortBetcherTestTaskMPI::CompareSplitHigh(int partner) {
  const int size = static_cast<int>(local_vec_.size());

  MPI_Sendrecv(local_vec_.data(), size, MPI_INT, partner, 0, remote_data_.data(), size, MPI_INT, partner, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  int i = size - 1;
  int j = size - 1;

  for (int k = size - 1; k >= 0; --k) {
    if (j < 0 || (i >= 0 && local_vec_[i] >= remote_data_[j])) {
      temp_data_[k] = local_vec_[i];
      --i;
    } else {
      temp_data_[k] = remote_data_[j];
      --j;
    }
  }

  local_vec_ = temp_data_;
}

void ErmakovAQuickSortBetcherTestTaskMPI::RunBatcherNetwork(int next_power_of_two) {
  for (int phase = 1; phase < next_power_of_two; phase <<= 1) {
    for (int step = phase; step > 0; step >>= 1) {
      for (int base = step % phase; base <= next_power_of_two - 1 - step; base += 2 * step) {
        for (int offset = 0; offset < step; ++offset) {
          const int p1 = base + offset;
          const int p2 = base + offset + step;

          const bool active_pair = (p1 < world_size_ && p2 < world_size_) && ((p1 / (phase * 2)) == (p2 / (phase * 2)));

          if (active_pair) {
            if (world_rank_ == p1) {
              CompareSplitLow(p2);
            } else if (world_rank_ == p2) {
              CompareSplitHigh(p1);
            }
          }
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
  }
}

bool ErmakovAQuickSortBetcherTestTaskMPI::RunImpl() {
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size_);

  int total_n = 0;
  if (world_rank_ == 0) {
    total_n = static_cast<int>(GetInput().size());
  }

  MPI_Bcast(&total_n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (total_n == 0) {
    return true;
  }

  const int elements_per_proc = (total_n + world_size_ - 1) / world_size_;
  const int padded_size = elements_per_proc * world_size_;

  local_vec_.assign(elements_per_proc, std::numeric_limits<int>::max());
  remote_data_.resize(elements_per_proc);
  temp_data_.resize(elements_per_proc);

  std::vector<int> full_vec;

  if (world_rank_ == 0) {
    full_vec.assign(padded_size, std::numeric_limits<int>::max());
    std::copy(GetInput().begin(), GetInput().end(), full_vec.begin());
  }

  MPI_Scatter(world_rank_ == 0 ? full_vec.data() : nullptr, elements_per_proc, MPI_INT, local_vec_.data(),
              elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  std::ranges::sort(local_vec_);

  int next_power_of_two = 1;
  while (next_power_of_two < world_size_) {
    next_power_of_two <<= 1;
  }

  RunBatcherNetwork(next_power_of_two);

  if (world_rank_ == 0) {
    full_vec.assign(padded_size, 0);
  }

  MPI_Gather(local_vec_.data(), elements_per_proc, MPI_INT, world_rank_ == 0 ? full_vec.data() : nullptr,
             elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank_ == 0) {
    full_vec.resize(total_n);
    GetOutput() = full_vec;
  }

  return true;
}

bool ErmakovAQuickSortBetcherTestTaskMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_quick_sort_betcher
