#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iomanip> 

#include "../utils/types.h"
#include "../utils/hash.h"
#include "../utils/file_utils.h"
#include "../utils/index_utils.h"
#include "../utils/blob.h"

void handleCommit(const Arguments& args) {
  std::map<std::string, FileMetadata> indexes = index_utils::readIndex();

  if (args.flagsWithValue.find("-m") == args.flagsWithValue.end() && args.flagsWithValue.find("--message") == args.flagsWithValue.end()) {
    std::cerr << "Aborting commit due to empty commit message.\n";
    std::cerr << "Please use \"dit commit\" with \"-m\" or \"--message\" flag.\n";
    return;
  }

  std::string message = "";
  
  if (args.flagsWithValue.find("-m") != args.flagsWithValue.end()) {
    message = args.flagsWithValue.at("-m");
  }

  if (args.flagsWithValue.find("--message") != args.flagsWithValue.end()) {
    message = args.flagsWithValue.at("--message"); 
  }

  for (std::map<std::string, FileMetadata>::const_iterator it = indexes.begin(); it != indexes.end(); ++it) {
    const std::string& filename = it->first;
    const FileMetadata& metadata = it->second;

    std::string fileData = file_utils::readFile(filename);
    std::string fileBlob = blob::convertToBlob(fileData);
    std::string hashedData = hash::hashDataSHA1(fileBlob);

    std::string objectDirectory = ".dit/objects/" + hashedData.substr(0, 2);
    std::string objectFilename = objectDirectory + "/" + hashedData.substr(2);

    std::cout << objectFilename << '\n';

    std::filesystem::create_directory(objectDirectory); 
    std::ofstream file(objectFilename);
    if (file.is_open()) {
        file << "This is the commit message: " << message;
        file.close();
    } else {
        std::cerr << "Failed to create file: " << objectFilename << '\n';
    }
  }
}