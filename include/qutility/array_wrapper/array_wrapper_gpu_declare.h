#pragma once

namespace qutility
{
    namespace array_wrapper
    {
        class ArrayGPUBase;
        template <class T>
        class DArrayGPU;
        template <class T>
        using ArrayGPU = DArrayGPU<T>;
        template <class T, std::size_t A = 64>
        class DArrayDDRPinned;
        template <class T, std::size_t A = 64>
        using ArrayDDRPinned = DArrayDDRPinned<T, A>;

    }
}