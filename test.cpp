#include <iostream>
#include <fstream>
#include <iomanip>
#include "json.hpp"
#include <vector>

#include <memory>
#include <string>
#include <typeinfo>
#include <regex>
using namespace std;
using json = nlohmann::json;

int main(int argc, char **argv) {
    cout << "test start " << endl;


    // create an empty structure (null)
    json j;

    // add a number that is stored as double (note the implicit conversion of j to an object)
    j["pi"] = 3.141;

    // add a Boolean that is stored as bool
    j["happy"] = true;

    // add a string that is stored as std::string
    j["name"] = "Niels";

    // add another null object by passing nullptr
    j["nothing"] = nullptr;

    // add an object inside the object
    j["answer"]["everything"] = 42;

    // add an array that is stored as std::vector (using an initializer list)
    j["list"] = {1, 0, 2};

    // add another object (using an initializer list of pairs)
    j["object"] = {{"currency", "USD"},
                   {"value",    42.99}};

    // instead, you could also write (which looks very similar to the JSON above)
     json children;
     
     children["list"] = {1, 0, 2};
 //   children.push_back(string("push_back test"));
j["children"] = children;

  //  string jsonFile = "new_tree.json";
  //  std::ofstream outFile(jsonFile);
 //   outFile << std::setw(4) << j << std::endl;

    std::cout << j.dump(4) << std::endl;
}