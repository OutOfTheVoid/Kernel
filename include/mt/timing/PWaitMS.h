#ifndef MT_TIMING_PWAIT_H
#define MT_TIMING_PWAIT_H

#include <cpputil/linkage.h>

C_LINKAGE void mt_timing_pwaitms ( double TimeMS );
C_LINKAGE void mt_timing_pwaitmsMechanism ( void ( * Mechanism ) ( double TimeMS ) );

#endif
