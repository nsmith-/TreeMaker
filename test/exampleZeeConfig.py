import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport = cms.untracked.PSet( reportEvery = cms.untracked.int32(1000) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
        '/store/data/Run2015C/DoubleEG/MINIAOD/PromptReco-v1/000/254/790/00000/765F69B4-3F4A-E511-B174-02163E01426E.root'
    ),
)

process.TFileService = cms.Service(
    "TFileService",
    fileName = cms.string("exampleZeeConfig.root")
)

process.decentElectrons = cms.EDFilter("PATElectronRefSelector",
    src = cms.InputTag("slimmedElectrons"),
    cut = cms.string("pt > 7 && abs(eta) < 2.5"),
    filter = cms.bool(True)
)

process.ZeeCandidates = cms.EDProducer("CandViewShallowCloneCombiner",
    decay = cms.string("decentElectrons@+ decentElectrons@-"),
    cut   = cms.string("40 < mass < 200"),
    roles = cms.vstring('e1', 'e2')
)

process.ZeeSequence = cms.Sequence(process.decentElectrons*process.ZeeCandidates)

process.demo = cms.EDAnalyzer('TreeMaker',
    rowKey = cms.string('Zcandidate'),
    collections = cms.PSet(
        Zcandidate = cms.PSet(
            candidates = cms.InputTag("ZeeCandidates"),
            functions = cms.PSet(
                pt = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
                mass = cms.string("mass"),
                e1_pt =  cms.string("daughter('e1').pt()"),
                e1_eta = cms.string("daughter('e1').eta()"),
                e1_phi = cms.string("daughter('e1').phi()"),
                e2_pt =  cms.string("daughter('e2').pt()"),
                e2_eta = cms.string("daughter('e2').eta()"),
                e2_phi = cms.string("daughter('e2').phi()"),
            )
        ),
        decentElectrons = cms.PSet(
            candidates = cms.InputTag("decentElectrons"),
            functions = cms.PSet(
                pt = cms.string("pt"),
                eta = cms.string("eta"),
                phi = cms.string("phi"),
            )
        ),
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
    )
)

process.p = cms.Path(process.ZeeSequence*process.demo)

process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
