
// #include<iostream>
// #include <vector>
// #include <fstream>
// #include <cstdio>


// using namespace std;

// struct Reading { // a temperature reading
// int hour; // hour after midnight [0:23]
// double temperature; // in Fahrenheit
// };


// int main() {
// cout << "Please enter input file name: ";
// string iname;
// cin >> iname;

// ifstream ist {iname};
// if (!ist)// ist reads from the file named iname
// {
//     std::cerr << "cant open file\n";
//     return 1;

// }

// string oname;
// cout << "Please enter name of output file: ";
// cin >> oname;
// ofstream ost {oname};
// if (!ost)// ost writes to a file named oname
// {
//     std::cerr << "cant open file\n";
//     return 1;

// }
// vector<Reading> temps; // store the readings here
// int hour = -1;
// double temperature = -700;
// while (ist >> hour >> temperature) {
// if (hour < 0 || 23 <hour){
//     std::cerr << "hour out of range\n";
//     return 1;}

// temps.push_back(Reading{hour,temperature});
// }

// for (auto x : temps)
// ost << '(' << x.hour << ',' << x.temperature << ")\n";


// return 0;
// }


#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

// 1. Text-book error helper functions to handle exceptions
void error(const std::string& s) {
    throw std::runtime_error(s);
}

void error(const std::string& s, const std::string& s2) {
    throw std::runtime_error(s + s2);
}

// 2. Your stream reading function (with syntax typos fixed)
void fill_vector(std::istream& ist, std::vector<int>& v, char terminator) {
    for (int x; ist >> x; ) {
        v.push_back(x);
    }

    if (ist.eof()) {
        return;
    }

    if (ist.bad()) {
        error("ist is bad");
    }

    if (ist.fail()) {
        ist.clear();
        char c = 0;
        if (ist >> c) {
            if (c != terminator) {
                ist.unget(); // Fixed typo 'ung et'
                ist.clear(std::ios_base::failbit); // Fixed scope
            }
        }
    }
}

// 3. Execution entry point to test the file handling logic
int main() {
    try {
        std::string inname;
        std::cout << "Please enter input file name: ";
        if (!(std::cin >> inname)) {
            error("No input file name provided");
        }

        std::ifstream ifs{inname};
        if (!ifs) {
            error("can't open input file ", inname);
        }

        std::vector<int> data;
        char term = '|'; // Defines when to safely stop reading integers

        std::cout << "Reading integers (terminator is '" << term << "')...\n";
           fill_vector(ifs, data, term);

           // NEW: Check if fill_vector left the stream in a failed state
           if (ifs.fail()) {
               error("Reading failed: Unexpected character or formatting error encountered.");
           }

           // This will now only run if the file ended cleanly (EOF) or hit '|'
           std::cout << "Successfully read " << data.size() << " integers:\n";
           for (int val : data) {
               std::cout << val << " ";
           }
           std::cout << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred\n";
        return 2;
    }

    return 0;
}
