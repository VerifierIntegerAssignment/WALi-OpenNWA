/*!
 * @author Nick Kidd
 */

#include "wali/witness/VisitorPrinter.hpp"
#include "wali/witness/Witness.hpp"
#include "wali/witness/WitnessExtend.hpp"
#include "wali/witness/WitnessCombine.hpp"
#include "wali/witness/WitnessTrans.hpp"
#include "wali/witness/WitnessRule.hpp"

namespace wali {

    namespace witness {

        VisitorPrinter::VisitorPrinter( std::ostream& o ) 
            : os(o), depth(0)
        {
        }

        VisitorPrinter::~VisitorPrinter()
        {
        }

        void VisitorPrinter::formatDepth() const
        {
            for( size_t i = 0; i < depth ; i++ ) os << "  |";
        }

        bool VisitorPrinter::visit( Witness * w ATTR_UNUSED )
        {
            return true;
        }

        bool VisitorPrinter::visitExtend( WitnessExtend * w )
        {
            formatDepth();
            os << "WitnessExtend: ";
            w->weight()->print(os) << std::endl;
            VisitorPrinter vp(os,depth+1);
            if( w->hasLeft() ) {
                w->left()->accept(vp,true);
            }
            if( w->hasRight() ) {
                w->right()->accept(vp,true);
            }
            // tell WitnessExtend to not visit his children
            return false;
        }

        bool VisitorPrinter::visitCombine( WitnessCombine * w )
        {
            formatDepth();
            os << "WitnessCombine: ";
            w->weight()->print(os) << std::endl;
            if( 1 ) {
                VisitorPrinter vp(os,depth+1);
                std::list< witness_t >::iterator it = w->children().begin();
                for( ; it != w->children().end() ; it++ ) {
                    witness_t child = *it;
                    child->accept(vp,true);
                }
            }
            else {
                os << "   has " << w->children().size() << " children\n";
            }
            // tell WitnessCombine to not visit his children
            return false;

        }

        bool VisitorPrinter::visitRule( WitnessRule * w )
        {
            formatDepth();
            os << "WitnessRule: ";
            w->getRuleStub().print(os) << std::endl;
            return true;
        }

        bool VisitorPrinter::visitTrans( WitnessTrans * w )
        {
            formatDepth();
            os << "WitnessTrans: ";
            w->getTrans().print(os) << std::endl;
            return true;
        }

        VisitorPrinter::VisitorPrinter( std::ostream& o, size_t d ) 
            : os(o), depth(d)
        {
        }

    } // namespace witness

} // namespace wali

/* Yo, Emacs!
   ;;; Local Variables: ***
   ;;; tab-width: 4 ***
   ;;; End: ***
   */

