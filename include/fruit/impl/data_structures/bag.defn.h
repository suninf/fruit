/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FRUIT_BAG_DEFN_H
#define FRUIT_BAG_DEFN_H

namespace fruit {
namespace impl {

template <typename T>
typename Bag<T>::Storage* Bag<T>::getStorage() {
  // Note that the Storage object is thread_local, but the contained elements of type T are not.
  thread_local Storage storage;
  return &storage;
}

template <typename T>
inline Bag<T>::Bag()
  : Bag(getStorage()) {
}

template <typename T>
inline void Bag<T>::check() {
  if (interval_id != invalid_interval_id) {
    assert(interval_id < storage->intervals.apparentSize());
    Interval& interval = storage->intervals[interval_id];
    // To avoid a warning in release mode.
    (void) interval;
    assert(interval.begin < interval.end);
    assert(interval.end <= storage->values.size());
    assert(!storage->intervals.empty());
    assert(!storage->values.empty());
  }
  assert(!storage->values.empty() == !storage->intervals.empty());
  if (!storage->intervals.empty()) {
    assert(storage->intervals[storage->intervals.apparentSize() - 1].end == storage->values.size());
  }
}

template <typename T>
inline Bag<T>::Bag(Storage* storage)
  : storage(storage), interval_id(LinkedVector<Interval>::invalid_pos) {
  check();
}

template <typename T>
inline Bag<T>::Bag(Bag&& other)
  : Bag() {
  swap(other);
  check();
  other.check();
}

template <typename T>
inline Bag<T>::~Bag() {
  if (interval_id != invalid_interval_id) {
    storage->intervals.erase(interval_id);
    std::size_t apparent_size = storage->intervals.apparentSize();
    if (apparent_size == 0) {
      storage->values.clear();
    } else {
      const Interval& last_interval = storage->intervals[apparent_size - 1];
      storage->values.erase(storage->values.begin() + last_interval.end, storage->values.end());
    }
    interval_id = invalid_interval_id;
  }
  check();
}

template <typename T>
inline Bag<T>& Bag<T>::operator=(Bag&& other) {
  swap(other);
  check();
  other.check();
}

template <typename T>
inline void Bag<T>::swap(Bag& other) {
  std::swap(storage, other.storage);
  std::swap(interval_id, other.interval_id);
  check();
  other.check();
}

template <typename T>
template <typename Iter>
inline Bag<T>::Bag(Storage* storage, Iter first, Iter last)
  : storage(storage) {
  interval_id = storage->intervals.apparentSize();
  std::size_t index = storage->values.size();
  storage->intervals.push_back(Interval{index, index + (last - first)});
  storage->values.insert(storage->values.end(), first, last);
  check();
}

template <typename T>
inline void Bag<T>::ensure_last_interval() {
  check();
  assert(interval_id != invalid_interval_id);
  if (storage->intervals.apparentSize() != interval_id + 1) {
    // Not the last interval, need to copy all values at the end of `values'.
    
    // This ensures that there's no reallocation of storage->values in the following loop, so the iterators remain valid.
    {
      const Interval& interval = storage->intervals[interval_id];
      storage->values.reserve(storage->values.size() + (interval.end - interval.begin));
    }
    Bag new_bag(storage, begin(), end());
    swap(new_bag);
  }
  check();
}

template <typename T>
inline T* Bag<T>::begin() {
  check();
  if (interval_id == invalid_interval_id) {
    return nullptr;
  }
  const Interval& interval = storage->intervals[interval_id];
  assert(interval.begin <= storage->values.size());
  return storage->values.data() + interval.begin;
}

template <typename T>
inline T* Bag<T>::end() {
  check();
  if (interval_id == invalid_interval_id) {
    return nullptr;
  }
  const Interval& interval = storage->intervals[interval_id];
  assert(interval.end <= storage->values.size());
  return storage->values.data() + interval.end;
}

template <typename T>
inline void Bag<T>::insert(const T& x) {
  if (interval_id == invalid_interval_id) {
    std::size_t index = storage->values.size();
    storage->values.push_back(x);
    interval_id = storage->intervals.apparentSize();
    storage->intervals.push_back(Interval{index, index + 1});
  } else {
    insertNonContiguous(x);
  }
  check();
}

template <typename T>
void Bag<T>::insertNonContiguous(const T& x) {
  assert(interval_id != invalid_interval_id);
  ensure_last_interval();
  
  // Now just add 1 element to `values' and increase the size of the interval.
  Interval& interval = storage->intervals[interval_id];
  assert(storage->values.size() == interval.end);
  storage->values.push_back(x);
  interval.end++;
}

template <typename T>
inline void Bag<T>::insert(Bag&& other) {
  check();
  other.check();
  if (other.interval_id == invalid_interval_id) {
    return;
  }
  if (interval_id == invalid_interval_id) {
    swap(other);
    return;
  }
  
  // Both intervals are non-empty.
  
  if (interval_id > other.interval_id) {
    swap(other);
  }
  assert(interval_id < other.interval_id);
  other.ensure_last_interval();
  
  Interval&       interval =       storage->intervals[      interval_id];
  Interval& other_interval = other.storage->intervals[other.interval_id];
  
  assert(storage->values.size() == other_interval.end);
  assert(interval.end <= other_interval.begin);
  
  if (interval.end == other_interval.begin && storage == other.storage) {
    // Happy flow, contiguous intervals.
    interval.end = other_interval.end;
    storage->intervals.erase(other.interval_id);
    other.interval_id = invalid_interval_id;
    check();
    other.check();
  } else {
    insertNonContiguous(std::move(other));
  }
}

// Intentionally not inline.
template <typename T>
void Bag<T>::insertNonContiguous(Bag&& other) {
  swap(other);
  Interval& interval       =       storage->intervals[      interval_id];    
  Interval& other_interval = other.storage->intervals[other.interval_id];
  assert(interval.end == storage->values.size());
  storage->values.reserve(storage->values.size() + other_interval.end - other_interval.begin);
  storage->values.insert(storage->values.end(),
                         other.storage->values.begin() + other_interval.begin,
                         other.storage->values.begin() + other_interval.end);
  interval.end = storage->values.size();
  check();
  other.check();
}

} // namespace impl
} // namespace fruit

#endif // FRUIT_BAG_DEFN_H
