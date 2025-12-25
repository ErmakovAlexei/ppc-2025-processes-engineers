# Отчёт по лабораторной работе 
## Быстрая сортировка с четно-нечетным слиянием Бэтчера
## Вариант: 15

**Студент:** Ермаков Алексей Викторович, группа 3823Б1ПР3  
**Преподаватели:** Сысоев А. В., Оболенский А. А., Нестеров А.

---

## 1. Введение

Цель работы - реализовать эффективную сортировку больших массивов с использованием алгоритма быстрой сортировки и сети Бэтчера для параллельной обработки.  

В работе реализованы:
- **Последовательная версия (SEQ):** сортировка с применением четно-нечетного слияния Бэтчера.
- **Параллельная версия (MPI):** распределение данных между процессами и параллельное выполнение сети Бэтчера.

---

## 2. Постановка задачи

**Входные данные:**
- `vector<int>` - массив чисел для сортировки.

**Выходные данные:**
- Отсортированный массив `vector<int>`.

**Ключевые требования:**
1. Сортировка должна корректно работать для массивов произвольного размера.  
2. Для массивов размером степени двойки применяется Batcher Network.  
3. Параллельная версия должна корректно распределять данные между процессами MPI.

---

## 3. Базовый алгоритм (последовательный)

В SEQ версии реализован гибридный алгоритм:
1. **Проверка размера:** Если размер массива является степенью двойки, используется гибридная схема.
2. **Гибридная схема:**
   - Массив делится на две половины.
   - Каждая половина сортируется независимо с помощью **итеративного QuickSort**. Вместо рекурсии используется `std::stack` для управления границами подмассивов.
   - Выполняется слияние отсортированных половин с помощью последовательной реализации сети Бэтчера.
3. **Обработка произвольных размеров:**
   - Если размер не является степенью двойки, массив сортируется итеративным QuickSort в один проход.

---

## 4. Схема параллелизации (MPI)

MPI версия реализует параллельную сеть Бэтчера:
- **Распределение данных:** Массив распределяется между процессами (`MPI_Scatter`). При необходимости данные дополняются значением `INT_MAX`.
- **Локальная сортировка:** Каждый процесс выполняет **итеративный QuickSort** над своим локальным сегментом данных.
- **Сеть Бэтчера (CompareSplit):**
  - Процессы обмениваются сегментами согласно шагам сети Бэтчера (`MPI_Sendrecv`).
  - Процесс с меньшим рангом сохраняет меньшие элементы (`CompareSplitLow`), процесс с большим — большие элементы (`CompareSplitHigh`).
- **Сбор результата:** Отсортированные сегменты собираются на процессе 0 (`MPI_Gather`), после чего фиктивные элементы удаляются.

---

## 5. Детали реализации

**Основные файлы:**
- `ops_seq.cpp` - SEQ реализация.
- `ops_mpi.cpp` - MPI реализация.
- `common.hpp` - общие структуры данных.

**Основные методы:**
- `ValidationImpl()` - проверка корректности данных.
- `PreProcessingImpl()` - подготовка массива.
- `RunImpl()` - выполнение алгоритма сортировки (SEQ или MPI).
- `DoBatcherSort()` - последовательная сеть Бэтчера.
- `CompareSplitLow()` / `CompareSplitHigh()` - MPI обмен и слияние сегментов.

---

## 6. Экспериментальная установка

**Оборудование:**
- CPU: Ryzen 5 1600 (3.8 GHz)
- RAM: 8 GB
- OS: Windows 11

**Программное обеспечение:**
- MS-MPI 10.0
- CMake 4.2.0-rc1
- Google Test
- Режим сборки: Release

---

## 7. Результаты и обсуждение

### 7.1 Корректность
Тестирование проводилось на массиве одного размера. В SEQ версии корректность обеспечивается сетью Бэтчера и быстрой сортировкой. MPI версия корректно сортирует данные при любом числе процессов, включая массивы, размер которых не является степенью двойки.

### 7.2 Производительность

Тестирование проводилось на массиве размера 2097152.  

| Режим          | Процессы | Время (сек) | Ускорение |
|----------------|----------|-------------|-----------|
| SEQ (task_run) | 1        | 2.238929    | 1.00      |
| MPI (task_run) | 1        | 0.129488    | 17.28     |
| MPI (task_run) | 2        | 0.074597    | 29.99     |
| MPI (task_run) | 3        | 0.059703    | 37.52     |
| MPI (task_run) | 8        | 0.043429    | 51.55     |
| MPI (task_run) | 12       | 0.049918    | 44.83     |
| MPI (task_run) | 13       | 0.113330    | 19.76     |

**Анализ масштабируемости:**  
- Для MPI наблюдается значительное ускорение по сравнению с SEQ при небольшом числе процессов (1-8).  
- При увеличении числа процессов (12-13) эффективность падает из-за накладных расходов на обмен данными между процессами и особенностей сети Бэтчера.
- Наибольшее ускорение достигается при 8 процессах (≈51×), а при 13 процессах ускорение снижается из-за высокой доли коммуникаций относительно объёма работы каждого процесса.
- Так как алгоритм Бэтчера применим только для массивов размер которых является степенью двойки, то для честного сравнения seq с mpi тесты проводились на массиве размер которого является степенью 2

---

## 8. Выводы
- Реализован алгоритм быстрой сортировки с сетью Бэтчера для массивов любых размеров и любого числа процессов.  
- Параллельная версия MPI корректно распределяет и сортирует данные между процессами.  
- Сеть Бэтчера обеспечивает детерминированную сортировку в параллельной среде.

---

## 9. Список источников

1. MPI Forum. MPI: A Message-Passing Interface Standard, Version 4.0, 2021.
2. Microsoft MPI Documentation.
3. Документация преподавателей: https://learning-process.github.io/parallel_programming_slides/
4. Лекции Сысоев А. В., Оболенский А. А., Нестеров А., ННГУ, 2025.

---

# Приложение

## Фрагмент кода (SEQ)

```cpp
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
```

## Фрагмент кода (MPI)
```cpp
void ErmakovAQuickSortBetcherTestTaskMPI::QuickSort(std::vector<int> &arr, int left, int right) {
  if (left >= right) {
    return;
  }

  std::stack<std::pair<int, int>> s;
  s.push({left, right});

  while (!s.empty()) {
    std::pair<int, int> range = s.top();
    s.pop();

    int l = range.first;
    int r = range.second;

    if (l >= r) {
      continue;
    }

    int pivot = arr[l + ((r - l) / 2)];
    int i = l;
    int j = r;

    while (i <= j) {
      while (arr[i] < pivot) {
        i++;
      }
      while (arr[j] > pivot) {
        j--;
      }
      if (i <= j) {
        std::swap(arr[i], arr[j]);
        i++;
        j--;
      }
    }

    if (l < j) {
      s.push({l, j});
    }
    if (i < r) {
      s.push({i, r});
    }
  }
}

void ErmakovAQuickSortBetcherTestTaskMPI::CompareSplitLow(int partner) {
  const int size = static_cast<int>(local_vec_.size());

  MPI_Sendrecv(local_vec_.data(), size, MPI_INT, partner, 0, remote_data_.data(), size, MPI_INT, partner, 0,
               MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  int i = 0;
  int j = 0;
  for (int k = 0; k < size; ++k) {
    if (j >= size || (i < size && local_vec_[i] <= remote_data_[j])) {
      temp_data_[k] = local_vec_[i++];
    } else {
      temp_data_[k] = remote_data_[j++];
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
      temp_data_[k] = local_vec_[i--];
    } else {
      temp_data_[k] = remote_data_[j--];
    }
  }

  local_vec_ = temp_data_;
}

void ErmakovAQuickSortBetcherTestTaskMPI::RunBatcherStep(int phase, int step, int next_power_of_two) {
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

void ErmakovAQuickSortBetcherTestTaskMPI::RunBatcherNetwork(int next_power_of_two) {
  for (int phase = 1; phase < next_power_of_two; phase <<= 1) {
    for (int step = phase; step > 0; step >>= 1) {
      RunBatcherStep(phase, step, next_power_of_two);
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

  if (!local_vec_.empty()) {
    QuickSort(local_vec_, 0, static_cast<int>(local_vec_.size()) - 1);
  }

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
```