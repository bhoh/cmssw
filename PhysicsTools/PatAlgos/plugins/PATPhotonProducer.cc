//
// $Id: PATPhotonProducer.cc,v 1.28 2010/09/03 15:41:27 hegner Exp $
//

#include "PhysicsTools/PatAlgos/plugins/PATPhotonProducer.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/ParameterSet/interface/ConfigurationDescriptions.h"
#include "FWCore/ParameterSet/interface/ParameterSetDescription.h"

#include <memory>

using namespace pat;

PATPhotonProducer::PATPhotonProducer(const edm::ParameterSet & iConfig) :
  isolator_(iConfig.exists("userIsolation") ? iConfig.getParameter<edm::ParameterSet>("userIsolation") : edm::ParameterSet(), false) ,
  useUserData_(iConfig.exists("userData"))
{
  // initialize the configurables
  photonSrc_         = iConfig.getParameter<edm::InputTag>("photonSource");
  embedSuperCluster_ = iConfig.getParameter<bool>         ("embedSuperCluster");

   // MC matching configurables
  addGenMatch_       = iConfig.getParameter<bool>         ( "addGenMatch" );
  if (addGenMatch_) {
      embedGenMatch_ = iConfig.getParameter<bool>         ( "embedGenMatch" );
      if (iConfig.existsAs<edm::InputTag>("genParticleMatch")) {
          genMatchSrc_.push_back(iConfig.getParameter<edm::InputTag>( "genParticleMatch" ));
      } else {
          genMatchSrc_ = iConfig.getParameter<std::vector<edm::InputTag> >( "genParticleMatch" );
      }
  }

  // Efficiency configurables
  addEfficiencies_ = iConfig.getParameter<bool>("addEfficiencies");
  if (addEfficiencies_) {
     efficiencyLoader_ = pat::helper::EfficiencyLoader(iConfig.getParameter<edm::ParameterSet>("efficiencies"));
  }
 
  // photon ID configurables
  addPhotonID_        = iConfig.getParameter<bool>         ( "addPhotonID" );
  if (addPhotonID_) {
      // it might be a single photon ID
      if (iConfig.existsAs<edm::InputTag>("photonIDSource")) {
          photIDSrcs_.push_back(NameTag("", iConfig.getParameter<edm::InputTag>("photonIDSource")));
      }
      // or there might be many of them
      if (iConfig.existsAs<edm::ParameterSet>("photonIDSources")) {
          // please don't configure me twice
          if (!photIDSrcs_.empty()) throw cms::Exception("Configuration") << 
                "PATPhotonProducer: you can't specify both 'photonIDSource' and 'photonIDSources'\n";
          // read the different photon ID names
          edm::ParameterSet idps = iConfig.getParameter<edm::ParameterSet>("photonIDSources");
          std::vector<std::string> names = idps.getParameterNamesForType<edm::InputTag>();
          for (std::vector<std::string>::const_iterator it = names.begin(), ed = names.end(); it != ed; ++it) {
              photIDSrcs_.push_back(NameTag(*it, idps.getParameter<edm::InputTag>(*it)));
          }
      }
      // but in any case at least once
      if (photIDSrcs_.empty()) throw cms::Exception("Configuration") <<
            "PATPhotonProducer: id addPhotonID is true, you must specify either:\n" <<
            "\tInputTag photonIDSource = <someTag>\n" << "or\n" <<
            "\tPSet photonIDSources = { \n" <<
            "\t\tInputTag <someName> = <someTag>   // as many as you want \n " <<
            "\t}\n";
  }
  


  // Resolution configurables
  addResolutions_ = iConfig.getParameter<bool>("addResolutions");
  if (addResolutions_) {
     resolutionLoader_ = pat::helper::KinResolutionsLoader(iConfig.getParameter<edm::ParameterSet>("resolutions"));
  }
 
  // Check to see if the user wants to add user data
  if ( useUserData_ ) {
    userDataHelper_ = PATUserDataHelper<Photon>(iConfig.getParameter<edm::ParameterSet>("userData"));
  }

 
  // produces vector of photons
  produces<std::vector<Photon> >();

  if (iConfig.exists("isoDeposits")) {
     edm::ParameterSet depconf = iConfig.getParameter<edm::ParameterSet>("isoDeposits");
     if (depconf.exists("tracker")) isoDepositLabels_.push_back(std::make_pair(pat::TrackIso, depconf.getParameter<edm::InputTag>("tracker")));
     if (depconf.exists("ecal"))    isoDepositLabels_.push_back(std::make_pair(pat::EcalIso, depconf.getParameter<edm::InputTag>("ecal")));
     if (depconf.exists("hcal"))    isoDepositLabels_.push_back(std::make_pair(pat::HcalIso, depconf.getParameter<edm::InputTag>("hcal")));
     if (depconf.exists("user")) {
        std::vector<edm::InputTag> userdeps = depconf.getParameter<std::vector<edm::InputTag> >("user");
        std::vector<edm::InputTag>::const_iterator it = userdeps.begin(), ed = userdeps.end();
        int key = UserBaseIso;
        for ( ; it != ed; ++it, ++key) {
            isoDepositLabels_.push_back(std::make_pair(IsolationKeys(key), *it));
        }
     }
  }
}

PATPhotonProducer::~PATPhotonProducer() {
}

void PATPhotonProducer::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
 
  // Get the vector of Photon's from the event
  edm::Handle<edm::View<reco::Photon> > photons;
  iEvent.getByLabel(photonSrc_, photons);

  if (iEvent.isRealData()){
    addGenMatch_ = false;
    embedGenMatch_ = false;
  }

  // prepare the MC matching
  std::vector<edm::Handle<edm::Association<reco::GenParticleCollection> > > genMatches(genMatchSrc_.size());
  if (addGenMatch_) {
        for (size_t j = 0, nd = genMatchSrc_.size(); j < nd; ++j) {
            iEvent.getByLabel(genMatchSrc_[j], genMatches[j]);
        }
  }

  if (isolator_.enabled()) isolator_.beginEvent(iEvent,iSetup);
  
  if (efficiencyLoader_.enabled()) efficiencyLoader_.newEvent(iEvent);
  if (resolutionLoader_.enabled()) resolutionLoader_.newEvent(iEvent, iSetup);
  
  std::vector<edm::Handle<edm::ValueMap<IsoDeposit> > > deposits(isoDepositLabels_.size());
  for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
    iEvent.getByLabel(isoDepositLabels_[j].second, deposits[j]);
  }
  
  // prepare ID extraction 
  std::vector<edm::Handle<edm::ValueMap<Bool_t> > > idhandles;
  std::vector<pat::Photon::IdPair>               ids;
  if (addPhotonID_) {
    idhandles.resize(photIDSrcs_.size());
    ids.resize(photIDSrcs_.size());
    for (size_t i = 0; i < photIDSrcs_.size(); ++i) {
      iEvent.getByLabel(photIDSrcs_[i].second, idhandles[i]);
      ids[i].first = photIDSrcs_[i].first;
    }
  }

  // loop over photons
  std::vector<Photon> * PATPhotons = new std::vector<Photon>(); 
  for (edm::View<reco::Photon>::const_iterator itPhoton = photons->begin(); itPhoton != photons->end(); itPhoton++) {
    // construct the Photon from the ref -> save ref to original object
    unsigned int idx = itPhoton - photons->begin();
    edm::RefToBase<reco::Photon> photonRef = photons->refAt(idx);
    edm::Ptr<reco::Photon> photonPtr = photons->ptrAt(idx);
    Photon aPhoton(photonRef);
    if (embedSuperCluster_) aPhoton.embedSuperCluster();

    // store the match to the generated final state muons
    if (addGenMatch_) {
      for(size_t i = 0, n = genMatches.size(); i < n; ++i) {
          reco::GenParticleRef genPhoton = (*genMatches[i])[photonRef];
          aPhoton.addGenParticleRef(genPhoton);
      }
      if (embedGenMatch_) aPhoton.embedGenParticle();
    }

    if (efficiencyLoader_.enabled()) {
        efficiencyLoader_.setEfficiencies( aPhoton, photonRef );
    }

    if (resolutionLoader_.enabled()) {
        resolutionLoader_.setResolutions(aPhoton);
    }

    // here comes the extra functionality
    if (isolator_.enabled()) {
        isolator_.fill(*photons, idx, isolatorTmpStorage_);
        typedef pat::helper::MultiIsolator::IsolationValuePairs IsolationValuePairs;
        // better to loop backwards, so the vector is resized less times
        for (IsolationValuePairs::const_reverse_iterator it = isolatorTmpStorage_.rbegin(), ed = isolatorTmpStorage_.rend(); it != ed; ++it) {
            aPhoton.setIsolation(it->first, it->second);
        }
    }

    for (size_t j = 0, nd = deposits.size(); j < nd; ++j) {
        aPhoton.setIsoDeposit(isoDepositLabels_[j].first, (*deposits[j])[photonRef]);
    }


    // add photon ID info
    if (addPhotonID_) {
      for (size_t i = 0; i < photIDSrcs_.size(); ++i) {
	ids[i].second = (*idhandles[i])[photonRef];    
      }
      aPhoton.setPhotonIDs(ids);
    }

    if ( useUserData_ ) {
      userDataHelper_.add( aPhoton, iEvent, iSetup );
    }
    

    // add the Photon to the vector of Photons
    PATPhotons->push_back(aPhoton);
  }

  // sort Photons in ET
  std::sort(PATPhotons->begin(), PATPhotons->end(), eTComparator_);

  // put genEvt object in Event
  std::auto_ptr<std::vector<Photon> > myPhotons(PATPhotons);
  iEvent.put(myPhotons);
  if (isolator_.enabled()) isolator_.endEvent();

}

// ParameterSet description for module
void PATPhotonProducer::fillDescriptions(edm::ConfigurationDescriptions & descriptions)
{
  edm::ParameterSetDescription iDesc;
  iDesc.setComment("PAT photon producer module");

  // input source 
  iDesc.add<edm::InputTag>("photonSource", edm::InputTag("no default"))->setComment("input collection");

  iDesc.add<bool>("embedSuperCluster", true)->setComment("embed external super cluster");

  // MC matching configurables
  iDesc.add<bool>("addGenMatch", true)->setComment("add MC matching");
  iDesc.add<bool>("embedGenMatch", false)->setComment("embed MC matched MC information");
  std::vector<edm::InputTag> emptySourceVector;
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("genParticleMatch", edm::InputTag(), true) xor 
                 edm::ParameterDescription<std::vector<edm::InputTag> >("genParticleMatch", emptySourceVector, true)
	       )->setComment("input with MC match information");

  pat::helper::KinResolutionsLoader::fillDescription(iDesc);

  // photon ID configurables
  iDesc.add<bool>("addPhotonID",true)->setComment("add photon ID variables");
  edm::ParameterSetDescription photonIDSourcesPSet;
  photonIDSourcesPSet.setAllowAnything(); 
  iDesc.addNode( edm::ParameterDescription<edm::InputTag>("photonIDSource", edm::InputTag(), true) xor
                 edm::ParameterDescription<edm::ParameterSetDescription>("photonIDSources", photonIDSourcesPSet, true)
                 )->setComment("input with photon ID variables");

  // IsoDeposit configurables
  edm::ParameterSetDescription isoDepositsPSet;
  isoDepositsPSet.addOptional<edm::InputTag>("tracker"); 
  isoDepositsPSet.addOptional<edm::InputTag>("ecal");
  isoDepositsPSet.addOptional<edm::InputTag>("hcal");
  isoDepositsPSet.addOptional<std::vector<edm::InputTag> >("user");
  iDesc.addOptional("isoDeposits", isoDepositsPSet);

  // Efficiency configurables
  edm::ParameterSetDescription efficienciesPSet;
  efficienciesPSet.setAllowAnything(); // TODO: the pat helper needs to implement a description.
  iDesc.add("efficiencies", efficienciesPSet);
  iDesc.add<bool>("addEfficiencies", false);

  // Check to see if the user wants to add user data
  edm::ParameterSetDescription userDataPSet;
  PATUserDataHelper<Photon>::fillDescription(userDataPSet);
  iDesc.addOptional("userData", userDataPSet);

  edm::ParameterSetDescription isolationPSet;
  isolationPSet.setAllowAnything(); // TODO: the pat helper needs to implement a description.
  iDesc.add("userIsolation", isolationPSet);

  descriptions.add("PATPhotonProducer", iDesc);

}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(PATPhotonProducer);
