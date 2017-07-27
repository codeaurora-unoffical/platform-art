/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "stack_trace_element.h"

#include "class.h"
#include "class-inl.h"
#include "gc/accounting/card_table-inl.h"
#include "gc_root-inl.h"
#include "object-inl.h"
#include "handle_scope-inl.h"
#include "string.h"

namespace art {
namespace mirror {

GcRoot<Class> StackTraceElement::java_lang_StackTraceElement_;

void StackTraceElement::SetClass(ObjPtr<Class> java_lang_StackTraceElement) {
  CHECK(java_lang_StackTraceElement_.IsNull());
  CHECK(java_lang_StackTraceElement != nullptr);
  java_lang_StackTraceElement_ = GcRoot<Class>(java_lang_StackTraceElement);
}

void StackTraceElement::ResetClass() {
  CHECK(!java_lang_StackTraceElement_.IsNull());
  java_lang_StackTraceElement_ = GcRoot<Class>(nullptr);
}

StackTraceElement* StackTraceElement::Alloc(Thread* self,
                                            Handle<String> declaring_class,
                                            Handle<String> method_name,
                                            Handle<String> file_name,
                                            int32_t line_number) {
  ObjPtr<StackTraceElement> trace =
      ObjPtr<StackTraceElement>::DownCast(GetStackTraceElement()->AllocObject(self));
  if (LIKELY(trace != nullptr)) {
    if (Runtime::Current()->IsActiveTransaction()) {
      trace->Init<true>(declaring_class.Get(), method_name.Get(), file_name.Get(), line_number);
    } else {
      trace->Init<false>(declaring_class.Get(), method_name.Get(), file_name.Get(), line_number);
    }
  }
  return trace.Ptr();
}

template<bool kTransactionActive>
void StackTraceElement::Init(ObjPtr<String> declaring_class,
                             ObjPtr<String> method_name,
                             ObjPtr<String> file_name,
                             int32_t line_number) {
  SetFieldObject<kTransactionActive>(OFFSET_OF_OBJECT_MEMBER(StackTraceElement, declaring_class_),
                                     declaring_class);
  SetFieldObject<kTransactionActive>(OFFSET_OF_OBJECT_MEMBER(StackTraceElement, method_name_),
                                     method_name);
  SetFieldObject<kTransactionActive>(OFFSET_OF_OBJECT_MEMBER(StackTraceElement, file_name_),
                                     file_name);
  SetField32<kTransactionActive>(OFFSET_OF_OBJECT_MEMBER(StackTraceElement, line_number_),
                                 line_number);
}

void StackTraceElement::VisitRoots(RootVisitor* visitor) {
  java_lang_StackTraceElement_.VisitRootIfNonNull(visitor, RootInfo(kRootStickyClass));
}


}  // namespace mirror
}  // namespace art
