/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : GMMMixup.h
*D
*D ��Ŀ����            : 
*D
*D �汾��              : 1.1.0001
*D
*D �ļ�����            : ��GMMģ�ͽ���mixture����
*D
*D
*D �ļ��޸ļ�¼
*D ------------------------------------------------------------------------------ 
*D �汾��       �޸�����       �޸���     �Ķ�����
*D ------------------------------------------------------------------------------ 
*D 1.1.0001     2007.03.20     plu        �����ļ�
*D*******************************************************************************/

#ifndef _GMMMIXUP_20070320_H_
#define _GMMMIXUP_20070320_H_

#include "GMMParam.h"

#define MAXSPLITNUM		5000				// 2007.03.20 plu : һ����˹�����������ķ��Ѵ���
#define MINMIX			1.0E-5				/* Min usable mixture weight */
#define LMINMIX			11.5129254649702	/* log(MINMIX) */

class GMMMixup : public GMMParam
{
protected:
	float			m_fMeanGC,m_fStdGC;
	float			m_fSplitDepth;			// ������ȿ��Ʋ���
	int				*m_pnSplitCount;		// ��¼ÿ����˹���������ѵĴ���
	int				m_nNewMixNum;			// ���Ѻ��mixNum
	GaussMixModel	*m_pNewGmmModel;		// ���Ѻ��ģ��buf
	
	// �����ã��õ���˹������GConst��һ�׶���ͳ����
	void    GConstStats(GaussMixModel *p_pGmmModle);		

	// �����и�˹������ѡ��һ�����ʺϷ��ѵ�,p_nModelIdxΪģ�͵�index
	int     HeaviestMix(GaussMixModel *p_pGmmModel,int p_nMixNum);

    // ��ĳ����˹�������ѳ���������
	// p_pGmmModelΪģ��Bufָ��
	// p_nSrcMixIdx �Ǵ����ѵĸ�˹index��
	// p_nSrcMixIdx��p_nDstMixIdx�Ƿ��Ѻ�ĸ�˹index
    void    SplitMix(GaussMixModel *p_pGmmModel,int p_nSrcMixIdx,int p_nDstMixIdx);
	
	void    CloneMixPDF(GaussPDF &p_pDst,GaussPDF &p_pSrc);
	void    FixInvDiagGConst(GaussMixModel *p_pGmmModel);
	void    FixInvDiagMat(GaussMixModel *p_pGmmModel);
	void	CopyModel(GaussMixModel *pGMMDes,GaussMixModel *pGMMSrc);
	void    CheckWeight(GaussMixModel *p_pGmmModel);
public:

	GMMMixup();
	~GMMMixup();

	// ����ѵ��
	void Mixup(int p_nNewMixNum,float p_fDepth);		// ģ�ͷ���
};

#endif // _GMMMIXUP_H_
