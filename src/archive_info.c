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
#include <libonsen/archive_info.h>
#include <libonsen/archive_plugin.h>

JEXPORT(jobject)
JMETHOD(GetArchiveInfo)(JNIEnv *jEnv, jobject jThis, jlong jPluginRef,
                            jlong jFileRef)
{
    /* Java vars */
    jobject jClass;
    jobject jArrayClass;
    jmethodID jInit;
    jmethodID jArrayInit;
    jmethodID jArrayAdd;
    jfieldID jField;
    jstring jstr;

    jobject jInfo;
    jobject jEntry;
    jobject jArray;
    jobject jArrayTmp;
    jbyteArray jFilename;
    jsize jFilenameLen;

    /* libonsen vars */
    OnsenFile_t          *pFile = (OnsenFile_t *)jFileRef;
    OnsenPlugin_t        *pPlugin = (OnsenPlugin_t *)jPluginRef;
    OnsenArchivePlugin_t *pInstance = NULL;
    OnsenArchiveInfo_t   *pInfo = NULL;
    OnsenArchiveEntry_t  *pEntry = NULL;
    long lOffset;
    void *pData;
    int rc;
    int i;
    int j;

    (void)jThis;

    if (0 == pFile->isMmaped) {
        pData = (void *)(&(pFile->fd));
        lOffset = 0;
    } else {
        pData = pFile->data;
        lOffset = pFile->fileSize;
    }

    /* Retrieve archive info */
    pInfo = onsen_new_archive_info();
    pInstance = pPlugin->instance;
    rc = pInstance->getArchiveInfo(pFile->isMmaped,
                                   lOffset,
                                   pFile->filename,
                                   pData,
                                   pInfo);
    if (0 == rc) {
        onsen_free_archive_info(pInfo);
        return NULL;
    }

    /* Build archive entries Java ArrayList */
    jArrayClass = (*jEnv)->FindClass(jEnv, "java/util/ArrayList");
    jArrayInit =  (*jEnv)->GetMethodID(jEnv, jArrayClass, "<init>", "()V");
    jArray = (*jEnv)->NewObject(jEnv, jArrayClass, jArrayInit);
    jArrayAdd = (*jEnv)->GetMethodID(jEnv, jArrayClass, "add",
                                                       "(Ljava/lang/Object;)Z");

    /* Adding all archive entries Java Objects */
    jClass = (*jEnv)->FindClass(jEnv, "info/lenain/onsen/OnsenArchiveEntry");
    jInit = (*jEnv)->GetMethodID(jEnv, jClass, "<init>", "()V");

    for (i = 0; i < pInfo->archiveEntriesCount+1; i++) {
        pEntry = pInfo->archiveEntries[i];

        jEntry = (*jEnv)->NewObject(jEnv, jClass, jInit);

        jField = (*jEnv)->GetFieldID(jEnv, jClass, "cRef", "J");
        (*jEnv)->SetLongField(jEnv, jEntry, jField, (jlong)pEntry);
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "filename",
                                                          "Ljava/lang/String;");
        jstr = (*jEnv)->NewStringUTF(jEnv, pEntry->filename);
        (*jEnv)->SetObjectField(jEnv, jEntry, jField, jstr);

        if (NULL != pEntry->filename) {
            jFilenameLen = strlen(pEntry->filename);
            jFilename = (*jEnv)->NewByteArray(jEnv, jFilenameLen);
            jField = (*jEnv)->GetFieldID(jEnv, jClass, "filenameC", "[B");
            (*jEnv)->SetByteArrayRegion(jEnv, jFilename, 0, jFilenameLen,
                                                    (jbyte*)(pEntry->filename));
            (*jEnv)->SetObjectField(jEnv, jEntry, jField, jFilename);
        }
        
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "offset", "I");
        (*jEnv)->SetIntField(jEnv, jEntry, jField, pEntry->offset);
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "size", "I");
        (*jEnv)->SetIntField(jEnv, jEntry, jField, pEntry->size);
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "compressedSize", "I");
        (*jEnv)->SetIntField(jEnv, jEntry, jField, pEntry->compressedSize);
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "isEncrypted", "Z");
        (*jEnv)->SetBooleanField(jEnv, jEntry, jField, pEntry->isEncrypted);
        jField = (*jEnv)->GetFieldID(jEnv, jClass, "isCompressed", "Z");
        (*jEnv)->SetBooleanField(jEnv, jEntry, jField, pEntry->isCompressed);

        /* Build additional fields Java ArrayList */
        jArrayTmp = (*jEnv)->NewObject(jEnv, jArrayClass, jArrayInit);
        for (j = 0; j < pEntry->addlFdsCount; j++) {
            jstr = (*jEnv)->NewStringUTF(jEnv, pEntry->addlFds[j]);
            (*jEnv)->CallBooleanMethod(jEnv, jArrayTmp, jArrayAdd, jstr);
        }

        jField = (*jEnv)->GetFieldID(jEnv, jClass, "additionalFields",
                                                       "Ljava/util/ArrayList;");
        (*jEnv)->SetObjectField(jEnv, jEntry, jField, jArrayTmp);

        (*jEnv)->CallBooleanMethod(jEnv, jArray, jArrayAdd, jEntry);
    }

    /* Building archive info Java Object */
    jClass = (*jEnv)->FindClass(jEnv, "info/lenain/onsen/OnsenArchiveInfo");
    jInit = (*jEnv)->GetMethodID(jEnv, jClass, "<init>", "()V");
    jInfo = (*jEnv)->NewObject(jEnv, jClass, jInit);

    jField = (*jEnv)->GetFieldID(jEnv, jClass, "cRef", "J");
    (*jEnv)->SetLongField(jEnv, jInfo, jField, (jlong)pInfo);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "archiveFileSize", "J");
    (*jEnv)->SetLongField(jEnv, jInfo, jField,
                                              (jlong)(pInfo->archiveFileSize));
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "archiveFilenamesEncoding", "I");
    (*jEnv)->SetIntField(jEnv, jInfo, jField, pInfo->archiveFilenamesEncoding);
    jField = (*jEnv)->GetFieldID(jEnv, jClass, "archiveEntries",
                                                       "Ljava/util/ArrayList;");
    (*jEnv)->SetObjectField(jEnv, jInfo, jField, jArray);

    return jInfo;
}

JEXPORT(void)
JMETHOD(FreeArchiveInfo)(JNIEnv *jEnv, jobject jThis, jlong jInfoRef)
{
    /* libonsen vars */
    OnsenArchiveInfo_t *pInfo = (OnsenArchiveInfo_t *)jInfoRef;

    (void)jEnv;
    (void)jThis;

    onsen_free_archive_info(pInfo);
}
