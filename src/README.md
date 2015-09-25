Description of source/headers in this directory
===============================================

#### GenericObjectBranchSet.cc/h
This class receives a set of functions to be applied to an arbitrary
persistent data format in CMSSW and fills float branches.
This does not loop over collections, but one can access collection offsets 
since `std::vector<Type>` is also a persistent data format that a `StringObjectFunction` can be applied to.

#### GenericObjectCollectionBranchSet.cc/h
TODO

#### CandidateBranchSet.cc/h
This class operates similarly to GenericObjectBranchSet, but specifically
for data formats that inheirit from `reco::Candidate`, e.g. most physics
objects.

#### CandidateCollectionBranchSet.cc/h
This class fills a vector of floats for each branch specified per candidate.

#### StringObjectFunctionBranch.cc/h
Support class for GenericObjectBranchSet, will fill a branch with a string expression
for arbitrary type, provided it is present in the reflex dictionary.
