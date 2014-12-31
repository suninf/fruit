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

#ifndef FRUIT_BAG_H
#define FRUIT_BAG_H

#include "linked_vector.h"

namespace fruit {
namespace impl {

/**
 * An unordered bag (multiset).
 */
template <typename T>
class Bag {
private:
  struct Interval {
    std::size_t begin;
    std::size_t end;
  };
  
  using IntervalId = std::size_t;
  static constexpr std::size_t invalid_interval_id = LinkedVector<Interval>::invalid_pos;
  
  struct Storage {
    LinkedVector<Interval> intervals;
    std::vector<T> values;
  };
  
  Storage* storage;
  // The ID of the interval containing the values in Storage, or invalid_interval_id if the interval is empty.
  IntervalId interval_id;
  
  static Storage* getStorage();
  
  Bag(Storage* storage);
  
  void ensure_last_interval();
  
  template <typename Iter>
  Bag(Storage* storage, Iter first, Iter last);
  
  void check();
  
  void insertNonContiguous(const T& x);
  void insertNonContiguous(Bag&& other);
  
public:
  Bag();
  
  Bag(Bag&& other);
  Bag(const Bag&) = delete;
  
  Bag& operator=(Bag&& other);
  Bag& operator=(const Bag&) = delete;
  
  ~Bag();
  
  T* begin();
  T* end();
  
  void insert(const T& x);
  void insert(Bag&& other);
  
  void swap(Bag& other);
};

} // namespace impl
} // namespace fruit

#include "bag.defn.h"

#endif // FRUIT_BAG_H
