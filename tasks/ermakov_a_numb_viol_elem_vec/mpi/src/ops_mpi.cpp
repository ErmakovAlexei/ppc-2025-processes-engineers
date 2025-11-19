#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

ErmakovANumbViolElemVecMPI::ErmakovANumbViolElemVecMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

auto ErmakovANumbViolElemVecMPI::ValidationImpl() -> bool {
  return true;
}

auto ErmakovANumbViolElemVecMPI::PreProcessingImpl() -> bool {
  return true;
}

auto ErmakovANumbViolElemVecMPI::RunImpl() -> bool {
  int rank;
  int size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::vector<int> &vec = GetInput();
  const int n = static_cast<int>(vec.size());

  if (n <= 0) {
    int zero = 0;
    MPI_Bcast(&zero, 1, MPI_INT, 0, MPI_COMM_WORLD);
    GetOutput() = 0;
    return true;
  }

  const int base = n / size;
  const int rem = n % size;

  std::vector<int> cnt(size);   // размер блоков
  std::vector<int> disp(size);  // вектор смещений
  int shift = 0;                // смещение
  for (int i = 0; i < size; ++i) {
    cnt[i] = base;
    if (i < rem) {
      cnt[i] = base + 1;
    }
    disp[i] = shift;
    shift += cnt[i];
  }

  const int local_n = cnt[rank];
  std::vector<int> interim_vec(local_n);

  if (rank == 0) {
    if (local_n > 0) {
      std::copy(vec.begin(), vec.begin() + local_n, interim_vec.begin());
    }

    for (int p = 1; p < size; ++p) {
      if (cnt[p] > 0) {
        MPI_Send(vec.data() + disp[p], cnt[p], MPI_INT, p, 0, MPI_COMM_WORLD);
      }
    }
  } else {
    if (local_n > 0) {
      MPI_Recv(interim_vec.data(), local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
  }

  int interim_viol = 0;
#ifdef _OPENMP
#  pragma omp parallel for reduction(+ : interim_viol)
#endif
  for (int i = 0; i < local_n - 1; ++i) {
    if (interim_vec[i] > interim_vec[i + 1]) {
      interim_viol += 1;
    }
  }

  int border = 0;
  int left_last = 0;

  if (rank > 0) {
    MPI_Recv(&left_last, 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (local_n > 0 && left_last > interim_vec[0]) {
      border = 1;
    }
  }

  if (rank < size - 1 && local_n > 0) {
    int my_last = interim_vec.back();
    MPI_Send(&my_last, 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
  }

  int total_local = interim_viol + border;
  int global_total = 0;

  MPI_Reduce(&total_local, &global_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&global_total, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = global_total;
  return true;
}

auto ErmakovANumbViolElemVecMPI::PostProcessingImpl() -> bool {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
