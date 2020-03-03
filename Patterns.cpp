#include "Patterns.h"
using namespace std;


stringstream ExpStrategy::getValue(ComplexNumber &cn) const {
    stringstream ss;
    ss<<cn.getModul()<< "*e^(i*" <<cn.getArg() << ")";
    return ss;
}

stringstream AlgStrategy::getValue(ComplexNumber &cn) const {
    stringstream ss;
    ss<<cn.getRe()<<" + i*"<<cn.getIm();
    return ss;
}
stringstream TrigStrategy::getValue(ComplexNumber &cn) const {
    stringstream ss;
    ss<<cn.getModul()<< "*(cos" <<cn.getArg() << ") + sin("<<cn.getArg()<<"))";
    return ss;
}




Type::Type() {
    type_ = "";
    strategy = nullptr;
}
Type::Type(const Type &t) {
    type_ = t.type_;
    strategy=t.strategy;
}
Type::Type(const string &type) {
    type_ = type;
    if (type == "Algb"){
        strategy = new AlgStrategy();
    }
    if (type == "Exponential"){
        strategy = new ExpStrategy();
    }
    if (type == "Trigonometrical"){
        strategy = new TrigStrategy();
    }
}
Type &Type::operator=(Type t) {
    if (this != &t){
        type_ = t.type_;
        strategy = t.strategy;
    }
    return *this;
}
bool Type::operator==(const Type &t) const {
    return t.type_==type_;
}
Strategy &Type::getStrategy() {
    return *strategy;
}
vector<Type> TypeFactory::types = *(new vector<Type>());
Type &TypeFactory::getComplexNumberType(const string &str) {
    auto it = find(types.begin(),types.end(), Type(str));
    if (it == types.end()) {
        Type t(str);
        types.push_back(t);
        return types[types.size() -1];
    }
    else return *it;

}





ComplexNumber::ComplexNumber(const int re, const int im, const Type &t) {
    re_ = re;
    im_ = im;
    type_ = t;
}
ComplexNumber::ComplexNumber(const ComplexNumber &complexNumber) {
    re_ = complexNumber.re_;
    im_ = complexNumber.im_;
    type_ = complexNumber.type_;
}




void Builder::setRe() {
    re_ = rand()%50 -25;
}

void Builder::setExType() {
    type_ = TypeFactory::getComplexNumberType("Exponential");
}

void Builder::setTrigType() {
    type_ = TypeFactory::getComplexNumberType("Trigonometrical");
}

void Builder::setDotType() {
    type_ = TypeFactory::getComplexNumberType("Algb");
}

void Builder::setIm() {
    im_ = rand()%50 -25;
}

ComplexNumber Builder::getResult() const {
    ComplexNumber c (ComplexNumber(re_,im_, type_).clone());
    return c;
}





void DirectorExp::create() {
    builder_->setRe();
    builder_->setIm();
    builder_->setExType();
}
void DirectorTrig::create() {
    builder_->setRe();
    builder_->setIm();
    builder_->setTrigType();
}
void DirectorAlg::create() {
    builder_->setRe();
    builder_->setIm();
    builder_->setDotType();
}
void Director::setBuilder(Builder *builder) {
    builder_ = builder;
}




ComplexNumber Decorator::createExpComplexNumber() const {
    Builder builder;
    DirectorExp dir;
    dir.setBuilder(&builder);
    dir.create();
    ComplexNumber cn (builder.getResult());
    return cn;
}
ComplexNumber Decorator::createTrigComplexNumber() const {
    Builder builder;
    DirectorTrig dir;
    dir.setBuilder(&builder);
    dir.create();
    ComplexNumber cn (builder.getResult());
    return cn;
}
ComplexNumber Decorator::createComplComplexNumber() const {
    Builder builder;
    DirectorAlg dir;
    dir.setBuilder(&builder);
    dir.create();
    ComplexNumber cn (builder.getResult());
    return cn;
}




ComplexNumber ExpState::createComplexNumber() const {
    Decorator decorator;
    ComplexNumber cn = decorator.createExpComplexNumber();
    return  cn;
}
ComplexNumber TrigState::createComplexNumber() const {
    Decorator decorator;
    ComplexNumber cn =decorator.createTrigComplexNumber();
    return cn;
}
ComplexNumber AlgState::createComplexNumber() const {
    Decorator decorator;
    ComplexNumber cn =decorator.createComplComplexNumber();
    return cn;
}



bool ChainOfResponsibility::handle() const {
    if (!cmd_.execute()){
        return false;
    }
    else {
        if (nextCor_!= nullptr)
            return nextCor_->handle();
        else return true;
    }
}

void ChainOfResponsibility::setNext(std::string::const_iterator first, std::string::const_iterator last) {
    if (first != last){
        nextCor_ = new ChainOfResponsibility(*first);
        nextCor_->setNext(first+1, last);
    }
}


Composite::Composite(const string &s) {
    startChain->setNext(s.begin(), s.end());
}

bool Composite::startCheck() {
    return startChain->handle();
}


Array &Array::getInstance(istream &is) {
    if (Array::instance == nullptr)
        Array::instance = new Array(is);
    return *instance;
}

Array::Array(istream &is) {
    AbstractFactory af(is);
    vec = af.createArray();
}




void Fasade::OutSeventeenth(ostream &os) {
    Iterator it(*ar);
    InputVisitor iv{};
    if (doesExistSeventeenth) {
        for (; !it.isEnd(); it.Next()) {;
            if (it.getIndex()+1 == 17){
                iv.setStrategy(it.CurrentItem().getType().getStrategy());
                iv.visit(it.CurrentItem(),os);
            }
        }
    }
    else os<<"Wrong input";
}

void Fasade::CreateArray(istream &is) {
    ar = &Array::getInstance(is);
}



vector<ComplexNumber> AbstractFactory::createArray() {
    nReaderProxy rp;
    rp.read(is_);
    rp.check();
    crm.subscribe(fc);
    crm.notify(rp.GetCorrect());
    vector<ComplexNumber> main_vector;
    if (rp.GetCorrect()) {
        int size = atoi(rp.GetN().c_str());
        for (int i = 0; i<size; i++) {
            if (i>size/3 && i<size*2/3) {
                st = new TrigState();
            }
            if (i>=size*2/3) {
                st = new AlgState();
            }
            main_vector.push_back(st->createComplexNumber());
        }
        return main_vector;
    }
    else return main_vector;
}


void nReaderProxy::check() {
    Composite c(s);
    isCorrect = (c.startCheck() && atoi(s.c_str()) >= 17);
}
