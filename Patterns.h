#pragma once
#include <iostream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
class Director;
class ComplexNumber;
class Builder;
class Strategy;
class AlgStrategy;
class TrigStrategy;
class ExpStrategy;
class Type;

/////////////////interface for listeners (this role plays Facade)
class Listener{
public:
    virtual void update(bool b)=0;
};

//////////Strategy
class Strategy {
public:
    virtual std::stringstream getValue(ComplexNumber& cn) const=0;
};

class ExpStrategy : public Strategy{
public:
    std::stringstream getValue(ComplexNumber& cn)const override;
};

class AlgStrategy : public Strategy{
public:
    std::stringstream getValue(ComplexNumber& cn)const override;
};

class TrigStrategy : public Strategy{
public:
    std::stringstream getValue(ComplexNumber& cn)const override;
};

//////////////Flyweight
class Type{
public:
    Type();
    Type(const Type& t);
    Type(const std::string& type);
    Type& operator= (Type t);
    bool operator == (const Type& t) const;
    Strategy& getStrategy();
private:
    Strategy* strategy;
    std::string type_;
};
////////////And Flyweight Factory
class TypeFactory{
public:
    static Type& getComplexNumberType(const std::string& str);;
private:
    static std::vector<Type> types;
};


////////////// Prototype is a member of my Array
class ComplexNumber {
public:
    ComplexNumber (const int re,const int im, const Type& t);
    ComplexNumber (const ComplexNumber& complexNumber);
    int getRe() const{
        return re_;
    }
    int getIm() const{
        return im_;
    }

    double getModul() const{
        return sqrt(im_*im_ + re_*re_);
    }
    double getArg() const{
        return atan(((double)im_)/re_);
    }
    Type getType() const{
        return type_;
    }
    ComplexNumber& clone(){
        return *this;
    }
private:
    int re_;
    int im_;
    Type type_;
};



///////////Builder
class Builder{
public:
    Builder (){
        srand(time(0));
    }
    void setRe();;
    void setExType();;
    void setTrigType();;
    void setDotType();;
    void setIm();;
    ComplexNumber getResult() const;
private:
    int re_;
    int im_;
    Type type_;
};
////////And Builder's Directors, which is bound with Builder using the Bridge
class Director{
protected:
    Builder* builder_;
public:
    void setBuilder(Builder* builder);
    virtual void create() =0;
};
class DirectorExp :public Director{
public:
    void create() override;
};
class DirectorTrig: public Director{
public:
    void create() override;
};
class DirectorAlg: public Director{
public:
    void create() override;
};

//////////////Decorator
class Decorator {
public:
    ComplexNumber createExpComplexNumber() const;
    ComplexNumber createTrigComplexNumber() const;
    ComplexNumber createComplComplexNumber() const;
};

/////////////// State
class State {
public:
    virtual ComplexNumber createComplexNumber() const =0;
};
class ExpState: public State{
    ComplexNumber createComplexNumber()const override;
};
class TrigState: public State{
    ComplexNumber createComplexNumber()const override;
};
class AlgState: public State{
    ComplexNumber createComplexNumber()const override;
};

/////////////Command
class Command{
public:
    Command() {
        c_ = '0';
    }
    Command(const char c): c_(c) {};
    bool execute () const{
        return c_>='0' && c_<='9';
    };
private:
    char c_;
};

/////////////////ChainOfResponsibility
class ChainOfResponsibility{
public:
    ChainOfResponsibility (const char c) : cmd_(Command(c)), nextCor_(nullptr)  { }
    ChainOfResponsibility (ChainOfResponsibility* cor) : nextCor_(cor) { }
    void setNext(std::string::const_iterator first, std::string::const_iterator last);
    bool handle () const;
private:
    Command cmd_;
    ChainOfResponsibility *nextCor_;
};

//////////Composite make a tree of ChainOfResponsibility
class Composite {
public:
    Composite(){};
    explicit Composite(const std::string& s);
    bool startCheck ();
private:
    ChainOfResponsibility *startChain;
};

////////// Proxy, which make check of input before creating the Array
class nReaderProxy {
public:
    void read(std::istream& is){ getline(is,s);  isCorrect = true; }
    void check ();
    std::string GetN() const{
        return s;
    };
    bool GetCorrect() const{
        return isCorrect;
    }
private:
    bool isCorrect;
    std::string s;
};
/////////// Singelton
class Array{
public:
    static Array& getInstance(std::istream& is);
    static void push_back(ComplexNumber& cn){
        vec.push_back(cn);
    }
    ComplexNumber& operator[](int idx) const{
        return vec[idx];
    }
    static void pop_back(){
        vec.pop_back();
    }
    size_t getSize() const{
        return vec.size();
    }
private:
    explicit Array(std::istream& is);;
    static Array* instance;
    static std::vector<ComplexNumber> vec;
};
std::vector<ComplexNumber> Array::vec = *(new std::vector<ComplexNumber>());
Array* Array::instance = nullptr;


/////////Facade
class Fasade: public Listener {
public:
    void CreateArray(std::istream& is);
    void update(bool b) override {
        doesExistSeventeenth = b;
    }
    void OutSeventeenth(std::ostream &os);
private:
    bool doesExistSeventeenth = true;
    Array* ar{};
};

static Fasade fc{};
///////////Observer
class CorrectManager{
public:
    void subscribe(Fasade& f){
        fac = &f;
    }
    void notify(bool b){
        fac->update(b);
    }
private:
    Fasade *fac;
};

///////////Abstract Factory
class AbstractFactory {
public:
    explicit AbstractFactory(std::istream& is): is_(is), st(new ExpState()){
    }

    std::vector<ComplexNumber> createArray();
private:
    CorrectManager crm;
    State *st;
    std::istream& is_;
};

//////////Visitor
class Visitor {
public:
    virtual void visit(ComplexNumber& cn)=0;
};


//////////Adapter, which adapt stringstream interface to string interface
class Adapter{
public:
    explicit Adapter(Strategy& strategy1){
        strategy = &strategy1;
    }
    std::string getValues(ComplexNumber& cn) const{
        std::string s = strategy->getValue(cn).str();
        return  s;
    }
private:
    Strategy *strategy;
};

class InputVisitor {
public:
    void visit(ComplexNumber& cn, std::ostream& os)const  {
        os<<ad_->getValues(cn);
    }
    void setStrategy(Strategy& strategy1) {
        Adapter ad(strategy1);
        ad_ = &ad;
    }
protected:
    Adapter *ad_;
};


/////////////Iterator
class Iterator{

public:
    explicit Iterator() : index(0) {};
    Iterator(Array &elem) : arr_(&elem), index(0) {};
    void operator = (const Iterator elem) {
        arr_ = elem.arr_;
        index = elem.index;
    }

    void First() {
        index = 0;
    }
    void Last() {
        index = arr_->getSize();
    }

    void Next() { if (index < arr_->getSize()) {
        index++;
    }
    }
    void Previous() {
        if (index > 0) index--;
    }
    int getIndex (){
        return index;
    }
    bool isEnd() {
        return (index == arr_->getSize());
    }
    ComplexNumber& CurrentItem() const {
        return arr_->operator[](index);
    }
private:
    Array* arr_;
    int index;
};
