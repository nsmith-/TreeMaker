import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2015B/ZeroBias/MINIAOD/PromptReco-v1/000/251/244/00000/4CE7A263-0728-E511-818D-02163E01379D.root'
    ),
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("testConfig.root")
)

process.met = cms.EDAnalyzer('TreeMaker',
    candidates = cms.InputTag("slimmedMETs"),
    function = cms.string("pt-uncorrectedPt"),
)

process.jet = cms.EDAnalyzer('TreeMaker',
    candidates = cms.InputTag("slimmedJets"),
    function = cms.string("pt"),
)

process.p = cms.Path(process.met+process.jet)
