// Original Author: Nick Smith <nick.smith@cern.ch>
// Created: Wed, 01 Jul 2015 11:59:48 GMT

#include <memory>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

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

    std::unique_ptr<TTree> tree_;
};

TreeMaker::TreeMaker(const edm::ParameterSet& iConfig)
{
  usesResource("TFileService");

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

  std::string treeName{iConfig.getParameter<std::string>("@module_label")};
  std::string treeTitle{treeName};
  tree_ = std::unique_ptr<TTree>{new TTree(treeName.c_str(), treeTitle.c_str())};
}


TreeMaker::~TreeMaker()
{
}

void
TreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
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

DEFINE_FWK_MODULE(TreeMaker);
