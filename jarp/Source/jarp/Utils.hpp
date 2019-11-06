#include <fstream>
#include <string>
#include <vector>

namespace jarp {

	class Utils
	{
	public:
		static std::vector<char> ReadFile(const std::string& filename)
		{
			std::ifstream file(filename.c_str(), std::ios::binary | std::ios::ate);
			if (!file.is_open())
			{
				throw std::runtime_error("Failed to open file!");
			}
			std::streampos size = file.tellg();
			std::vector<char> buffer(size);

			file.seekg(0);
			file.read(buffer.data(), size);
			file.close();

			return buffer;
		}
	};

}
