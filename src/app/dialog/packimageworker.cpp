/*
 * packimageworker.cpp
 *
 *  Created on: 2019-8-21
 *      Author: Brant
 */

#include "app_global.h"
#include "packimagedlg.h"
#include "packimageworker.h"


typedef struct _bmp_info_data_header_
{
	U8	version;	///< info data version
	U8	length;		///< header length, include version & length
	U16	bmp_num;	///< number of bmp in info data
} BmpInfoDataHeader;

typedef struct _bmp_item_
{
	char	name[32];		// bmp name
	U32		addr;			// bmp offset in this data
	U32		length;			// bmp len in this data
	U32 	original_len;	// bmp len before compress, 0 is not compress
	U16 	width;			// bmp width(Pixel)
	U16		height;			// bmp height(Pixel)
	U32		color_mode:4;	// 0:ARGB8888, 1:RGB565, 2:ARGB1555, 3:RGB888
	U32		has_trans:1;
	U32		reserved:27;
} BmpItem;

typedef struct _bmp_slice_item_
{
	unsigned int bmp_index;
	BmpSlice slice;
} BmpSliceItem;

typedef struct _iamge_file_header_
{
	char mark[4];
	unsigned int version;
	unsigned int block_num;
	unsigned int block_desc_size;
} ImageFileHeader;

typedef struct _data_block_header_
{
	char id[4];						///< id of data block
	unsigned int  offset;			///< offset of data block in this file
	unsigned int  compress_size;	///< length of data block
	unsigned int  original_size;	///< length before compress, set to 0 if data haven't been compressed
	unsigned int  checksum;			///< checksum of data block
} DataBlockHeader;

extern "C"
{
extern int compress_data(unsigned char *buf_in, unsigned int in_size, unsigned char *buf_out, unsigned int *out_size);
}


PackImageWorkerThread::PackImageWorkerThread(PackImageDialog *parent) : wxThread()
{
	m_Parent = parent;
}

void PackImageWorkerThread::OnExit()
{
}

void PackImageWorkerThread::NotifyProgress(int percent)
{
	wxThreadEvent event(wxEVT_THREAD, WORKER_EVENT);
	event.SetInt(percent);

	// send in a thread-safe way
	wxQueueEvent(m_Parent, event.Clone());
}

void PackImageWorkerThread::CalcStep(void)
{
	m_PercentWriteHeaderFinish = 1;

	if (m_Parent->m_ComprSingle)
	{
		if (m_Parent->m_ComprAll)
		{
			m_PercentWritePixelFinish = 80;
			m_PercentWriteSliceFinish = 82;
		}
		else
		{
			m_PercentWritePixelFinish = 95;
			m_PercentWriteSliceFinish = 97;
		}
	}
    m_PercentMergeFinish = 99;
}

void PackImageWorkerThread::WriteImageNumHeader(wxFile &file, int num)
{
	BmpInfoDataHeader infoHeader;

	infoHeader.version = 2;
	infoHeader.length = sizeof(BmpInfoDataHeader);
	infoHeader.bmp_num = num;

	file.Write(&infoHeader, sizeof(BmpInfoDataHeader));
}

void PackImageWorkerThread::WriteImageInfoAndPixel(wxFile &infoFile, wxFile &dataFile)
{
	unsigned int dataOffset = 0;

	WriteImageNumHeader(infoFile, m_Parent->m_ImageList->Count());
	NotifyProgress(m_PercentWriteHeaderFinish);

	for (unsigned int i = 0; i < m_Parent->m_ImageList->Count(); i++)
	{
		Image *image = m_Parent->m_ImageList->GetImageAt(i);
		BmpItem imageItem;
		int hasTrans;

		unsigned char *pixels = new unsigned char[image->GetImageDataSize()];
		unsigned char *pixelsCompr = new unsigned char[image->GetImageDataSize() * 2 + 13];
		unsigned int comprSize = image->GetImageDataSize() * 2 + 13;

		image->GetImagePixels(pixels, image->GetImageDataSize(), &hasTrans);

		memset(&imageItem, 0, sizeof(BmpItem));
		snprintf(imageItem.name, sizeof(imageItem.name), "%s", dsU2C(image->GetName()));
		imageItem.addr = dataOffset;
		imageItem.length = image->GetImageDataSize();
		imageItem.original_len = 0;
		imageItem.width = image->GetWidth2();
		imageItem.height = image->GetHeight2();
		imageItem.color_mode = image->GetColorMode2();
		imageItem.has_trans = hasTrans;
		if (m_Parent->m_ComprSingle)
		{
			compress_data(pixels, image->GetImageDataSize(), pixelsCompr, &comprSize);
			imageItem.length = comprSize;
			imageItem.original_len = image->GetImageDataSize();
		}

		infoFile.Write(&imageItem, sizeof(BmpItem));
		infoFile.Flush();
		if (m_Parent->m_ComprSingle)
		{
			dataFile.Write(pixelsCompr, comprSize);
			dataOffset += comprSize;
		}
		else
		{
			dataFile.Write(pixels, image->GetImageDataSize());
			dataOffset += image->GetImageDataSize();
		}
		dataFile.Flush();

		delete [] pixels;
		delete [] pixelsCompr;

		NotifyProgress(i * (m_PercentWritePixelFinish - m_PercentWriteHeaderFinish) / m_Parent->m_ImageList->Count() + m_PercentWriteHeaderFinish);
	}
}

bool PackImageWorkerThread::WriteImageSlice(wxFile &sliceFile)
{
	int imageHaveSliceCount = 0;

	/* write blank data for placeholder */
	WriteImageNumHeader(sliceFile, 0);

	for (unsigned int i = 0; i < m_Parent->m_ImageList->Count(); i++)
	{
		Image *image = m_Parent->m_ImageList->GetImageAt(i);
		BmpSliceItem sliceItem;

		if (image->GetSliceMode() <= 0)
			continue;

		imageHaveSliceCount++;

		sliceItem.bmp_index = i;
		sliceItem.slice.slice_mode = image->GetSliceMode();
		sliceItem.slice.fill_mode = image->GetSliceFillMode();
		sliceItem.slice.left = image->GetSliceLeft();
		sliceItem.slice.top = image->GetSliceTop();
		sliceItem.slice.fill_width = image->GetSliceFillWidth();
		sliceItem.slice.fill_height = image->GetSliceFillHeight();

		sliceFile.Write(&sliceItem, sizeof(BmpSliceItem));
		sliceFile.Flush();
	}

	/* write real image num */
	sliceFile.Seek(0);
	WriteImageNumHeader(sliceFile, imageHaveSliceCount);

	return imageHaveSliceCount > 0;
}

void PackImageWorkerThread::WriteMergeFileHeader(wxFile &destFile, int blockNum)
{
	ImageFileHeader fileHeader;

	strncpy(fileHeader.mark, "BMPS", 4);
	fileHeader.version = 1;
	fileHeader.block_num = blockNum;
	fileHeader.block_desc_size = sizeof(DataBlockHeader);

	destFile.Write(&fileHeader, sizeof(ImageFileHeader));
	destFile.Flush();
}

void PackImageWorkerThread::MergeFiles(const wxString& destFileName, wxString **srcFileNames, MergeOption *options, int srcNum)
{
	wxFile destFile(destFileName, wxFile::write);

	/* write file header */
	WriteMergeFileHeader(destFile, srcNum);

	/* write blank desc information for placeholder */
	DataBlockHeader *allDesc = new DataBlockHeader[srcNum];
	wxFileOffset descDataOffset = destFile.Tell();
	destFile.Write(allDesc, srcNum * sizeof(DataBlockHeader));

	/* get desc information and write block data */
	for (int i = 0; i < srcNum; i++)
	{
		wxFile srcFile(*(srcFileNames[i]), wxFile::read);
		wxFileOffset srcFileLength = srcFile.Length();
		unsigned int comprBufSize = srcFileLength * 2 + 13;
		unsigned char *readBuf = new unsigned char[srcFileLength];
		unsigned char *comprBuf = new unsigned char[comprBufSize];

		strncpy(allDesc[i].id, options[i].id, 4);
		allDesc[i].offset = (unsigned int)destFile.Tell();
		allDesc[i].compress_size = (unsigned int)srcFileLength;
		allDesc[i].original_size = 0;
		allDesc[i].checksum = 0;

		srcFile.Read(readBuf, srcFileLength);
		if (options[i].compress)
		{
			compress_data(readBuf, srcFileLength, comprBuf, &comprBufSize);
			allDesc[i].compress_size = comprBufSize;
			allDesc[i].original_size = (unsigned int)srcFileLength;

			destFile.Write(comprBuf, comprBufSize);
		}
		else
		{
			destFile.Write(readBuf, srcFileLength);
		}

		delete [] readBuf;
		delete [] comprBuf;
		srcFile.Close();
	}

	/* write real desc information */
	destFile.Seek(descDataOffset);
	destFile.Write(allDesc, srcNum * sizeof(DataBlockHeader));

	/* finish */
	destFile.Close();
}

wxThread::ExitCode PackImageWorkerThread::Entry()
{
	CalcStep();

	wxString infoFileName = m_Parent->m_ExportPath + _T(".info");
	wxString dataFileName = m_Parent->m_ExportPath + _T(".data");
	wxString sliceFileName = m_Parent->m_ExportPath + _T(".slic");

	wxFile dataFile(dataFileName, wxFile::write);
	wxFile infoFile(infoFileName, wxFile::write);
	wxFile sliceFile(sliceFileName, wxFile::write);

	NotifyProgress(0);

	WriteImageInfoAndPixel(infoFile, dataFile);
	dataFile.Close();
	infoFile.Close();
	NotifyProgress(m_PercentWritePixelFinish);

	/* write slice */
	bool haveSliceInformation;
	haveSliceInformation = WriteImageSlice(sliceFile);
	sliceFile.Close();
	NotifyProgress(m_PercentWriteSliceFinish);

	/* merge file */
	wxString* fileNames[3] = {&infoFileName, &dataFileName, &sliceFileName};
	MergeOption options[3];

	strncpy(options[0].id, "INFO", 4);
	strncpy(options[1].id, "DATA", 4);
	strncpy(options[2].id, "SLIC", 4);
	options[0].compress = true;
	options[1].compress = m_Parent->m_ComprAll;
	options[1].compress = true;

	MergeFiles(m_Parent->m_ExportPath, fileNames, options, haveSliceInformation ? 3 : 2);
	NotifyProgress(m_PercentMergeFinish);

	/* cleanup */
	wxRemoveFile(dataFileName);
	wxRemoveFile(infoFileName);
	wxRemoveFile(sliceFileName);
	NotifyProgress(100);

	return 0;
}
