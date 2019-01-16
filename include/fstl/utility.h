#pragma once

#ifndef FSTL_UTILITY_H
#define FSTL_UTILITY_H

namespace fstl {
template<class First, class Second>
struct pair {
  pair() = default;
  pair(const First &fst, const Second &snd) : first(fst), second(snd) {}
  First first {};
  Second second {};
};
}

#endif //FSTL_UTILITY_H
