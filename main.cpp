#include <iostream>
#include "FlightSearch.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Error: Please choose a date (YYYY-MM-DD).\n";
        return 1;
    }

    std::string date = argv[1];
    FlightSearch search(date);

    search.FindRoundTripRoutes("c2", "c54");


    search.ClearCache();


    search.ClearFileCache("cache.json");


    search.ClearAllCache("cache.json");

    return 0;
}
