#pragma once

#ifdef QUTILITY_ARRAY_WRAPPER_GPU_CUDA
#define qutilityGPUCheckErrors(val) checkCudaErrors(val)
#define qutilityGPUSetDevice cudaSetDevice
#define qutilityGPUMemcpy cudaMemcpy
#define qutilityGPUMemcpyAsync cudaMemcpyAsync
#define qutilityGPUMemcpyDeviceToDevice cudaMemcpyDeviceToDevice
#define qutilityGPUMemcpyHostToDevice cudaMemcpyHostToDevice
#define qutilityGPUMemcpyDeviceToHost cudaMemcpyDeviceToHost
#define qutilityGPUMemcpyHostToHost cudaMemcpyHostToHost
#define qutilityGPUHostRegister cudaHostRegister
#define qutilityGPUHostUnregister cudaHostUnregister
#define qutilityGPUHostRegisterDefault cudaHostRegisterDefault
#define qutilityGPUStream_t cudaStream_t

#endif // QUTILITY_ARRAY_WRAPPER_GPU_CUDA

#ifdef QUTILITY_ARRAY_WRAPPER_GPU_HIP
#define qutilityGPUCheckErrors(val) ::qutility::array_wrapper::hipCheck((val), #val, __FILE__, __LINE__)
#define qutilityGPUSetDevice hipSetDevice
#define qutilityGPUMemcpy hipMemcpy
#define qutilityGPUMemcpyAsync hipMemcpyAsync
#define qutilityGPUMemcpyDeviceToDevice hipMemcpyDeviceToDevice
#define qutilityGPUMemcpyHostToDevice hipMemcpyHostToDevice
#define qutilityGPUMemcpyDeviceToHost hipMemcpyDeviceToHost
#define qutilityGPUMemcpyHostToHost hipMemcpyHostToHost
#define qutilityGPUHostRegister hipHostRegister
#define qutilityGPUHostUnregister hipHostUnregister
#define qutilityGPUHostRegisterDefault hipHostRegisterDefault
#define qutilityGPUStream_t hipStream_t

#endif // QUTILITY_ARRAY_WRAPPER_GPU_HIP
