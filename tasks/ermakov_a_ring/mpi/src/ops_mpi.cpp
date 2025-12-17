#include "ermakov_a_ring/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <vector>

#include "ermakov_a_ring/common/include/common.hpp"

namespace ermakov_a_ring {

ErmakovATestTaskMPI::ErmakovATestTaskMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
}

bool ErmakovATestTaskMPI::ValidationImpl() {
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  return size > 0;
}

bool ErmakovATestTaskMPI::PreProcessingImpl() {
  GetOutput().clear();
  return true;
}

bool ErmakovATestTaskMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto &input = GetInput();
  std::vector<int> payload = input.data;

  const int src = ((input.source % size) + size) % size;
  const int dst = ((input.dest % size) + size) % size;

  std::vector<int> path;

  const int cw_dist = (dst - src + size) % size;
  const int cc_dist = (src - dst + size) % size;
  const bool clockwise = (cw_dist <= cc_dist);
  const int steps = clockwise ? cw_dist : cc_dist;

  const int next = clockwise ? (rank + 1) % size : (rank - 1 + size) % size;
  const int prev = clockwise ? (rank - 1 + size) % size : (rank + 1) % size;

  const int dist_from_src = clockwise ? (rank - src + size) % size : (src - rank + size) % size;

  if (src == dst) {
    if (rank == src) {
      path = {src};
    }
  } else {
    if (rank == src) {
      path = {src};
      const int path_sz = static_cast<int>(path.size());
      const int data_sz = static_cast<int>(payload.size());

      MPI_Send(payload.data(), data_sz, MPI_INT, next, 100, MPI_COMM_WORLD);
      MPI_Send(&path_sz, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
      MPI_Send(path.data(), path_sz, MPI_INT, next, 1, MPI_COMM_WORLD);
    } else if (dist_from_src > 0 && dist_from_src <= steps) {
      int path_sz = 0;
      const int data_sz = static_cast<int>(payload.size());

      MPI_Recv(payload.data(), data_sz, MPI_INT, prev, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      MPI_Recv(&path_sz, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      path.resize(static_cast<size_t>(path_sz));
      MPI_Recv(path.data(), path_sz, MPI_INT, prev, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      path.push_back(rank);

      if (rank != dst) {
        const int next_path_sz = static_cast<int>(path.size());
        MPI_Send(payload.data(), data_sz, MPI_INT, next, 100, MPI_COMM_WORLD);
        MPI_Send(&next_path_sz, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        MPI_Send(path.data(), next_path_sz, MPI_INT, next, 1, MPI_COMM_WORLD);
      }
    }
  }

  int final_path_sz = 0;
  if (rank == dst) {
    final_path_sz = static_cast<int>(path.size());
  }

  MPI_Bcast(&final_path_sz, 1, MPI_INT, dst, MPI_COMM_WORLD);
  if (rank != dst) {
    path.resize(static_cast<size_t>(final_path_sz));
  }
  MPI_Bcast(path.data(), final_path_sz, MPI_INT, dst, MPI_COMM_WORLD);

  GetOutput() = path;

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool ErmakovATestTaskMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_ring
