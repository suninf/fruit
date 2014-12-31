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

#ifndef FRUIT_LINKED_VECTOR_DEFN_H
#define FRUIT_LINKED_VECTOR_DEFN_H

namespace fruit {
namespace impl {

// TODO: Remove, this is O(n).
template <typename T>
inline void LinkedVector<T>::check() {
#ifdef FRUIT_EXTRA_DEBUG
  std::size_t last_used_pos = invalid_pos;
  for (std::size_t i = 0; i < v.size(); ++i) {
    if (!v[i].deleted) {
      assert(v[i].previous_used_pos == last_used_pos);
      last_used_pos = i;
    }
  }
  std::size_t first_used_pos = invalid_pos;
  for (std::size_t i = v.size(); i > 0; --i) {
    if (!v[i-1].deleted) {
      assert(v[i-1].next_used_pos == first_used_pos);
      first_used_pos = i-1;
    }
  }
  assert(this->first_used_pos == first_used_pos);
#endif
}

template <typename T>
inline void LinkedVector<T>::push_back(const T& x) {
  check();
  std::size_t last_pos;
  if (v.empty()) {
    last_pos = invalid_pos;
    first_used_pos = 0;
  } else {
    v.back().next_used_pos = v.size();
    last_pos = v.size() - 1;
  }
  v.push_back(ValueType{last_pos, invalid_pos, x
#ifdef FRUIT_EXTRA_DEBUG
    , false /* deleted */
#endif    
  });
  check();
}

template <typename T>
inline T& LinkedVector<T>::operator[](std::size_t i) {
  assert(i < v.size());
#ifdef FRUIT_EXTRA_DEBUG
  assert(!v[i].deleted);
#endif
  return v[i].value;
}

template <typename T>
inline const T& LinkedVector<T>::operator[](std::size_t i) const {
  assert(i < v.size());
#ifdef FRUIT_EXTRA_DEBUG
  assert(!v[i].deleted);
#endif
  return v[i].value;
}

template <typename T>
inline std::pair<std::size_t, std::size_t> LinkedVector<T>::erase(std::size_t i) {
  check();
  assert(i < v.size());
#ifdef FRUIT_EXTRA_DEBUG
  assert(!v[i].deleted);
  v[i].deleted = true;
#endif
  std::size_t previous_used_pos = v[i].previous_used_pos;
  std::size_t next_used_pos = v[i].next_used_pos;
  if (previous_used_pos == invalid_pos) {
    first_used_pos = next_used_pos;
  } else {
    v[previous_used_pos].next_used_pos = next_used_pos;
  }
  if (next_used_pos == invalid_pos) {
    // Actually erase the elements [previous_used_pos+1,end) (or [begin,end) if previous_used_pos==invalid_pos).
    if (first_used_pos == invalid_pos) {
#ifdef FRUIT_EXTRA_DEBUG
      for (auto i = v.begin(), i_end = v.end(); i != i_end; ++i) {
        assert(i->deleted);
      }
#endif
      v.clear();
    } else {
#ifdef FRUIT_EXTRA_DEBUG
      for (auto i = v.begin() + previous_used_pos + 1, i_end = v.end(); i != i_end; ++i) {
        assert(i->deleted);
      }
#endif
      v.erase(v.begin() + previous_used_pos + 1, v.end());
    }
#ifdef FRUIT_EXTRA_DEBUG
    assert(v.empty() || !v.back().deleted);
#endif
  } else {
    v[next_used_pos].previous_used_pos = previous_used_pos;
  }
  check();
  return std::make_pair(previous_used_pos, next_used_pos);
}

template <typename T>
inline bool LinkedVector<T>::empty() const {
  return v.empty();
}

template <typename T>
inline std::size_t LinkedVector<T>::apparentSize() const {
  return v.size();
}

} // namespace impl
} // namespace fruit

#endif // FRUIT_LINKED_VECTOR_DEFN_H
