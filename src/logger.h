// PSILog.h
//
// Class for handling logging, with support for multiple outputs, thread safety and modular extensions of
// user configurable output destinations
//
// Copyright (c) 2018 Sakari Lehtonen <sakari AT psitriangle DOT net>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <stdio.h>

using std::unique_ptr;
using std::make_unique;
using std::move;

class PSILogOutput;
class PSILogConsoleOutput;
class PSILogStream;

// Our main logger class
class PSILog {

public:
	// Binary arithmetic current log level mask
        enum LogLevel {
		NONE	=  0,
		INFO	=  1,
		WARN	=  2,
		ERR	=  2 << 1,
		FREQ	=  2 << 2,
		ALL	= (2 << 3) - 1
        };

	PSILog() = default;
	~PSILog() = default;

	// Functors for returning a log stream, enabling multithreading safe logging
	PSILogStream operator ()();
	PSILogStream operator ()(int log_level);

	// The main logging method
	void log(const std::string &entry, int log_level);

	// Return the log message prefix header
	std::string get_log_entry_prefix(const std::string &log_entry) const;

	// Add new logger to our output chain
	// We have multiple output destinations which implement the actual writing of the messages
	// This enables easy extending of log destinations by the user
	void add_output(unique_ptr<PSILogOutput> output);

	// Flush all output now to the destination outputs
	void flush();

	// Pure accessors written here for easier implementation
	int get_level() const { return _level; }
	void set_level(int level) { _level = level; }

	int get_filter() const { return _filter; }
	void set_filter(int filter) { _filter = filter; }

	bool get_add_prefix() const { return _add_prefix; }
	void set_add_prefix(bool add_prefix) { _add_prefix = add_prefix; }

private:
	// The current log level we are logging messages with
	int _level = LogLevel::INFO;

	// The log filter that filters the output, compared against the current
	// log level. Binary arithmetic mask.
	int _filter = LogLevel::INFO;

	// Do we add the log message prefix to our entries ?
	bool _add_prefix = true;

	// Our log message outputters chain
	// We dispatch the actual log messages to these in sequential order
	std::vector<unique_ptr<PSILogOutput>> _outputs;
};

// Stream class for thread safety
// Temporary instance of this class is returned when
// logger() << "Log entry" << std::endl;
// Or the << operation is called
// Meaning the calling thread gets a temporary LogStream object
// And when in the calling thread the object is destroyed, it actually logs the log messages
// This enables thread safe log message construction, without multiple threads intefering
// with each other
class PSILogStream : public std::ostringstream {
public:
	// Store reference to the current log level and logger object
	PSILogStream(PSILog &log, int log_level) :
		_log(log), _log_level(log_level)
	{}

	// Copy constructor
	PSILogStream(const PSILogStream &ls) :
		_log(ls._log),
		_log_level(ls._log_level)
	{}

	~PSILogStream() {
		// Filter log messages with the binary arithmetic mask
		if (_log.get_filter() & _log_level) {
			_log.log(str(), _log_level);
		}
	}

private:
	PSILog &_log;
	int _log_level;
};

// The logger outputs to PSILogOutput objects

// Base class for implementing logger outputs
// This allows modular extension of the outputs by the user,
// Just provide an implementatin extending this class, and call add_output()
class PSILogOutput {
public:
	PSILogOutput() = default;
	~PSILogOutput() = default;

	// This will write the current log entry to the destination output, ensuring that
	// the output is flushed also
	virtual bool write_log_entry(const std::string &log_entry, int log_level) = 0;

	// Provide a way to implement flushing the output manually
	virtual void flush() = 0;
};

// Default implementation of outputting log messages to the console
class PSILogConsoleOutput : public PSILogOutput {
public:
	PSILogConsoleOutput() = default;
	~PSILogConsoleOutput() = default;

	bool write_log_entry(const std::string &log_entry, int log_level) override;
	void flush() override;
};

// Default implementation of outputting to a file
class PSILogFileOutput : public PSILogOutput {
public:
	PSILogFileOutput(const char *output_path);
	~PSILogFileOutput();

	bool write_log_entry(const std::string &log_entry, int log_level) override;
	void flush() override;

private:
	const char *_output_path = "";
	std::fstream _fs;
	std::mutex _mutex;
};
