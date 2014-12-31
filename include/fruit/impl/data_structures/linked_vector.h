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

#ifndef FRUIT_LINKED_VECTOR_H
#define FRUIT_LINKED_VECTOR_H

namespace fruit {
namespace impl {

/**
 * A vector where elements can be erased in O(1) time (but erasing an element doesn't actually release memory if some elements
 * after that are not yet erased).
 */
template <typename T>
class LinkedVector {
private:
  struct ValueType {
    std::size_t previous_used_pos;
    std::size_t next_used_pos;
    T value;
#ifdef FRUIT_EXTRA_DEBUG
    bool deleted;
#endif
  };
  
  std::vector<ValueType> v;
  std::size_t first_used_pos = invalid_pos;
  
  void check();
  
public:
  static constexpr std::size_t invalid_pos = ~std::size_t(0);
  
  void push_back(const T& x);
  
  T& operator[](std::size_t i);
  const T& operator[](std::size_t i) const;
  
  // Erases the element with index i and returns the index of the previous and next existing elements.
  // The previous/next indexes ase invalid_pos if the respective element does not exist.
  std::pair<std::size_t, std::size_t> erase(std::size_t i);
  
  bool empty() const;
  // This is 0 if no elements are present, and i+1 if the last element is in position i.
  // It's only an upper bound on the actual size, since deletions of non-last elements don't change this value.
  std::size_t apparentSize() const;
  
  T& back();
};

} // namespace impl
} // namespace fruit

#include "linked_vector.defn.h"

#endif // FRUIT_LINKED_VECTOR_H
