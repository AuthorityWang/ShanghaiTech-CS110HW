//中点扩散算法
#include<bits/stdc++.h>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    std::string str;
    std::cin >> str;
    int res=0;
    int n = str.length();
    for(int i=0;i<n;i++)
    {
        int l=i-1,r=i+1;//判断奇数长度回文串
        while(l>=0 && r<n && str[l]==str[r]) l--,r++;
        res=max(res,r-l-1);
        l=i,r=i+1;//判断偶数长度回文串
        while(l>=0 && r<n && str[l]==str[r]) l--,r++;
        res=max(res,r-l-1);
    }
    cout<<res<<endl;
    return 0;
}