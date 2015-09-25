// Original Author: Nick Smith <nick.smith@cern.ch>
// Created: Wed, 01 Jul 2015 11:59:48 GMT

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Analysis/TreeMaker/src/GenericObjectBranchSet.h"
#include "Analysis/TreeMaker/src/CandidateBranchSet.h"
#include "Analysis/TreeMaker/src/CandidateCollectionBranchSet.h"

#include "TTree.h"

class TreeMaker : public edm::one::EDAnalyzer<edm::one::SharedResources> {
  public:
    explicit TreeMaker(const edm::ParameterSet&);
    ~TreeMaker();

    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  private:
    virtual void beginJob() override;
    virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
    virtual void endJob() override;

    void dumpPSet(const edm::ParameterSet&);

    // Owned by TFileService
    TTree * tree_;

    int run_;
    int lumi_;
    int event_;

    std::unique_ptr<CandidateBranchSet> rowKeyCollection_;
    std::vector<std::unique_ptr<CandidateCollectionBranchSet>> collections_;
    std::vector<std::unique_ptr<GenericObjectBranchSet>> specials_;
};

TreeMaker::TreeMaker(const edm::ParameterSet& iConfig)
{
  usesResource("TFileService");
  std::string treeName{iConfig.getUntrackedParameter<std::string>("treeName", "Ntuple")};
  std::string treeTitle{treeName};

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>(treeName.c_str(), treeTitle.c_str());
  tree_->Branch("run", &run_, "run/i");
  tree_->Branch("lumi", &lumi_, "lumi/i");
  tree_->Branch("event", &event_, "event/i");

  const edm::ParameterSet collections = iConfig.getParameterSet("collections");
  std::string rowKeyCollectionName = iConfig.getParameter<std::string>("rowKey");

  for ( auto collectionName : collections.getParameterNamesForType<edm::ParameterSet>() ) {
    if ( rowKeyCollectionName == collectionName ) {
      rowKeyCollection_ = std::unique_ptr<CandidateBranchSet>(new CandidateBranchSet(tree_, collectionName, collections.getParameterSet(collectionName), consumesCollector()));
    } else {
      collections_.emplace_back(new CandidateCollectionBranchSet(tree_, collectionName, collections.getParameterSet(collectionName), consumesCollector()));
    }
  }
  if ( rowKeyCollection_ == nullptr && !rowKeyCollectionName.empty() ) {
    throw cms::Exception("TreeMaker") << "Invalid rowKey!" << std::endl
      << "Value must be a collection name from the collections PSet";
  }

  if ( iConfig.existsAs<edm::ParameterSet>("specials") ) {
    const edm::ParameterSet specials = iConfig.getParameterSet("specials");
    for ( auto collectionName : specials.getParameterNamesForType<edm::ParameterSet>() ) {
      specials_.emplace_back(new GenericObjectBranchSet(tree_, collectionName, specials.getParameterSet(collectionName), consumesCollector()));
    }
  }
}


TreeMaker::~TreeMaker()
{
}

void
TreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  run_ = iEvent.id().run();
  lumi_ = iEvent.id().luminosityBlock();
  event_ = iEvent.id().event();

  for ( auto& collection : collections_ ) {
    collection->fill(iEvent);
  }

  for ( auto& collection : specials_ ) {
    collection->fill(iEvent);
  }

  if ( rowKeyCollection_ == nullptr ) {
    tree_->Fill();
  } else {
    rowKeyCollection_->fill(iEvent);
  }
}


void 
TreeMaker::beginJob()
{
}

void 
TreeMaker::endJob() 
{
}

void
TreeMaker::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  // I swear I'm going to do whatever this is...
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void
TreeMaker::dumpPSet(const edm::ParameterSet& iConfig) {
  edm::LogPrint("TreeMaker") << "Parameter names:" << std::endl << "------------------------------";
  std::function<void(const edm::ParameterSet&, int)> dumpPSetNames = [&dumpPSetNames](const edm::ParameterSet& pset, int indent) {
    std::string spaces{};
    spaces.resize(indent*2, ' ');
    for (auto name : pset.getParameterNames()) {
      edm::LogPrint("TreeMaker") << spaces << name;
      if ( pset.existsAs<edm::ParameterSet>(name) ) {
        dumpPSetNames(pset.getParameterSet(name), indent+1);
      }
    }
  };
  dumpPSetNames(iConfig, 0);
}


DEFINE_FWK_MODULE(TreeMaker);
