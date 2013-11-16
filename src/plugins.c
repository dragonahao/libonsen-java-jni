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

JEXPORT(jobject)
JMETHOD(LoadPlugin)(JNIEnv *jEnv, jobject jThis, jstring jPluginName)
{
    /* Java vars */
    jobject jClass;
    jobject jObject;
    jmethodID jConstructor;
    jfieldID jField;
    jstring jstr;

    /* libonsen vars */
    OnsenPlugin_t *pPlugin = NULL;
    int rc;
    const char *str;

    (void)jThis;

    pPlugin = onsen_new_plugin();
    if (NULL == pPlugin) {
        return NULL;
    }

    str = (*jEnv)->GetStringUTFChars(jEnv, jPluginName, 0);
    rc = onsen_load_plugin(pPlugin, str);
    (*jEnv)->ReleaseStringUTFChars(jEnv, jPluginName, str);

    if (rc != 0) {
        onsen_free_plugin(pPlugin);
        return NULL;
    }

    jClass = (*jEnv)->FindClass(jEnv, "info/lenain/onsen/OnsenPlugin");
    jConstructor = (*jEnv)->GetMethodID(jEnv, jClass, "<init>", "()V");
    jObject = (*jEnv)->NewObject(jEnv, jClass, jConstructor);

    jField = (*jEnv)->GetFieldID(jEnv, jClass, "cRef", "J");
    (*jEnv)->SetLongField(jEnv, jObject, jField, (jlong)(pPlugin));
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "type", "C");
    (*jEnv)->SetCharField(jEnv, jObject, jField, (jchar)(pPlugin->type));
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "name", "Ljava/lang/String;");
    jstr = (*jEnv)->NewStringUTF(jEnv, pPlugin->name);
    (*jEnv)->SetObjectField(jEnv, jObject, jField, jstr);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "version", "Ljava/lang/String;");
    jstr = (*jEnv)->NewStringUTF(jEnv, pPlugin->version);
    (*jEnv)->SetObjectField(jEnv, jObject, jField, jstr);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "authors", "Ljava/lang/String;");
    jstr = (*jEnv)->NewStringUTF(jEnv, pPlugin->authors);
    (*jEnv)->SetObjectField(jEnv, jObject, jField, jstr);

    return jObject;
}

JEXPORT(void)
JMETHOD(UnloadPlugin)(JNIEnv *jEnv, jobject jThis, jlong jPluginRef)
{
    /* libonsen vars */
    OnsenPlugin_t *pPlugin = (OnsenPlugin_t *)jPluginRef;

    (void)jEnv;
    (void)jThis;

    onsen_free_plugin(pPlugin);
}

JEXPORT(jboolean)
JMETHOD(IsFileSupported)(JNIEnv *jEnv, jobject jThis, jlong jPluginRef,
                            jlong jFileRef)
{
    /* libonsen vars */
    OnsenFile_t *pFile = (OnsenFile_t *)jFileRef;
    OnsenPlugin_t *pPlugin = (OnsenPlugin_t *)jPluginRef;
    long lOffset;
    void *pData;

    (void)jEnv;
    (void)jThis;

    if (0 == pFile->isMmaped) {
        pData = (void *)(&(pFile->fd));
        lOffset = 0;
    } else {
        pData = pFile->data;
        lOffset = pFile->fileSize;
    }

    return (pPlugin->isFileSupported(pFile->isMmaped,
                                     pFile->filename,
                                     pData,
                                     lOffset));
}
