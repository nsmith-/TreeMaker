#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CandidateBranchSet.h"

CandidateFunction::CandidateFunction(TTree * tree, std::string functionName, std::string functionString) :
  function_(functionString),
  branch_(tree->Branch(functionName.c_str(), &value_))
{};

void
CandidateFunction::evaluate(const reco::Candidate& candidate) {
  value_ = function_(candidate);
};

CandidateBranchSet::CandidateBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc) {
  collectionToken_ = cc.consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"));
  for ( auto functionName : iConfig.getParameterSet("functions").getParameterNamesForType<std::string>() ) {
    auto functionString = iConfig.getParameterSet("functions").getParameter<std::string>(functionName);
    functions_.emplace_back(new CandidateFunction(tree, collectionName+"_"+functionName, functionString));
  }
  tree_ = tree;
}

void
CandidateBranchSet::fill(const edm::Event& iEvent) {
  edm::Handle<reco::CandidateView> candidates;
  iEvent.getByToken(collectionToken_, candidates);

  for ( const auto& candidate : *candidates ) {
    for ( auto& f : functions_ ) {
      f->evaluate(candidate);
    }
    tree_->Fill();
  }
}
