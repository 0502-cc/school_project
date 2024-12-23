#pragma once
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <qstring.h>
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")

extern QString srcDirPathLog;

static std::string get_date()
{
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	std::stringstream ss;
	ss << ct.wYear << '-'
		<< ct.wMonth << "-"
		<< ct.wDay;
	return ss.str();
}

static std::string get_time()
{
	SYSTEMTIME ct;
	GetLocalTime(&ct);
	std::stringstream ss;
	ss << ct.wHour << ':'
		<< ct.wMinute << ':'
		<< ct.wSecond;
	return ss.str();
}

template <typename T>
static int to_string(std::stringstream& ss, const T &t)
{
	ss << t;
	return NULL;
}

template <typename T, typename...Args>
static int to_string(std::stringstream& ss, const T &t, const Args&...rest)
{
	ss << t;
	return to_string(ss, rest...);
}

template <typename T, typename...Args>
static int Log(const T &t, const Args&...rest)
{
	std::stringstream ss;
	to_string(ss, t, rest...);
	//std::string path = "D://Log/";
	std::string path = srcDirPathLog.toStdString();
	path += get_date();
	path += ".txt";
	std::fstream log_file;
	log_file.open(path, std::ios::out | std::ios::app);
	log_file << "[" << get_time() << "] " << ss.str() << std::endl;
	log_file.close();
	return NULL;
}

