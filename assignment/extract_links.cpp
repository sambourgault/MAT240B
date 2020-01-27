#include <regex> // regex
#include <iostream>
using namespace std;

int main()
{
  while (true)
  {
    string line;
    getline(cin, line);

    if (!cin.good())
    {
      return 0;
    }

    // do things
    std::smatch matches;
    std::regex reg("(<a href=\"(.*?)\"(.*?)>)(.*?)(</a>)");

    while (std::regex_search(line, matches, reg))
    {
      cout << matches.str(4) << " -->  " << matches.str(2) << endl;
      line = matches.suffix().str();
    }
  }

  return 0;
}