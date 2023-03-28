#ifndef ABSTRACT_SYNTAX_TREE_H
#define ABSTRACT_SYNTAX_TREE_H

#include<string>
#include<vector>
#include<limits>
#include<optional>
#include<type_traits>
#include <map>
#include"types.hpp"

template<typename T>
class TemplateTree{

    protected:

        std::string name;
        int user_symbol;
        std::vector<T> children;
        code_t action;
        Node_type type;

    public:

            /* Construtores */
        TemplateTree(){}
        TemplateTree(const std::string& name, const int user_symbol, const Node_type& type)
            : name{ name }, user_symbol{ user_symbol }, type{ type }{}
        TemplateTree(const std::string& name, const Yamg::User_Symbols user_symbol, const Node_type& type)
            : name{ name }, user_symbol{ (int)user_symbol }, type{ type }{}
        TemplateTree(const std::string& name, const int user_symbol, const Node_type& type, const code_t& action)
            : name{ name }, user_symbol{ user_symbol }, type{ type }, action{ action }{}


            /* Getters/Setters */
        std::string getName(){ return this->name; }
        Node_type getType(){ return this->type; }
        int getSymbol(){ return this->user_symbol; }
        code_t  getAction(){ return this->action; }
        void setAction(code_t acao){ this->action = acao; }
        void setName(const std::string& name){ this->name = name; }
        void setSymbol(const Yamg::User_Symbols symbol){ this->user_symbol = (int)symbol; }
        int size(){
            int size{ 1 };
            for(T child: this->children){
                size += child.size();
            }
            return size;
        }
        int nonTermsSize(bool (*isNonTerm)(const std::string&)) {
            int size{0};
            if(isNonTerm(this->name)) size++;
            for(T child: this->children) {
                size += child.nonTermsSize(isNonTerm);
            }
            return size;
        }
        std::vector<int> nonTermsIndexes(bool (*isNonTerm)(const std::string&), int currIteration=0){
            std::vector<int> nonTerms{};
            if(isNonTerm(this->name))  nonTerms.emplace_back(currIteration);
            for(T child: this->children){
                currIteration++;
                auto v = child.nonTermsIndexes(isNonTerm, currIteration);
                nonTerms.insert(nonTerms.end(), v.begin(), v.end());
            }
            return nonTerms;
        }

        template<typename Funcao, typename... Args>
        void map(Funcao f, Args... a) {
            f(this, a...);
            for(T &child: this->children) { child.map(f, a...); }
        }

            /* Atributos */
        int children_size{0};

            /* Métodos */
        void insertChild(const T& c){
            this->children.push_back(c);
            this->children_size++;
        }
        
        std::optional<T> getChild(int index){
            try{
                return this->children.at(index);
            }
             catch (const std::out_of_range& e) {
                return std::nullopt;
            }
        }

        T& getChildReference(int index){
            return this->children.at(index);
        }
        
        std::vector<T>& getChildren(){ return this->children; }

};


class BasicTree: public TemplateTree<BasicTree>{
    public:
        BasicTree();
        BasicTree(const std::string& name, const int non_term, const Node_type& type);
        BasicTree(const std::string& name, const Yamg::User_Symbols non_term, const Node_type& type);
        BasicTree(const std::string& name, const int non_term, const Node_type& type, const code_t& action);  

        Cost_expression cost;  
};


template<typename TreeType, typename ReadType, typename HelpersType = std::string>
class YamgTree: public TemplateTree<TreeType>{
//    static_assert(std::is_base_of<YamgTree, TreeType>::value, "TreeType deve ser uma subclasse de YamgTree");

    private:

        int& _getChildCost(int& child);

        std::pair<int&, std::string> _getChildName(int& child, int& goal_child, std::vector<Yamg::RuleLimit_t> &limit);
        std::pair<int&, std::string> _getChildName(int& child, int& goal_child);

        TreeType* _getChild(int& child, int& goal_child, std::vector<Yamg::RuleLimit_t> &limit);
        TreeType* _getChild(int& child);
        std::pair<int&, std::string> _getChildName(int& child);

        TreeType* _getImmediateChild(int &child);

    protected:

        YamgTree(){}
        YamgTree(const std::string& name, const Yamg:: User_Symbols non_term, const Node_type& type)
            : TemplateTree<TreeType>{ name, non_term, type }
            {}

    public:

        /**
         * Função de leitura da AST.
         * A maneira como a leitura é feita a discrição do usuário, mas o argumento 'this' deve se tornar a árvore lida.
         */
        virtual void readTree(ReadType) = 0;

        /**
         * Verifica qual o Node_type do nó passado.
         * Uma possível implementação é ler o 'name' da árvore para determinar o valor.
         * @return Node_type do nó.
         */
        virtual Node_type readNodeType(HelpersType) = 0;
        /**
         * Verifica qual o User_Symbols do nó passado.
         * Uma possível implementação é ler o 'name' da árvore para determinar o valor.
         * @return User_Symbols do nó.
         */
        virtual Yamg::User_Symbols readUserSymbol(HelpersType) = 0;

        virtual ~YamgTree() = default;

        MyArray<int> non_terminal{};
        MyArray<std::pair<Yamg::Rules, cost_t>> matched_rules{ std::pair<Yamg::Rules, cost_t>{static_cast<Yamg::Rules>(-1), std::numeric_limits<cost_t>::max()} };
        cost_t cost{ 0 };
        Cost_expression cost_expression{};

        MyArray<int> getNonTerms() { return this->non_terminal; }

        int& getChildCost(int child);

        std::string getChildName(int child, std::vector<Yamg::RuleLimit_t> limit);
        std::string getChildName(int child);

        TreeType& getChild(int index, std::vector<Yamg::RuleLimit_t> limit);
        TreeType& getChild(int index);

        TreeType& getImmediateChild(int index);
};

#include"Tree.tpp"

#endif