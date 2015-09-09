#include <memory>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/Utils/interface/StringObjectFunction.h"

#include "TTree.h"

class CandidateFunction {
  public:
    CandidateFunction(TTree * tree, std::string functionName, std::string functionString);

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    CandidateFunction(const CandidateFunction&) = delete;
    CandidateFunction& operator=(const CandidateFunction&) = delete;
    CandidateFunction(const CandidateFunction&& previous) = delete;
    CandidateFunction& operator=(const CandidateFunction&&) = delete;

    void evaluate(const reco::Candidate& candidate);

  private:
    StringObjectFunction<reco::Candidate, true> function_;
    // Branches owned by tree
    TBranch * branch_;
    float value_;
};

class CandidateBranchSet {
  public:
    CandidateBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc);

    void fill(const edm::Event& iEvent);

  private:
    edm::EDGetTokenT<reco::CandidateView> collectionToken_;
    std::vector<std::unique_ptr<CandidateFunction>> functions_;
    TTree * tree_;
};

