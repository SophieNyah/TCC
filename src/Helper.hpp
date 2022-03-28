#ifndef GENERATOR_HELPER_HPP
#define GENERATOR_HELPER_HPP

#include<vector>
#include<string>
#include"Rule.hpp"
#include"types.hpp"

using namespace std;

class Helper {
    
    private:

        static bool hadError;

        static array<string> terminals;
        static array<string> non_terminals;
        static array<Rule> rules;

    public:

            /* A classe não pode ser instanciada */
        Helper() = delete;
        Helper(const Helper& h) = delete;
        Helper& operator=(const Helper& h) = delete;

            
        static bool isTerm(const string &str);
        static bool isNonTerm(const string &str);

        static void newTerm(const string &str);
        static void newNonTerm(const string &str);
        static void newRule(const Rule&);

        static array<Rule>& getRules();

        static void semanticError(const string &str);
        static void setError();
        static bool getError();

};

#endif