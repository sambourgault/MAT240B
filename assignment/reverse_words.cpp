#include <iostream>
#include <string>
using namespace std;

int main() {
  while (true) {

    printf("Type a sentence (then hit return): ");

    string line;
    string word;

    getline(cin, line);

    if (!cin.good()) {
      printf("Done\n");
      return 0;
    }

    // put your code here
    //
    // reverse each word of the input 
    for (int j = 0; j < line.length()+1; j++){
      if (line[j] != ' ' && j != line.length()){
        word.push_back(line[j]);
      } else {

         // reverse the word
        for (int i = 0; i < word.length()/2; i++){
          char temp = word[i];
          word[i] = word[(word.length()-1)-i];
          word[(word.length()-1)-i] = temp;
        }

        // add a space between the words except for the last word.
        if (j != line.length()){
          cout << word << " ";
        } else {
          cout << word;
        }

        word.clear();
      }
    }
    
    cout << "\n";
  }
}