#ifndef wali_KEY_PAIR_SOURCE_GUARD
#define wali_KEY_PAIR_SOURCE_GUARD 1

/*!
 * @author Nick Kidd
 */

#include "wali/Common.hpp"
#include "wali/KeySource.hpp"
#include "wali/KeyContainer.hpp"

namespace wali
{
    class KeyPairSource : public wali::KeySource
    {
        public:
            KeyPairSource( wali_key_t k1, wali_key_t k2 );

            KeyPairSource( const KeyPair& kp );

            virtual ~KeyPairSource();

            virtual bool equal( KeySource* rhs );

            virtual size_t hash() const;

            virtual std::ostream& print( std::ostream& o ) const;

            // TODO: probably shouldn't be virtual
            virtual KeyPair get_key_pair() const;

            // TODO: probably shouldn't be virtual
            virtual Key first() const;

            // TODO: probably shouldn't be virtual
            virtual Key second() const;

        protected:
            KeyPair kp;

    }; // class KeyPairSource

} // namespace wali

#endif  // wali_KEY_PAIR_SOURCE_GUARD

/* Yo, Emacs!
;;; Local Variables: ***
;;; tab-width: 4 ***
;;; End: ***
*/

