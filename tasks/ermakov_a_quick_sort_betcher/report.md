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
1. Если размер массива является степенью двойки:
   - Массив делится на две равные части.
   - Каждая часть сортируется независимо с помощью ручной реализации **QuickSort** (схема Хоара).
   - Затем обе отсортированные части объединяются в единый массив с помощью **сети слияния Бэтчера** (DoBatcherSort). Это позволяет эффективно использовать структуру сети для уже частично упорядоченных данных.
2. Если размер массива не является степенью двойки:
   - Весь массив сортируется с использованием ручной реализации **QuickSort**.

---

## 4. Схема параллелизации (MPI)

MPI версия реализует параллельную сеть Бэтчера:
- **Распределение данных:** Массив делится на сегменты для каждого процесса (`MPI_Scatter`), при необходимости данные дополняются `INT_MAX`.
- **Локальная сортировка:** Каждый процесс сортирует свой сегмент, используя ручную реализацию **QuickSort**.
- **Сеть Бэтчера (CompareSplit):**
  - Процессы выполняют итеративное слияние сегментов.
  - На каждом шаге процессы обмениваются данными (`MPI_Sendrecv`).
  - Процесс с меньшим рангом оставляет меньшие элементы (`CompareSplitLow`), процесс с большим - большие (`CompareSplitHigh`).
- **Сбор результата:** Отсортированные сегменты собираются на процессе 0 (`MPI_Gather`).

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
void ErmakovAQuickSortBetcherTestTaskSEQ::QuickSort(std::vector<int>& arr, int left, int right) {
  if (left >= right) return;
  int pivot = arr[left + (right - left) / 2];
  int i = left, j = right;
  while (i <= j) {
    while (arr[i] < pivot) i++;
    while (arr[j] > pivot) j--;
    if (i <= j) {
      std::swap(arr[i++], arr[j--]);
    }
  }
  if (left < j) QuickSort(arr, left, j);
  if (i < right) QuickSort(arr, i, right);
}

bool ErmakovAQuickSortBetcherTestTaskSEQ::RunImpl() {
  if (GetOutput().empty()) return true;
  int n = static_cast<int>(GetOutput().size());

  if (IsPowerOfTwo(n)) {
    int mid = n / 2;
    QuickSort(GetOutput(), 0, mid - 1);
    QuickSort(GetOutput(), mid, n - 1);
    DoBatcherSort(); 
  } else {
    QuickSort(GetOutput(), 0, n - 1);
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

  int pivot = arr[left + (right - left) / 2];
  int i = left;
  int j = right;

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

  if (left < j) {
    QuickSort(arr, left, j);
  }
  if (i < right) {
    QuickSort(arr, i, right);
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