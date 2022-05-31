#pragma once

namespace qutility {
    namespace array_wrapper {

        template <typename T>
        inline void hipCheck(T result, char const* const func, const char* const file,
            int const line) {
            if (result) {
                fprintf(stderr, "HIP error at %s:%d code=%d \"%s\" \n", file, line,
                    static_cast<unsigned int>(result), func);
                exit(EXIT_FAILURE);
            }
        }
    }
}