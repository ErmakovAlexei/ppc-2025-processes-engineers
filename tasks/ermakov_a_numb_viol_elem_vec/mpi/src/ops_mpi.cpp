#include "ermakov_a_numb_viol_elem_vec/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <random>
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

bool ErmakovANumbViolElemVecMPI::ValidationImpl() {
  return GetInput() > 0;
}

bool ErmakovANumbViolElemVecMPI::PreProcessingImpl() {
  return true;
}

bool ErmakovANumbViolElemVecMPI::RunImpl() {
  int world_rank, world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  int n = GetInput();

  std::vector<int> full_vec;
  if (world_rank == 0){
    std::mt19937 rng(static_cast<unsigned int>(GetInput()));
    std::uniform_int_distribution<int> dist(0, 1000000);

    full_vec.resize(n);
    for (int i = 0; i < n; ++i){
      full_vec[i] = dist(rng);
    }
  }

  int base_elem = n / world_size;
  int rest = n % world_size;

  int my_size = base_elem;
  if (world_rank < rest){
    my_size += 1;
  }

  std::vector<int> interim_vec(my_size);

  if (world_rank == 0){
    int offset = 0;
    for (int i = 0; i < world_size; ++i){
      int send_size = base_elem;
      if (i < rest) {
        send_size += 1;
      }

      if (i == 0){
        for (int j = 0; j < send_size; ++j){
          interim_vec[j] = full_vec[j];
        }
      } else {
          MPI_Send(full_vec.data() + offset, send_size, MPI_INT, i, 0, MPI_COMM_WORLD);
      }

      offset += send_size;
    }
  }else {
    MPI_Recv(interim_vec.data(), my_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  }

  int interim_viol = 0;
  for (int i = 0; i + 1 < my_size; ++i){
    if (interim_vec[i] > interim_vec[i + 1]){
      interim_viol++;
    }
  }

  if (world_rank < world_size - 1 && !interim_vec.empty()){
    int last_val = interim_vec.back();
    MPI_Send(&last_val, 1, MPI_INT, world_rank + 1, 1, MPI_COMM_WORLD);
  }

  if (world_rank > 0 && !interim_vec.empty()){
    int loc_val;
    MPI_Recv(&loc_val, 1, MPI_INT, world_rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (loc_val > interim_vec.front()){
      interim_viol++;
    }
  }

  int res = 0;
  MPI_Reduce(&interim_viol, &res, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (world_rank == 0){
    GetOutput() = res;
  }
  return true;
}

bool ErmakovANumbViolElemVecMPI::PostProcessingImpl() {
  return true;
}

}  // namespace ermakov_a_numb_viol_elem_vec
