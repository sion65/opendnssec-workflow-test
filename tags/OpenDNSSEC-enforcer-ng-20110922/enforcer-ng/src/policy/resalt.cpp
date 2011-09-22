#include "config.h"

/* On MacOSX arc4random is only available when we 
   undef _ANSI_SOURCE and define _DARWIN_C_SOURCE. */
#ifdef __APPLE__
#undef _ANSI_SOURCE
#define _DARWIN_C_SOURCE 1
#include <stdlib.h>
#endif

#include "resalt.h"

#include "shared/duration.h"

int PolicyUpdateSalt(::ods::kasp::Policy &policy)
{
    /* This will be the salt that we create 
     * salt never longer than 255 so 512 can
     * hold a hex encoded version of the salt.
    */
    char salt[512];
    /* for mapping random numbers in range 0..15 to hex characters */
    const char *hex_chars = "0123456789abcdef";
    /* Number of hex characters */
    unsigned int nhex_chars = strlen(hex_chars);
    
    if (!policy.denial().has_nsec3()) {
        // no nsec3 present so no need to resalt either.
        return 0;
    }    

    const ::ods::kasp::NSEC3 &nsec3 = policy.denial().nsec3();
    if ( nsec3.has_salt() && nsec3.has_salt_last_change() ) {
        if (time_now() < nsec3.salt_last_change()+nsec3.resalt() ) {
            // Current time is within resalt period since last change.
            return 0;
        }
    }
    
    // Seems we have a nsec3 section that needs a salt generated
    
    // required uint32 resalt = 3 [(xml).path="Resalt", (xml).type=duration]; // re-salting interval
    // required uint32 saltlength = 6 [(xml).path="Hash/Salt/@length"];// nsec3 salt length 0..255
    // optional string salt = 7 [(xml).path="Hash/Salt"];// the actual salt is generated by the enforcer e.g. 0438eb9a93a6d6c5
    // optional uint32 salt_last_change = 8 [(xml).path="Hash/Salt/@lastchanged"]; // timestamp for when the last resalt took place

    unsigned int saltlength = nsec3.saltlength();
    if (saltlength <= 0 || saltlength > 255) {
        // error, saltlength out of range..
        return -3;
    }
        
#ifdef HAVE_ARC4RANDOM
    for (int i = 0; i < 2*saltlength; ++i) {
        salt[i] = hex_chars[ arc4random() % nhex_chars ];
    }
#else
   srand( time(NULL) );
    for (int i = 0; i < 2*saltlength; ++i) {
        salt[i] = hex_chars[rand()%nhex_chars];

    }
#endif

    policy.mutable_denial()->mutable_nsec3()->set_salt(&salt[0],2*saltlength);
    policy.mutable_denial()->mutable_nsec3()->set_salt_last_change(time_now());
    return 1;
}
