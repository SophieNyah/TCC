#ifndef GENERATOR_HELPER_HPP
#define GENERATOR_HELPER_HPP

#include<list>
#include<string>

using namespace std;

class Helper {
    
    private:

        static bool hadError;

        static list<string> terminals;
        static list<string> non_terminals;
    
        static void insertIntoList(list<string> &list, const string &str);
        static bool findInList(const list<string> &list, const string &str);

    public:

        Helper();

        static bool error();

        static bool isTerm(const string &str);
        static bool isNonTerm(const string &str);

        static void newTerm(const string &str);
        static void newNonTerm(const string &str);

        static void semanticError(const string &str);

};

#endif