#pragma once

#include <vector>
#include <cstddef>
#include "scriba/token.h"

namespace scriba {
	class StorageManager {

	public:
		StorageManager();

		void clear();
		void add_token(const Token& token);

		const Token& peek() const;
		const Token& peek_next() const;
		Token advance();

		bool is_at_end() const;
		const Token& previous() const;

		bool check(TokenType type) { return peek().type == type; };

	private:
		std::vector<Token> tokens;
		std::size_t index = 0;
	};
}