#include <memory>
#include <string>
#include <vector>

#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/Candidate.h"
#include "CommonTools/Utils/interface/StringObjectFunction.h"

#include "TTree.h"

class CandidateCollectionFunction {
  public:
    CandidateCollectionFunction(TTree * tree, std::string functionName, std::string functionString) :
      function_(functionString),
      vectorBranch_(tree->Branch(functionName.c_str(), "std::vector<float>", &vector_))
    {};

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    CandidateCollectionFunction(const CandidateCollectionFunction&) = delete;
    CandidateCollectionFunction& operator=(const CandidateCollectionFunction&) = delete;
    CandidateCollectionFunction(const CandidateCollectionFunction&& previous) = delete;
    CandidateCollectionFunction& operator=(const CandidateCollectionFunction&&) = delete;

    void evaluate(const reco::CandidateView& candidates) {
      vector_.clear();
      for ( const auto& candidate : candidates ) {
        vector_.push_back(function_(candidate));
      }
    };

  private:
    StringObjectFunction<reco::Candidate, true> function_;
    // Branches owned by tree
    TBranch * vectorBranch_;
    std::vector<float> vector_;
};

class CandidateCollectionBranchSet {
  public:
    CandidateCollectionBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc) {
      collectionToken_ = cc.consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"));
      for ( auto functionName : iConfig.getParameterSet("functions").getParameterNamesForType<std::string>() ) {
        auto functionString = iConfig.getParameterSet("functions").getParameter<std::string>(functionName);
        functions_.emplace_back(new CandidateCollectionFunction(tree, collectionName+"_"+functionName, functionString));
      }
      collectionSizeBranch_ = tree->Branch(("n"+collectionName).c_str(), &collectionSize_);
    };

    void fill(const edm::Event& iEvent) {
      edm::Handle<reco::CandidateView> candidates;
      iEvent.getByToken(collectionToken_, candidates);

      for ( auto& f : functions_ ) {
        f->evaluate(*candidates);
      }

      collectionSize_ = candidates->size();
    };

  private:
    edm::EDGetTokenT<reco::CandidateView> collectionToken_;
    std::vector<std::unique_ptr<CandidateCollectionFunction>> functions_;
    TBranch * collectionSizeBranch_;
    int collectionSize_;
};

