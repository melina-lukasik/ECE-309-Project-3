// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <stack>

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {
    
    // GUARANTEED (Fuhgettaboutit) :
    //              Numbers & operators alternate, Parentheses balanced
    //              Non-empty list, Doesn't start or end with an operator
    
    // kinds: Number, Plus, Minus, Times, Divide, LParen, RParen

    double evaluated_number;

    // Two stacks of CalcTokens, for values and operators
    std::stack<CalcToken> values;
    std::stack<CalcToken> ops;

    // Add tokens to the values and ops stacks
    for(auto& iter : tokens){
        // Numbers get pushed to values
        if(iter.kind == CalcToken::Kind::Number){
            values.push(iter);
            continue;
        }

        // Right parenthesis begins the process of Applying all the prior operators
        if(iter.kind == CalcToken::Kind::RParen){
            
            while(!ops.empty() && (ops.top().kind != CalcToken::Kind::LParen)){
                // Apply top operation to top values
                if(!calc_apply(values, ops, error)){ return false; }
            }

            // In case ops is unexpectedly empty
            // To prevent top() from being called in that extreme case
            if(ops.empty()){
                *error = "unexpected empty stack; possible token balancing error";
                return false;
            }

            // Pop the left parenthesis
            ops.pop();
        }

        // For other operators, Apply previous operator if
        // 1.) it isn't '('
        // 2.) previous operator has greater than or equal precedence
        else if((!ops.empty()) && (ops.top().kind != CalcToken::Kind::LParen) &&
                (iter.kind != CalcToken::Kind::LParen) &&
                (calc_get_precedence(ops.top()) >= calc_get_precedence(iter))){
            
            // Apply previous operation to top values
            if(!calc_apply(values, ops, error)){ return false; }
            
            // Push current operation
            ops.push(iter);
        }
        else{
            // Push current operation
            ops.push(iter);
        }
    }

    // Tokens expended. Apply the remaining operations
    while(!ops.empty()){
        // Apply top operation to top values
        if(!calc_apply(values,ops,error)){ return false; }
    }

    // Last remaining value is the result of the calculation
    evaluated_number = values.top().value;
    *out = evaluated_number;
    *error = "none";
    return true;
}

// Performs the operation: "<second-highest value> <top operator> <top value>",
// popping the top value and operator and replacing the second-highest value with the result
bool calc_apply(std::stack<CalcToken>& values, std::stack<CalcToken>& ops, std::string* error){
    // In case ops or values are unexpectedly empty
    // To prevent top() from being called in that extreme case
    if(ops.empty() || (values.size() < 2)){
        *error = "unexpected empty stack; possible token balancing error";
        return false;
    }
     
    // Get and pop right operand
    double right_val = values.top().value;
    values.pop();

    // Apply to left operand
    // It's a struct, so we can set the value directly, no need to pop and then push it again
    switch (ops.top().kind){
        case CalcToken::Kind::Plus:
            values.top().value = values.top().value + right_val;
            break;
        case CalcToken::Kind::Minus:
            values.top().value = values.top().value - right_val;
            break;
        case CalcToken::Kind::Times:
            values.top().value = values.top().value * right_val;
            break;
        case CalcToken::Kind::Divide:
            // No dividing by zero allowed, sorry </3
            if(right_val == 0){
                *error = "division by zero";
                return false;
            }
            values.top().value = values.top().value / right_val;
            break;
        default:
            break;
    }
    // Pop operator after use
    ops.pop();
    return true;
}

// Returns the precedence of an operator
int calc_get_precedence(CalcToken oper){
    if((oper.kind == CalcToken::Kind::Times) || (oper.kind == CalcToken::Kind::Divide)){
        return 2;
    }
    return 1;
}