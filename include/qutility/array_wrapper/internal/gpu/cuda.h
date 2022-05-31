#pragma once

#define QUTILITY_ARRAY_WRAPPER_GPU_CUDA
#include "cuda_runtime.h"
#include "helper_cuda.h"
#include "thrust/device_vector.h"
#include "defs.h"
#include "base.h"
#undef QUTILITY_ARRAY_WRAPPER_GPU_CUDA