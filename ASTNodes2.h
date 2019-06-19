
#ifndef __ASTNODES2_H__
#define __ASTNODES2_H__

#include <llvm/IR/Value.h>
#include <json/json.h>
#include <iostream>
#include <vector>

#include <memory>
#include <string>
#include <typeinfo>
#include <regex>

#include "json.hpp"
//puts("$1"); return $1;
using namespace std;
using json = nlohmann::json;

/*
using std::cout;
using std::endl;
using std::string;
using std::shared_ptr;
using std::make_shared;
*/
class CodeGenContext;

class NBlock;

class NStatement;

class NExpression;

class NVariableDeclaration;

typedef std::vector <shared_ptr<NStatement>> StatementList;
typedef std::vector <shared_ptr<NExpression>> ExpressionList;
typedef std::vector <shared_ptr<NVariableDeclaration>> VariableList;

std::unique_ptr <NExpression> LogError(const char *str);

class Node {
protected:
    const char m_DELIM = ':';
    string className;
public:
    Node() { className = __func__; }

    virtual ~Node() {}

    virtual string getClassName() const { return className; };



    virtual llvm::Value *codeGen(CodeGenContext &context) { return (llvm::Value *) 0; }

    virtual json AST_JSON_Generate() const { return json(); }


};

class NExpression : public Node {
public:
    NExpression() { className = __func__; }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        
        return j;
    }


};

class NStatement : public Node {
public:
    NStatement() { className = __func__; }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        return j;
    }


};

class NDouble : public NExpression {
public:
    double value;

    NDouble() { className = __func__; }

    NDouble(double value)
            : value(value) {
        // return "NDoub le=" << value << endl;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + std::to_string(value);
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NInteger : public NExpression {
public:
    uint64_t value;

    NInteger() { className = __func__; }

    NInteger(uint64_t value)
            : value(value) {

    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + std::to_string(value);
        return j;
    }


    operator NDouble() {
        return NDouble(value);
    }

    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NIdentifier : public NExpression {
public:
    std::string name;
    bool isType = false;
    bool isArray = false;

    shared_ptr <ExpressionList> arraySize = make_shared<ExpressionList>();

    NIdentifier() { className = __func__; }

    NIdentifier(const std::string &name)
            : name(name) {
        className = __func__;
        // return "NIdentifier=" << name << endl;
    }

    string getClassName() const override {
        return className;
    }

    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + name + (isArray ? "(Array)" : "");
        bool flag = false;
        json children;
        for (auto it = arraySize->begin(); it != arraySize->end(); it++) {
            if (flag == false) {
                flag = true;
            }
            children.push_back((*it)->AST_JSON_Generate());
        }
        if (flag == true) {
            j["children"] = children;
        }
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NMethodCall : public NExpression {
public:
    const shared_ptr <NIdentifier> id;
    shared_ptr <ExpressionList> arguments = make_shared<ExpressionList>();

    NMethodCall() {
        className = __func__;
    }

    NMethodCall(const shared_ptr <NIdentifier> id, shared_ptr <ExpressionList> arguments)
            : id(id), arguments(arguments) {
        className = __func__;
    }

    NMethodCall(const shared_ptr <NIdentifier> id)
            : id(id) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }

    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(this->id->AST_JSON_Generate());
        for (auto it = arguments->begin(); it != arguments->end(); it++) {
            children.push_back((*it)->AST_JSON_Generate());
        }
        j["children"] = children;
        return j;
    }




    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBinaryOperator : public NExpression {
public:
    int op;
    shared_ptr <NExpression> lhs;
    shared_ptr <NExpression> rhs;

    NBinaryOperator() { className = __func__; }

    NBinaryOperator(shared_ptr <NExpression> lhs, int op, shared_ptr <NExpression> rhs)
            : lhs(lhs), rhs(rhs), op(op) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + std::to_string(op);
        json children;

        children.push_back(lhs->AST_JSON_Generate());
        children.push_back(rhs->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }




    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NAssignment : public NExpression {
public:
    shared_ptr <NIdentifier> lhs;
    shared_ptr <NExpression> rhs;

    NAssignment() { className = __func__; }

    NAssignment(shared_ptr <NIdentifier> lhs, shared_ptr <NExpression> rhs)
            : lhs(lhs), rhs(rhs) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(lhs->AST_JSON_Generate());
        children.push_back(rhs->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NBlock : public NExpression {
public:
    shared_ptr <StatementList> statements = make_shared<StatementList>();

    NBlock() {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        cout<<"AST JSON Generate start!!"<<endl;
        json j;
        j["name"] = getClassName();
        bool flag = false;
        json children;
        for (auto it = statements->begin(); it != statements->end(); it++) {
            if (flag == false) {
                flag = true;
            }
            children.push_back((*it)->AST_JSON_Generate());
        }
        if (flag == true) {
            j["children"] = children;
        }
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NExpressionStatement : public NStatement {
public:
    shared_ptr <NExpression> expression;

    NExpressionStatement() { className = __func__; }

    NExpressionStatement(shared_ptr <NExpression> expression)
            : expression(expression) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(expression->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NVariableDeclaration : public NStatement {
public:
    const shared_ptr <NIdentifier> type;
    shared_ptr <NIdentifier> id;
    shared_ptr <NExpression> assignmentExpr = nullptr;

    NVariableDeclaration() { className = __func__; }

    NVariableDeclaration(const shared_ptr <NIdentifier> type, shared_ptr <NIdentifier> id,
                         shared_ptr <NExpression> assignmentExpr = NULL)
            : type(type), id(id), assignmentExpr(assignmentExpr) {
        className = __func__;
    //    cout << "isArray = " << type->isArray << endl;
        assert(type->isType);
        assert(!type->isArray || (type->isArray && type->arraySize != nullptr));
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(type->AST_JSON_Generate());
        children.push_back(id->AST_JSON_Generate());
        if (assignmentExpr != nullptr) {
            children.push_back(assignmentExpr->AST_JSON_Generate());
        }
        j["children"] = children;
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NFunctionDeclaration : public NStatement {
public:
    shared_ptr <NIdentifier> type;
    shared_ptr <NIdentifier> id;
    shared_ptr <VariableList> arguments = make_shared<VariableList>();
    shared_ptr <NBlock> block;
    bool isExternal = false;

    NFunctionDeclaration() { className = __func__; }

    NFunctionDeclaration(shared_ptr <NIdentifier> type, shared_ptr <NIdentifier> id,
                         shared_ptr <VariableList> arguments,
                         shared_ptr <NBlock> block, bool isExt = false)
            : type(type), id(id), arguments(arguments), block(block), isExternal(isExt) {
        className = __func__;
        assert(type->isType);
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(type->AST_JSON_Generate());
        children.push_back(id->AST_JSON_Generate());

        for (auto it = arguments->begin(); it != arguments->end(); it++) {
            children.push_back((*it)->AST_JSON_Generate());
        }

        assert(isExternal || block != nullptr);
        if (block) {
            children.push_back(block->AST_JSON_Generate());
        }
        j["children"] = children;
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NStructDeclaration : public NStatement {
public:
    shared_ptr <NIdentifier> name;
    shared_ptr <VariableList> members = make_shared<VariableList>();

    NStructDeclaration() { className = __func__; }

    NStructDeclaration(shared_ptr <NIdentifier> id, shared_ptr <VariableList> arguments)
            : name(id), members(arguments) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + this->name->name;
        bool flag = false;
        json children;
        for (auto it = members->begin(); it != members->end(); it++) {
            if (flag == false) {
                flag = true;

            }
            children.push_back((*it)->AST_JSON_Generate());
        }
        if (flag == true) {
            j["children"] = children;
        }
        return j;
    }


    virtual llvm::Value *codeGen(CodeGenContext &context) override;
};

class NReturnStatement : public NStatement {
public:
    shared_ptr <NExpression> expression;

    NReturnStatement() { className = __func__; }

    NReturnStatement(shared_ptr <NExpression> expression)
            : expression(expression) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(expression->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }



    virtual llvm::Value *codeGen(CodeGenContext &context) override;

};

class NIfStatement : public NStatement {
public:

    shared_ptr <NExpression> condition;
    shared_ptr <NBlock> trueBlock;          // should not be null
    shared_ptr <NBlock> falseBlock;         // can be null


    NIfStatement() { className = __func__; }

    NIfStatement(shared_ptr <NExpression> cond, shared_ptr <NBlock> blk, shared_ptr <NBlock> blk2 = nullptr)
            : condition(cond), trueBlock(blk), falseBlock(blk2) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(condition->AST_JSON_Generate());
        children.push_back(trueBlock->AST_JSON_Generate());
        if (falseBlock) {
            children.push_back(falseBlock->AST_JSON_Generate());
        }
        j["children"] = children;
        return j;
    }


    llvm::Value *codeGen(CodeGenContext &context) override;


};

class NForStatement : public NStatement {
public:
    shared_ptr <NExpression> initial, condition, increment;
    shared_ptr <NBlock> block;

    NForStatement() { className = __func__; }

    NForStatement(shared_ptr <NBlock> b, shared_ptr <NExpression> init = nullptr,
                  shared_ptr <NExpression> cond = nullptr,
                  shared_ptr <NExpression> incre = nullptr)
            : block(b), initial(init), condition(cond), increment(incre) {
        className = __func__;
        if (condition == nullptr) {
            condition = make_shared<NInteger>(1);
        }
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        if (initial) {
            children.push_back(initial->AST_JSON_Generate());
        }

        if (condition) {
            children.push_back(condition->AST_JSON_Generate());
        }

        if (increment) {
            children.push_back(increment->AST_JSON_Generate());
        }

        j["children"] = children;
        return j;
    }


    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NStructMember : public NExpression {
public:
    shared_ptr <NIdentifier> id;
    shared_ptr <NIdentifier> member;

    NStructMember() { className = __func__; }

    NStructMember(shared_ptr <NIdentifier> structName, shared_ptr <NIdentifier> member)
            : id(structName), member(member) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(id->AST_JSON_Generate());
        children.push_back(member->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }


    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NArrayIndex : public NExpression {
public:
    shared_ptr <NIdentifier> arrayName;
//    shared_ptr<NExpression>  expression;
    shared_ptr <ExpressionList> expressions = make_shared<ExpressionList>();

    NArrayIndex() { className = __func__; }

    NArrayIndex(shared_ptr <NIdentifier> name2, shared_ptr <NExpression> exp)
            : arrayName(name2) {
        className = __func__;
        expressions->push_back(exp);
    }


    NArrayIndex(shared_ptr <NIdentifier> name2, shared_ptr <ExpressionList> list)
            : arrayName(name2), expressions(list) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(arrayName->AST_JSON_Generate());
        for (auto it = expressions->begin(); it != expressions->end(); it++) {
            children.push_back((*it)->AST_JSON_Generate());
        }
        j["children"] = children;
        return j;
    }

    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NArrayAssignment : public NExpression {
public:
    shared_ptr <NArrayIndex> arrayIndex;
    shared_ptr <NExpression> expression;

    NArrayAssignment() { className = __func__; }

    NArrayAssignment(shared_ptr <NArrayIndex> index, shared_ptr <NExpression> exp)
            : arrayIndex(index), expression(exp) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(arrayIndex->AST_JSON_Generate());
        children.push_back(expression->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }


    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NArrayInitialization : public NStatement {
public:

    NArrayInitialization() { className = __func__; }

    shared_ptr <NVariableDeclaration> declaration;
    shared_ptr <ExpressionList> expressionList = make_shared<ExpressionList>();

    NArrayInitialization(shared_ptr <NVariableDeclaration> dec, shared_ptr <ExpressionList> list)
            : declaration(dec), expressionList(list) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(declaration->AST_JSON_Generate());

        for (auto it = expressionList->begin(); it != expressionList->end(); it++)
            children.push_back((*it)->AST_JSON_Generate());
        //   children.push_back(rhs->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }


    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NStructAssignment : public NExpression {
public:
    shared_ptr <NStructMember> structMember;
    shared_ptr <NExpression> expression;

    NStructAssignment() { className = __func__; }

    NStructAssignment(shared_ptr <NStructMember> member, shared_ptr <NExpression> exp)
            : structMember(member), expression(exp) {
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }



    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName();
        json children;

        children.push_back(structMember->AST_JSON_Generate());
        children.push_back(expression->AST_JSON_Generate());
        j["children"] = children;
        return j;
    }

    llvm::Value *codeGen(CodeGenContext &context) override;

};

class NLiteral : public NExpression {
public:
    string value;

    NLiteral() { className = __func__; }

    NLiteral(const string &str) {
        value = str.substr(1, str.length() - 2);
        className = __func__;
    }

    string getClassName() const override {
        return className;
    }

 


    json AST_JSON_Generate() const override {
        json j;
        j["name"] = getClassName() + this->m_DELIM + value;

        return j;
    }

    llvm::Value *codeGen(CodeGenContext &context) override;

};


#endif
