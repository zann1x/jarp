#include "jarppch.h"

#include <fstream>

class Utils
{
public:
	static std::vector<char> ReadFile(const std::string& Filename)
	{
		std::ifstream File(Filename, std::ios::binary | std::ios::ate);
		if (!File.is_open())
		{
			throw std::runtime_error("Failed to open file!");
		}
		std::streampos Size = File.tellg();
		std::vector<char> Buffer(Size);

		File.seekg(0);
		File.read(Buffer.data(), Size);
		File.close();

		return Buffer;
	}
};
