import FWCore.ParameterSet.Config as cms

# maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
readFiles = cms.untracked.vstring()
secFiles = cms.untracked.vstring() 
source = cms.Source ("PoolSource",fileNames = readFiles, secondaryFileNames = secFiles)
readFiles.extend( [
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/065483BC-12B0-DD11-BB6A-000423D9863C.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/0882C12B-F4AF-DD11-9FAA-001617C3B710.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/0E72DFED-F5AF-DD11-9193-000423D94E70.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/281E27DE-01B0-DD11-B0F3-000423D99660.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/2A34AB7B-F5AF-DD11-8623-001D09F2438A.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/2AE48074-F4AF-DD11-BF1C-000423D9880C.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/38392BA3-F4AF-DD11-86D3-0016177CA778.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/3C42EF71-F5AF-DD11-8722-000423D985B0.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/58BF08E5-F2AF-DD11-A12A-000423D99264.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/5AFF40D9-F2AF-DD11-BA86-001617C3B70E.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/7E9272B8-F3AF-DD11-9C1F-000423D94E1C.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/94EBD556-F6AF-DD11-BED8-000423D94E70.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/9CF965A1-F6AF-DD11-9D09-001617C3B78C.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/AABE8F52-F3AF-DD11-9F10-001617C3B70E.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/D6AFE6C3-03B0-DD11-893E-001617E30D38.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/E4E8736E-F3AF-DD11-9908-001617C3B76A.root',
       '/store/relval/CMSSW_2_2_0_pre1/RelValTTbar/GEN-SIM-DIGI-RAW-HLTDEBUG/STARTUP_V7_v2/0000/FE3F8EFE-F5AF-DD11-899A-0030487A1990.root' ] );


secFiles.extend( [
               ] )
