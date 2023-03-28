
template<typename TreeType, typename ReadType, typename HelpersType>
/**
 * Membros Privados
 */

int &YamgTree<TreeType, ReadType, HelpersType>::
_getChildCost(int &child) {
    if(child == 0) return this->matched_rules.at(0).second;

    for( TreeType& c: this->getChildren() ){
        child--;
        int& value = c._getChildCost(child);
        if(child==0) return value;
    }
    return this->matched_rules.at(0).second; // linha para o compilador não reclamar
}

template<typename TreeType, typename ReadType, typename HelpersType>
std::pair<int &, std::string> YamgTree<TreeType, ReadType, HelpersType>::
_getChildName(int &child, int &goal_child, std::vector<Yamg::RuleLimit_t> &limit){
    if(child == goal_child){
        std::pair<int&, std::string> par{ child, this->getName() };
        return par;
    }

    for(TreeType& c: this->getChildren()){
        child++;
        if(!limit.empty() && child !=goal_child && child == limit[0].first) {
            limit.erase(limit.begin());
            continue;
        }
        std::pair<int&, std::string> value = c._getChildName(child, goal_child, limit);
        if(child == goal_child) return value;
    }
    std::pair<int&, std::string> par{ child, this->getName() };
    return par;
}
template<typename TreeType, typename ReadType, typename HelpersType>
std::pair<int &, std::string> YamgTree<TreeType, ReadType, HelpersType>::
_getChildName(int &child, int &goal_child) {
    if(child == goal_child){
        std::pair<int&, std::string> par{ child, this->getName() };
        return par;
    }

    for(TreeType& c: this->getChildren()){
        child++;
        std::pair<int&, std::string> value = c._getChildName(child, goal_child);
        if(child == goal_child) return value;
    }
    std::pair<int&, std::string> par{ child, this->getName() };
    return par;
}

template<typename TreeType, typename ReadType, typename HelpersType>
TreeType *YamgTree<TreeType, ReadType, HelpersType>::
_getChild(int &child, int &goal_child, std::vector<Yamg::RuleLimit_t> &limit) {
    if(child == goal_child) return dynamic_cast<TreeType*>(this);
    for(TreeType &c: this->getChildren()) {
        child++;
        if(!limit.empty() && child != goal_child && child == limit[0].first) {
            limit.erase(limit.begin());
            continue;
        }
        TreeType *t = c._getChild(child, goal_child, limit);
        if(child == goal_child) return t;
    }
    return dynamic_cast<TreeType*>(this);
}
template<typename TreeType, typename ReadType, typename HelpersType>
TreeType *YamgTree<TreeType, ReadType, HelpersType>::
_getChild(int &child) {
    if(child == 0) return dynamic_cast<TreeType*>(this);
    for(TreeType &c: this->getChildren()) {
        child--;
        TreeType *t = c._getChild(child);
        if(child == 0) return t;
    }
    return dynamic_cast<TreeType*>(this);
}

template<typename TreeType, typename ReadType, typename HelpersType>
std::pair<int &, std::string> YamgTree<TreeType, ReadType, HelpersType>::
_getChildName(int &child){
    if(child == 0){
        std::pair<int&, std::string> par{ child, this->getName() };
        return par;
    }

    for(TreeType& c: this->getChildren()){
        child--;
        std::pair<int&, std::string> value = c._getChildName(child);
        if(child == 0) return value;
    }
    std::pair<int&, std::string> par{ child, this->getName() };
    return par;
}

template<typename TreeType, typename ReadType, typename HelpersType>
TreeType *YamgTree<TreeType, ReadType, HelpersType>::
_getImmediateChild(int &child) {
    if(child == 0) return dynamic_cast<TreeType*>(this);
    return &this->children.at(child-1);
}

/**
 * Membros Públicos
 */

template<typename TreeType, typename ReadType, typename HelpersType>
int& YamgTree<TreeType, ReadType, HelpersType>::
getChildCost(int child){
    return this->_getChildCost(child);
}

template<typename TreeType, typename ReadType, typename HelpersType>
std::string YamgTree<TreeType, ReadType, HelpersType>::
getChildName(int child, std::vector<Yamg::RuleLimit_t> limit) {
    int init = 0;
    return this->_getChildName(init, child, limit).second;
}
template<typename TreeType, typename ReadType, typename HelpersType>
std::string YamgTree<TreeType, ReadType, HelpersType>::
getChildName(int child) {
    int init = 0;
    return this->_getChildName(init, child).second;
}

template<typename TreeType, typename ReadType, typename HelpersType>
TreeType &YamgTree<TreeType, ReadType, HelpersType>::
getChild(int index, std::vector<Yamg::RuleLimit_t> limit) {
    int init = 0;
    return *(this->_getChild(init, index, limit));
}

template<typename TreeType, typename ReadType, typename HelpersType>
TreeType& YamgTree<TreeType, ReadType, HelpersType>::
getChild(int index) {
    int init = 0;
    return *(this->_getChild(init, index));
}

template<typename TreeType, typename ReadType, typename HelpersType>
TreeType& YamgTree<TreeType, ReadType, HelpersType>::
getImmediateChild(int index){
    return *(this->_getImmediateChild(index));
}
