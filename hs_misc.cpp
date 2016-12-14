#include <sstream>
#include <iomanip>
#include <memory>
#include <stdexcept>

#include "hs_misc.h"

// http://stackoverflow.com/questions/154536/encode-decode-urls-in-c
std::string url_encode(const std::string &data)
{
	std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto i = data.begin(); i != data.end(); ++i)
	{
		std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

// http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
std::string hs_exec(const std::string &cmd)
{
    char buffer[128];
    std::string result = "";
    std::unique_ptr<FILE, std::function<int (FILE *)>> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
	{
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}
