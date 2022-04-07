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

        static MyArray<string> terminals;
        static MyArray<string> non_terminals;
        static MyArray<Rule> rules;
        static code_t user_code;
        static code_t header_code;

    public:

            /* A classe não pode ser instanciada */
        Helper() = delete;
        Helper(const Helper& h) = delete;
        Helper& operator=(const Helper& h) = delete;

            
            /* Métodos */
        static bool isTerm(const string &str);
        static bool isNonTerm(const string &str);

        static void newTerm(const string &str);
        static void newNonTerm(const string &str);
        static void newRule(const Rule&);

        static string ltrim(const string&);
        static string rtrim(const string&);
        static string trim(const string&);
        

            /* Getters e Setters */
        static MyArray<Rule>& getRules();
        static void setCode(code_t&);
        static code_t& getCode();
        static void setHeader(code_t&);
        static code_t& getHeader();
        static MyArray<string>& getTerms();
        static MyArray<string>& getNonTerms();

        static void semanticError(const string &str);
        static void setError();
        static bool getError();

};

#endif