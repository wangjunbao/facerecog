// RecogDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "frtest.h"
#include "RecogDlg.h"
#include "WndUtils.h"
#include "ProjDefs.h"

// CRecogDlg �Ի���

IMPLEMENT_DYNAMIC(CRecogDlg, CDialog)

CRecogDlg::CRecogDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRecogDlg::IDD, pParent)
, m_bUseBuf(FALSE)
{
	m_thrdMatch = NULL;
	noface = cvLoadImage("noface.bmp");
	resss = cvLoadImage("result.bmp");
}

CRecogDlg::~CRecogDlg()
{
	cvReleaseImage(&noface);
	cvReleaseImage(&resss);
}

void CRecogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_USEBUF, m_bUseBuf);
}


BEGIN_MESSAGE_MAP(CRecogDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CRecogDlg::OnBnClickedCancel)
	//ON_NOTIFY(NM_CLICK, IDC_INFO, &CRecogDlg::OnNMClickFilepath)
	ON_BN_CLICKED(IDC_PIC_RECOG, &CRecogDlg::OnBnClickedPicRecog)
	ON_BN_CLICKED(IDC_USEBUF, &CRecogDlg::OnBnClickedUsebuf)
END_MESSAGE_MAP()

BOOL CRecogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	faceSz = g_faceMngr->m_faceSz;
	showSz = faceSz;

	if (showSz.height != 160) // (160*130)��Ԥ����ʾ��С�������һ����С��������е���
		showSz *= (160.0/showSz.height);
	if (showSz.width > 130)
		showSz *= (130.0/showSz.width);


	if (g_bHasWebcam)
	{
		m_bFoundFace = false;
		SetDlgItemText(IDC_INFO, "    �����ʾ\"no face found\"���������̬�����պͱ���");
		m_frame = cvCreateImage(g_webcam.GetFrameSize(), IPL_DEPTH_8U, 3);
		m_bMatchStarted = true;
		m_thrdMatch = ::AfxBeginThread(ShowMatchProc, this);
		GetDlgItem(IDC_USEBUF)->ShowWindow(SW_SHOW);
	}
	else
	{
		SetDlgItemText(IDC_INFO, "");
		GetDlgItem(IDC_USEBUF)->EnableWindow(FALSE);
	}

	InitWindows();

	RECT rc;
	GetWindowRect(&rc);
	SetWindowPos(NULL, rc.left + 30, rc.top + 300, 0,0, SWP_NOSIZE | SWP_NOZORDER);

	ShowWindow(SW_SHOWNORMAL);
	return TRUE;
}

void CRecogDlg::InitWindows()
{
	CWnd	*pWnd = GetDlgItem(IDC_QUERYIMG);
	EmbedCvWindow(pWnd->m_hWnd, "query", showSz.width, showSz.height);
	if (noface) cvShowImage("query", noface);

	CWnd	*pWnd1 = GetDlgItem(IDC_MATCHIMG);
	EmbedCvWindow(pWnd1->m_hWnd, "match", showSz.width, showSz.height);
	if (resss) cvShowImage("match", resss);
}

UINT ShowMatchProc( LPVOID pParam )
{
	CRecogDlg *pDlg = (CRecogDlg *)pParam;
	return pDlg->ShowMatch();
}

void CRecogDlg::ShowMatchRun()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	bool res = g_faceMngr->PicRecognize(faceImg8, FM_DO_NOT_NORM, &matchInfo);

	if (res)
	{
		CString msg, fn, distname;
		int		i;
		strFd = matchInfo.picPath;
		i = strFd.ReverseFind('\\');
		fn = strFd.Right(strFd.GetLength() - i - 1);
		strFd = strFd.Left(i+1);

		if (! m_bMatchStarted) return; // ����߳��Ƿ�Ӧ���������û�����Exit��
		double score = matchInfo.dist;
		if (g_faceMngr->m_modelSz > 1) 
		{
			score = -score;
			distname = "���ƶ�(-1~1)";
		}
		else distname = "ƥ�����(0~)";
		msg.Format("ƥ������%s��%s��%f\nƥ��ͼƬ·����%s%s", 
			::FindName(fn), distname, score, strFd, fn);
		SetDlgItemText(IDC_INFO, msg);

		IplImage *ming = cvLoadImage(matchInfo.picPath);
		if (!ming) ming = cvLoadImage("noimage.bmp"); // ƥ��ͼƬ��ȡʧ��
		if (!ming)
		{
			ming = cvCreateImage(faceSz, 8, 1);
			cvSet(ming, cvScalar(255));
		}

		CvSize msz = cvGetSize(ming); // ����ͼƬ��С
		float showscale = (float)max(msz.width, msz.height)/300;
		if (showscale < .3)	msz *= 2;
		else if (showscale > 1)	msz /= showscale;

		if (! m_bMatchStarted) return;
		CWnd *pWnd = GetDlgItem(IDC_MATCHIMG); // �������ڴ�С��λ��
		pWnd->SetWindowPos(NULL, 0,0, msz.width, msz.height, SWP_NOZORDER | SWP_NOMOVE);
		SetWindowPos(NULL, 0, 0, faceSz.width + 150 + msz.width,
			msz.height + 130, SWP_NOZORDER | SWP_NOMOVE);
		cvResizeWindow("match", msz.width, msz.height);
		cvShowImage("match", ming);
		cvReleaseImage(&ming);
	}
}

void CRecogDlg::OnBnClickedCancel()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	Release();
	OnCancel();
}

void CRecogDlg::Release()
{
	if (g_bHasWebcam)
	{
		m_bMatchStarted = false;
		if (m_thrdMatch) ::WaitForSingleObject(m_thrdMatch->m_hThread, INFINITE);
		cvReleaseImage(&m_frame);
	}

	cvDestroyWindow("query");
	cvDestroyWindow("match");
	cvDestroyWindow("face");
}

void CRecogDlg::OnBnClickedPicRecog()
{
	// �����߳�
	if (g_bHasWebcam)
	{
		m_bMatchStarted = false;
		if (m_thrdMatch) ::WaitForSingleObject(m_thrdMatch->m_hThread, INFINITE);
	}

	// ѡ��ͼƬ
	CFileDialog fileDlg(TRUE, 0, 0, OFN_NOCHANGEDIR | OFN_HIDEREADONLY);
	int ret = fileDlg.DoModal();
	if (ret != IDOK) return;
	CString path = fileDlg.GetPathName();
	IplImage *queryimg = cvLoadImage(path, CV_LOAD_IMAGE_COLOR);
	if (!queryimg) return;

	// ����query img��С
	CvSize qsz = cvGetSize(queryimg);
	float showscale = (float)max(qsz.width, qsz.height)/300;
	if (showscale < .3)	qsz *= 2;
	else if (showscale > 1)	qsz /= showscale;

	// ��ʾquery img
	CWnd *pWndq = GetDlgItem(IDC_QUERYIMG);
	pWndq->SetWindowPos(NULL, 0,0, qsz.width, qsz.height, SWP_NOZORDER | SWP_NOMOVE);
	cvResizeWindow("query", qsz.width, qsz.height);
	cvShowImage("query", queryimg);

	// ʶ��
	bool res = g_faceMngr->PicRecognize(queryimg, FM_DO_NORM, &matchInfo);
	cvReleaseImage(&queryimg);
	if (!res) return;

	// ��ʾquery img�Ĺ�һ������
	cvNamedWindow("face", 0); 
	cvResizeWindow("face", showSz.width, showSz.height);
	cvShowImage("face", g_faceMngr->tfaceImg8);

	// ��ʾ���
	CString msg, fn, distname;
	int		i;
	strFd = matchInfo.picPath;
	i = strFd.ReverseFind('\\');
	fn = strFd.Right(strFd.GetLength() - i - 1);
	strFd = strFd.Left(i+1);

	double score = matchInfo.dist;
	if (g_faceMngr->m_modelSz > 1) 
	{
		score = -score;
		distname = "���ƶ�(-1~1)";
	}
	else distname = "ƥ�����(0~)";
	msg.Format("ƥ������%s��%s��%f\nƥ��ͼƬ·����%s%s", 
		::FindName(fn), distname, score, strFd, fn);
	SetDlgItemText(IDC_INFO, msg);

	// ��ȡƥ��ͼƬ
	IplImage *mimg = cvLoadImage(matchInfo.picPath);
	if (!mimg) mimg = cvLoadImage("noimage.bmp");
	if (!mimg)
	{
		mimg = cvCreateImage(qsz, 8, 1);
		cvSet(mimg, cvScalar(255));
	}

	// ����ƥ��ͼƬ��С
	CvSize msz = cvGetSize(mimg); // ����ͼƬ��С
	showscale = (float)max(msz.width, msz.height)/300;
	if (showscale < .3) msz *= 2;
	else if (showscale > 1) msz /= showscale;

	// ��ʾƥ��ͼƬ���������ڴ�С
	CWnd *pWndm = GetDlgItem(IDC_MATCHIMG);
	pWndm->SetWindowPos(NULL, qsz.width + 50, 79, msz.width, msz.height, SWP_NOZORDER);
	SetWindowPos(NULL, 0, 0, qsz.width + 100 + msz.width,
		max(qsz.height, msz.height) + 130, SWP_NOZORDER | SWP_NOMOVE);
	GetDlgItem(IDC_INFO)->
		SetWindowPos(NULL, 0,0, qsz.width + 20 + msz.width, 30, SWP_NOZORDER | SWP_NOMOVE);
	cvResizeWindow("match", msz.width, msz.height);
	cvShowImage("match", mimg);
	cvReleaseImage(&mimg);
}

UINT CRecogDlg::ShowMatch()
{
	faceImg8 = cvCreateMat(faceSz.height, faceSz.width, CV_8UC1);
	m_bFoundFace = false;

	while(m_bMatchStarted)
	{
		g_webcam.GetFrame(m_frame);

		// ʶ��ʱӦʹ��һ��������Ҫ����ͼ��߽磬Ҳ����Ӧ����������ͼ�����У��������ƥ��
		DWORD flag = FM_DO_NORM;
		if (m_bUseBuf) flag |= FM_ALIGN_USE_BUF;
		m_bFoundFace = g_faceMngr->Pic2NormFace(m_frame, faceImg8, flag);

		if (m_bFoundFace) // ����������͹�һ���ɹ����ٵ��� ShowMatchRun ����ƥ��
		{
			cvShowImage("query", faceImg8);
			m_bFoundFace = true;
			ShowMatchRun();
		}
		else cvShowImage("query", noface);

		Sleep(100);
	}

	cvReleaseMat(&faceImg8);
	return 0;
}

void CRecogDlg::OnBnClickedUsebuf()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	UpdateData();
}