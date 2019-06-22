#include <iostream>
#include <typeinfo>
#include <regex>

using namespace std;

class Node {
private:
    const char colon = ':';
    const char *m_PREFIX = "--";
    string name;
    //   bool flag= false;
public:

    Node() {
        name = __func__;
        cout << name << endl;
        //   name = init();
    }

    virtual string init() {
        static string name;
        static bool flag = false;
        //   cout << "Node init!!" << endl;

        if (flag == true) {
            //         cout << "Node has init!!" << endl;
            return name;
        }
        flag = true;
        name = typeid(this).name();
        cout << name << endl;
        int index = name.find("N");
        if (index < 0) {
            cout << "node name not match!!" << endl;
        } else {
            name = name.substr(index);
            cout << name << endl;
        }
        return name;
/*
        regex search("*N(.*)");
        smatch matchResult;
        if(regex_match(name,matchResult,search)){
            cout << "Match: ";
            //打印子表达式结果
            for (size_t i = 1; i < matchResult.size(); ++i)
            {
                cout << matchResult[i] << " ";
            }
        }else{
            cout<<"node name not match!!"<<endl;

        }
        */
        //  name= typeid(this).name();

    }

    virtual ~Node() {}

    virtual string getTypeName() const = 0;

    virtual void print(string prefix) const {}
};

class NExpression : public Node {
private:
    string name;

public:
    //   string name;
    NExpression() {
        name = __func__;
        cout << name << endl;
        //   name = init();
        //      name = init();
        //  name= typeid(this).name();
        //   cout << typeid(this).name() << endl;
    }

    string init() {
        static string name;
        static bool flag = false;
        //   cout << "Node init!!" << endl;

        if (flag == true) {
            //         cout << "Node has init!!" << endl;
            return name;
        }
        flag = true;
        name = typeid(this).name();
        cout << name << endl;
        int index = name.find("N");
        if (index < 0) {
            cout << "node name not match!!" << endl;
        } else {
            name = name.substr(index);
            cout << name << endl;
        }
        return name;
/*
        regex search("*N(.*)");
        smatch matchResult;
        if(regex_match(name,matchResult,search)){
            cout << "Match: ";
            //打印子表达式结果
            for (size_t i = 1; i < matchResult.size(); ++i)
            {
                cout << matchResult[i] << " ";
            }
        }else{
            cout<<"node name not match!!"<<endl;

        }
        */
        //  name= typeid(this).name();

    }

    string getTypeName() const override {
        return "NExpression";
    }

    virtual void print(string prefix) const override {
        cout << prefix << getTypeName() << endl;
    }

    void print2() {
        cout << typeid(this).name() << endl;
    }

};

class NStatement : public Node {
public:
    //  string name;
    NStatement() {
        //     name= typeid(this).name();
        //     cout << typeid(this).name() << endl;
    }

    string getTypeName() const override {
        return "NStatement";
    }

    virtual void print(string prefix) const override {
        cout << prefix << getTypeName() << endl;
    }


};

int main() {
    //  bool  NExpression::flag= false;
    //   NExpression.flag= false;
    NExpression a;
    NExpression b;
    //  a.print2();
    //  NStatement b;
    //= new NExpression();
    //  a->print2();
}