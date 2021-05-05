#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include <chrono>
#include <locale>

using namespace std;

#define NUM_THREADS 1
#define DEBUG 1

mutex fmtx;
mutex wmtx;

bool comp(pair<string, int> a, pair<string, int> b)
{
    return a.second > b.second;
}

// Read from file
void processFile(ifstream &infile, unordered_map<string, int> &words, int numLines)
{
    string line;
    string word;
    locale loc;
    stringstream ss(" ");

    while (!infile.eof())
    {
        //file mutex start
        fmtx.lock();
        ss.clear();

        for (int i = 0; i < numLines; i++)
        {
            getline(infile, line);
            ss << " " << line;
        }

        fmtx.unlock();
        //file mutex end

        while (ss >> word)
        {
            for (string::size_type i = 0; i < word.length(); i++)
                word[i] = tolower(word[i], loc);

            words[word]++;
        }
    }

    return;
}

int main()
{
    auto start = chrono::high_resolution_clock::now();

    // Build data structure to hold words
    unordered_map<string, int> words;

    ifstream infile, lineFile;
    string line;
    lineFile.open("wordlist.txt");

    int num_lines = 0;
    int lines_per_thread = 0;

    while (getline(lineFile, line))
    {
        num_lines++;
    }

    lines_per_thread = num_lines / NUM_THREADS;
    lineFile.close();
    infile.open("wordlist.txt");

    // start threads
    vector<thread> threads;
    vector<unordered_map<string, int>> wordCounts(NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (i == NUM_THREADS - 1)
        {
            threads.push_back(thread(processFile, ref(infile), ref(wordCounts[i]), (num_lines - (NUM_THREADS - 1) * lines_per_thread)));
        }
        else
        {
            threads.push_back(thread(processFile, ref(infile), ref(wordCounts[i]), lines_per_thread));
        }
    }

    //join threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    //combine wordCounts
    for (int i = 1; i < NUM_THREADS; i++)
    {
        for (unordered_map<string, int>::iterator it = wordCounts[i].begin(); it != wordCounts[i].end(); it++)
        {
            unordered_map<string, int>::const_iterator got = wordCounts[0].find(it->first);
            if (got == wordCounts[0].end())
            {
                pair<string, int> newPair(it->first, it->second);
                wordCounts[0].insert(newPair);
            }
            else
                wordCounts[0][it->first] += it->second;
        }
    }

    // copy over and sort words and counts from unordered_map to vector
    vector<pair<string, int>> sorted_words(wordCounts[0].begin(), wordCounts[0].end());
    sort(sorted_words.begin(), sorted_words.end(), comp);

    infile.close();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    for (vector<pair<string, int>>::iterator it = sorted_words.begin(); it != sorted_words.end(); it++)
    {
        cout << it->first << " " << it->second << "\n";
    }

    if (DEBUG)
    {
        cout << "\nThere are " << wordCounts[0].size() << " unique words.\n";
        cout << "Total execution time with " << NUM_THREADS << " threads is " << duration.count() << " microseconds.\n";
    }

    return 0;
}
