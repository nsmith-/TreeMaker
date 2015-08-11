// Original Author: Nick Smith <nick.smith@cern.ch>
// Created: Wed, 01 Jul 2015 11:59:48 GMT

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Candidate/interface/Candidate.h"

#include "CommonTools/Utils/interface/StringObjectFunction.h"

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

    edm::EDGetTokenT<reco::CandidateView> token_;
    StringObjectFunction<reco::Candidate, true> function_;

    // Owned by TFileService
    TTree * tree_;

    int run_;
    int lumi_;
    int event_;
    std::vector<float> vector_;
};

TreeMaker::TreeMaker(const edm::ParameterSet& iConfig) :
  token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"))),
  function_(iConfig.getParameter<std::string>("function"))
{
  usesResource("TFileService");
  std::string treeName{iConfig.getUntrackedParameter<std::string>("treeName", "Ntuple")};
  std::string treeTitle{treeName};

  edm::Service<TFileService> fs;
  tree_ = fs->make<TTree>(treeName.c_str(), treeTitle.c_str());
  tree_->Branch("run", &run_, "run/i");
  tree_->Branch("lumi", &lumi_, "lumi/i");
  tree_->Branch("event", &event_, "event/l");
  tree_->Branch("vect", "std::vector<float>", &vector_);
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

  edm::Handle<reco::CandidateView> candidates;
  iEvent.getByToken(token_, candidates);

  vector_.clear();
  for ( const auto& candidate : *candidates ) {
    vector_.push_back(function_(candidate));
  }

  tree_->Fill();
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
