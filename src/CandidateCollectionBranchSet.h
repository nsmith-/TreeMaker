#include <memory>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/Utils/interface/StringObjectFunction.h"

#include "TTree.h"

class CandidateCollectionFunction {
  public:
    CandidateCollectionFunction(TTree * tree, std::string functionName, std::string functionString);

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    CandidateCollectionFunction(const CandidateCollectionFunction&) = delete;
    CandidateCollectionFunction& operator=(const CandidateCollectionFunction&) = delete;
    CandidateCollectionFunction(const CandidateCollectionFunction&& previous) = delete;
    CandidateCollectionFunction& operator=(const CandidateCollectionFunction&&) = delete;

    void evaluate(const reco::CandidateView& candidates);

  private:
    StringObjectFunction<reco::Candidate, true> function_;
    // Branches owned by tree
    TBranch * vectorBranch_;
    std::vector<float> vector_;
};

class CandidateCollectionBranchSet {
  public:
    CandidateCollectionBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc);

    void fill(const edm::Event& iEvent);

  private:
    edm::EDGetTokenT<reco::CandidateView> collectionToken_;
    std::vector<std::unique_ptr<CandidateCollectionFunction>> functions_;
    TBranch * collectionSizeBranch_;
    int collectionSize_;
};

