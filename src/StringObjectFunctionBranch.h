#include <string>

#include "FWCore/Utilities/interface/ObjectWithDict.h"
#include "CommonTools/Utils/src/ExpressionPtr.h"
#include "TTree.h"

class StringObjectFunctionBranch {
  public:
    StringObjectFunctionBranch(TTree * tree, std::string functionName, std::string functionString, std::string typeName);

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    StringObjectFunctionBranch(const StringObjectFunctionBranch&) = delete;
    StringObjectFunctionBranch& operator=(const StringObjectFunctionBranch&) = delete;
    StringObjectFunctionBranch(const StringObjectFunctionBranch&& previous) = delete;
    StringObjectFunctionBranch& operator=(const StringObjectFunctionBranch&&) = delete;

    void evaluate(const edm::ObjectWithDict& object);

  private:
    reco::parser::ExpressionPtr expr_;
    // Branches owned by tree
    TBranch * branch_;
    float value_;
};
