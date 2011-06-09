#include "wali/nwa/NWA.hpp"

namespace wali
{
  namespace nwa
  {
    namespace nwa_pds
    {
      wpds::WPDS NWAToPDSReturns( NWA const & nwa, WeightGen & wg )
      {
	return nwa._private_NwaToPdsReturns_(wg);
      }

      wpds::WPDS NWAToPDSCalls( NWA const & nwa, WeightGen & wg )
      {
	return nwa._private_NwaToPdsCalls_(wg);
      }

      wpds::WPDS NWAToBackwardsPDSReturns( NWA const & nwa, WeightGen & wg )
      {
	return nwa._private_NwaToBackwardsPdsReturns_(wg);
      }

      wpds::WPDS NWAToBackwardsPDSCalls( NWA const & nwa, WeightGen & wg )
      {
	return nwa._private_NwaToBackwardsPdsCalls_(wg);
      }
    }


    // constructs the PDS equivalent to this NWA, stacking returns
    wpds::WPDS NWA::_private_NwaToPdsReturns_( WeightGen & wg ) const
    {
      //TODO: beware the implicit transitions
      //Q: do we need to make all transitions explicit in order to make this correct?

      wpds::WPDS result = wpds::WPDS();

      Key program = getProgramControlLocation();  //= wali::getKey("program");

      wali::sem_elem_t wgt;

      //Internal Transitions
      for( InternalIterator iit = trans.beginInternal(); iit != trans.endInternal(); iit++ )
      {      
        //(q,sigma,q') in delta_i goes to <p,q> -w-> <p,q'> in delta_1
        //where the weight w depends on sigma

        State src = Trans::getSource(*iit);
        State tgt = Trans::getTarget(*iit);
      
        if( Trans::getInternalSym(*iit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getInternalSym(*iit),
                             WeightGen::INTRA,
                             tgt, getClientInfo(tgt));  //w

        result.add_rule(program,                       //from_state (p)
                        src,									         //from_stack (q)
                        program,                       //to_state (p)
                        tgt,									         //to_stack1 (q')
                        wgt);                          //weight  (w)      
      }

      //Call Transitions 
      for( CallIterator cit = trans.beginCall(); cit != trans.endCall(); cit++ )
      {
        State src = Trans::getCallSite(*cit);
        State tgt = Trans::getEntry(*cit);
        for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
        {
          if( Trans::getCallSite(*cit) == Trans::getCallSite(*rit) )
          {
            //(q_c,sigma,q_e) in delta_c and (q_x,q_c,*,q_r) in delta_r goes to
            // <p,q_c> -w-> <p,q_e q_r> in delta_2 and w depends on sigma

            if( Trans::getCallSym(*cit) == WALI_WILD )
              wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt)); // w
            else
              wgt = wg.getWeight(src, getClientInfo(src),
                                 Trans::getCallSym(*cit),
                                 WeightGen::CALL_TO_ENTRY,  
                                 tgt, getClientInfo(tgt)); // w   

            result.add_rule(program,                      //from_state (p)
                            src,     //from_stack (q_c)
                            program,                      //to_state (p)
                            tgt,        //to_stack1 (q_e)
                            Trans::getReturnSite(*rit),   //to_stack2 (q_r)
                            wgt);                         //weight  (w)    
          }
        }
      }

      //Return Transitions
      for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
      {
        //(q_x,q_c,*,q_r) in delta_r goes to
        // <p,q_x> -w-> <p_q_xcr,epsilon> in delta_0
        // and <p_q_xcr,q_r> -1-> <p,q_r> in delta_1
        // where p_q_xcr = (p,q_x,q_c,q_r) and w depends on sigma

        State src = Trans::getExit(*rit);
        State tgt = Trans::getReturnSite(*rit);
        
        if( Trans::getReturnSym(*rit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w  
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getReturnSym(*rit),
                             WeightGen::EXIT_TO_RET,
                             tgt, getClientInfo(tgt));  // w            

        Key rstate = getControlLocation(src,Trans::getCallSite(*rit),tgt);    //p_q_xcr

        result.add_rule(program,                    //from_state (p)
                        src,                        //from_stack (q_x)
                        rstate,                     //to_state (p_q_xcr == (p,q_x,q_c,q_c))
                        wgt);                       //weight  (w)

        wgt = wg.getOne();                          // 1

        result.add_rule(rstate,                     //from_state (p_q_xcr == (p,q_x,q_c,q_r))
                        tgt,                        //from_stack (q_r)
                        program,                    //to_state (p)
                        tgt,                        //to_stack (q_r)
                        wgt);                       //weight  (1)   
      }  

      return result;
    }


    // constructs the backwards PDS equivalent to this NWA, stacking returns
    wpds::WPDS NWA::_private_NwaToBackwardsPdsReturns_( WeightGen & wg ) const
    {
      //TODO: beware the implicit transitions
      //Q: do we need to make all transitions explicit in order to make this correct?

      wpds::WPDS result = wpds::WPDS();

      Key program = getProgramControlLocation();  //= wali::getKey("program");

      wali::sem_elem_t wgt;

      //Internal Transitions
      for( InternalIterator iit = trans.beginInternal(); iit != trans.endInternal(); iit++ )
      {      
        //(q,sigma,q') in delta_i goes to <p,q'> -w-> <p,q> in delta_1
        //where the weight w depends on sigma

        State src = Trans::getSource(*iit);
        State tgt = Trans::getTarget(*iit);
        
        if( Trans::getInternalSym(*iit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getInternalSym(*iit),
                             WeightGen::INTRA,
                             tgt, getClientInfo(tgt));          // w

        result.add_rule(program,                                //from_state (p)
                        tgt,                                    //from_stack (q')
                        program,                                //to_state (p)
                        src,                                    //to_stack1 (q)
                        wgt);                                   //weight (w)      
      }

      //Call Transitions 
      for( CallIterator cit = trans.beginCall(); cit != trans.endCall(); cit++ )
      {
        State src = Trans::getCallSite(*cit);
        State tgt = Trans::getEntry(*cit);
        for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
          if( Trans::getCallSite(*cit) == Trans::getCallSite(*rit) )
          {
            //(q_c,sigma,q_e) in delta_c and (q_x,q_c,*,q_r) in delta_r goes to
            // <p,q_e> -w-> <p_q_erc,epsilon> in delta_0
            // and <p_q_erc,q_c> -1-> <p,q_c> in delta_1
            // where p_q_erc = (p,q_e,q_r,q_c) and w depends on sigma

            if( Trans::getCallSym(*cit) == WALI_WILD )
              wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt)); // w
            else
              wgt = wg.getWeight(src, getClientInfo(src), 
                                 Trans::getCallSym(*cit),
                                 WeightGen::CALL_TO_ENTRY,  
                                 tgt, getClientInfo(tgt)); // w    

            Key cstate = getControlLocation(tgt,Trans::getReturnSite(*rit),src);    //p_q_erc

            result.add_rule(program,                    //from_state (p)
                            Trans::getEntry(*cit),      //from_stack (q_e)
                            cstate,                     //to_state (p_q_erc == (p,q_e,q_r,q_c))
                            wgt);                       //weight (w)

            wgt = wg.getOne();                          // 1
            
            result.add_rule(cstate,                     //from_state (p_q_erc == (p,q_e,q_r,q_c))
                            src,                        //from_stack (q_c)
                            program,                    //to_state (p)
                            src,                        //to_stack (q_c)
                            wgt);                       //weight (1)    
          }  
      }

      //Return Transitions
      for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
      {
        //(q_x,q_c,*,q_r) in delta_r goes to
        // <p,q_r> -w-> <p,q_x q_c> in delta_2 
        // and w depends on sigma

        State src = Trans::getExit(*rit);
        State tgt = Trans::getReturnSite(*rit);
        
        if( Trans::getReturnSym(*rit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getReturnSym(*rit),
                             WeightGen::EXIT_TO_RET,
                             tgt, getClientInfo(tgt));  // w

        result.add_rule(program,                      //from_state (p)
                        tgt,                          //from_stack (q_r)
                        program,                      //to_state (p)
                        src,                          //to_stack1 (q_x)
                        Trans::getCallSite(*rit),     //to_stack2 (q_c)
                        wgt);                         //weight (w)      
      }

      return result;
    }


    // constructs the PDS equivalent to this NWA, stacking calls
    wpds::WPDS NWA::_private_NwaToPdsCalls_( WeightGen & wg ) const
    {
      //TODO: beware the implicit transitions
      //Q: do we need to make all transitions explicit in order to make this correct?

      wpds::WPDS result = wpds::WPDS();

      Key program = getProgramControlLocation();  // = wali::getKey("program");

      wali::sem_elem_t wgt;

      //Internal Transitions
      for( InternalIterator iit = trans.beginInternal(); iit != trans.endInternal(); iit++ )
      {  
        // (q,sigma,q') in delta_i goes to <p,q> -w-> <p,q'> in delta_1
        // where the weight w depends on sigma

        State src = Trans::getSource(*iit);
        State tgt = Trans::getTarget(*iit);

        if( Trans::getInternalSym(*iit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getInternalSym(*iit),
                             WeightGen::INTRA,
                             tgt, getClientInfo(tgt));           // w

        result.add_rule(program,                                //from_state (p)
                        src,                                    //from_stack (q)
                        program,                                //to_state (p)
                        tgt,                                    //to_stack1 (q')
                        wgt);                                   //weight (w)      
      }

      //Call Transitions
      for( CallIterator cit = trans.beginCall(); cit != trans.endCall(); cit++ )
      {           
        // (q_c,sigma,q_e) in delta_c goes to
        // <p,q_c> -w-> <p,q_e q_c> in delta_2 
        // and the weight w depends on sigma

        State src = Trans::getCallSite(*cit);
        State tgt = Trans::getEntry(*cit);

        if( Trans::getCallSym(*cit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt)); // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getCallSym(*cit),
                             WeightGen::CALL_TO_ENTRY,
                             tgt, getClientInfo(tgt));          // w

        result.add_rule(program,                                //from_state (p)
                        src,                                    //from_stack (q_c)
                        program,                                //to_state (p)
                        Trans::getEntry(*cit),                  //to_stack1 (q_e)
                        src,                                    //to_stack2 (q_c)
                        wgt);                                   //weight (w)  
      } 

      //Return Transitions
      int r_count = 0;
      for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
      {
        ++r_count;
          
        //std::cerr << "Return transition #" << r_count << " of " << trans.size() << "\n";
        // (q_x,q_c,sigma,q_r) in delta_r goes to 
        // <p,q_x> -w-> <p_q_xcr,epsilon> in delta_0
        // and <p_q_xcr,q_c> -1-> <p,q_r> in delta_1
        // where p_q_xcr = (p,q_x,q_c,q_r), and w depends on sigma

        State src = Trans::getExit(*rit);
        State tgt = Trans::getReturnSite(*rit);

        if( Trans::getReturnSym(*rit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w 
        else
          wgt = wg.getWeight(src, getClientInfo(src), 
                             Trans::getReturnSym(*rit),
                             WeightGen::EXIT_TO_RET,  
                             tgt, getClientInfo(tgt));    // w     

        //Note: if you change this, make sure you modify the code in NWPForest.createCA()
        Key rstate = getControlLocation(src,Trans::getCallSite(*rit),tgt);  //p_q_xcr

        result.add_rule(program,                              //from_state (p)
                        src,                                  //from_stack (q_x)
                        rstate,                               //to_state (p_q_xcr == (p,q_x,q_c,q_r))
                        wgt);                                 //weight (w)

        
        wgt = wg.getOne();                                    // 1                      
         
        result.add_rule(rstate,                               //from_state (p_q_xcr == (p,q_x,q_c,q_r))
                        Trans::getCallSite(*rit),             //from_stack (q_c)
                        program,                              //to_state (p)
                        tgt,                                  //to_stack (q_r)
                        wgt);                                 //weight (1)
      }

      return result;
    }


    // constructs the backwards PDS equivalent to this NWA, stacking calls
    wpds::WPDS NWA::_private_NwaToBackwardsPdsCalls_( WeightGen & wg ) const
    {
      //TODO: beware the implicit transitions
      //Q: do we need to make all transitions explicit in order to make this correct?

      wpds::WPDS result = wpds::WPDS();

      Key program = getProgramControlLocation();  // = wali::getKey("program"); 

      wali::sem_elem_t wgt;

      //Internal Transitions
      for( InternalIterator iit = trans.beginInternal(); iit != trans.endInternal(); iit++ )
      {  
        // (q,sigma,q') in delta_i goes to <p,q'> -w-> <p,q> in delta_1
        // where the weight w depends on sigma

        State src = Trans::getSource(*iit);
        State tgt = Trans::getTarget(*iit);
        
        if( Trans::getInternalSym(*iit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt));  // w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getInternalSym(*iit),
                             WeightGen::INTRA,
                             tgt, getClientInfo(tgt));         // w

        result.add_rule(program,                                //from_state (p)
                        tgt,                                    //from_stack (q')
                        program,                                //to_state (p)
                        src,                                    //to_stack1 (q)
                        wgt);                                   //weight (w)     
      }

      //Call Transitions
      for( CallIterator cit = trans.beginCall(); cit != trans.endCall(); cit++ )
      {    
        State src = Trans::getCallSite(*cit);
        State tgt = Trans::getEntry(*cit);
        for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
          if( Trans::getCallSite(*cit) == Trans::getCallSite(*rit) )
          {
            // (q_c,sigma,q_e) in delta_c and (q_x,q_c,*,q_r) in delta_r goes to
            // <p,q_e> -w-> <p_q_erc,epsilon> in delta_0
            // and <p_q_erc,q_r> -1-> <p,q_c> in delta_1
            // where p_q_erc = (p,q_e,q_r,q_c) and w depends on sigma
            
            if( Trans::getCallSym(*cit) == WALI_WILD )
              wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt)); // w  
            else
              wgt = wg.getWeight(src, getClientInfo(src),
                                 Trans::getCallSym(*cit),
                                 WeightGen::CALL_TO_ENTRY,
                                 tgt, getClientInfo(tgt));         // w                  

            Key cstate = getControlLocation(tgt,Trans::getReturnSite(*rit),src);  //p_q_erc

            result.add_rule(program,                              //from_state (p)
                            tgt,                                  //from_stack (q_e)
                            cstate,                               //to_state (p_q_erc == (p,q_e,q_r,q_c))
                            wgt);                                 //weight (w)  

            wgt = wg.getOne();                                    // 1

            result.add_rule(cstate,                               //from_state (p_q_erc == (p,q_e,q_r,q_c))
                            Trans::getReturnSite(*rit),           //from_stack (q_r)
                            program,                              //to_state (p)
                            src,                                  //to_stack (q_c)
                            wgt);                                 //weight (1)    
          }
      } 

      //Return Transitions
      for( ReturnIterator rit = trans.beginReturn(); rit != trans.endReturn(); rit++ )
      {
        // (q_x,q_c,sigma,q_r) in delta_r goes to 
        // <p,q_r> -w-> <p,q_x q_r> in delta_2 
        // and the weight w depends on sigma

        State src = Trans::getExit(*rit);
        State tgt = Trans::getReturnSite(*rit);

        if( Trans::getReturnSym(*rit) == WALI_WILD )
          wgt = wg.getWildWeight(src,getClientInfo(src),tgt,getClientInfo(tgt)); //w
        else
          wgt = wg.getWeight(src, getClientInfo(src),
                             Trans::getReturnSym(*rit),
                             WeightGen::EXIT_TO_RET,  
                             tgt, getClientInfo(tgt));      //w
         
        result.add_rule(program,                                //from_state (p)
                        tgt,                                    //from_stack (q_r)
                        program,                                //to_state (p)
                        src,                                    //to_stack1 (q_x)
                        tgt,                                    //to_stack2 (q_r)
                        wgt);                                   //weight (w)  
      }

      return result;
    }
      
  }
}


// Yo, Emacs!
// Local Variables:
//   c-file-style: "ellemtel"
//   c-basic-offset: 2
// End:

