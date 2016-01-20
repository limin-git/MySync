#pragma once

typedef boost::filesystem::path Path;
typedef std::pair<long, std::time_t> Key;
typedef std::set<Path> PathSet;
typedef std::map<Key, PathSet> KeyPathMap;
typedef std::map<Path, Key> PathKeyMap;
