// write a program that find the length of the longest palindromic substring of a given string
// for example, the longest palindromic substring of "abbsdcds" is "sdcds" so output 5
// time complexity: O(n^2)
#include <iostream>
#include <string>
using namespace std;

// from each index, expand to both sides to find the longest palindromic substring
int main() {
    string s;
    cin >> s;
    int n = s.length();
    int maxLen = 0;
    // consider the length is odd and even
    for (int i = 0; i < n; i++) {
        int len = 1;
        // odd
        for (int j = 1; i - j >= 0 && i + j < n; j++) {
            if (s[i - j] == s[i + j]) {
                len += 2;
            } else {
                break;
            }
        }
        maxLen = max(maxLen, len);
        // even
        len = 0;
        for (int j = 0; i - j >= 0 && i + j + 1 < n; j++) {
            if (s[i - j] == s[i + j + 1]) {
                len += 2;
            } else {
                break;
            }
        }
        maxLen = max(maxLen, len);
    }
    cout << maxLen << endl;
}