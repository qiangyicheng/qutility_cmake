#pragma once

#include<iostream>
#include<string>

namespace qutility {
	namespace message {
		inline void exit_with_message(char const* const message, char const* const file, int const line) {
			std::cerr << "Error at " << file << ":" << line << ":\n"
				<< message << std::endl;
			exit(1);
		}

		inline void exit_with_message(const std::string & message, char const* const file, int const line) {
			std::cerr << "Error at " << file << ":" << line << ":\n"
				<< message << std::endl;
			exit(1);
		}
	}
}