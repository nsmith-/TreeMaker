#include "FWCore/Framework/interface/GenericHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/TypeID.h"
#include "FWCore/Utilities/interface/TypeToGet.h"

#include "GenericObjectBranchSet.h"

GenericObjectBranchSet::GenericObjectBranchSet(TTree * tree, std::string collectionName, const edm::ParameterSet& iConfig, edm::ConsumesCollector cc) {
  typeName_ = iConfig.getParameter<std::string>("productType");
  inputTag_ = iConfig.getParameter<edm::InputTag>("inputTag");

  // Although we cannot use getByToken with GenericHandle,
  // we still need to declare the consumption
  auto typeInfo = edm::TypeID(edm::TypeWithDict::byName(typeName_).typeInfo());
  auto consumesType = edm::TypeToGet(typeInfo, edm::PRODUCT_TYPE);
  cc.consumes(consumesType, inputTag_);

  for ( auto functionName : iConfig.getParameterSet("functions").getParameterNamesForType<std::string>() ) {
    auto functionString = iConfig.getParameterSet("functions").getParameter<std::string>(functionName);
    branches_.emplace_back(new StringObjectFunctionBranch(tree, collectionName+"_"+functionName, functionString, typeName_));
  }
  tree_ = tree;
}

void
GenericObjectBranchSet::fill(const edm::Event& iEvent) {
  edm::GenericHandle object(typeName_);
  iEvent.getByLabel(inputTag_, object);

  for ( auto& branch : branches_ ) {
    branch->evaluate(*object);
  }
}
