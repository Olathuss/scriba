#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

namespace scriba {
	class Value;

	class Environment {
		std::unordered_map<std::string, Value> table;

	public:
		Value get(std::string var_name);

		void set(std::string var_name, Value var_value);

		bool exists(std::string var_name) {
			auto it = table.find(var_name);
			if (it != table.end()) {
				return true;
			}
			return false;
		}
	};
} // namespace scriba