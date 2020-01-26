#include <algorithm>     // sort
#include <iostream>      // cin, cout, printf
#include <unordered_map> // unordered_map
#include <vector>        // vector
using namespace std;

int main()
{
  unordered_map<string, unsigned> dictionary;

  string word;
  while (cin >> word)
  {

    // put your counting code here
    //
    // use dictionary.find, dictionary.end, and dictionary[]
    //cout >> word;
    // remove some punctuations 
    string chars = "\" . , ! ? ; _ ( ) [ ]";
    for (char c : chars)
    {
      word.erase(remove(word.begin(), word.end(), c), word.end());
    }

    unordered_map<string, unsigned>::const_iterator iterator = dictionary.find(word);

    if (iterator == dictionary.end())
    {
      // not found,so add a new entry
      dictionary.insert(make_pair<string, unsigned>(word, 1));
    }
    else
    {
      // add one to the existing entry
      dictionary[iterator->first] += 1;
    }
  }

  //cout << dictionary["the"];

  vector<pair<string, unsigned> > wordList;
  for (auto &t : dictionary)
    wordList.push_back(t);

  // put your sorting code here
  //
  // use the sort function
  // sort using a custom function object
  struct
  {
    bool operator()(pair<string, unsigned> a, pair<string, unsigned> b) const
    {
      return a.second > b.second;
    }
  } sortPair;

  sort(wordList.begin(), wordList.end(), sortPair);

  for (auto &t : wordList)
    printf("%u:%s\n", t.second, t.first.c_str());
}