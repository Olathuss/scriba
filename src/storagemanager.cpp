#include "scriba/storagemanager.h"

namespace scriba {
	StorageManager::StorageManager()
		: index(0) {

	}

	void StorageManager::clear() {
		tokens.clear();
		index = 0;
	}

	void StorageManager::add_token(const Token& token) {
		tokens.push_back(token);
	}

	const Token& StorageManager::peek() const
	{
		if (index >= tokens.size()) {
			return tokens.back();
		}

		return tokens[index];
	}

	const Token& StorageManager::peek_next() const
	{
		if (index + 1 >= tokens.size()) {
			return tokens.back();
		}

		return tokens[index + 1];
	}

	bool StorageManager::is_at_end() const {
		return tokens[index].type == TokenType::END_OF_FILE;
	}

	Token StorageManager::advance() {
		if (!is_at_end()) index++;
		return tokens[index - 1];
	}

	const Token& StorageManager::previous() const {
		return tokens[index - 1];
	}
} // namespace scriba