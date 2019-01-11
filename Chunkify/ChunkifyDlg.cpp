// ChunkifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Chunkify.h"
#include "ChunkifyDlg.h"

// includes for dummy tests
#include "DJTGroup.h"
#include "DJTCluster.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define TEST_KW_SIZE 10

/////////////////////////////////////////////////////////////////////////////
// CChunkifyDlg dialog

CChunkifyDlg::CChunkifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChunkifyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChunkifyDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChunkifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChunkifyDlg)
	DDX_Control(pDX, IDC_URL, m_StaticUrl);
	DDX_Control(pDX, IDC_KW_EDIT5, m_DigestEdit5);
	DDX_Control(pDX, IDC_KW_EDIT4, m_DigestEdit4);
	DDX_Control(pDX, IDC_KW_EDIT3, m_DigestEdit3);
	DDX_Control(pDX, IDC_KW_EDIT2, m_DigestEdit2);
	DDX_Control(pDX, IDC_KW_EDIT1, m_DigestEdit1);
	DDX_Control(pDX, IDC_LIST4, m_ClusterListBox4);
	DDX_Control(pDX, IDC_LIST5, m_ClusterListBox5);
	DDX_Control(pDX, IDC_LIST2, m_ClusterListBox2);
	DDX_Control(pDX, IDC_LIST3, m_ClusterListBox3);
	DDX_Control(pDX, IDC_LIST1, m_ClusterListBox1);
	DDX_Control(pDX, IDC_NUM_OF_URLS, m_EditNumOfUrls);
	DDX_Control(pDX, IDCANCEL2, m_ButtOk);
	DDX_Control(pDX, IDC_NEW_SESS, m_ButtNewSession);
	DDX_Control(pDX, IDOK, m_ButtPerformTest);
	DDX_Control(pDX, IDC_QUERY, m_QueryEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChunkifyDlg, CDialog)
	//{{AFX_MSG_MAP(CChunkifyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnPerformTest)
	ON_BN_CLICKED(IDC_NEW_SESS, OnNewSess)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkList2)
	ON_LBN_DBLCLK(IDC_LIST3, OnDblclkList3)
	ON_LBN_DBLCLK(IDC_LIST4, OnDblclkList4)
	ON_LBN_DBLCLK(IDC_LIST5, OnDblclkList5)
	ON_EN_SETFOCUS(IDC_KW_EDIT1, OnSetfocusKwEdit1)
	ON_EN_SETFOCUS(IDC_KW_EDIT2, OnSetfocusKwEdit2)
	ON_EN_SETFOCUS(IDC_KW_EDIT3, OnSetfocusKwEdit3)
	ON_EN_SETFOCUS(IDC_KW_EDIT4, OnSetfocusKwEdit4)
	ON_EN_SETFOCUS(IDC_KW_EDIT5, OnSetfocusKwEdit5)
	ON_BN_CLICKED(IDCANCEL2, OnCancel)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	//}}AFX_MSG_MAP
	ON_MESSAGE(DJ_WM_GROUPUPDATE, OnGroupUpdate)
	ON_MESSAGE(DJ_WM_CLUSTERAGENT_DEAD, OnClusterAgentDead)
	ON_MESSAGE(DJ_WM_CLUSTERAGENT_STOPPED, OnClusterAgentStopped)
	ON_MESSAGE(DJ_WM_ALL_AGENTS_DELETED, OnAllAgentsDead)
	ON_MESSAGE(DJ_WM_FETCHAGENT_FILE, OnFetchAgentFile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChunkifyDlg message handlers

BOOL CChunkifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Chunkify initialization.
	m_pClusterAgent = DJCreateChunkify( AfxGetMainWnd(),// Pointer to dialog. 
										4,				// 4 fetch threads.
										5, 				// 5 clusters,
										50000, 			// max DL 100k file..
										FALSE, 			// don't save tmp files, 
										NULL );			// system tmp dir.

	// Collect all interesting groups.
	ClusterPtrArray* pClusterArray = m_pClusterAgent->Clusters();
	GroupPtrArray* pGroupArray;
	
	m_pCluster1 = pClusterArray->GetAt(0);
	pGroupArray = m_pCluster1->Groups();
	for( int j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList1.AddTail( pGroupArray->GetAt(j) );

	m_pCluster2 = pClusterArray->GetAt(1);
	pGroupArray = m_pCluster2->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList2.AddTail( pGroupArray->GetAt(j) );

	m_pCluster3 = pClusterArray->GetAt(2);
	pGroupArray = m_pCluster3->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList3.AddTail( pGroupArray->GetAt(j) );

	m_pCluster4 = pClusterArray->GetAt(3);
	pGroupArray = m_pCluster4->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList4.AddTail( pGroupArray->GetAt(j) );

	m_pCluster5 = pClusterArray->GetAt(4);
	pGroupArray = m_pCluster5->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList5.AddTail( pGroupArray->GetAt(j) );

	// Set default query string;
	m_QueryEdit.SetWindowText( "grass" );
	m_EditNumOfUrls.SetWindowText( "200" );
	
	m_DigestEdit1.FmtLines( TRUE );
	m_DigestEdit2.FmtLines( TRUE );
	m_DigestEdit3.FmtLines( TRUE );
	m_DigestEdit4.FmtLines( TRUE );
	m_DigestEdit5.FmtLines( TRUE );


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChunkifyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChunkifyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

afx_msg void CChunkifyDlg::OnGroupUpdate(WPARAM wParam)
{
	CString strTmp;
	CStringArray Strings;

	CDJTGroup* pGrp = (CDJTGroup*) wParam;	

	if( m_GrpPtrList1.Find(pGrp) )
	{
		UpdateGroup( pGrp, &m_ClusterListBox1 );
		OnSetfocusKwEdit1();
	}

	if( m_GrpPtrList2.Find(pGrp) )
	{
		UpdateGroup( pGrp, &m_ClusterListBox2 );
		OnSetfocusKwEdit2();
	}

	if( m_GrpPtrList3.Find(pGrp) )
	{
		UpdateGroup( pGrp, &m_ClusterListBox3 );
		OnSetfocusKwEdit3();
	}

	if( m_GrpPtrList4.Find(pGrp) )
	{
		UpdateGroup( pGrp, &m_ClusterListBox4 );
		OnSetfocusKwEdit4();
	}

	if( m_GrpPtrList5.Find(pGrp) )
	{
		UpdateGroup( pGrp, &m_ClusterListBox5 );
		OnSetfocusKwEdit5();
	}

}

void CChunkifyDlg::OnPerformTest() 
{
	CString strQuery;
	CString strUrls;

	m_ButtPerformTest.EnableWindow(FALSE);
	m_QueryEdit.EnableWindow(FALSE);
	m_EditNumOfUrls.EnableWindow(FALSE);

	m_QueryEdit.GetWindowText( strQuery );
	m_EditNumOfUrls.GetWindowText( strUrls );
	m_pClusterAgent = DJStartChunkify( strQuery, atoi((LPCTSTR)strUrls) );

	m_ButtNewSession.EnableWindow(TRUE);
}

void CChunkifyDlg::OnCancel() 
{
	// Close down chunkify.
	

	POSITION pos = m_ClusterAgentList.GetTailPosition();
	while( pos != NULL )
		delete m_ClusterAgentList.GetPrev(pos);
	m_ClusterAgentList.RemoveAll();
	
	//EndDialog( 0 );
	ShowWindow( SW_HIDE );
	DJDeleteChunkify();
}

void CChunkifyDlg::OnClusterAgentDead(WPARAM wParam)
{
	CDJClusterAgent* pCA = (CDJClusterAgent*) wParam;	
	CString strTmp;
	CDJClusterAgent* pLastAgent;

	// Which Agent is last in line?
	if( m_ClusterAgentList.IsEmpty() )
		pLastAgent = m_pClusterAgent;
	else
		pLastAgent = m_ClusterAgentList.GetTail();

//	int nRet = m_ListBox.AddString( "ClusterAgent sent death notice..." );
//	ASSERT( nRet != LB_ERR );
//	ASSERT( nRet != LB_ERRSPACE );
//	m_ListBox.UpdateWindow();

	if( pCA == pLastAgent )
	{
		m_ButtOk.ShowWindow( SW_SHOWNA );
		m_ButtOk.SetFocus();
		CWnd::SetForegroundWindow();
//		MessageBeep( MB_OK );
	}
}


void CChunkifyDlg::OnClusterAgentStopped(WPARAM wParam)
{
/*	int nRet = m_ListBox.AddString( "---------------------------" );
	ASSERT( nRet != LB_ERR );
	ASSERT( nRet != LB_ERRSPACE );
	m_ListBox.UpdateWindow();
*/}


void CChunkifyDlg::OnAllAgentsDead(WPARAM wParam)
{
	::MessageBeep( MB_ICONEXCLAMATION );
	CDialog::OnCancel();
}


void CChunkifyDlg::OnNewSess() 
{
	CDJClusterAgent* pSourceAgent;
	ClusterPtrArray* pClusterArray;
	CDJTCluster* pCluster;
	GroupPtrArray* pGroupArray;
	GroupPtrList FilterList;

	// Which Agent is last in line?
	if( m_ClusterAgentList.IsEmpty() )
		pSourceAgent = m_pClusterAgent;
	else
		pSourceAgent = m_ClusterAgentList.GetTail();

	// Select some groups for filtering.
	FilterList.RemoveAll();
	pClusterArray = pSourceAgent->Clusters();
	pCluster = pClusterArray->GetAt(pClusterArray->GetUpperBound());	// Get last cluster.
	pGroupArray = pCluster->Groups();	// Get all groups.

	for( int i = 0; i<DJ_GENRE_PALETTE_SIZE; i++ )
		FilterList.AddTail( pGroupArray->GetAt(i) );

	// Make a new agent, and collect new set of groups we're interested in.
	CDJClusterAgent* pCA = new CDJClusterAgent(5);
	m_ClusterAgentList.AddTail( pCA );
	pClusterArray = pCA->Clusters();

	m_GrpPtrList1.RemoveAll();
	m_GrpPtrList2.RemoveAll();
	m_GrpPtrList3.RemoveAll();
	m_GrpPtrList4.RemoveAll();
	m_GrpPtrList5.RemoveAll();

	m_pCluster1 = pClusterArray->GetAt(0);
	pGroupArray = m_pCluster1->Groups();
	for( int j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList1.AddTail( pGroupArray->GetAt(j) );

	m_pCluster2 = pClusterArray->GetAt(1);
	pGroupArray = m_pCluster2->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList2.AddTail( pGroupArray->GetAt(j) );

	m_pCluster3 = pClusterArray->GetAt(2);
	pGroupArray = m_pCluster3->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList3.AddTail( pGroupArray->GetAt(j) );

	m_pCluster4 = pClusterArray->GetAt(3);
	pGroupArray = m_pCluster4->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList4.AddTail( pGroupArray->GetAt(j) );

	m_pCluster5 = pClusterArray->GetAt(4);
	pGroupArray = m_pCluster5->Groups();
	for( j = 0; j<DJ_GENRE_PALETTE_SIZE; j++ )
			m_GrpPtrList5.AddTail( pGroupArray->GetAt(j) );

	m_ClusterListBox1.ResetContent();
	m_ClusterListBox2.ResetContent();
	m_ClusterListBox3.ResetContent();
	m_ClusterListBox4.ResetContent();
	m_ClusterListBox5.ResetContent();

	// Use Chunkify API function.
	DJSetupFilterFromTo( pSourceAgent, &FilterList, pCA );
}


void CChunkifyDlg::StartBrowser(LPCTSTR pszUrl)
{
	// Start Netscape ( ie http client )
	::ShellExecute( NULL, "open", pszUrl ,NULL, NULL, SW_SHOWNORMAL );	
}

void CChunkifyDlg::OnDblclkList1() 
{
	CString strUrl;
	m_ClusterListBox1.GetText( m_ClusterListBox1.GetCurSel(), strUrl );
	StartBrowser( strUrl.Mid(strUrl.Find("http://")) );
}

void CChunkifyDlg::OnDblclkList2() 
{
	CString strUrl;
	m_ClusterListBox2.GetText( m_ClusterListBox2.GetCurSel(), strUrl );
	StartBrowser( strUrl.Mid(strUrl.Find("http://")) );
}

void CChunkifyDlg::OnDblclkList3() 
{
	CString strUrl;
	m_ClusterListBox3.GetText( m_ClusterListBox3.GetCurSel(), strUrl );
	StartBrowser( strUrl.Mid(strUrl.Find("http://")) );
}

void CChunkifyDlg::OnDblclkList4() 
{
	CString strUrl;
	m_ClusterListBox4.GetText( m_ClusterListBox4.GetCurSel(), strUrl );
	StartBrowser( strUrl.Mid(strUrl.Find("http://")) );
}

void CChunkifyDlg::OnDblclkList5() 
{
	CString strUrl;
	m_ClusterListBox5.GetText( m_ClusterListBox5.GetCurSel(), strUrl );
	StartBrowser( strUrl.Mid(strUrl.Find("http://")) );
}

void CChunkifyDlg::OnSetfocusKwEdit1() 
{
	CStringArray Strings;
	CString strTmp;

	Strings.SetSize(TEST_KW_SIZE);
	int nRet = m_pCluster1->ClusterKeywords( Strings );
	strTmp.Format( "(%d):", nRet );
	for( int i=0; i<nRet; i++ )
		strTmp += " " + Strings[i];
	
	m_DigestEdit1.SetWindowText( strTmp );	
}


void CChunkifyDlg::OnSetfocusKwEdit2() 
{
	CStringArray Strings;
	CString strTmp;

	Strings.SetSize(TEST_KW_SIZE);
	int nRet = m_pCluster2->ClusterKeywords( Strings );
	strTmp.Format( "(%d):", nRet );
	for( int i=0; i<nRet; i++ )
		strTmp += " " + Strings[i];
	
	m_DigestEdit2.SetWindowText( strTmp );	
}

void CChunkifyDlg::OnSetfocusKwEdit3() 
{
	CStringArray Strings;
	CString strTmp;

	Strings.SetSize(TEST_KW_SIZE);
	int nRet = m_pCluster3->ClusterKeywords( Strings );
	strTmp.Format( "(%d):", nRet );
	for( int i=0; i<nRet; i++ )
		strTmp += " " + Strings[i];
	
	m_DigestEdit3.SetWindowText( strTmp );	
}

void CChunkifyDlg::OnSetfocusKwEdit4() 
{
	CStringArray Strings;
	CString strTmp;

	Strings.SetSize(TEST_KW_SIZE);
	int nRet = m_pCluster4->ClusterKeywords( Strings );
	strTmp.Format( "(%d):", nRet );
	for( int i=0; i<nRet; i++ )
		strTmp += " " + Strings[i];
	
	m_DigestEdit4.SetWindowText( strTmp );	
}

void CChunkifyDlg::OnSetfocusKwEdit5() 
{
	CStringArray Strings;
	CString strTmp;

	Strings.SetSize(TEST_KW_SIZE);
	int nRet = m_pCluster5->ClusterKeywords( Strings );
	strTmp.Format( "(%d):", nRet );
	for( int i=0; i<nRet; i++ )
		strTmp += " " + Strings[i];
	
	m_DigestEdit5.SetWindowText( strTmp );	
}

void CChunkifyDlg::UpdateGroup(CDJTGroup* pGrp, CListBox* pList)
{
	CString strTmp, strUrl;
	DocumentPtrList DocList;

	pGrp->GetDocPtrs( &DocList );
	if( !DocList.IsEmpty() )
	{
		strTmp.Format( "%s, %s", 
				   pGrp->m_pGenre->szLabel, 
  				   DocList.GetTail()->m_strUrl );
		int nRet = pList->AddString( strTmp );
		ASSERT( nRet != LB_ERR );
		ASSERT( nRet != LB_ERRSPACE );

		pList->UpdateWindow();
	}
}


void CChunkifyDlg::OnStop() 
{
	DJStopChunkify();	
}


LRESULT CChunkifyDlg::OnFetchAgentFile(WPARAM wParam,LPARAM lParam)
{
	CString *pstrUrl = (CString*) wParam;
	double fileSize = (int)lParam/(double)1000;

	CString strText;
	strText.Format( "Fetching: %s, %.2f kB", (LPCTSTR) *pstrUrl, fileSize );

	m_StaticUrl.SetWindowText( strText );

	return( 1 );
}
