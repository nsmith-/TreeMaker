import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("EmptySource")

process.demo = cms.EDAnalyzer('TreeMaker',
    asdf = cms.PSet(
        hi = cms.bool(True),
        basdf = cms.string("hi")
    ),
)


process.p = cms.Path(process.demo)
