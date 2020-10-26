// FastPass
#ifdef _MSC_VER
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#endif

#include <ATen/ScalarOps.h>
#include <ATen/ATen.h>

namespace at {

// When filling a number to 1-element CPU tensor, we want to skip
// everything but manipulate data ptr directly.
// Ideally this fast pass should be implemented in TensorIterator,
// but we also want to skip compute_types which in not avoidable
// in TensorIterator for now.
Tensor& scalar_fill(Tensor& self, Scalar value) {
  AT_DISPATCH_ALL_TYPES_AND3(kHalf, kBool, kBFloat16, self.scalar_type(), "fill_out", [&]() {
    fill_inplace<scalar_t>(self, value);});
  return self;
}

Tensor scalar_tensor_static(Scalar s, const TensorOptions& options) {
  TORCH_CHECK(options.device() == kCPU);
  at::tracer::impl::NoTracerDispatchMode tracer_guard;
  at::AutoNonVariableTypeMode non_var_type_mode(true);
  auto result = native::empty_cpu({}, options);
  scalar_fill(result, s);
  return result;
}
} //namespace at
