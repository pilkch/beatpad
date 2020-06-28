#include <cstring>

#include <iostream>
#include <fstream>
#include <list>
#include <string>

#include <experimental/filesystem>

namespace beatpad {

const std::string sVersion = "0.9";
const std::string sVersionDateTime = "2020/06/28 10:10PM";

void PrintVersion()
{
  std::cout<<"create-index-html v"<<sVersion<<", "<<sVersionDateTime<<std::endl;
}

void PrintUsage()
{
  std::cout<<"Usage:"<<std::endl;
  std::cout<<"create-index-html [-v|--v|--version] [-h|--h|--help]"<<std::endl;
  std::cout<<"-v|--v|--version:\tPrint the version information"<<std::endl;
  std::cout<<"-h|--h|--help:\tPrint this usage information"<<std::endl;
}

bool TestFileExists(const std::string& sFilePath)
{
  return std::experimental::filesystem::exists(sFilePath);
}

size_t GetFileSizeBytes(const std::string& sFilePath)
{
  return std::experimental::filesystem::file_size(sFilePath);
}

bool ReadFileIntoString(const std::string& sFilePath, size_t nMaxFileSizeBytes, std::string& contents)
{
  if (!TestFileExists(sFilePath)) {
    std::cerr<<"create-index-html Config file \""<<sFilePath<<"\" not found"<<std::endl;
    return false;
  }

  const size_t nFileSizeBytes = GetFileSizeBytes(sFilePath);
  if (nFileSizeBytes == 0) {
    std::cerr<<"Empty config file \""<<sFilePath<<"\""<<std::endl;
    return false;
  } else if (nFileSizeBytes > nMaxFileSizeBytes) {
    std::cerr<<"Config file \""<<sFilePath<<"\" is too large"<<std::endl;
    return false;
  }

  std::ifstream f(sFilePath);

  contents.reserve(nFileSizeBytes);

  contents.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());

  return true;
}

struct cFolder {
  std::string name;
  std::string colour;
  std::list<std::string> files;
};

std::string GetThirdFolder(const std::string& sPath)
{
  std::string delim = "/";

  auto start = 0U;
  auto end = sPath.find(delim);
  size_t i = 0;
  while (end != std::string::npos) {
    if (i == 2) {
      return sPath.substr(start, end - start);
    }

    start = end + delim.length();
    end = sPath.find(delim, start);

    i++;
  }

  return "";
}

void AddFile(const std::string& sFilePath, std::list<cFolder>& folders)
{
  for (auto& folder : folders) {
    std::string type = GetThirdFolder(sFilePath);
    if (folder.name == type) {
      folder.push_back(sFilePath);
      return;
    }
  }

  // We didn't find a match so just add it to the misc group
  for (auto& folder : folders) {
    if (folder.name == "misc") {
      folder.push_back(sFilePath);
      return;
    }
  }
}

void GetAudioFilesInDirectory(const std::string& sFilePath, std::list<cFolder>& folders)
{
  for (const auto & entry : std::experimental::filesystem::recursive_directory_iterator(sFilePath)) {
    if ((entry.path().extension() == ".mp3") || (entry.path().extension() == ".wav")) {
      AddFile(entry.path(), folders);
    }
  }
}

}

int main(int argc, char **argv)
{
  if (argc >= 2) {
    for (size_t i = 1; i < size_t(argc); i++) {
      if (argv[i] != nullptr) {
        const std::string sAction = argv[i];
        if ((sAction == "-v") || (sAction == "-version") || (sAction == "--version")) beatpad::PrintVersion();
        else if ((sAction == "-h") || (sAction == "-help") || (sAction == "--help")) beatpad::PrintUsage();
        else {
          std::cerr<<"Unknown command line parameter \""<<sAction<<"\", exiting"<<std::endl;
          beatpad::PrintUsage();
          return -1;
        }
      }
    }

    return 0;
  }

  // Read the template file
  const std::string sTemplateFile = "index.html.template";
  const size_t nMaxFileSizeBytes = 1 * 1024 * 1024; // Maximum size of the file
  std::string contents;
  if (!beatpad::ReadFileIntoString(sTemplateFile, nMaxFileSizeBytes, contents)) {
    std::cerr<<"Failed to load template file \""<<sTemplateFile<<"\", exiting"<<std::endl;
    return EXIT_FAILURE;
  }

  // Get the text before and after the "INSERT_BUTTONS" line in the file
  const std::string before = contents.substr(0, contents.find("INSERT_BUTTONS"));
  const std::string after = contents.substr(contents.find("INSERT_BUTTONS") + strlen("INSERT_BUTTONS")); 


  const std::string sOutputFile = "index.html";

  {
    std::ofstream o(sOutputFile, std::ostream::out);
    if (!o.good()) {
      std::cerr<<"Failed to open output file \""<<sOutputFile<<"\", exiting"<<std::endl;
      return EXIT_FAILURE;
    }

    o<<before;

    std::list<beatpad::cFolder> folders = {
      { "tom", "red" },
      { "crash", "orange" },
      { "snare", "mustard" },
      { "kick", "yellow" },
      { "bass", "blue" },
      { "break", "purple" },
      { "synth", "cyan" },
      { "808", "teal" },
      { "voice", "yellow" },
      { "vox", "green" },
      { "misc", "brown" },
    };
    beatpad::GetAudioFilesInDirectory("samples", folders);

    // Write out each item
    size_t id = 0;
    for (auto& folder : folders) {
      const std::string colour = folder.colour;
      for (auto& file : folder.files) {
        id++;
        const std::string sTitle = "Title";
        o<<"      <figure>"<<std::endl;
        o<<"        <img class=\"soundboardimg\" src=\"images/"<<colour<<".png\" alt=\"\" onclick=\"document.getElementById('"<<id<<"').play();\">"<<std::endl;
        o<<"        <audio id=\""<<id<<"\" title=\""<<sTitle<<"\">"<<std::endl;
        o<<"          <source src=\""<<file<<"\" />"<<std::endl;
        o<<"        </audio>"<<std::endl;
        o<<"      </figure>"<<std::endl;
      }
    }

    o<<after;
  }

  std::cout<<sOutputFile<<" generated, exiting"<<std::endl;

  return EXIT_SUCCESS;
}
