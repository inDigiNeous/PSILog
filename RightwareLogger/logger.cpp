#include <assert.h>
#include "logger.h"

Logger::Logger() {
	std::cout << "Logger created" << std::endl;
}

Logger::~Logger() {
	std::cout << "Logger destroyed" << std::endl;
}

// Flush our output stream to our target destination
// Here we should limit the output according the current output level ?
// Actually we shouldn't even store the log messages if the filter level
// Is limiting that
void Logger::flush() {
	// Log to console for now
	//std::cout << _output_stream.str();

	// We assume that log messages are text
	// So, we dispatch the log message to our outputter
	// We should also have a default outputter if none is assigned that outputs to the console
	// And we should
	_outputter->write_log_entry(_output_stream.str());

	// Clear it
	_output_stream.str( std::string() );
	_output_stream.clear();
}

bool Logger::add_output(std::unique_ptr<LoggerOutput> output) {
	//assert(output != nullptr);
	_outputter = std::move(output);
}

void Logger::set_log_level(int log_level) {
	_log_level = log_level;
}

int Logger::get_log_level() const {
	return _log_level;
}

int Logger::get_log_filter() const {
	return _log_filter;
}

void Logger::set_log_filter(int log_filter) {
	_log_filter = log_filter;
}

// Default console output implementation
LoggerConsoleOutput::LoggerConsoleOutput() {
	fprintf(stderr, "Initialized ConsoleOutput\n");
}

LoggerConsoleOutput::~LoggerConsoleOutput() {
	fprintf(stderr, "Destroyed ConsoleOutput\n");
}

// Write the the log entry to our output
bool LoggerConsoleOutput::write_log_entry(const std::string &log_entry) {
	std::cout << log_entry;
	return true;
}
