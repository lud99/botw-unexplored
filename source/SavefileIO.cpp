#include "SavefileIO.h"

#include <fstream>

#include <iostream>
#include <iomanip>
#include <stdio.h> 

uint32_t SavefileIO::ReadU32(unsigned char* buffer, int offset)
{
	// Little endian byte order
	return  ((buffer[offset + 0] << 0 ) + 
			(buffer[offset + 1] << 8 ) + 
			(buffer[offset + 2] << 16) + 
			(buffer[offset + 3] << 24)) >> 0 /* Make it positive? */;
}

void SavefileIO::ParseFile(const char* filepath)
{
	std::ifstream file;
	file.open(filepath, std::ios::binary);

	// Get length of file
	file.seekg(0, file.end);
	unsigned int fileSize = (unsigned int) file.tellg(); // File size is below sizeof(unsigned int), so the cast is fine
	file.seekg(0, file.beg);

	unsigned char* buffer = new unsigned char[fileSize];

	// Read the entire file into the buffer. Need to cast the buffer to a non-signed char*.
	file.read((char*) &buffer[0], fileSize);

	file.close();

	// Based on https://github.com/d4mation/botw-unexplored-viewer/blob/master/assets/js/zelda-botw.js

	// Iterate through the entire savefile to find the korok seed and location hashes
	for (unsigned int offset = 0x0c; offset < fileSize - 4; offset += 8)
	{
		// Read the korok or location hash ('id')
		uint32_t hashValue = ReadU32(buffer, offset);

		// Check if a korok with the hash exists
		Data::Korok* korok = Data::KorokExists(hashValue);
		if (korok)
		{
			// Read the 4 bytes after the hash. If it's not 0, then the seed has been found.
			bool found = ReadU32(buffer, offset + 4) != 0;

			found ? foundKoroks.push_back(korok) : missingKoroks.push_back(korok);
		}

		// Check if a location with the hash exists
		Data::Location* location = Data::LocationExists(hashValue);
		if (location)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the location has been visited.
			bool visited = ReadU32(buffer, offset + 4) != 0;

			visited ? visitedLocations.push_back(location) : unexploredLocations.push_back(location);
		}

		// Check if a hinox with the hash exists
		Data::Hinox* hinox = Data::HinoxExists(hashValue);
		if (hinox)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the hinox has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedHinoxes.push_back(hinox) : undefeatedHinoxes.push_back(hinox);
		}

		// Check if a talus with the hash exists
		Data::Talus* talus = Data::TalusExists(hashValue);
		if (talus)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the talus has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedTaluses.push_back(talus) : undefeatedTaluses.push_back(talus);
		}

		// Check if a molduga with the hash exists
		Data::Molduga* molduga = Data::MoldugaExists(hashValue);
		if (molduga)
		{
			// Read the 4 bytes after the hash. If it is not 0, then the molduga has been defeated.
			bool defeated = ReadU32(buffer, offset + 4) != 0;

			defeated ? defeatedMoldugas.push_back(molduga) : undefeatedMoldugas.push_back(molduga);
		}
	}
}

std::vector<Data::Korok*> SavefileIO::foundKoroks;
std::vector<Data::Korok*> SavefileIO::missingKoroks;

std::vector<Data::Location*> SavefileIO::visitedLocations;
std::vector<Data::Location*> SavefileIO::unexploredLocations;

std::vector<Data::Hinox*> SavefileIO::defeatedHinoxes;
std::vector<Data::Hinox*> SavefileIO::undefeatedHinoxes;

std::vector<Data::Talus*> SavefileIO::defeatedTaluses;
std::vector<Data::Talus*> SavefileIO::undefeatedTaluses;

std::vector<Data::Molduga*> SavefileIO::defeatedMoldugas;
std::vector<Data::Molduga*> SavefileIO::undefeatedMoldugas;