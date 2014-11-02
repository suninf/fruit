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

#ifndef FRUIT_METAPROGRAMMING_SET_H
#define FRUIT_METAPROGRAMMING_SET_H

#include "list.h"

#include <boost/mpl/fold.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/insert.hpp>

namespace fruit {
namespace impl {

using namespace boost::mpl;

// Converts a List to an mpl::set.
// O(size<L>)
struct ListToSet {
  template <typename L>
  struct apply {
    using type = Eval<fold<Apply<ToMplVector, L>,
                           set<>,
                           insert<_1, _2>>>;
  };
};

// O(size<S1>)
struct SetUnion {
  template <typename S1, typename S2>
  struct apply {
    using type = Eval<fold<S1,
                           S2,
                           insert<_1, _2>>>;
  };
};

} // namespace impl
} // namespace fruit


#endif // FRUIT_METAPROGRAMMING_SET_H
