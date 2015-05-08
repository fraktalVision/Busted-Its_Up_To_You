#pragma once
//ini file loader

#include <vector>
#include <map>

class DataDrive
{
public:
	DataDrive();

	//!returns data based on the context string
	float GetData(const std::string dataString);

	//!reloads all data from files
	void RefreshData();

private:

	//!map structure holding the context and the string
	std::map<std::string,float> m_dataMap;

	//!enters the data into the context map
	void AddDataContext(const std::string dataString, float dataVal);

	//!accesses the independent files from the seperate ini files
	std::vector<std::string> GetFileNamesFromMasterIni(const std::string file_name);

	//!read all the data and contexts out of a file
	void ScanFileForData(const std::string file_name);

};