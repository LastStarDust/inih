// Read an INI file into easy-to-access name/value pairs.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include "ini.h"
#include "INIReader.h"

using std::string;

static int cmpistr(const char* s1, const char* s2);

INIReader::INIReader(string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

int INIReader::ParseError()
{
    return _error;
}

string INIReader::Get(string section, string name, string default_value)
{
    string key = MakeKey(section, name);
    return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(string section, string name, long default_value)
{
    string valstr = Get(section, name, "");
    const char* value = valstr.c_str();
    char* end;
    // This parses "1234" (decimal) and also "0x4D2" (hex)
    long n = strtol(value, &end, 0);
    return end > value ? n : default_value;
}

bool INIReader::GetBoolean(string section, string name, bool default_value)
{
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

string INIReader::MakeKey(string section, string name)
{
    string key = section + "." + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
                            const char* value)
{
    INIReader* reader = (INIReader*)user;
    reader->_values[MakeKey(section, name)] = value;
    return 1;
}

int cmpistr(const char* s1, const char* s2)
{
    int c1, c2;
    int cmp = 0;

    if (!s1 || !s2)
        return 2;

    while (1)
    {
        c1 = *s1++;
        c2 = *s2++;
        if (c1 && c2)
        {
            c1 = tolower(c1) & 0xFF;
            c2 = tolower(c2) & 0xFF;
            if (c1 < c2)
            {
                cmp = -1;
                break;
            }
            else if (c1 > c2)
            {
                cmp = 1;
                break;
            }
        }
        else
        {
            if (c1)
                cmp = 1;
            else if (c2)
                cmp = -1;
            break;
        }
    }

    return cmp;
}
