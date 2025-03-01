#include "main.h"

#include <functional>
#include <iomanip>
#include <sstream>
#include <unordered_map>

typedef std::function<void(std::ofstream&, std::string const&, json const&)> ExporterFunc;

static std::string ReadTextFile(std::string const& filepath) 
{
    std::ifstream fileStream(filepath);

    if (!fileStream.is_open())
        FATAL_ERROR("Cannot open file %s for reading.\n", filepath.c_str());

    std::string text;

    fileStream.seekg(0, std::ios::end);
    text.resize(fileStream.tellg());

    fileStream.seekg(0, std::ios::beg);
    fileStream.read(&text[0], text.size());

    fileStream.close();

    return text;
}

json ReadJsonFile(std::string const& filepath)
{
    json jsonData;
    try
    {
        jsonData = json::parse(ReadTextFile(filepath));
    }
    catch (json::exception const& e)
    {
        FATAL_ERROR("Json Error: \n@ %s\n%s", filepath.c_str(), e.what());
    }

    return jsonData;
}

void ExportTrainerData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportTrainerData_Pory(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportBattleMusicData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportQuestData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportQuestData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportQuestData_ConstsH(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportQuestData_Pory(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportNicknameData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportCustomMonData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportCustomMonData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportDecorationData_C(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);
void ExportDecorationData_H(std::ofstream& fileStream, std::string const& dataPath, json const& jsonData);


static std::map<std::string, ExporterFunc> CreateExportMap()
{
    std::map<std::string, ExporterFunc> mapping;

    mapping["trainers_c"] = ExportTrainerData_C;
    mapping["trainers_pory"] = ExportTrainerData_Pory;
    mapping["battle_music_c"] = ExportBattleMusicData_C;
    mapping["quests_c"] = ExportQuestData_C;
    mapping["quests_h"] = ExportQuestData_H;
    mapping["quest_consts_h"] = ExportQuestData_ConstsH;
    mapping["quests_pory"] = ExportQuestData_Pory;
    mapping["nicknames_c"] = ExportNicknameData_C;
    mapping["custom_mons_c"] = ExportCustomMonData_C;
    mapping["custom_mons_h"] = ExportCustomMonData_H;
    mapping["decoration_c"] = ExportDecorationData_C;
    mapping["decoration_h"] = ExportDecorationData_H;

    return mapping;
}

int main(int argc, char* argv[])
{
    std::string inputArgs = "customjson ";
    for (int i = 1; i < argc; ++i)
    {
        inputArgs += " ";
        inputArgs += argv[i];
    }

    LOG_MSG("%s", inputArgs.c_str());

    if (argc != 4)
        FATAL_ERROR("USAGE: mapjson <exporter> <input path> <output path>\n");

    std::map<std::string, ExporterFunc> validExporters = CreateExportMap();

    std::string exporter(argv[1]);
    std::string sourceFilePath(argv[2]);
    std::string exportFilePath(argv[3]);

    auto exporterIt = validExporters.find(exporter);

    if (exporterIt == validExporters.end())
    {
        LOG_MSG("Invalid exporter '%s'", exporter.c_str());
        LOG_MSG("Valid exporters:");

        for (auto it = validExporters.begin(); it != validExporters.end(); ++it)
        {
            std::string const& str = it->first;
            LOG_MSG("\t'%s'", str.c_str());
        }


        FATAL_ERROR("USAGE: mapjson <exporter> <input path> <output path>\n");
    }

    json jsonData;

    if (strutil::ends_with(sourceFilePath, ".txt"))
    {
        LOG_MSG("Skipping json parsing (Assuming alternative format)");
    }
    else
    {
        jsonData = ReadJsonFile(sourceFilePath);
    }

    std::ofstream writeStream(exportFilePath, std::ofstream::binary);
    if (!writeStream.is_open())
    {
        FATAL_ERROR("Cannot open file %s for writing.\n", exportFilePath.c_str());
    }

    writeStream << "//\n";
    writeStream << "// DO NOT EDIT THIS FILE\n";
    writeStream << "// This file was generated using Pokabbie\\CustomJson from '" << sourceFilePath << "'\n";
    writeStream << "//\n";
    writeStream << "\n";

    // Run the exporter
    exporterIt->second(writeStream, sourceFilePath, jsonData);

    writeStream.flush();
    writeStream.close();

    return 0;
}

void FatalExit()
{
    std::exit(1);
}

// Helpers
//

static std::string GetSourceDirectory(std::string path)
{
    strutil::replace_all(path, "/", "\\");
    size_t index = path.find_last_of('\\');
    path = path.substr(0, index);
    return path + '\\';
}

static json ExpandCommonArrayGroupInternal(std::string const& sourcePath, json const& rawData, std::string const& groupName, int* counter)
{
    json outputData;
    json& outputGroup = outputData[groupName] = json::object();

    std::string condition = "";

    if (rawData.contains("condition"))
        condition = rawData["condition"].get<std::string>();

    // Expand source into output
    json inputGroups = rawData[groupName];

    for (auto inputGroupIt = inputGroups.begin(); inputGroupIt != inputGroups.end(); ++inputGroupIt)
    {
        std::string groupName = inputGroupIt.key();
        json const& sourceGroups = inputGroupIt.value();

        if (!condition.empty())
        {
            groupName = "#if " + condition + " // " + groupName;
        }

        json& currentOutputGroup = outputGroup[groupName] = json::array();

        for (auto groupIt = sourceGroups.begin(); groupIt != sourceGroups.end(); ++groupIt)
        {
            json destGroup;
            json const& sourceGroup = groupIt.value();

            if (rawData.contains("defaults"))
            {
                json defaults = rawData["defaults"];

                destGroup = defaults;
            }

            for (auto kvpIt = sourceGroup.begin(); kvpIt != sourceGroup.end(); ++kvpIt)
            {
                destGroup[kvpIt.key()] = kvpIt.value();
            }

            currentOutputGroup.push_back(destGroup);
        }
    }

    // Process includes
    if (rawData.contains("includes"))
    {
        json includes = rawData["includes"];
        std::string sourceDir = GetSourceDirectory(sourcePath);

        for (auto it = includes.begin(); it != includes.end(); ++it)
        {
            std::string fullPath = sourceDir + it.value().get<std::string>();
            strutil::replace_all(fullPath, "\\", "/");

            json parsedInclude = ExpandCommonArrayGroupInternal(fullPath, ReadJsonFile(fullPath), groupName, counter);
            json parsedInnerGroups = parsedInclude[groupName];

            for (auto inputGroupIt = parsedInnerGroups.begin(); inputGroupIt != parsedInnerGroups.end(); ++inputGroupIt)
            {
                std::string sourceName = inputGroupIt.key();
                json const& sourceGroups = inputGroupIt.value();

                // Prepend a number so we maintain the order they're included elsewhere in the code
                std::stringstream ss;
                ss << std::setw(6) << std::setfill('0') << (*counter)++;
                std::string prefixString = ss.str() + "_";

                outputGroup[prefixString + sourceName + " // [" + fullPath + "]"] = sourceGroups;
            }
        }
    }

    return outputData;
}

json ExpandCommonArrayGroup(std::string const& sourcePath, json const& rawData, std::string const& groupName)
{
    int counter = 0;
    return ExpandCommonArrayGroupInternal(sourcePath, rawData, groupName, &counter);
}