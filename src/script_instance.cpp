#include "scriba/script_instance.h"
#include "scriba/storage_manager.h"
#include "scriba/parser.h"
#include "scriba/scanner.h"
#include "scriba/environment.h"
#include "scriba/evaluator.h"

#include <exception>

namespace scriba {
	ScriptInstance::ScriptInstance(Environment in_env, TypeRegistry in_reg)
	{
		environment = in_env;
		registry = in_reg;
	}

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

		EventBlock& event = it->second;

		Environment local = environment.make_child();

		for (size_t i = 0; i < event.parameters.size(); ++i) {
			if (i < args.size())
				local.set(event.parameters[i].lexeme, args[i]);
			else
				local.set(event.parameters[i].lexeme, Value::null());
		}

		Evaluator ev = Evaluator(registry, local);
		try {
			ev.execute_block(event.statements);
		}
		catch (const std::exception& e) {
			error = e.what();

			return false;
		}

		return true;
	}
} // namespace scriba