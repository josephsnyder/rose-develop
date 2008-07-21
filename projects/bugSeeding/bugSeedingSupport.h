// Bug Seeding Support header


class SecurityFlaw
   {
  // This design permits us to organize the design by security flaw, which should scale better.

     public:
       // User option to permit seeding of original code or a separate code fragement and a selected 
       // level of grainularity (e.g. alternate statement, enclosing statement, function, class, file, etc.).
       // bool seedOriginalCode;

       // Forward declaration
          class SeedSecurityFlaw;

          class Vulnerability
             {
               public:
                    std::set<SeedSecurityFlaw*> associatedSeedingTechniques;

                    Vulnerability();
                    virtual ~Vulnerability();

                 // This abstracts the details of setting up a call to the AST traversal.
                    virtual void detector( SgProject *project ) = 0;

                    virtual std::string get_name();
                    virtual std::string get_color();

                    void associateSeeding ( SeedSecurityFlaw* seedingApproach );
             };

       // Since we first find all the vulnerabilities and then seed them, this refinds the marked vulnerabilities 
       // after the first pass as part of the seeding process.  Since we have to find the vulnerability and then
       // backup within the AST subtree to a predefined level of grainularity, this traversal generates each AST 
       // copy and then applies the seeding to each copy.  As an alternative to marking the AST with AST persistant
       // attributes, we could have just saved a list of IR nodes where vulnerabilites were defined.  That might
       // make for a simple but more coupled implementation later!
          class CloneVulnerability
             {
            // Note that the cloned vulnerability might be the whole function as a rule, or some arbitrarily sized 
            // fragment of code that contains the security flaws vulnerability.  Clones that are constructed are
            // build with there marked security vulnerability attributes in place.  This allows for separate steps
            // to mark the different vulnerablities in each clone differently (to seed all or a subset of all
            // the vulnerabilities into the constructed clones).  At present the number of clones is equal to
            // the number of identified vulnerabilities, but this need not be the case in the future.  The goal
            // is to avoid seeding all vulnerabilities into a single clone since that would be unnecessarily 
            // messy (too many security flaws to be useful in the evaluation).  A concern is that a clone with 
            // too many security flaws would likely confuse a static analysis tool with no particular purpose.
            // The static analysis of massively flawed source code is likely not interesting.

            // This function is substantially independent of the type of security flaw being evaluated, so this 
            // class can likely go into the base class.
               public:
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined. In this case
                      // we detect buffer overflow (or under flow) vulnerabilities that appear in loops.  The 
                      // reason for this degree of specificity is that that such vulnerabilities can be seeded 
                      // in a wide range of ways (e.g. modifying the loop bounds, in addition to the modifications 
                      // to the array subscript expression).
                         public:
                              bool inOriginalCode;
                              InheritedAttribute() : inOriginalCode(false) {}

                           // Note that we don't really need a copy constructor, since the one generated by C++ 
                           // would work in this limited case, it is just a safe practice.
                              InheritedAttribute(const InheritedAttribute & X) : inOriginalCode(X.inOriginalCode) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class CloneVulnerabilityTraversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines what level of graniulatity to seed the security flaw (by building the clones).
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );

                           // Pointer to the BufferOverFlowSecurityFlaw (required in traversal visit() function)
                              SecurityFlaw::SeedSecurityFlaw* associtedSeedSecurityFlaw;

                           // Constructor
                              CloneVulnerabilityTraversal(SeedSecurityFlaw* X);
                             ~CloneVulnerabilityTraversal();
                       };

                    class PrimaryVulnerabilityTraversal : public SgSimpleProcessing
                       {
                         public:
                              SgNode* primaryVulnerabilityNodeInClone;
                              SgNode* primaryVulnerabilityNodeInOriginalCode;
                              SgNode* rootOfClone;

                              void visit( SgNode* astNode );

                           // Constructor
                              PrimaryVulnerabilityTraversal(SgNode* primaryVulnerabilityNodeInClone, SgNode* primaryVulnerabilityNodeInOriginalCode, SgNode* rootOfClone )
                                 : primaryVulnerabilityNodeInClone(primaryVulnerabilityNodeInClone), 
                                   primaryVulnerabilityNodeInOriginalCode(primaryVulnerabilityNodeInOriginalCode), 
                                   rootOfClone(rootOfClone)
                                 {
                                // This function could not be defined in the source file (g++ 3.4.6 problem)
                                 }

                             ~PrimaryVulnerabilityTraversal()
                                 {
                                // This function could not be defined in the source file (g++ 3.4.6 problem)
                                 }
                       };

                 // Hide details of calling the traversal
                    static void makeClones( SgProject* project, SeedSecurityFlaw* flaw );

                 // If primaryNodeInClonedCode is always NULL then we should remove it from the argument parameter list.
                    static void markPrimaryCloneVulnerability( SgNode* primaryNodeInClonedCode, SgNode* primaryNodeInOriginalCode, SgNode* rootOfClone );
             };

          class MarkClones
             {
            // Note that the cloned vulnerability have all vulnerabilities marked (copied from the original code).
            // This step marks the IR nodes contianing vulnerabilities with additional attributes that control
            // which vulnerabilities will be seeded in a separate step.  This function is substantially independent
            // of the type of security flaw being evaluated, so this class can likely go into the base class.

               public:
                    class InheritedAttribute
                       {
                      // This class defines the constraints under which a vulnerability is defined. In this case
                      // we detect buffer overflow (or under flow) vulnerabilities that appear in loops.  The 
                      // reason for this degree of specificity is that that such vulnerabilities can be seeded 
                      // in a wide range of ways (e.g. modifying the loop bounds, in addition to the modifications 
                      // to the array subscript expression).
                         public:
                              bool inOriginalCode;
                              bool inClonedCode;

                              InheritedAttribute() : inOriginalCode(false), inClonedCode(false) {}

                           // Note that we don't really need a copy constructor, since the one generated by C++ 
                           // would work in this limited case, it is just a safe practice.
                              InheritedAttribute(const InheritedAttribute & X) : inOriginalCode(X.inOriginalCode), inClonedCode(X.inClonedCode) {}
                       };

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class MarkClonesTraversal : public SgTopDownProcessing<InheritedAttribute>
                       {
                         public:
                           // This function defines what level of graniulatity to seed the security flaw (by building the clones).
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              InheritedAttribute evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute );

                           // Pointer to the BufferOverFlowSecurityFlaw (required in traversal visit() function)
                              SecurityFlaw::SeedSecurityFlaw* associtedSeedSecurityFlaw;

                           // Constructor
                              MarkClonesTraversal(SeedSecurityFlaw* X);
                             ~MarkClonesTraversal();
                       };

                // Hide details of calling the traversal
                   static void markVulnerabilitiesInClones( SgProject* project, SeedSecurityFlaw* flaw );
             };

          class CommentClones
             {
            // This supports the annotation of clones and seciroty flaws in the AST so that the output code 
            // generated with have appropriate comments to identify the different pieces.

               public:

                 // This is the ROSE AST traversal mechanism (see ROSE Tutorial for details).
                    class CommentClonesTraversal : public SgSimpleProcessing
                       {
                         public:
                           // This function defines what level of graniulatity to seed the security flaw (by building the clones).
                           // This function defines how to recognise the vulnerability (array accesses in loops)
                              void visit ( SgNode* astNode );
                       };

                // Hide details of calling the traversal
                   static void commentClones( SgProject* project );
             };

          class SeedSecurityFlaw
             {
            // This class introduces a single kind of seeding at either a specific grainularity 
            // (file, function, block, statement) or using a specific mechanism to hide it as a 
            // security flaw (hidden behind modification of array indexing, or behind a modification 
            // to the loop bound, etc.).

               public:
                 // This value is used to control to AST clone generation to support seeding, but only 
                 // when (seedOriginalCode == false).
                    GrainularitySpecification seedGrainulatity;

                    bool seedOriginalCode;

                 // Constructor and virtual destructor
                    SeedSecurityFlaw();
                    virtual ~SeedSecurityFlaw();

                    bool get_seedOriginalCode();
                    void set_seedOriginalCode( bool t );

                 // For any security flaw marked previously as a vulnerability, back-track up the AST to a subtree 
                 // to copy so that the seeded security flaw can be introduced in the copy (so that we can leave 
                 // in place the original code associated with the security vulnerability.
                 // New function to generate a vector of positions at which to build subtrees.
                 // static std::vector<SgNode*> grainularityOfSeededCode( SgNode* astNode );
                    std::vector<SgNode*> grainularityOfSeededCode( SgNode* astNode );

                    virtual void seed( SgNode *astNode ) = 0;

                    virtual std::string get_name() { return "SecurityFlaw::SeedSecurityFlaw"; }
             };

       // Constructor and destructor
          SecurityFlaw();
          virtual ~SecurityFlaw();

       // ***********
       // Static Data 
       // ***********

       // This is the static collection of all security flaws (each is derived from the SecurityFlaw class)
          static std::vector<SecurityFlaw*> securityFlawCollection;

       // Collection of different sorts of vulnerabilities that we want to identify as a buffer over flow 
       // security flaw (there can be many).  This data member might be placed into the base class and
       // a base class provided for the Vulnerability class.
          std::vector<Vulnerability*> vulnerabilityKindList;

       // Collection of different way to seed buffer overflow security flaws into code (in a loop, in a 
       // conditional test, etc.).  This data member might be placed into the base class and
       // a base class provided for the SeedSecurityFlaw class.
          std::vector<SeedSecurityFlaw*> seedKindList;

       // **************************
       // Virtual Function Interface
       // **************************

       // This is the only virtual function required by the derived class
       // virtual void initialize () = 0;

       // This function must be defined since we at least have to detect the vulnerabilities 
       // (unless we just want to generate simple test codes).
          virtual void detectVunerabilities( SgProject *project );

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
       // If it is defined then it specifies the points in the search space where security flaws 
       // would be seeded.  Initial testing may just assume that all possible sites will be seeded.
          virtual void defineSearchSpace();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          virtual void codeCloneGeneration( SgProject *project );

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          virtual void seedSecurityFlaws( SgProject *project );

       // *************************
       // Static Function Interface
       // *************************

       // This constructs the securityFlawCollection (at a later point this collection can be 
       // tailored using some mechanism not yet defined)
          static void initialize();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          static void defineAllSearchSpaces();

       // This function need not be defined  (since we migh only want to detect vulnerabilities).
          static void generationAllClones( SgProject *project );

       // This calls the detectVunerabilities() member function for each SecurityFlaw.
          static void detectAllVunerabilities( SgProject *project );

       // This calls the seedSecurityFlaws() member function for each SecurityFlaw
          static void seedAllSecurityFlaws( SgProject *project );

       // Low level support for adding comments to locations in the source code where security 
       // vulnerabilities are identified or where security flaws are seeded.
          static void addComment( SgNode* astNode, std::string comment );

       // Support for generating a unique integer values used to build names of things (e.g. cloned function namens)
          static int uniqueValue();
   };

