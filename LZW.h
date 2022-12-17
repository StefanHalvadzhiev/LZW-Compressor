#pragma once
#include <filesystem> // C++17 standard header file name
#include <experimental/filesystem> // Header file for pre-standard implementation
#include <iostream>
#include <vector>
#include <fstream>	
#include <unordered_map>

using namespace std;
namespace fs = std::experimental::filesystem::v1;
using std::ios_base;

class LZW{
private:
	const string compressedExtension = ".lzw";
	const string hashExtension = ".lzwh";
	const string informationExtension = ".lzwi";

	string input(); // enters a string

	void inner_zip_file(); // zips one or multiple files
	void inner_zip_directory(); // zips a directory
	
	bool create_output_directory(const string& inputPath, const string& outputPath); // copies the folder tree from the input
	bool create_output_root_directory(const string& path); // creates root directory for the output

	void inner_unzip_file(); // unzips a single file
	void inner_unzip_directory(); // unzips a whole directory

	void generateInformationFile(const fs::path& inputPath, const fs::path& outputPath); // generates information file read by the INFO function

	void createHashSecurity(const fs::path& path); // creates txt file containing hashes of each compressed file in a direcotry
	void getVectorBoolData(vector<bool>& data, ifstream& file); // fills the bool vector that would be hashed
	unsigned long long calculateHash(const vector<bool>& data); // calculates the hash of a bool vector

	bool LZWCompressor(const fs::path& inputPath, const fs::path& outputPath); // compresses a single file
	bool LZWDecompressor(const fs::path& inputPath, const fs::path& outputPath); // decompresses a single file
	void encoder(vector<unsigned>& sequence, const string& s1); // used by the compressor to encode a string from a file
	void decoder(const vector<unsigned>& sequence, ofstream& file); // used by the decompressor to decode the whole sequence
	

public:
	void zip();
	void unzip();
	void info();
	void refresh(); // not working at the current moment
	void ec();

	LZW();
	~LZW();
	LZW(const LZW& other) = delete;
	LZW& operator= (const LZW& other) = delete;
};