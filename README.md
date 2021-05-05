# TT_Words
Reads a text file in a multi-threaded fashion and prints out words and counts in descending order. Case-insensitive.

After downloading, execute the following commands to compile and run the code:

g++ -std=c++11 -lpthread New_TT_words.cpp -o TT_words
./TT_words

The code functions as follows:
1. Each thread calculates the number of lines to be read from the file.
2. The threads read those lines and parse individual words, converting them to lower case and adding them to an unordered map.
3. If a repeat word is found, the count is updated. While reading, the threads use a mutex to ensure that no line is read twice.
4. Once all lines are read and words are added, the threads join and all unordered maps are combined into a single map, located at "wordCounts[0]", which is a vector of unordered maps, one per thread.
5. This combined map is then sorted in descending order, and all words and respective counts are printed.

Some helpful variables:
- NUM_THREADS sets the number of threads, which can be changed and then recompiled
- DEBUG can be used to print total execution time in microseconds and the total number of unique words
