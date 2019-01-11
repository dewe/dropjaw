// ChunkifyDlg.h : header file
//

#if !defined(AFX_CHUNKIFYDLG_H__F8492F47_E641_11D0_8C27_08002BE6493E__INCLUDED_)
#define AFX_CHUNKIFYDLG_H__F8492F47_E641_11D0_8C27_08002BE6493E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CChunkifyDlg dialog

class CChunkifyDlg : public CDialog
{
// Construction
public:
	void UpdateGroup( CDJTGroup* pGrp, CListBox* pList );
	void StartBrowser( LPCTSTR pszUrl );
	GroupPtrList m_GrpPtrList1;
	GroupPtrList m_GrpPtrList2;
	GroupPtrList m_GrpPtrList3;
	GroupPtrList m_GrpPtrList4;
	GroupPtrList m_GrpPtrList5;
	CDJTCluster* m_pCluster1;
	CDJTCluster* m_pCluster2;
	CDJTCluster* m_pCluster3;
	CDJTCluster* m_pCluster4;
	CDJTCluster* m_pCluster5;

	ClusterAgentPtrList m_ClusterAgentList;
	CDJClusterAgent* m_pClusterAgent;
	CChunkifyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CChunkifyDlg)
	enum { IDD = IDD_CHUNKIFY_DIALOG };
	CStatic	m_StaticUrl;
	CEdit	m_DigestEdit5;
	CEdit	m_DigestEdit4;
	CEdit	m_DigestEdit3;
	CEdit	m_DigestEdit2;
	CEdit	m_DigestEdit1;
	CListBox	m_ClusterListBox4;
	CListBox	m_ClusterListBox5;
	CListBox	m_ClusterListBox2;
	CListBox	m_ClusterListBox3;
	CListBox	m_ClusterListBox1;
	CEdit	m_EditNumOfUrls;
	CButton	m_ButtOk;
	CButton	m_ButtNewSession;
	CButton	m_ButtPerformTest;
	CEdit	m_QueryEdit;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChunkifyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	afx_msg void OnGroupUpdate(WPARAM wParam);
	afx_msg void OnClusterAgentDead(WPARAM wParam);
	afx_msg void OnClusterAgentStopped(WPARAM wParam);
	afx_msg void OnAllAgentsDead(WPARAM wParam);
	afx_msg LRESULT OnFetchAgentFile(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CChunkifyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPerformTest();
	virtual void OnCancel();
	afx_msg void OnNewSess();
	afx_msg void OnDblclkList1();
	afx_msg void OnDblclkList2();
	afx_msg void OnDblclkList3();
	afx_msg void OnDblclkList4();
	afx_msg void OnDblclkList5();
	afx_msg void OnSetfocusKwEdit1();
	afx_msg void OnSetfocusKwEdit2();
	afx_msg void OnSetfocusKwEdit3();
	afx_msg void OnSetfocusKwEdit4();
	afx_msg void OnSetfocusKwEdit5();
	afx_msg void OnStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHUNKIFYDLG_H__F8492F47_E641_11D0_8C27_08002BE6493E__INCLUDED_)
