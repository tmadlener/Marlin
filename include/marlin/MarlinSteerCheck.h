#ifndef MARLINSTEERCHECK_H
#define MARLINSTEERCHECK_H

/**@class
 * this class is a Marlin Steering File consistency check Tool.
 * It can parse XML steering files using the marlin parser and search
 * the lcio file(s) specified at the global section for available collections.
 * Together with the active and inactive processors specified in the steering file
 * and their respective collections it can perform a full consistency check and
 * check for inavailable collections.  The Program checks if an adequate
 * available collection exists for every input collection of a processor.
 * 
 * (where input collection means a collection the processor requires as input, 
 * and output collections mean a collection that is created in the processor and
 * marked as available for the upcoming processors)
 *
 * Information about the consistency check can be dumped.
 * 
 *
 * @author Benjamin Eberhardt, Jan Engels
 * @version $Id: MarlinSteerCheck.h,v 1.18 2006-11-20 15:53:24 engels Exp $
 */

#include "marlin/CCProcessor.h"
#include "marlin/CCCollection.h"

#include "marlin/XMLParser.h"

// to make colored output
#define clrscr() printf("\033[2J")
#define dblue() printf("\x1b[34m")
#define dred() printf("\x1b[31m")
#define dyellow() printf("\x1b[33m")
#define dgreen() printf("\x1b[32m")
#define dunderline() printf("\x1b[4m")
#define ditalic() printf("\x1b[3m")
#define ddunkel() printf("\x1b[2m")
#define dhell() printf("\x1b[1m")
#define dblink() printf("\x1b[5m")
#define endcolor() printf("\x1b[m")

#define MAXEVENTS 30

namespace marlin {

  class CCCollection;
  class CCProcessor;

  typedef std::vector< CCProcessor* > ProcVec;

  class MarlinSteerCheck{

  public:

    // Constructor
    MarlinSteerCheck( const char* steerFileName=NULL );

    // Destructor
    ~MarlinSteerCheck();

    /** Returns the Marlin Processors */
    CMProcessor* getMProcs() { return _marlinProcs; };
    
    /** Returns the Active Processors */
    ProcVec& getAProcs() { return _aProc; };
    
    /** Returns the Inactive Processors */
    ProcVec& getIProcs() { return _iProc; };

    /** Returns true if an Active Processor of the given type already exists */
    bool existsActiveProcessor( const std::string& type );
    
    /** Returns the collections read from LCIO files */
    ColVec& getLCIOCols() const;
    
    /** Returns the names of the LCIO files found in the global section*/
    StringVec& getLCIOFiles(){ return _lcioFiles; }

    /** Returns a list of all available Collections for a given type and processor (to use in a ComboBox) */
    sSet& getColsSet( const std::string& type, CCProcessor* proc );

    /** Returns the Global Parameters */
    StringParameters* getGlobalParameters(){ return _gparam; }
    
    /** Returns the Errors for an Active Processor at the given index */
    const std::string& getErrors( unsigned int index );
  
    /** Add LCIO file and read all collections inside it */
    const std::string addLCIOFile( const std::string& file );

    /** Remove LCIO file and all collections associated to it */
    void remLCIOFile( const std::string& file );
    
    /** Change the LCIO File at the given index to the new given position */
    void changeLCIOFilePos( unsigned int pos, unsigned int newPos );
 
    /** Add a new processor */
    void addProcessor( bool status, const std::string& name, const std::string& type, StringParameters* p=NULL );

    /** Remove processor with the given status at the given index */
    void remProcessor( unsigned int index, bool status );

    /** Activate processor at the given index */
    void activateProcessor( unsigned int index );
    
    /** Deactivate processor at the given index */
    void deactivateProcessor( unsigned int index );

    /** Change the active processor at the given index to the new given position */
    void changeProcessorPos( unsigned int pos, unsigned int newPos );
    
    /** Performs a check at all active processors to search for unavailable collections */
    void consistencyCheck();

    /** Saves the data to an XML file with the given name
     *  Returns false if error occured */
    bool saveAsXMLFile( const std::string& file );

    /* Returns the relative path of the XML file */
    const std::string getXMLFileRelPath(){ return _XMLFileRelPath; }
    
    /* Returns the absolute path of the XML file */
    const std::string getXMLFileAbsPath(){ return _XMLFileAbsPath; }
    
    /* Returns the XML fileName */
    const std::string getXMLFileName(){ return _XMLFileName; }
    
    /* Returns the XML file as given by the user in the command prompt */
    const std::string getXMLFile(){ return _steeringFile; }
    
    /** Dumps all information read from the steering file to stdout */
    void dump_information();

    /** Dumps collection errors found in the steering file for all active processors */
    void dump_colErrors();

    int getErrors(){ return errors_found; };

    
  private:
    
    /////////////////////////////////////////////////////
    //METHODS
    /////////////////////////////////////////////////////

    // Return all available collections from the active or inactive processors
    ColVec& getProcCols( const ProcVec& v ) const;

    // Return all available collections found in LCIO files and active/inactive processors
    ColVec& getAllCols() const;
   
    // Return all available collections found in LCIO files and active/inactive processors
    ProcVec& getAllProcs() const;
    
    // Parse the given steering file
    bool parseXMLFile( const std::string& file );
    
    // Pop a processor out of a vector from processors
    CCProcessor* popProc(ProcVec& v, CCProcessor* p);

    // Find matching collections of a given type and value in a vector of collections for a given processor
    ColVec& findMatchingCols( ColVec& v, CCProcessor* srcProc, const std::string& type, const std::string& value="UNDEFINED" );
    
    /////////////////////////////////////////////////////
    //VARIABLES
    /////////////////////////////////////////////////////

    int errors_found;			//errors variable
    IParser* _parser;			//parser
    StringParameters* _gparam;		//global parameters (without LCIO Files)
    
    std::string _steeringFile;		//steering file name + path as passed by the user in the command line
    std::string _XMLFileName;		//steering file name
    std::string _XMLFileAbsPath;	//steering file (absolute path without filename)
    std::string _XMLFileRelPath;	//steering file (relative path without filename)

    ProcVec _aProc;			//active processors
    ProcVec _iProc;			//inactive processors
    sColVecMap _lcioCols;		//LCIO collections
    StringVec _lcioFiles;		//LCIO filenames
    
    sSet _colValues;			//all available collection values for a given type (use in ComboBox)

    CMProcessor* _marlinProcs;		//Sigleton class containing all marlin processors
  };

} // namespace
#endif
