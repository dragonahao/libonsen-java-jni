/*
 * Copyright 2012 - Etienne 'lenaing' GIRONDEL <lenaing@gmail.com>
 * 
 * libonsen-java-jni
 * -----------------
 * This library is a JNI wrapper to the libonsen library.
 * 
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use, 
 * modify and/ or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL
 * "http://www.cecill.info". 
 * 
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability. 
 * 
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or 
 * data to be ensured and,  more generally, to use and operate it in the 
 * same conditions as regards security. 
 * 
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-C license and that you accept its terms.
 */
#include "globals.h"
#include <libonsen/string_utils.h>

JEXPORT(jobject)
JMETHOD(OpenFile)(JNIEnv *jEnv, jobject jThis, jstring jfilename)
{
    /* Java vars */
    jobject jClass;
    jobject jObject;
    jmethodID jConstructor;
    jfieldID jField;
    jstring jstr;

    /* libonsen vars */
    OnsenFile_t *pFile = NULL;
    const char *str;

    (void)jThis;

    str = (*jEnv)->GetStringUTFChars(jEnv, jfilename, 0);
    pFile = onsen_new_disk_file(str, ONSEN_READ_ONLY, 0);
    (*jEnv)->ReleaseStringUTFChars(jEnv, jfilename, str);

    if (NULL == pFile) {
        return NULL;
    }

    jClass = (*jEnv)->FindClass(jEnv, "info/lenain/onsen/OnsenFile");
    jConstructor = (*jEnv)->GetMethodID(jEnv, jClass, "<init>", "()V");
    jObject = (*jEnv)->NewObject(jEnv, jClass, jConstructor);

    jField = (*jEnv)->GetFieldID(jEnv, jClass, "cRef", "J");
    (*jEnv)->SetLongField(jEnv, jObject, jField, (jlong)pFile);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "filename",
                                                          "Ljava/lang/String;");
    jstr = (*jEnv)->NewStringUTF(jEnv, pFile->szFilename);
    (*jEnv)->SetObjectField(jEnv, jObject, jField, jstr);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "isMmaped", "Z");
    (*jEnv)->SetBooleanField(jEnv, jObject, jField, pFile->bIsMmaped);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "fd", "I");
    (*jEnv)->SetIntField(jEnv, jObject, jField, pFile->iFd);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "fileSize", "J");
    (*jEnv)->SetLongField(jEnv, jObject, jField, pFile->lFileSize);

    return jObject;
}

JEXPORT(void)
JMETHOD(CloseFile) (JNIEnv *jEnv, jobject jThis, jlong jFileRef)
{
    /* libonsen vars */
    OnsenFile_t *pFile = (OnsenFile_t *)jFileRef;

    (void)jEnv;
    (void)jThis;

    onsen_free_disk_file(pFile);
}
