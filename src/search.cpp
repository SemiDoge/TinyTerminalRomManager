#include "../inc/search.h"

// Function to calculate the Levenshtein Distance between two strings
// https://en.wikipedia.org/wiki/Wagner%E2%80%93Fischer_algorithm
int levenshteinDistance(const std::string& str1, const std::string& str2) {
    const int m = str1.length();
    const int n = str2.length();

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1));

    for (int i = 0; i <= m; ++i)
        dp[i][0] = i;

    for (int j = 0; j <= n; ++j)
        dp[0][j] = j;

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (str1[i - 1] == str2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
        }
    }

    return dp[m][n];
}

// Function to perform a fuzzy search on roms vector
std::vector<Rom> fuzzySearch(const std::vector<Rom>& roms, const std::string& searchString, int maxDistance) {
    std::map<int, Rom> subResults;
    std::vector<Rom> results;
    maxDistance = (searchString.length() == 0) ? maxDistance / 1 : maxDistance / searchString.length();
    

    for (const auto& rom : roms) {
        std::string romName = toLower(rom.name);
        int distance = levenshteinDistance(romName, toLower(searchString));

        size_t found = romName.find(searchString);
        bool bSubstring = (found != std::string::npos);

        //Biases the results if searchString is a substring of the ROM name
        if(bSubstring) {
            distance -= rom.name.size();
        }

        if (distance <= maxDistance) {
            auto insertionRes = subResults.insert(std::make_pair(distance, rom));
            
            //If it tried to insert into a preexisting key find next available key
            while(!insertionRes.second) {
                insertionRes = subResults.insert(std::make_pair(distance++, rom));
            }
        }
    }

    for(const auto& pair : subResults) {
        results.push_back(pair.second);
    }

    return results;
}

void romSearch(const std::vector<Rom>& allRoms, std::vector<Rom>& menu, std::string searchString) {
    std::vector<Rom> searchResult = fuzzySearch(allRoms, searchString, 83);
    menu.clear();
    menu.reserve(searchResult.size());
    std::copy(searchResult.begin(), searchResult.end(), std::back_inserter(menu));

    return;
}