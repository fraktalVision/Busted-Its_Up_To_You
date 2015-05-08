#include "../../core/all.h"
#include "datadrive.h"

//DataDrive* DATA_MAN;

#define	FILEDATASIZEMAX 1024

/*
DataDrive Constructor
Author Jamie Gault
*/
DataDrive::DataDrive()
{}

/*
DataDrive RefreshData
Author Jamie Gault
*/
void DataDrive::RefreshData()
{
	m_dataMap.clear();
	std::vector<std::string> fileNames = GetFileNamesFromMasterIni("../Busted/assets/datadrive/main.ini");
	for(unsigned int i=0; i<fileNames.size(); ++i)
	{
		ScanFileForData(fileNames[i]);
	}
}

/*
DataDrive GetData
Author Jamie Gault
*/
float DataDrive::GetData(const std::string dataString)
{
	if(m_dataMap.count(dataString) == 0)
	{
		if(DATA("debug")) { std::cout<<"Error: DataDrive '"<<dataString.c_str()<<"' does not exist.  Returned 1.0f.\n"; }
		return 1.0f;
	}
	else
	{
		return m_dataMap[dataString];
	}
}

/*
DataDrive AddDataContext
Author Jamie Gault
*/
void DataDrive::AddDataContext(const std::string dataString, float dataVal)
{
	m_dataMap[dataString] = dataVal;
}

/*
DataDrive GetFileNamesFromMasterIni
Author Jamie Gault
*/
std::vector<std::string> DataDrive::GetFileNamesFromMasterIni(const std::string fileName)
{
	std::vector<std::string> fileNames;
	std::ifstream inputFile;
	inputFile.open(fileName.c_str());

	//if the main input file failed ot load
	if(!inputFile)
	{
		throw std::runtime_error("Could not open master ini file");
	}
	else
	{
		char buffer[FILEDATASIZEMAX]; //rediculously sized buffer

		while(!inputFile.eof())
		{
			inputFile.getline(buffer,FILEDATASIZEMAX,'\n');
			fileNames.push_back(std::string(buffer));
		}
		inputFile.close();
	}

	if(fileNames[fileNames.size()-1] == "")
		fileNames.pop_back();
	return fileNames;
}

/*
DataDrive ScanFileForData
Author Jamie Gault
*/
void DataDrive::ScanFileForData(const std::string file_name)
{
	std::string realFileName = std::string("../Busted/assets/datadrive/")+file_name;
	std::ifstream inputFile;
	inputFile.open(realFileName.c_str());
	if(!inputFile)
	{
		printf("Error: could not load data file '%s'\n",file_name.c_str());
		return;
	}

	char buffer[FILEDATASIZEMAX];

	while(!inputFile.eof())
	{
		inputFile.getline(buffer,FILEDATASIZEMAX,'\n');
		std::stringstream myStream(buffer);
		std::string firstPart;
		myStream >> firstPart;

		//check for __VAR key word
		if(firstPart == "__VAR")
		{
			myStream >> firstPart;
			float varVal;
			myStream >> varVal;
			AddDataContext(firstPart,varVal);
		}
	}

	inputFile.close();
}