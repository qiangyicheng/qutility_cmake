#pragma once

#include <cstdio>
#include <vector>
#include <stdexcept>
#include "../common/utilities.h"
#include "../cpu/ddr.h"

namespace qutility {
	namespace array_wrapper {

		class ArrayGPUBase {};

		class ArrayGPUSelectDevice : ArrayGPUBase {
		public:
			ArrayGPUSelectDevice() = delete;
			ArrayGPUSelectDevice(const ArrayGPUSelectDevice& rhs) : device_(rhs.device_) { qutilityGPUCheckErrors(qutilityGPUSetDevice(device_)); }
			ArrayGPUSelectDevice(ArrayGPUSelectDevice&& rhs) : device_(rhs.device_) { qutilityGPUCheckErrors(qutilityGPUSetDevice(device_)); }
			ArrayGPUSelectDevice& operator=(const ArrayGPUSelectDevice& rhs) {
				if (device_ != rhs.device_)	throw std::logic_error("Assignment can not be done between two different devices");
				qutilityGPUCheckErrors(qutilityGPUSetDevice(device_));
				return *this;
			}
			ArrayGPUSelectDevice& operator=(ArrayGPUSelectDevice&& rhs) {
				if (device_ != rhs.device_)	throw std::logic_error("Assignment can not be done between two different devices");
				qutilityGPUCheckErrors(qutilityGPUSetDevice(device_));
				return *this;
			}
			ArrayGPUSelectDevice(int device) :device_(device) { qutilityGPUCheckErrors(qutilityGPUSetDevice(device_)); }
			~ArrayGPUSelectDevice() { }
			const int device_;
		};

		//Allocate memory in GPU
		//Note that it is done through a thrust interface.
		template <class T>
		class DArrayGPU :public ArrayGPUSelectDevice {
		public:
			DArrayGPU() = delete;
			DArrayGPU(std::size_t S, int device) : ArrayGPUSelectDevice(device), size_(S), data_(S, T()), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			DArrayGPU(const T& val, std::size_t S, int device) : ArrayGPUSelectDevice(device), size_(S), data_(S, val), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			template<typename OtherT, typename OtherAlloc>
			DArrayGPU(const std::vector<OtherT, OtherAlloc>& v, int device) : ArrayGPUSelectDevice(device), size_(v.size()), data_(utilities::duplicate(v, v.size())), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			template<typename OtherT, typename OtherAlloc>
			DArrayGPU(const std::vector<OtherT, OtherAlloc>& v, std::size_t S, int device) : ArrayGPUSelectDevice(device), size_(S), data_(utilities::duplicate(v, S)), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			DArrayGPU(const DArrayGPU& rhs) :ArrayGPUSelectDevice(rhs), size_(rhs.size_), data_(rhs.data_), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			DArrayGPU(DArrayGPU&& rhs) :ArrayGPUSelectDevice(rhs), size_(rhs.size_), data_(std::move(rhs.data_)), pointer_(thrust::raw_pointer_cast(&(data_[0]))) {}
			DArrayGPU& operator=(const DArrayGPU& rhs) {
				ArrayGPUSelectDevice::operator=(rhs);
				if (size_ < rhs.size_) throw std::logic_error("Assignment can not be done from a larger array to a smaller one");
				qutilityGPUCheckErrors(qutilityGPUMemcpy(pointer_, rhs.pointer(), sizeof(T) * rhs.size_, qutilityGPUMemcpyDeviceToDevice));
				return *this;
			}
			DArrayGPU& operator=(DArrayGPU&& rhs) {
				ArrayGPUSelectDevice::operator=(rhs);
				if (size_ < rhs.size_) throw std::logic_error("Assignment can not be done from a larger array to a smaller one");
				qutilityGPUCheckErrors(qutilityGPUMemcpy(pointer_, rhs.pointer(), sizeof(T) * rhs.size_, qutilityGPUMemcpyDeviceToDevice));
				return *this;
			}


			operator T* () { return pointer_; }
			operator const T* () const { return pointer_; }
			T* operator+(size_t shift) { return pointer_ + shift; }
			const T* operator+(size_t shift) const { return pointer_ + shift; }

			const std::size_t size_;

			const T* pointer() const { return pointer_; }
			T* pointer() { return pointer_; }
			inline const auto operator[](size_t pos) const { return data_[pos]; }
			inline auto operator[](size_t pos) { return data_[pos]; }

		protected:
			thrust::device_vector<T> data_;
			T* const pointer_;
		};

		template <class T, std::size_t A>
		class DArrayDDRPinned : public DArrayDDR<T, A> {
		public:
			DArrayDDRPinned() = delete;
			DArrayDDRPinned(std::size_t S) :DArrayDDR<T, A>(S) { register_host_memory(); }
			DArrayDDRPinned(const T& val, std::size_t S) :DArrayDDR<T, A>(val, S) { register_host_memory(); }
			template<typename OtherT, typename OtherAlloc>
			DArrayDDRPinned(const std::vector<OtherT, OtherAlloc>& v) : DArrayDDR<T, A>(v, v.size()) { register_host_memory(); }
			template<typename OtherT, typename OtherAlloc>
			DArrayDDRPinned(const std::vector<OtherT, OtherAlloc>& v, std::size_t S) : DArrayDDR<T, A>(v, S) { register_host_memory(); }
			DArrayDDRPinned(const DArrayDDRPinned& rhs) : DArrayDDR<T, A>(rhs) { register_host_memory(); }
			DArrayDDRPinned(DArrayDDRPinned&& rhs) : DArrayDDR<T, A>(rhs) { register_host_memory(); }
			DArrayDDRPinned& operator=(const DArrayDDRPinned&) = default;
			DArrayDDRPinned& operator=(DArrayDDRPinned&&) = default;
			~DArrayDDRPinned() { unregister_host_memory(); }
			using DArrayDDR<T, A>::size_;
		protected:
			using DArrayDDR<T, A>::pointer_;
		private:
			void register_host_memory() { qutilityGPUCheckErrors(qutilityGPUHostRegister((void*)pointer_, size_ * sizeof(T), qutilityGPUHostRegisterDefault)); }
			void unregister_host_memory() { qutilityGPUCheckErrors(qutilityGPUHostUnregister((void*)pointer_)); }
		};

		template<
			class DstArrayT, class SrcArrayT,
			class = std::enable_if_t<
			(std::is_base_of<ArrayCPUBase, DstArrayT>::value || std::is_base_of<ArrayGPUBase, DstArrayT>::value) &&
			(std::is_base_of<ArrayCPUBase, SrcArrayT>::value || std::is_base_of<ArrayGPUBase, SrcArrayT>::value) &&
			std::is_same<
			std::decay_t<std::remove_pointer_t< decltype(std::declval<SrcArrayT>().pointer())>>,
			std::decay_t<std::remove_pointer_t< decltype(std::declval<DstArrayT>().pointer())>>
			>::value
			>
		>
			void array_copy(DstArrayT& dst, const SrcArrayT& src, size_t shift = 0) {
			using ValTy = std::decay_t<std::remove_pointer_t< decltype(std::declval<SrcArrayT>().pointer())>>;
			if (dst.size_ < (src.size_ + shift)) throw std::logic_error("array_copy can not be done from a larger array to a smaller one");
			if constexpr (std::is_base_of<ArrayCPUBase, DstArrayT>::value && std::is_base_of<ArrayCPUBase, SrcArrayT>::value)
				std::memcpy(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_);
			else if constexpr (std::is_base_of<ArrayGPUBase, DstArrayT>::value && std::is_base_of<ArrayGPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpy(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyDeviceToDevice));
			else if constexpr (std::is_base_of<ArrayCPUBase, DstArrayT>::value && std::is_base_of<ArrayGPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpy(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyDeviceToHost));
			else if constexpr (std::is_base_of<ArrayGPUBase, DstArrayT>::value && std::is_base_of<ArrayCPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpy(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyHostToDevice));
		}

		template<
			class DstArrayT, class SrcArrayT,
			class = std::enable_if_t<
			(std::is_base_of<ArrayCPUBase, DstArrayT>::value || std::is_base_of<ArrayGPUBase, DstArrayT>::value) &&
			(std::is_base_of<ArrayCPUBase, SrcArrayT>::value || std::is_base_of<ArrayGPUBase, SrcArrayT>::value) &&
			std::is_same<
			std::decay_t<std::remove_pointer_t< decltype(std::declval<SrcArrayT>().pointer())>>,
			std::decay_t<std::remove_pointer_t< decltype(std::declval<DstArrayT>().pointer())>>
			>::value
			>
		>
			void array_copy_async(DstArrayT& dst, const SrcArrayT& src, size_t shift = 0, qutilityGPUStream_t stream = nullptr) {
			using ValTy = std::decay_t<std::remove_pointer_t< decltype(std::declval<SrcArrayT>().pointer())>>;
			if (dst.size_ < (src.size_ + shift)) throw std::logic_error("array_copy can not be done from a larger array to a smaller one");
			if constexpr (std::is_base_of<ArrayCPUBase, DstArrayT>::value && std::is_base_of<ArrayCPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpyAsync(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyHostToHost, stream));
			else if constexpr (std::is_base_of<ArrayGPUBase, DstArrayT>::value && std::is_base_of<ArrayGPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpyAsync(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyDeviceToDevice, stream));
			else if constexpr (std::is_base_of<ArrayCPUBase, DstArrayT>::value && std::is_base_of<ArrayGPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpyAsync(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyDeviceToHost, stream));
			else if constexpr (std::is_base_of<ArrayGPUBase, DstArrayT>::value && std::is_base_of<ArrayCPUBase, SrcArrayT>::value)
				qutilityGPUCheckErrors(qutilityGPUMemcpyAsync(dst.pointer(), src.pointer(), sizeof(ValTy) * src.size_, qutilityGPUMemcpyHostToDevice, stream));
		}
	}
}

