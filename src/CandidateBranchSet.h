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

class CandidateFunction {
  public:
    CandidateFunction(TTree * tree, std::string functionName, std::string functionString) :
      function_(functionString),
      branch_(tree->Branch(functionName.c_str(), &value_))
    {};

    // Allow no copy/move, so memory location will not change
    // TODO: in principle we should be able to move, and update branch address later
    CandidateFunction(const CandidateFunction&) = delete;
    CandidateFunction& operator=(const CandidateFunction&) = delete;
    CandidateFunction(const CandidateFunction&& previous) = delete;
    CandidateFunction& operator=(const CandidateFunction&&) = delete;

    void evaluate(const reco::Candidate& candidate) {
      value_ = function_(candidate);
    };

  private:
    StringObjectFunction<reco::Candidate, true> function_;
    // Branches owned by tree
    TBranch * branch_;
    float value_;
};

class CandidateBranchSet {
  public:
    CandidateBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc) {
      collectionToken_ = cc.consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"));
      for ( auto functionName : iConfig.getParameterSet("functions").getParameterNamesForType<std::string>() ) {
        auto functionString = iConfig.getParameterSet("functions").getParameter<std::string>(functionName);
        functions_.emplace_back(new CandidateFunction(tree, collectionName+"_"+functionName, functionString));
      }
      tree_ = tree;
    };

    void fill(const edm::Event& iEvent) {
      edm::Handle<reco::CandidateView> candidates;
      iEvent.getByToken(collectionToken_, candidates);

      for ( const auto& candidate : *candidates ) {
        for ( auto& f : functions_ ) {
          f->evaluate(candidate);
        }
        tree_->Fill();
      }
    };

  private:
    edm::EDGetTokenT<reco::CandidateView> collectionToken_;
    std::vector<std::unique_ptr<CandidateFunction>> functions_;
    TTree * tree_;
};

