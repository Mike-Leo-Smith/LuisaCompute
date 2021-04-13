//
// Created by Mike Smith on 2021/3/13.
//

#include "ast/variable.h"
#include <core/logging.h>
#include <ast/expression.h>
#include <ast/function_builder.h>

namespace luisa::compute {

void RefExpr::_mark(Variable::Usage usage) const noexcept {
    FunctionBuilder::current()->mark_variable_usage(_variable.uid(), usage);
}

void CallExpr::_mark(Variable::Usage) const noexcept {
    if (is_builtin()) {
        if (_op == CallOp::IMAGE_WRITE) {
            _arguments[0]->mark(Variable::Usage::WRITE);
            for (auto i = 1u; i < _arguments.size(); i++) {
                _arguments[i]->mark(Variable::Usage::READ);
            }
        } else {
            for (auto arg : _arguments) {
                arg->mark(Variable::Usage::READ);
            }
        }
    } else {
        auto f = Function::callable(_uid);
        auto args = f.arguments();
        for (auto i = 0u; i < args.size(); i++) {
            auto arg = args[i];
            _arguments[i]->mark(
                arg.tag() == Variable::Tag::BUFFER || arg.tag() == Variable::Tag::IMAGE
                    ? f.variable_usage(arg.uid())
                    : Variable::Usage::READ);
        }
    }
}

}// namespace luisa::compute
