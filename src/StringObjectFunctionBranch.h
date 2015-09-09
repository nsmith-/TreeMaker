#include <string>

#include "FWCore/Utilities/interface/TypeWithDict.h"
#include "FWCore/Utilities/interface/ObjectWithDict.h"

#include "CommonTools/Utils/src/ExpressionPtr.h"
#include "CommonTools/Utils/src/ExpressionBase.h"
#include "CommonTools/Utils/interface/expressionParser.h"

#include "TTree.h"

class StringObjectFunctionBranch {
  public:
    StringObjectFunctionBranch(TTree * tree, std::string functionName, std::string functionString, std::string typeName) :
      branch_(tree->Branch(functionName.c_str(), &value_))
    {
      if( !reco::parser::expressionParser(edm::TypeWithDict::byName(typeName), functionString, expr_, true) ) {
        throw cms::Exception("TreeMaker") << "Failed to parse expression for StringObjectFunctionBranch" << std::endl
          << "Name: " << functionName << std::endl
          << "Object type: " << typeName << std::endl
          << "Expression: " << functionString;
      }
    };

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    StringObjectFunctionBranch(const StringObjectFunctionBranch&) = delete;
    StringObjectFunctionBranch& operator=(const StringObjectFunctionBranch&) = delete;
    StringObjectFunctionBranch(const StringObjectFunctionBranch&& previous) = delete;
    StringObjectFunctionBranch& operator=(const StringObjectFunctionBranch&&) = delete;

    void evaluate(const edm::ObjectWithDict& object) {
      value_ = expr_->value(object);
    };

  private:
    reco::parser::ExpressionPtr expr_;
    // Branches owned by tree
    TBranch * branch_;
    float value_;
};
