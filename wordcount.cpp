/*
 * wordcount.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: ryan
 *
 *
 * wordcount.cpp consists of the class wordCount and a demo program main(),
 * which invites user to input a text ENDING WITH '$' and display the result.
 *
 */

#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <boost/regex.hpp>

using namespace std;

typedef map<string, unsigned long> frequencyMap;

class wordCount{
public:

	unsigned long getNchar() const;
	unsigned long getNword() const;
	unsigned long getNSentence() const;
	frequencyMap getFWord() const;
	frequencyMap getFaphabet() const;
	unsigned long getFWord(string);
	unsigned long getFaphabet(string);

	void printFWord();
	void printFAlphabet();
	void print() const;

	string toUpper();
	string toLodwer();

	void setStr(string&);
	void count();
	wordCount(string&);

private:
	unsigned long nChar;
	unsigned long nWord;
	unsigned long nSentence;
	frequencyMap fWord;
	frequencyMap fAlpha;

	string inputStr;
};


unsigned long wordCount::getNchar() const
{
	return nChar;
}
unsigned long wordCount::getNword() const{
	return nWord;
}
unsigned long wordCount::getNSentence() const{
	return nSentence;
}
frequencyMap wordCount::getFWord() const
{
	return fWord;
}
frequencyMap wordCount::getFaphabet() const
{
	return fAlpha;
}
unsigned long wordCount::getFWord(string str)
{
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return fWord[str];
}
unsigned long wordCount::getFaphabet(string alpha)
{
	transform(alpha.begin(), alpha.end(), alpha.begin(), ::tolower);
	return fAlpha[alpha];
}
void wordCount::printFWord()
{
	cout << "Word frequency:"<<endl;
	for (frequencyMap::iterator it = fWord.begin(); it != fWord.end(); ++it )
		cout << " " << it->first << ":" << it->second <<endl;
}
void wordCount::printFAlphabet()
{
	cout << "Alphabet frequency:"<<endl;
	for (frequencyMap::iterator it = fAlpha.begin(); it != fAlpha.end(); ++it )
		cout << " " << it->first << ":" << it->second <<endl;
}

string wordCount::toUpper()
{
	string tmp=inputStr;
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
	return tmp;
}
string wordCount::toLodwer()
{
	string tmp=inputStr;
	transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
	return tmp;
}

void wordCount::setStr(string& str)
{
	inputStr=str;
	count();
}
void wordCount::count()
{
	string str;
	boost::smatch match;
	boost::regex alphaPat("([a-z])");
	boost::regex wordPat("\\b([^ ]*)\n|\\b([^ ]*)\\b");
	boost::regex sentencePat("([^ ]*)\n");
	unsigned long wordCount=0;
	unsigned long charCount=0;
	unsigned long sentenceCount=1;
	frequencyMap wordFreq;
	frequencyMap alphaFreq;

	//loop through all matches of word and increment counts
	str=inputStr;
	while(boost::regex_search(str,match,wordPat)){
		//choose a submatch that is properly trimmed
		int n=(regex_search(match[0].str(),sentencePat))?1:2;
		string word=match[n].str();
		//transform all the words to lower case for easy map index
		transform(word.begin(), word.end(), word.begin(), ::tolower);
		//increment all the counts
		charCount+=word.length();
		wordCount++;
		wordFreq[word]=wordFreq[word]+1;
		for (unsigned int i=0; i<word.length(); i++){
			string c=word.substr(i,1);
			if(regex_search(c,alphaPat))
				alphaFreq[c]=alphaFreq[c]+1;
		}
		//go to next match
		str=match.suffix().str();
	}

	//loop through all matches of '/n' and increment counts
	str=inputStr;
	while(boost::regex_search(str,match,sentencePat)){
		sentenceCount++;
		str=match.suffix().str();
	}

	nChar=charCount;
	nWord=wordCount;
	nSentence=sentenceCount;
	fAlpha=alphaFreq;
	fWord=wordFreq;

}
void wordCount::print() const
{
	cout<<"the input string is: "<<inputStr<<endl;
}
wordCount::wordCount(string& str)
{
	setStr(str);
}

int main()
{
	string usrInput;

	cout<<"please input a string:"<<endl;
	getline(std::cin,usrInput,'$');
	wordCount wc=wordCount(usrInput);

	cout<<"number of characters:";
	cout<<wc.getNchar()<<endl;

	cout<<"number of word:";
	cout<<wc.getNword()<<endl;

	cout<<"number of sentences:";
	cout<<wc.getNSentence()<<endl;

	wc.printFWord();
	cout<<endl;
	wc.printFAlphabet();
	cout<<endl;

	cout<<"to upper:"<<endl;
	cout<<wc.toUpper()<<endl;

	cout<<"to lower:"<<endl;
	cout<<wc.toLodwer()<<endl;
	return 1;
}
