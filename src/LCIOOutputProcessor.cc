#include "marlin/LCIOOutputProcessor.h"
#include <iostream>

#include "IMPL/LCRunHeaderImpl.h"
#include "UTIL/LCTOOLS.h"
#include "EVENT/LCCollection.h"
#include "IMPL/LCCollectionVec.h"

#include <algorithm>
#include <bitset>

namespace marlin{
  
  LCIOOutputProcessor anLCIOOutputProcessor ;
  
  LCIOOutputProcessor::LCIOOutputProcessor() : Processor("LCIOOutputProcessor") {
    
    _description = "Writes the current event to the specified LCIO outputfile."
      " Needs to be the last ActiveProcessor." ;
    
    
    registerProcessorParameter( "LCIOOutputFile" , 
				" name of output file "  ,
				_lcioOutputFile ,
				std::string("outputfile.slcio") ) ;
    
    registerProcessorParameter( "LCIOWriteMode" , 
				"write mode for output file:  WRITE_APPEND or WRITE_NEW"  ,
				_lcioWriteMode ,
				std::string("None") ) ;


    StringVec dropNamesExamples ;
    dropNamesExamples.push_back("TPCHits");
    dropNamesExamples.push_back("HCalHits");
    

    registerOptionalParameter( "DropCollectionNames" , 
 			       "drops the named collections from the event"  ,
 			       _dropCollectionNames ,
 			       dropNamesExamples ) ;
    
    
    StringVec dropTypesExample ;
    dropTypesExample.push_back("SimTrackerHit");
    dropTypesExample.push_back("SimCalorimeterHit");
    
    registerOptionalParameter( "DropCollectionTypes" , 
			       "drops all collections of the given type from the event"  ,
			       _dropCollectionTypes ,
			       dropTypesExample ) ;
    
  }

void LCIOOutputProcessor::init() { 

  printParameters() ;

  _nRun = 0 ;
  _nEvt = 0 ;

  _lcWrt = LCFactory::getInstance()->createLCWriter() ;

  if( _lcioWriteMode == "WRITE_APPEND" ) {
    
    _lcWrt->open( _lcioOutputFile , LCIO::WRITE_APPEND ) ;
  }
  else if( _lcioWriteMode == "WRITE_NEW" ) {
    
    _lcWrt->open( _lcioOutputFile , LCIO::WRITE_NEW ) ;
  }
  else {
    _lcWrt->open( _lcioOutputFile ) ;
  }

//   _lcWrt->writeRunHeader( new LCRunHeaderImpl ) ;
//   _lcWrt->close() ;
//   _lcWrt->open( _lcioOutputFile , LCIO::WRITE_APPEND ) ;
}



void LCIOOutputProcessor::processRunHeader( LCRunHeader* run) { 

//    std::cout << "LCIOOutputProcessor::processRun()  " << name() <<" this << " << this
// 	     << " in run " << run->getRunNumber() 
// 	     << std::endl ;

  _lcWrt->writeRunHeader( run ) ;

  _nRun++ ;
} 

  void LCIOOutputProcessor::dropCollections( LCEvent * evt ) { 

//     bool isLCIO_v01_04_01 = false ; 
// #ifdef LCIO_PATCHVERSION_GE
//     isLCIO_v01_04_01 = LCIO_PATCHVERSION_GE( 1, 4, 1) ;
// #endif
//     if( ! isLCIO_v01_04_01 ) {
//       static bool firstCall = true ;
//       if( firstCall ) {
// 	std::cout << " *** WARNING: LCIOOutputProcessor::dropCollections requires LCIO v01-04-01 or higher "
// 		  << std:: endl
// 		  << "      -> no collections droped from the event !! " 
// 		  << std:: endl ;
//       }
//       firstCall = false ;

//       return ;
//     }

    const StringVec*  colNames = evt->getCollectionNames() ;


    // if all tracker hits are droped we don't store the hit pointers with the tracks below ...
    bool trackerHitsDroped = false ;
    bool calorimeterHitsDroped = false ;

    if( parameterSet("DropCollectionTypes") ){
      
      if( std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end()
		     , LCIO::TRACKERHIT )   != _dropCollectionTypes.end()  ) {
	
	trackerHitsDroped =  true ;
      }
      
      if( std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end()
		     , LCIO::CALORIMETERHIT )   != _dropCollectionTypes.end()  ) {
	
	calorimeterHitsDroped =  true ;
      }
    }      
    
    for( StringVec::const_iterator it = colNames->begin();
	 it != colNames->end() ; it++ ){
      
      LCCollectionVec*  col =  dynamic_cast<LCCollectionVec*> (evt->getCollection( *it ) ) ;
      
      std::string type  = col->getTypeName() ;
      
      if( parameterSet("DropCollectionTypes") && std::find( _dropCollectionTypes.begin(), _dropCollectionTypes.end(), type ) 
	  != _dropCollectionTypes.end()  ) {
	
	col->setTransient( true ) ;
      }
      if( parameterSet("DropCollectionNames") && std::find( _dropCollectionNames.begin(), _dropCollectionNames.end(), *it ) 
	  != _dropCollectionNames.end() ) {
	
	col->setTransient( true ) ;
      }

      // don't store hit pointers if hits are droped
      if(  type == LCIO::TRACK && trackerHitsDroped ){
	
	std::bitset<32> flag( col->getFlag() ) ;
	flag[ LCIO::TRBIT_HITS ] = 0 ;
 	col->setFlag( flag.to_ulong() ) ;
      }
      if(  type == LCIO::CLUSTER && calorimeterHitsDroped ){
	
	std::bitset<32> flag( col->getFlag() ) ;
	flag[ LCIO::CLBIT_HITS ] = 0 ;
 	col->setFlag( flag.to_ulong() ) ;
      }

    }
  }

void LCIOOutputProcessor::processEvent( LCEvent * evt ) { 
//   std::cout << "LCIOOutputProcessor::processEvent()  " << name() 
// 	    << " in event " << evt->getEventNumber() << " (run " << evt->getRunNumber() << ") "
// 	    << std::endl ;

//   std::cout << " writing event : " << std::endl ;
//   LCTOOLS::dumpEvent( evt ) ;


  dropCollections( evt ) ;

  _lcWrt->writeEvent( evt ) ;

  _nEvt ++ ;
}

void LCIOOutputProcessor::end(){ 

  std::cout  << std::endl 
	     << "LCIOOutputProcessor::end()  " << name() 
	     << ": " << _nEvt << " events in " << _nRun << " runs written to file  " 
	     <<  _lcioOutputFile  
	     << std::endl
	     << std::endl ;
  
  _lcWrt->close() ;

}

} // namespace marlin{
