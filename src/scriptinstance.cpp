#include "scriba/scriptinstance.h"
#include "scriba/storagemanager.h"
#include "scriba/parser.h"
#include "scriba/scanner.h"
#include "scriba/environment.h"

#include <exception>

namespace scriba {
	ScriptInstance::ScriptInstance() = default;

	ScriptInstance::~ScriptInstance() = default;

	void ScriptInstance::set_output_callback(OutputCallback callback) {
		output = std::move(callback);
	}

	bool ScriptInstance::load_script(const std::string& source)
	{
		events.clear();
		event_order.clear();
		error.clear();

		try {
			Scanner scanner(source);
			auto tokens = scanner.scan_tokens();

			std::unique_ptr<StorageManager> storage = std::make_unique<StorageManager>();
			for (auto token : tokens) storage->add_token(token);

			Parser parser;
			parser.set_storage_manager(storage.get());
			parser.parse_script();

			events = parser.get_events();
			event_order = parser.get_event_names();
			return true;
		}
		catch (const std::exception& e) {
			error = e.what();
			events.clear();
			event_order.clear();

			return false;
		}
	}

	bool ScriptInstance::trigger_event(const std::string& name, const std::vector<Value>& args)
	{
		auto it = events.find(name);
		if (it == events.end()) {
			return false;
		}

		Environment env;
		EventBlock& event = it->second;

		for (int i = 0; i < event.arguments.size(); ++i) {
			if (i >= args.size()) {
				env.set(event.arguments[i].lexeme, Value(std::monostate()));
				continue;
			}
			env.set(event.arguments[i].lexeme, args[i]);
		}

		return true;
	}
} // namespace scriba