#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

class Solution
{
    typedef unordered_map<string, string> UrlMap;
    typedef vector<string> UrlVector; 
public:
    Solution()
    {
        dict = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    }

    string encode(string longUrl)
    {
        UrlMap::iterator it = m_mapUrls.find(longUrl);
        if (it != m_mapUrls.end())
        {
            return it->second;
        }

        m_vecUrls.push_back(longUrl);

        string res;
        size_t id = m_vecUrls.size();
        while (id > 0)
        {
            res += dict[id % dict.size()];
            id = id / dict.size();
        }

        while (res.size() < 6)
        {
            res = "0" + res;
        }

        m_mapUrls.insert(make_pair(longUrl, res));

        return res;
    }

    string decode(string shortUrl)
    {
        size_t idx = 0;
        for (size_t i = 0; i < shortUrl.size(); ++i)
        {
            idx = idx * 62 + dict.find(shortUrl[i]);
        }

        if (idx <= m_vecUrls.size() && idx > 0)
        {
            return m_vecUrls[idx - 1];
        }
        else
        {
            return "hello";
        }
    }

private:
    UrlMap      m_mapUrls;
    UrlVector   m_vecUrls;
    string      dict;
};

int main()
{
    string longUrl = "https://leetcode.com/problems/design-tinyurl";
    Solution s;
    string shortUrl = s.encode(longUrl);
    string retUrl = s.decode(shortUrl);
    cout << shortUrl << endl << retUrl << endl;
    return 0;
}