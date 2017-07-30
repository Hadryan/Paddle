/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserve.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#include "paddle/framework/scope.h"
#include "paddle/string/printf.h"

namespace paddle {
namespace framework {

Scope::~Scope() {
  for (auto& kv : vars_) delete kv.second;
  for (Scope* s : kids_) delete s;
}

Scope& Scope::NewScope() {
  kids_.push_back(new Scope(this));
  return *kids_.back();
}

Variable* Scope::NewVar(const std::string& name) {
  auto iter = vars_.find(name);
  if (iter != vars_.end()) {
    return iter->second;
  }
  Variable* v = new Variable();
  vars_[name] = v;
  v->name_ = &(vars_.find(name)->first);
  return v;
}

Variable* Scope::NewVar() {
  return NewVar(string::Sprintf("%p.%d", this, vars_.size()));
}

Variable* Scope::FindVar(const std::string& name) const {
  auto it = vars_.find(name);
  if (it != vars_.end()) return it->second;
  return (parent_ == nullptr) ? nullptr : parent_->FindVar(name);
}

Scope* Scope::FindScope(const Variable* var) {
  for (auto& kv : vars_) {
    if (kv.second == var) {
      return this;
    }
  }
  return (parent_ == nullptr) ? nullptr : parent_->FindScope(var);
}

}  // namespace framework
}  // namespace paddle
