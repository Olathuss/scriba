#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

#include "scriba/value.h"

namespace scriba {
	class Environment {
	public:
		Environment(Environment* in_parent = nullptr);

		Value get(const std::string& var_name);

		void set(const std::string& var_name, Value var_value);

		bool exists(const std::string& var_name);

		Environment make_child();

	private:
		std::unordered_map<std::string, Value> table;
		Environment* parent;

	};
} // namespace scriba