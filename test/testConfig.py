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
        vertices = cms.PSet(
            productType = cms.string("vector<reco::Vertex>"),
            inputTag = cms.InputTag("offlineSlimmedPrimaryVertices"),
            functions = cms.PSet(
                size = cms.string("size"),
                firstVertexNTracks = cms.string("at(0).nTracks()")
            )
        )
    )
)

process.p = cms.Path(process.demo)
