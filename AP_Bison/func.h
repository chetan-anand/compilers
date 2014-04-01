#include <iostream>     // std::cout
#include <algorithm>    // std::set_union, std::sort
#include <vector>       // std::vector
#include<set>
using namespace std;

/* returns size of the array formed.*/
int set_to_array(set<string> s,string a[100])
{
    std::set<string>::iterator it;
    int k=0;
    for(it=s.begin();it!=s.end();++it)
    {
        a[k++]=*it;
    }
    return k;
}
void vector_to_set(vector<string> a,set<string> &s)
{
    int size=a.size();
    for(int i=0;i<size;i++)
    {
            s.insert(a[i]);
    }
}
void Union (set<string> s1,set<string> s2,set<string> &s3) {
  string first[100];
  string second[100];
  std::vector<string>::iterator ip;
  int size1,size2;
  size1=set_to_array(s1,first);
  size2=set_to_array(s2,second);

   vector<string> v(200);
   ip=set_union(first,first+size1,second,second+size2,v.begin());
   v.resize(ip-v.begin());
   vector_to_set(v,s3);
   set<string>::iterator it;

}
void Diff (set<string> s1,set<string> s2,set<string> &s3) {
  string first[100];
  string second[100];
  std::vector<string>::iterator ip;
  int size1,size2;
  size1=set_to_array(s1,first);
  size2=set_to_array(s2,second);

   vector<string> v(200);
   ip=set_difference(first,first+size1,second,second+size2,v.begin());
   v.resize(ip-v.begin());
   vector_to_set(v,s3);
   set<string>::iterator it;

}
