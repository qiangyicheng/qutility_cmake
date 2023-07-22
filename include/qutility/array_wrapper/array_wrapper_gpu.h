#pragma once

#include "array_wrapper_gpu_declare.h"

#ifndef QUTILITY_ARRAY_WRAPPER_DISABLE_GPU_WRAPPER

#ifdef QUTILITY_ARRAY_WRAPPER_GPU_USE_CUDA
#include "internal/gpu/cuda.h"
#endif

#ifdef QUTILITY_ARRAY_WRAPPER_GPU_USE_HIP
#include "internal/gpu/hip.h"
#endif

#endif