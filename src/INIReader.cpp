// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <utility>
#include <set>

#include <ini.h>
#include <INIReader.h>

using std::string;

INIReader::INIReader(const string &filename) {
  _error = ini_parse(filename.c_str(), ValueHandler, this);
}

INIReader::~INIReader() {
  // Clean up the field sets
  std::map<std::string, std::set<std::string> *>::iterator fieldSetsIt;
  for (fieldSetsIt = _fields.begin(); fieldSetsIt != _fields.end(); ++fieldSetsIt)
    delete fieldSetsIt->second;
}

int INIReader::ParseError() const {
  return _error;
}

string INIReader::Get(const string &section, const string &name, const string &default_value) {
  string key = MakeKey(section, name);
  return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(const string &section, const string &name, long default_value) {
  string valstr = Get(section, name, "");
  const char *value = valstr.c_str();
  char *end;
  // This parses "1234" (decimal) and also "0x4D2" (hex)
  long n = strtol(value, &end, 0);
  return end > value ? n : default_value;
}

double INIReader::GetReal(const string &section, const string &name, double default_value) {
  string valstr = Get(section, name, "");
  const char *value = valstr.c_str();
  char *end;
  double n = strtod(value, &end);
  return end > value ? n : default_value;
}

bool INIReader::GetBoolean(const string &section, const string &name, bool default_value) {
  string valstr = Get(section, name, "");
  // Convert to lower case to make string comparisons case-insensitive
  std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
  if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
    return true;
  else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
    return false;
  else
    return default_value;
}

std::set<std::string> INIReader::GetSections() const {
  return _sections;
}

std::set<std::string> INIReader::GetFields(std::string section) const {
  string sectionKey = std::move(section);
  std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);
  auto fieldSetIt = _fields.find(sectionKey);
  if (fieldSetIt == _fields.end())
    return std::set<std::string>();
  return *(fieldSetIt->second);
}

string INIReader::MakeKey(const string &section, const string &name) {
  string key = section + "." + name;
  // Convert to lower case to make section/name lookups case-insensitive
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  return key;
}

int INIReader::ValueHandler(void *user, const char *section, const char *name,
                            const char *value) {
  auto *reader = (INIReader *) user;

  // Add the value to the lookup map
  string key = MakeKey(section, name);
  if (!reader->_values[key].empty())
    reader->_values[key] += "\n";
  reader->_values[key] += value;

  // Insert the section in the sections set
  reader->_sections.insert(section);

  // Add the value to the values set
  string sectionKey = section;
  std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);

  std::set<std::string> *fieldsSet;
  auto fieldSetIt = reader->_fields.find(sectionKey);
  if (fieldSetIt == reader->_fields.end()) {
    fieldsSet = new std::set<std::string>();
    reader->_fields.insert(std::pair<std::string, std::set<std::string> *>(sectionKey, fieldsSet));
  } else {
    fieldsSet = fieldSetIt->second;
  }
  fieldsSet->insert(name);

  return 1;
}
