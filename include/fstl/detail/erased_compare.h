#pragma once

#ifndef FSTL_ERASED_COMPARE_H
#define FSTL_ERASED_COMPARE_H

namespace fstl::detail {
struct erased_compare_base {
  virtual bool compare_eq(const void *, const void *) = 0;
};
}

#endif //FSTL_ERASED_COMPARE_H
