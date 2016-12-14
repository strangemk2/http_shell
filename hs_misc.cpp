#include <sstream>
#include <iomanip>

#include "hs_misc.h"

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
