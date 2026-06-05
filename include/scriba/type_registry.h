#include <typeindex>
#include "scriba/object.h"

namespace scriba {
	class TypeRegistry {
		std::unordered_map<std::type_index, ObjectTypeInfo> table;

	public:
		template<typename T>
		ObjectTypeInfo& get() {
			auto key = std::type_index(typeid(T));
			return table.at(key);
		}

		template<typename T>
		ObjectTypeInfo& ensure_type(std::string_view name) {
			auto key = std::type_index(typeid(T));
			auto it = table.find(key);
			if (it != table.end()) {
				return it->second;
			}

			ObjectTypeInfo info;
			info.name = std::string(name);
			table.emplace(key, std::move(info));
			return table.at(key);
		}
	};
} // namespace scriba