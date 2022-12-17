#include "LZW.h"

// PRIVATE METHODS

string LZW::input(){
	string str;
	cin >> str;
	return str;
}

void LZW::inner_zip_file(){
	unsigned fileCount;
	
	cin >> fileCount;

	string outputPath = input();

	if (!create_output_root_directory(outputPath))
		return;
	
	while (fileCount > 0) {
		// compresses each file from the input
		string inputPath;
		cin >> inputPath;
		fs::path inPath(inputPath);
		if (!fs::is_regular_file(inPath)) {
			cout << "Incorrect path. Please try again!" << endl;
			continue;
		}
		fs::path outPath(outputPath + "\\" + inPath.filename().string() + ".lzw");
		bool compressStatus = LZWCompressor(inPath, outPath);
		if (!compressStatus) {
			cout << "An error has occured while compressing <" << inputPath << ">" << endl;
		}

		fileCount--;
	}

	createHashSecurity(outputPath);
}

void LZW::inner_zip_directory(){
	// zips a given directory
	string inputPath = input();
	string outputPath = input();

	unsigned inputPathSize = inputPath.size();

	if (!create_output_directory(inputPath, outputPath)) {
		cout << "Can not create output directory." << endl;
		return;
	}

	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator{ inputPath }) {
		if (dir_entry.is_regular_file()) {
			// generate output path for each file
			string outPath = dir_entry.path().string();
			outPath.erase(0, inputPathSize);
			outPath = (outputPath + outPath);
			outPath += compressedExtension;

			fs::path out(outPath);
			fs::path in(dir_entry.path().string());
			LZWCompressor(in, out);
		}
	}
}

bool LZW::create_output_directory(const string& inputPath, const string& outputPath){
	// creates directory where the compressed files would be saved
	if (!create_output_root_directory(outputPath))
		return false;

	unsigned inputPathSize = inputPath.size();
	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator{ inputPath }){
		if (dir_entry.is_directory()) {
			string subTreePath = dir_entry.path().string();

			subTreePath.erase(0, inputPathSize);
			fs::create_directory(outputPath + subTreePath);
		}
	}

	return true;	
}

bool LZW::create_output_root_directory(const string& path){
	try {
		fs::create_directory(path);
	}
	catch (...) {
		cout << "An error has occured while creating the output roor directory." << endl;
		return false;
	}

	if (!fs::exists(path)) {
		cout << "Directory could not be created." << endl;
		return false;
	}

	return true;
}

void LZW::inner_unzip_file(){
	string iPath = input();
	string oPath = input();

	// check whether the input path is correct
	fs::path inputPath(iPath);
	if (!fs::is_regular_file(inputPath) || !fs::exists(inputPath) || inputPath.extension() != compressedExtension) {
		cout << "UNZIP FILE: input path is not a file or does not exist." << endl;
		return;
	}

	// copy the subtree folders (only) from the input path to the output path 
	if (!create_output_root_directory(oPath))
		return;

	fs::path in(iPath);
	

	string inputFileName = in.filename().string();
	oPath += ("\\" + inputFileName);
	oPath.erase(oPath.size() - 4, oPath.size());

	fs::path out(oPath);
	LZWDecompressor(in, out);
}

void LZW::inner_unzip_directory(){
	string iPath = input();
	string oPath = input();

	fs::path inputPath(iPath);
	if (!fs::is_directory(inputPath) || !fs::exists(inputPath)) {
		cout << "UNZIP DIRECTORY: input path is not a directory or does not exist." << endl;
		return;
	}

	// copy the subtree folders from the input path to the output path
	if (!create_output_directory(iPath, oPath))
		return;

	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator{ iPath }) {
		if (dir_entry.is_regular_file() && dir_entry.path().extension() == compressedExtension) {
			// generate the output path
			string subTreePath = dir_entry.path().string();
			subTreePath.erase(0, iPath.size());
			string outPutPath = oPath + subTreePath;
			outPutPath.erase(outPutPath.size() - 4, outPutPath.size());
			
			// input and output paths
			fs::path in(dir_entry.path().string());
			fs::path out(outPutPath);

			// compress a file from the input path to a file in the output path
			if (!LZWDecompressor(in, out)) {
				cout << "Can not decompress." << endl;
			}
		}
	}
}

void LZW::generateInformationFile(const fs::path& inputPath, const fs::path& outputPath){
	// generate the inmformation file path
	string informationFileName = outputPath.string();
	informationFileName.erase(informationFileName.size() - compressedExtension.size(), informationExtension.size());
	informationFileName += informationExtension;
	
	// save the information
	ofstream informationFile(informationFileName);
	informationFile << fs::file_size(inputPath) << '\n';
	informationFile << fs::file_size(outputPath) << '\n';
	informationFile.close();
}

void LZW::createHashSecurity(const fs::path& path){
	ifstream fileToHash(path);

	// set the bool vector that would be hashed
	vector<bool> vectorToHash;
	getVectorBoolData(vectorToHash, fileToHash);
	fileToHash.close();

	// create the hash file path
	string hashPath = path.string();
	hashPath.erase(hashPath.size() - compressedExtension.size(), hashPath.size());
	hashPath += hashExtension;

	// calculating the hash of the file and save it
	unsigned long long fileHash = calculateHash(vectorToHash);
	ofstream hashFile(hashPath);
	hashFile << fileHash;
	hashFile.close();
}

void LZW::getVectorBoolData(vector<bool>& data, ifstream& file){
	while (!file.eof()) {
		unsigned number;
		file >> number;
		if (number % 3 == 0)
			data.push_back(false);
		else
			data.push_back(true);
		if (number % 3 == 1)
			data.push_back(false);
		else
			data.push_back(true);
		if (number % 3 == 2)
			data.push_back(false);
		else
			data.push_back(true);
	}
}

unsigned long long LZW::calculateHash(const vector<bool>& data){
	unsigned long long fileHash = hash<vector<bool>>{}(data);
	return fileHash;
}

bool LZW::LZWCompressor(const fs::path& inputPath, const fs::path& outputPath){
	// check whether the input path is not leading to a file
	if (!fs::is_regular_file(inputPath))
		return false;

	ifstream in(inputPath, ios_base::in | ios_base::binary);
	ofstream out(outputPath, ios_base::out | ios_base::binary);

	// sequence containing the information of the input file
	vector<unsigned> data;

	// check wheter an error has occured and the input and output files can not be opened
	if (!in.is_open() || !out.is_open()) {
		cout << "Can not open in or out file" << endl;
		return false;
	}

	// read the input file line by line and encode each line in the sequence
	char reader[16384];
	do {
		in.read((&reader)[0], 16384);
		string str(reader, in.gcount());
		encoder(data, str);
	} while (in.gcount());
	data.push_back(0);
	
	// save the data
	for (unsigned i = 0; i < data.size(); ++i) 
		out << data[i] << '\n';

	in.close();
	out.close();

	// generate information and hash files
	generateInformationFile(inputPath, outputPath);
	createHashSecurity(outputPath);

	return true;
}

bool LZW::LZWDecompressor(const fs::path& inputPath, const fs::path& outputPath){
	ifstream inFile(inputPath, ios_base::in | ios_base::binary);
	ofstream outFile(outputPath, ios_base::out | ios_base::binary);

	if (!inFile.is_open() || !outFile.is_open()) {
		cout << "Decompressor: can not open files" << endl;
		return false;
	}

	vector<unsigned> sequence;
	while (!inFile.eof()) {
		unsigned data;
		inFile >> data;
		sequence.push_back(data);
	}

	decoder(sequence, outFile);

	inFile.close();
	outFile.close();
	return true;
}

void LZW::encoder(vector<unsigned>& sequence, const string& strFromFile){
	vector<unsigned> temporary; // temporary buffer to save the substrings as indexes

	// generate initial substring table
	unordered_map<string, unsigned> table;
	for (int i = 0; i < 256; i++) {
		string ch;
		ch += char(i);
		table[ch] = i;
	}
	unsigned index = 256;

	// fill the substring table with information from the file string
	string subString, buff;
	subString += strFromFile[0];
	for (unsigned i = 0; i < strFromFile.length(); i++) {
		if (i != strFromFile.length() - 1)
			buff += strFromFile[i + 1];
		if (table.find(subString + buff) != table.end())
			subString = subString + buff;
		else {
			temporary.push_back(table[subString]);
			table[subString + buff] = index;
			index++;
			subString = buff;
		}
		buff.clear();
	}
	temporary.push_back(table[subString]);

	// specifying the length of the sequence encoding a line from the file
	sequence.push_back(temporary.size()); 

	// loads information in the vector that is going to be saved in the .lzw file
	for (unsigned i = 0; i < temporary.size(); ++i) 
		sequence.push_back(temporary[i]);
}

void LZW::decoder(const vector<unsigned>& sequence, ofstream& file){
	// get the sequence encoding a line from the file and its starting position 
	unsigned chunkSize = sequence[0];
	unsigned chunkIndex = 1;

	while (chunkSize) {
		string result; // decoded line from the sequence
		unordered_map<unsigned, string> table;
		for (int i = 0; i < 256; i++) {
			string ch;
			ch += char(i);
			table[i] = ch;
		}

		unsigned old = sequence[chunkIndex], n;
		string subString = table[old];
		string buff;
		buff.clear();
		buff += subString[0];
		result += subString;
		unsigned index = 256;

		for (unsigned i = chunkIndex; i < chunkIndex + chunkSize - 1; i++) {
			n = sequence[i + 1];
			if (table.find(n) == table.end()) {
				subString = table[old];
				subString = subString + buff;
			}
			else {
				subString = table[n];
			}
			result += subString;
			buff.clear();
			buff += subString[0];
			table[index] = table[old] + buff;
			index++;
			old = n;
		}

		// set the new chunk size and index
		unsigned oldChunkSize = chunkSize;
		chunkSize = sequence[chunkIndex + chunkSize];
		chunkIndex = chunkIndex + oldChunkSize + 1;

		// write the decoded line to the decompressed file
		file.write(&result.c_str()[0], result.size());
	}
}

// PUBLIC METHODS

void LZW::zip(){
	char type;
	cin >> type;

	if (type == 'f')
		inner_zip_file();

	else if (type == 'd')
		inner_zip_directory();

	else 
		cout << "Unknown zip type." << endl;
}

void LZW::unzip(){
	char type;
	string outputPath;
	 
	cin >> type;
	if (type == 'f')
		inner_unzip_file();
	else if (type == 'd')
		inner_unzip_directory();
	else
		cout << "Unknown unzip type." << endl;
	
}

void LZW::info(){
	string directory = input();

	fs::path path(directory);
	if (!fs::is_directory(path)) {
		cout << "Given path is not directory." << endl;
		return;
	}

	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator{ directory }) {
		if (dir_entry.is_regular_file() && dir_entry.path().extension() == compressedExtension) {
			string informationPath = dir_entry.path().string();
			informationPath.erase(informationPath.size() - compressedExtension.size(), informationPath.size());
			informationPath += informationExtension;
			ifstream informationFile(informationPath);
			if (!informationFile.is_open()) {
				cout << "Can not open " << informationPath << endl;
				continue;
			}
			unsigned long long originalFileSize, compressedFileSize;
			informationFile >> originalFileSize;
			informationFile >> compressedFileSize;
			if (originalFileSize && compressedFileSize) {
				cout << dir_entry.path().string() << " <COMPRESSED ";
				cout << ((float)compressedFileSize / originalFileSize) * 100 << "%>" << endl;
			}
			else if (!compressedFileSize && originalFileSize)
				cout << "100%" << endl;
		}
	}

}

void LZW::refresh(){

}

void LZW::ec(){
	string directoryPath = input();
	if (!fs::exists(directoryPath)) {
		cout << "Path does not exist" << endl;
		return;
	}

	for (const std::filesystem::directory_entry& dir_entry : std::filesystem::recursive_directory_iterator{ directoryPath }) {
		if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".lzw") {
			string hashFilePath = dir_entry.path().string();
			hashFilePath.erase(hashFilePath.size() - compressedExtension.size(), hashFilePath.size());
			hashFilePath += hashExtension;

			ifstream hashFile(hashFilePath);
			if (!hashFile.is_open()) {
				cout << "Error with file " << dir_entry.path().string() << endl;
			}

			ifstream fileToBeHashed(dir_entry.path().string());
			unsigned long long oldHash;
			hashFile >> oldHash;

			vector<bool> boolData;
			getVectorBoolData(boolData, fileToBeHashed);
			unsigned long long currentHash = calculateHash(boolData);
			if (oldHash != currentHash) {
				cout << dir_entry.path().string() << " has been tempered." << endl;
			}

		}
	}
}

LZW::LZW(){

}

LZW::~LZW(){
}
