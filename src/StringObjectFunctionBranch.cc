#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/TypeWithDict.h"

#include "CommonTools/Utils/interface/expressionParser.h"
#include "CommonTools/Utils/src/ExpressionBase.h"

#include "StringObjectFunctionBranch.h"

StringObjectFunctionBranch::StringObjectFunctionBranch(TTree * tree, std::string functionName, std::string functionString, std::string typeName) :
  branch_(tree->Branch(functionName.c_str(), &value_))
{
  if( !reco::parser::expressionParser(edm::TypeWithDict::byName(typeName), functionString, expr_, true) ) {
    throw cms::Exception("TreeMaker") << "Failed to parse expression for StringObjectFunctionBranch" << std::endl
      << "Name: " << functionName << std::endl
      << "Object type: " << typeName << std::endl
      << "Expression: " << functionString;
  }
}

void
StringObjectFunctionBranch::evaluate(const edm::ObjectWithDict& object) {
  value_ = expr_->value(object);
}
