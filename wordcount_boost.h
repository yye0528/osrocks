/*
 * wordcount.h
 *
 *  Created on: Oct 23, 2013
 *      Author: ryan
 *
 *
 * wordcount.cpp consists of the class wordCount and a demo program main(),
 * which invites user to input a text ENDING WITH '$' and display the result.
 *
 *
 *
 *  Created on: Nov 2, 2013
 *      Author: ryan
 */

#ifndef WORDCOUNT_H_
#define WORDCOUNT_H_


#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

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
	string getSummary();

	void printFWord();
	void printFAlphabet();
	void print() const;

	string toUpper();
	string toLower();

	void setStr(string&);
	void count();

	wordCount(string&);
	wordCount();
	void loadFromFile(char fname[50]);


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
string wordCount::toLower()
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
	boost::regex wordPat("\\b([\\w'-]*)\\b");
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
		string word=match[0].str();
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
wordCount::wordCount(){
	string str="";
	setStr(str);
}

void wordCount::loadFromFile(char fname[50])
{
	string str;
	ifstream infile;
	infile.open(fname);

	if (!infile)
	{
		cout << "error: file '"<<fname<<"' does not exist." << endl;
		return;
	}

	while (infile)
	{
		string tmp;
		getline(infile,tmp);
		str+=tmp;
	}

	infile.close();

	setStr(str);
}
string wordCount::getSummary(){
	string summary;

	summary+="To upper case:\n";
	summary+=toUpper();
	summary+="\n\n";
	summary+="To lower case:\n";
	summary+=toLower();
	summary+="\n\n";
	summary+="number of characters:";
	summary+=boost::lexical_cast<string>(getNchar());
	summary+="\n";
	summary+="number of word:";
	summary+=boost::lexical_cast<string>(getNword());
	summary+="\n";
	summary+="number of sentences:";
	summary+=boost::lexical_cast<string>(getNSentence());
	summary+="\n\n";

	summary+= "Word frequency:\n";
	for (frequencyMap::iterator it = fWord.begin(); it != fWord.end(); ++it )
	{
		summary+= " ";
		summary+= it->first;
		summary+= ":";
		summary+=boost::lexical_cast<string>(it->second);
		summary+="\n";
	}
	summary+= "\nAlphabet frequency:\n";
	for (frequencyMap::iterator it = fAlpha.begin(); it != fAlpha.end(); ++it )
	{
		summary+= " ";
		summary+= it->first;
		summary+= ":";
		summary+=boost::lexical_cast<string>(it->second);
		summary+="\n";
	}
	return summary;
}




#endif /* WORDCOUNT_H_ */
