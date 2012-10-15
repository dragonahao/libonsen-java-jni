#include "globals.h"

JEXPORT(void)
JMETHOD(Init) (JNIEnv *jEnv, jobject jThis, jint jVerbosity)
{
    (void)jEnv;
    (void)jThis;

    onsen_init(jVerbosity);
}
