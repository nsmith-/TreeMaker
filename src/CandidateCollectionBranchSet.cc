#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "CandidateCollectionBranchSet.h"

CandidateCollectionFunction::CandidateCollectionFunction(TTree * tree, std::string functionName, std::string functionString) :
  function_(functionString),
  vectorBranch_(tree->Branch(functionName.c_str(), "std::vector<float>", &vector_))
{
}

void
CandidateCollectionFunction::evaluate(const reco::CandidateView& candidates) {
  vector_.clear();
  for ( const auto& candidate : candidates ) {
    vector_.push_back(function_(candidate));
  }
}

CandidateCollectionBranchSet::CandidateCollectionBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc) {
  collectionToken_ = cc.consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("collection"));
  auto& branches = iConfig.getParameterSet("branches");
  for ( auto functionName : branches.getParameterNamesForType<std::string>() ) {
    auto functionString = branches.getParameter<std::string>(functionName);
    functions_.emplace_back(new CandidateCollectionFunction(tree, collectionName+"_"+functionName, functionString));
  }
  collectionSizeBranch_ = tree->Branch(("n"+collectionName).c_str(), &collectionSize_);
}

void
CandidateCollectionBranchSet::fill(const edm::Event& iEvent) {
  edm::Handle<reco::CandidateView> candidates;
  iEvent.getByToken(collectionToken_, candidates);

  for ( auto& f : functions_ ) {
    f->evaluate(*candidates);
  }

  collectionSize_ = candidates->size();
};
