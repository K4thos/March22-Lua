#include "FileOp.h"

#include <codecvt>
#include <algorithm>
#include <fstream>
#include <M22toLua.h>
#include <Label.h>

int LoadTXTIntoVector(const char* _file, std::vector<std::wstring>& _vector)
{
	std::wifstream inputFile;
	inputFile.open(_file);

	if (!inputFile) {
		printf("Failed to load: %s\n", _file);
		return -1;
	};

	std::wstring line;
	while (std::getline(inputFile, line))
	{
		if (line.size() >= 2 && line.at(0) == L'/' && line.at(1) == L'/')
		{

		}
		else
		{
			_vector.push_back(line);
		}
	};

	inputFile.close();
	return 0;
};

int LoadScriptIntoVector(const wchar_t* _input, std::vector<std::wstring>& _vector)
{
	// Load file
	std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>);
	std::wifstream inputFile;
	inputFile.open(_input);
	inputFile.imbue(ulocale);

	// Check file
	if (!inputFile) {
		wprintf(L"Failed to load: %s\n", _input);
		return -1;
	};

	// Load meaningful lines into vector
	std::wstring line;
	while (std::getline(inputFile, line))
	{
		if (line != L"") {
			//std::wstringstream iss(line);
			//::wstring item;
			//iss >> item;

			_vector.push_back(line);
		};
	};
	inputFile.seekg(0, inputFile.beg);

	// Cleanup
	inputFile.close();
	return 0;
};

unsigned int SplitString(const std::wstring &txt, std::vector<std::wstring> &strs, char ch)
{
	unsigned int pos = txt.find(ch);
	unsigned int initialPos = 0;
	strs.clear();

	// Decompose statement
	while (pos != std::string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;

		pos = txt.find(ch, initialPos);
	}

	// Add the last one
	strs.push_back(txt.substr(initialPos, std::min(pos, txt.size()) - initialPos + 1));

	for (size_t i = 0; i < strs.size(); i++)
	{
		strs.at(i).erase(std::remove_if(strs.at(i).begin(), strs.at(i).end(), iswspace), strs.at(i).end());
	}

	return strs.size();
};

void WriteLabelIndexFile(void)
{
	std::wofstream textOutput(L"./output/LabelIndex.lua");
	textOutput.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff>));

	if (textOutput)
	{
		textOutput << L"-- This file was automatically generated by M22toLua v" << VERSION_MAJOR << L"." << VERSION_MINOR << L"." << VERSION_PATCH;
		textOutput << L"\n-- For use with March22-Lua\n-- By Sam Lynch\n\n";
		textOutput << L"LABELINDEX = {};\n\n";
		for (size_t i = 0; i < Label2::LABEL_INDEX.size(); i++)
		{
			myReplace(Label2::LABEL_INDEX.at(i).script_name, L".rpy", L"");
			myReplace(Label2::LABEL_INDEX.at(i).script_name, L".lua", L""); // just in case
			Label2::LABEL_INDEX.at(i).script_name += L".lua";
			textOutput << L"LABELINDEX[\"" << Label2::LABEL_INDEX.at(i).label_name << L"\"] = \"" << Label2::LABEL_INDEX.at(i).script_name << "\";\n";
		}
		textOutput.close();
	}
	else
	{
		printf("Failed to create index file!\n");
	}

	return;
}

// http://stackoverflow.com/questions/1494399/how-do-i-search-find-and-replace-in-a-standard-string
void myReplace(std::wstring& str,
	const std::wstring& oldStr,
	const std::wstring& newStr)
{
	std::wstring::size_type pos = 0u;
	while ((pos = str.find(oldStr, pos)) != std::wstring::npos) {
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}