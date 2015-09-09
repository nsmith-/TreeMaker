#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "StringObjectFunctionBranch.h"

class ObjectBranchSet {
  public:
    ObjectBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc);

    void fill(const edm::Event& iEvent);

  private:
    edm::InputTag inputTag_;
    std::string typeName_;
    std::vector<std::unique_ptr<StringObjectFunctionBranch>> branches_;
    TTree * tree_;
};
