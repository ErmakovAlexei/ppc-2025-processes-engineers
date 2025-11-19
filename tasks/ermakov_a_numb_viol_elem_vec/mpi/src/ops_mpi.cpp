#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

#include "ermakov_a_numb_viol_elem_vec/common/include/common.hpp"
#include "util/include/util.hpp"

namespace ermakov_a_numb_viol_elem_vec {

ErmakovANumbViolElemVecMPI::ErmakovANumbViolElemVecMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool ErmakovANumbViolElemVecMPI::ValidationImpl() {
  return true;
}

bool ErmakovANumbViolElemVecMPI::PreProcessingImpl() {
  return true;
}

bool ErmakovANumbViolElemVecMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const std::vector<int> &vec = GetInput();
  int n = vec.size();

  MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

  if (n == 0) {
    GetOutput() = 0;
    return true;
  }

  int base = n / size;
  int rem = n % size;

  std::vector<int> counts(size);
  std::vector<int> displs(size);

  int offset = 0;
  for (int i = 0; i < size; i++) {
    counts[i] = base;
    if (i < rem) {
      counts[i] += 1;
    }
    displs[i] = offset;
    offset += counts[i];
  }

  int local_n = counts[rank];
  std::vector<int> local_vec(local_n);

  if (rank == 0) {
    std::vector<MPI_Request> reqs;
    reqs.reserve(size)

        for (int p = 1; p < size; p++) {
      if (counts[p] > 0) {
        MPI_Request req;
        MPI_Isend(vec.data() + displs[p], counts[p], MPI_INT, p, 0, MPI_COMM_WORLD, &req);
        reqs.push_back(req);
      }
    }

    for (int i = 0; i < local_n; i++) {
      local_vec[i] = vec[i];
    }

    if (!reqs.empty()) {
      MPI_Waitall(static_cast<int>(reqs.size()), reqs.data(), MPI_STATUSES_IGNORE);
    }

  } else {
    if (local_n > 0) {
      MPI_Request req;
      MPI_Irecv(local_vec.data(), local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, &req);
      MPI_Wait(&req, MPI_STATUS_IGNORE);
    }
  }

  int local_viol = 0;
  for (int i = 0; i + 1 < local_n; i++) {
    if (local_vec[i] > local_vec[i + 1]) {
      local_viol++;
    }
  }

  int my_first = 0;
  int my_last = 0;

  if (local_n > 0) {
    my_first = local_vec[0];
    my_last = local_vec[local_n - 1];
  }

  int send_to = -1;
  int recv_from = -1;

  for (int i = rank - 1; i >= 0; --i) {
    if (counts[i] > 0) {
      recv_from = i;
      break;
    }
  }

  for (int i = rank + 1; i < size; ++i) {
    if (counts[i] > 0) {
      send_to = i;
      break;
    }
  }

  int send_val = 0;
  if (local_n > 0) {
    send_val = my_last;
  }

  int recv_val = 0;

  MPI_Request reqs2[2];
  int reqC = 0;

  if (send_to != -1) {
    MPI_Isend(&send_val, 1, MPI_INT, send_to, 1, MPI_COMM_WORLD, &reqs2[reqC++]);
  }

  if (recv_from != -1) {
    MPI_Irecv(&recv_val, 1, MPI_INT, recv_from, 1, MPI_COMM_WORLD, &reqs2[reqC++]);
  }

  if (reqC > 0) {
    MPI_Waitall(reqC, reqs2, MPI_STATUSES_IGNORE);
  }

  int border = 0;
  if (recv_from != -1 && local_n > 0) {
    if (recv_val > my_first) {
      border = 1;
    }
  }

  int total_viol = local_viol + border;
  int result = 0;

  MPI_Reduce(&total_viol, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Bcast(&result, 1, MPI_INT, 0, MPI_COMM_WORLD);

  GetOutput() = result;
  return true;
}

bool ErmakovANumbViolElemVecMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
