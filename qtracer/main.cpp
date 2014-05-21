#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
    if (argc != 2) {
        cout << "usage: " << string(argv[0]) << "trace" << endl;
        return 2;
    }

    ifstream ifs(argv[1]);

    string str;
    while (ifs >> str)
    {
        double tim;
        ifs >> tim;
        cout << str << " <--> " << tim << endl;
    }
    ifs.close();
    return 0;
}
