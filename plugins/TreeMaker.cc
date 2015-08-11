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
            functions_.push_back(std::unique_ptr<CandidateCollectionFunction>(new CandidateCollectionFunction(tree, collectionName+"_"+functionName, functionString)));
          }
          collectionSizeBranch_ = tree->Branch(("n"+collectionName).c_str(), &collectionSize_);
        };

        // Allow no copy/move, so memory location will not change
        // TODO: in principle we should be able to move, and update branch address later
        CandidateCollectionBranchSet(const CandidateCollectionBranchSet&) = delete;
        CandidateCollectionBranchSet& operator=(const CandidateCollectionBranchSet&) = delete;
        CandidateCollectionBranchSet(const CandidateCollectionBranchSet&& previous) = delete;
        CandidateCollectionBranchSet& operator=(const CandidateCollectionBranchSet&&) = delete;

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


    // Owned by TFileService
    TTree * tree_;

    int run_;
    int lumi_;
    int event_;

    std::vector<std::unique_ptr<CandidateCollectionBranchSet>> collections_;
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
  tree_->Branch("event", &event_, "event/l");

  const edm::ParameterSet collections = iConfig.getParameterSet("collections");
  for ( auto collectionName : collections.getParameterNamesForType<edm::ParameterSet>() ) {
    collections_.push_back(std::unique_ptr<CandidateCollectionBranchSet>(new CandidateCollectionBranchSet(tree_, collectionName, collections.getParameterSet(collectionName), consumesCollector())));
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
