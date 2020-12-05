// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// http://code.google.com/p/inih/

#ifndef __INIREADER_H__
#define __INIREADER_H__

#include <map>
#include <string>
#include <vector>
#include <set>

// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader {
public:
  // Construct INIReader and parse given filename. See ini.h for more info
  // about the parsing.
  INIReader(const std::string& filename); // NOLINT(google-explicit-constructor)
  ~INIReader();

  // Return the result of ini_parse(), i.e., 0 on success, line number of
  // first error on parse error, or -1 on file open error.
  int ParseError() const;

  // Get a string value from INI file, returning default_value if not found.
  std::string Get(const std::string& section, const std::string& name,
                  const std::string& default_value);

  // Get an integer (long) value from INI file, returning default_value if
  // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
  long GetInteger(const std::string& section, const std::string& name, long default_value);

  // Get a real (floating point double) value from INI file, returning
  // default_value if not found or not a valid floating point value
  // according to strtod().
  double GetReal(const std::string& section, const std::string& name, double default_value);

  // Get a boolean value from INI file, returning default_value if not found or if
  // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
  // and valid false values are "false", "no", "off", "0" (not case sensitive).
  bool GetBoolean(const std::string& section, const std::string& name, bool default_value);

  // Returns all the sections (groups) from the associated INI file
  std::set<std::string> GetSections() const;

  // Return all the fields of a section
  std::set<std::string> GetFields(std::string section) const;

private:
  int _error;
  std::set<std::string> _sections;
  // Because we want to retain the original casing in _fields, but
  // want lookups to be case-insensitive, we need both _fields and _values
  std::map<std::string, std::set<std::string> *> _fields;
  std::map<std::string, std::string> _values;

  static std::string MakeKey(const std::string& section, const std::string& name);

  static int ValueHandler(void *user, const char *section, const char *name,
                          const char *value);
};

#endif  // __INIREADER_H__
