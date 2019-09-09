/*
 * packimageworker.h
 *
 *  Created on: 2019-8-21
 *      Author: Brant
 */

#ifndef PACK_IMAGE_WORKER_H_
#define PACK_IMAGE_WORKER_H_


typedef struct _merge_option_
{
	char id[4];
	bool compress;
} MergeOption;


class PackImageWorkerThread : public wxThread
{
public:
	PackImageWorkerThread(PackImageDialog *parent);

    // thread execution starts here
    virtual void *Entry() wxOVERRIDE;

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit() wxOVERRIDE;

private:
    void NotifyProgress(int percent);
    void CalcStep(void);
    void WriteImageNumHeader(wxFile &file, int num);
    void WriteImageInfoAndPixel(wxFile &infoFile, wxFile &dataFile);
    bool WriteImageSlice(wxFile &sliceFile);

    void WriteMergeFileHeader(wxFile &destFile, int blockNum);
    void MergeFiles(const wxString& destFileName, wxString **srcFileNames, MergeOption *options, int srcNum);

    PackImageDialog *m_Parent;

    int m_PercentWriteHeaderFinish = 1;
    int m_PercentWritePixelFinish = 78;
    int m_PercentWriteSliceFinish = 80;
    int m_PercentMergeFinish = 100;
};


#endif /* PACK_IMAGE_WORKER_H_ */
