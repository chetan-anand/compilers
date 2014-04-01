#include <iostream>

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <malloc.h>

using namespace std ;

class Node

{

  public:

    Node* parent ;



    char letter ;



    Node* child[26] ;



    int childNum ;

  public:

    Node()

    {

      parent = NULL ;

      letter = NULL ;

      childNum = 0 ;

      for(int i=0 ; i<26 ; i++)

      {

        child[i] = NULL ;

      }

    }

};

class PrefixTree

{

  private:

    Node* root ;

    char** words ;

  public:

    PrefixTree(char* path) ;

    void getPrefix() ;





  private:

    char** readWordsFile(char* path) ;

    void insertNode(Node* parent, char* word, int cur) ;

    void createPrefixTree() ;



};

PrefixTree::PrefixTree(char* path)

{
cout<<"constructor"<<endl;
  words = readWordsFile(path) ;

  createPrefixTree() ;

}

void PrefixTree::getPrefix()

{

  int word_Count = _msize(words)/sizeof(char*) ;

  for(int i=0 ; i<word_Count ; i++)  //Travel each word of the line

  {

    //cout<<words[i]<<" " ;  //Output each word

                //We should be able to print the word by this statement.

                //However, since the last character of the word is '/n' or EOF,

                //our words and prefix won't be on the same line if we print the word by "cout" directly.

                //And it's a little complicated to control our output format.

                //Therefore, we will output the word when we travel the tree.

    int word_length = strlen(words[i])-1 ;

    Node* node = root ;

    for(int j=0 ; j<word_length ; j++)  //Travel each letter of the word and

    {                  //And get the node of the last letter of word

      char letter = *((words[i])+j) ;

      cout<<letter ;          //Output the letter of the word.
      cout<<node->childNum<<endl;
                      //So, we can get the whole of word when the FOR loop is finished.

      node = node->child[letter%26] ;

    }

                      //Now, node should direct to the last letter of word

    cout<<" " ;

                      //Backtracking and find first matched node whose parent has more than 1 child.

                      //Then, this node is the end letter for prefix word.

    node = node->parent ;
    Node *e;
    while(node!=root)

    {
      if(node->childNum!=1)
        e = node;
      node = node->parent ;

    }

    Node* end = e ;          //Now, "end" is what we wanted.

    //cout<<"end="<<node->letter<<endl ;

    node = root ;

    for(int j=0 ; j<word_length ; j++)    //Travel the tree to output each letter of prefix word

    {                  //until we arrive at the End Node.

      char letter = *((words[i])+j) ;

      node = node->child[letter%26] ;

      if(node==end)

        break ;

      cout<<letter ;

    }

    cout<<endl ;

  }

}

char** PrefixTree::readWordsFile(char* path)

{

  FILE *fp;

  char buf[256] ;

  if ((fp=fopen(path, "rt"))==NULL)

  {

    printf("open file error!!/n");

    return NULL;

  }

  int lineCount = 0;

  while(fgets(buf, 256, fp) != NULL)

  {

    lineCount++ ;

  }

  //cout<<"lineCount="<<lineCount<<endl ;

  char** words = new char*[lineCount] ;

  //init words buffer

  for(int i=0 ; i<lineCount ; i++)

  {

    words[i] = new char[256] ;

  }

  fseek(fp,0,SEEK_SET);

 int  i = 0 ;

  while(fgets(words[i++], 256, fp) != NULL) ;

  return words ;



}//end readWordsFile

void PrefixTree::insertNode(Node* parent, char* word, int cur)

{

  int word_length = strlen(word)-1 ;
cout<<"insert word: "<<word<<endl;
  if(cur==word_length)

  {

    return ;

  }

  int pos = word[cur]%26 ;

  if(parent->child[pos]==NULL)

  {

    //cout<<"inserting "<<word[cur]<<endl ;

    parent->child[pos] = new Node() ;

    parent->child[pos]->childNum++ ;

    parent->child[pos]->letter = word[cur] ;

    parent->child[pos]->parent = parent ;



   }

  else

  {

    parent->child[pos]->childNum++ ;

    //cout<<word[cur]<<" has been inserted."<<endl ;

  }



  insertNode(parent->child[pos],word,++cur) ;

}//end insertNode

void PrefixTree::createPrefixTree()

{
    cout<<"Create tree"<<endl;

  root = new Node() ;

  root->parent = NULL ;



  int word_Count = _msize(words)/sizeof(char*) ;
  cout<<word_Count<<endl;
  for(int i=0 ; i<word_Count ; i++)

  {

    cout<<words[i]<<endl;

    insertNode(root,words[i],0) ;

  }



}//end createPrefixTree

int main(int argc, char* argv[])

{
cout<<"main"<<endl;
  PrefixTree* tree = new PrefixTree("words.txt") ;

  tree->getPrefix() ;

  delete tree ;

  return 0 ;

}
