#include <iostream>     // std::cout
#include <algorithm>    // std::set_union, std::sort
#include <vector>       // std::vector
#include<set>
using namespace std;

/* returns size of the array formed.*/
int set_to_array(set<int> s,int a[100])
{
    std::set<int>::iterator it;
    int k=0;
    for(it=s.begin();it!=s.end();++it)
    {
        a[k++]=*it;
    }
    return k;
}
void vector_to_set(vector<int> a,set<int> &s)
{
    int size=a.size();
    for(int i=0;i<size;i++)
    {
            s.insert(a[i]);
    }
}
void Union (set<int> s1,set<int> s2,set<int> &s3) {
  int first[100];
  int second[100];
  std::vector<int>::iterator ip;
  int size1,size2;
  size1=set_to_array(s1,first);
  size2=set_to_array(s2,second);

   vector<int> v(200);
   ip=set_union(first,first+size1,second,second+size2,v.begin());
   v.resize(ip-v.begin());
   vector_to_set(v,s3);
   set<int>::iterator it;

}
void Diff (set<int> s1,set<int> s2,set<int> &s3) {
  int first[100];
  int second[100];
  std::vector<int>::iterator ip;
  int size1,size2;
  size1=set_to_array(s1,first);
  size2=set_to_array(s2,second);

   vector<int> v(200);
   ip=set_difference(first,first+size1,second,second+size2,v.begin());
   v.resize(ip-v.begin());
   vector_to_set(v,s3);
   set<int>::iterator it;

}
