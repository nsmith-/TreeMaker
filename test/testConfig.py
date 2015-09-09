import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2015C/DoubleEG/MINIAOD/PromptReco-v1/000/254/790/00000/765F69B4-3F4A-E511-B174-02163E01426E.root'
    ),
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("testConfig.root")
)

process.demo = cms.EDAnalyzer('TreeMaker',
    rowKey = cms.string(''),
    collections = cms.PSet(
        Jets = cms.PSet(
            candidates = cms.InputTag("slimmedJets"),
            functions = cms.PSet(
                pt = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
            )
        ),
        MET = cms.PSet(
            candidates = cms.InputTag("slimmedMETs"),
            functions = cms.PSet(
                pt = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
            )
        )
    ),
    specials = cms.PSet(
        vertex = cms.PSet(
            productType = cms.string("vector<reco::Vertex>"),
            inputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
            functions = cms.PSet(
                collectionSize = cms.string("size"),
                ndof = cms.string("at(0).ndof()"),
                chi2 = cms.string("at(0).chi2()"),
                x = cms.string("at(0).x()"),
            )
        )
    )
)

process.p = cms.Path(process.demo)
