#include "scriba/scriptinstance.h"
#include "scriba/storagemanager.h"
#include "scriba/parser.h"
#include "scriba/scanner.h"

namespace scriba {

	ScriptInstance::ScriptInstance(std::string_view source)
		: storage(std::make_unique<StorageManager>()),
		parser(std::make_unique<Parser>()),
		scanner(std::make_unique<Scanner>(source))
	{
		parser->set_storage_manager(storage.get());
	}

	ScriptInstance::~ScriptInstance() = default;

	void ScriptInstance::set_output_callback(OutputCallback callback) {
		output = std::move(callback);
	}

	void ScriptInstance::run_script(const std::string& source) {
		if (output) {
			output("Running script...");
		}

		// TODO: tokenize, parse, execute
	}
} // namespace scriba