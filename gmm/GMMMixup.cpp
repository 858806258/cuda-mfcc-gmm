#include "GMMMixup.h"

#include <math.h>

GMMMixup::GMMMixup()
{
	m_pNewGmmModel = NULL;
	m_pnSplitCount = NULL; 
}

GMMMixup::~GMMMixup()
{
	if (NULL!=m_pnSplitCount)	delete []m_pnSplitCount;
	if (NULL!=m_pNewGmmModel)	FreeGaussMixModel(m_pNewGmmModel,m_nModelNum);
	m_pNewGmmModel=NULL;
}

void GMMMixup::Mixup(int p_nNewMixNum,float p_fDepth)
{
	ASSERT2(m_pGmmModel,"Error call Mixup() : please call LoadModel() firstly!");
	ASSERT3(p_nNewMixNum>m_nMixNum,
			"Error call Mixup() : p_nNewMixNum=%d, too small!",p_nNewMixNum);
	ASSERT3(p_fDepth>0.f&&p_fDepth<1.f,
			"Error call Mixup() : p_fDepth=%f (should in (0,1))!",p_fDepth);
//	ASSERT2(m_fpLog,"Error call Mixup(): m_fpLog==NULL!");

	m_fSplitDepth = p_fDepth;
	m_nNewMixNum = p_nNewMixNum;
		
	// ���·���ģ��Buf
	if (NULL!=m_pNewGmmModel)	FreeGaussMixModel(m_pNewGmmModel,m_nModelNum);
	m_pNewGmmModel = (GaussMixModel *)Malloc(m_nModelNum,sizeof(GaussMixModel),false);
	for(int i=0;i<m_nModelNum;i++)
		// ���� GMM ģ��
		AllocGaussMixModel(&m_pNewGmmModel[i],p_nNewMixNum,m_nVecSize4);

	// ����split count
	if (NULL!=m_pnSplitCount)	
	{
		delete []m_pnSplitCount;	m_pnSplitCount = NULL;
	}
	m_pnSplitCount = new int[m_nNewMixNum];
	ASSERT2(m_pnSplitCount,"Error call Mixup() : m_pnSplitCount==NULL!");
	memset(m_pnSplitCount,0,sizeof(int)*m_nNewMixNum);

	FixInvDiagGConst(m_pGmmModel);

	int nIdx;

	// ԭ��ģ�Ϳ������µ�buf��
	for (nIdx=0;nIdx<m_nModelNum;nIdx++)
		CopyModel(&m_pNewGmmModel[nIdx],&m_pGmmModel[nIdx]);

	// ����
	if (p_nNewMixNum==2)	// ��mix=1���ѵ�mix=2ʱ
	{
		for (nIdx=0;nIdx<m_nModelNum;nIdx++)
		{
			SplitMix(&m_pNewGmmModel[nIdx],0,1);
			// ����mix��Ŀ
			m_pNewGmmModel[nIdx].nMixNum = m_nNewMixNum;
		}
	}
	else
	{
		int nSelect;
		for (nIdx=0;nIdx<m_nModelNum;nIdx++)
		{
			GConstStats(&m_pGmmModel[nIdx]);

			for (int m=m_nMixNum;m<m_nNewMixNum;m++)
			{
				nSelect = HeaviestMix(&m_pNewGmmModel[nIdx],m);

				SplitMix(&m_pNewGmmModel[nIdx],nSelect,m);
			}

			// ����mix��Ŀ
			m_pNewGmmModel[nIdx].nMixNum = m_nNewMixNum;
		}
	}

	// ����mixNum
	m_nMixNum = m_nNewMixNum;				

	// ���weight
	CheckWeight(m_pNewGmmModel);

	// ����ģ�͵�matΪ����ģ�͵�matֵ
	FixInvDiagMat(m_pNewGmmModel);

	// �ͷ�ԭ�е�ģ��Buf�����µ�ģ��buf����m_pGmmModel
	FreeGaussMixModel(m_pGmmModel,m_nModelNum);
	m_pGmmModel = m_pNewGmmModel;
	m_pNewGmmModel = NULL;
}

void GMMMixup::CheckWeight(GaussMixModel *p_pGmmModel)
{
	float fSum;
	for (int n=0;n<m_nModelNum;n++)
	{
		fSum=0;

		for (int m=0;m<p_pGmmModel[n].nMixNum;m++)
			fSum += p_pGmmModel[n].pfWeight[m];

		if (fabs(fSum-1.0)>1.0e-6)
		{
			printf("Warning: sum of weight in %d-th model != 1 \a\n",n);

			for (int m=0;m<p_pGmmModel[n].nMixNum;m++)
				p_pGmmModel[n].pfWeight[m]/=fSum;
		}
	}
}

void GMMMixup::CopyModel(GaussMixModel *pGMMDes,GaussMixModel *pGMMSrc)
{
	ASSERT2(pGMMDes,"Error call CopyModel() : pGMMDes==NULL!");
	ASSERT2(pGMMSrc,"Error call CopyModel() : pGMMSrc==NULL!");

	ASSERT2(pGMMSrc->nMixNum>0,"Error call CopyModel() : pGmmSrc->nMixNum<=0!");
	ASSERT4(pGMMSrc->nMixNum==m_nMixNum,
			"Error call CopyModel() : pGmmSrc->nMixNum(%d) != m_nMixNum(%d)!",
			pGMMSrc->nMixNum,m_nMixNum);

	pGMMDes->nMixNum = pGMMSrc->nMixNum;
	memcpy(pGMMDes->pfWeight,pGMMSrc->pfWeight,sizeof(float)*m_nMixNum);
	for (int m=0;m<m_nMixNum;m++)
	{
		pGMMDes->pGauss[m].nDim = pGMMSrc->pGauss[m].nDim;
		pGMMDes->pGauss[m].dMat = pGMMSrc->pGauss[m].dMat;
		memcpy(pGMMDes->pGauss[m].pfMean,pGMMSrc->pGauss[m].pfMean,sizeof(float)*m_nVecSize4);
		memcpy(pGMMDes->pGauss[m].pfDiagCov,pGMMSrc->pGauss[m].pfDiagCov,sizeof(float)*m_nVecSize4);
	}
}

// dMat = Dim*log(2pi) - log|��б������|
void GMMMixup::FixInvDiagGConst(GaussMixModel *p_pGmmModel)
{
	ASSERT2(p_pGmmModel,"Error call FixInvDiagGConst() : p_pGmmModel==NULL!");
	
	float *pfVar,fValue,fConst;

	fConst=log2pi*m_nVecSize;
	for (int n=0;n<m_nModelNum;n++)
	{
		for (int m=0;m<p_pGmmModel[n].nMixNum;m++)
		{
			p_pGmmModel[n].pGauss[m].dMat = fConst;

			pfVar = p_pGmmModel[n].pGauss[m].pfDiagCov;
			for (int k=0;k<m_nVecSize;k++)
			{
				fValue = (pfVar[k]<=0.f)?-LZERO:-log(pfVar[k]);

				p_pGmmModel[n].pGauss[m].dMat += fValue;
			}
		}
	}
}

// dMat = log(weight) - 0.5*Dim*log(2pi) + 0.5*log|��б������|
void GMMMixup::FixInvDiagMat(GaussMixModel *p_pGmmModel)
{
	ASSERT2(p_pGmmModel,"Error call FixInvDiagGConst() : p_pGmmModel==NULL!");
	
	float *pfVar,fValue,fConst,fSum;

	fConst=log2pi*m_nVecSize;
	for (int n=0;n<m_nModelNum;n++)
	{
		for (int m=0;m<p_pGmmModel[n].nMixNum;m++)
		{
			fSum=0.f;
			pfVar = p_pGmmModel[n].pGauss[m].pfDiagCov;
			for (int k=0;k<m_nVecSize;k++)
			{
				fValue = (pfVar[k]<=0.f)?LZERO:log(pfVar[k]);

				fSum += fValue;
			}

			p_pGmmModel[n].pGauss[m].dMat = log(p_pGmmModel[n].pfWeight[m]) + 0.5*(fSum-fConst);
		}
	}
}


void GMMMixup::CloneMixPDF(GaussPDF &p_pDst,GaussPDF &p_pSrc)
{
	p_pDst.nDim=p_pSrc.nDim;
	memcpy(p_pDst.pfDiagCov,p_pSrc.pfDiagCov,sizeof(float)*m_nVecSize);
	memcpy(p_pDst.pfMean,p_pSrc.pfMean,sizeof(float)*m_nVecSize);

	// 2007.03.27 plu : 
	p_pDst.dMat = p_pSrc.dMat;
}

void GMMMixup::SplitMix(GaussMixModel *p_pGmmModel,int p_nSrcMixIdx,int p_nDstMixIdx)
{
	ASSERT2(p_pGmmModel,"Error call SplitMix() : p_pGmmModel=NULL!");
	ASSERT3(p_nSrcMixIdx>=0&&p_nSrcMixIdx<m_nNewMixNum,"Error call SplitMix() : p_nSrcMixIdx=%d!",p_nSrcMixIdx);
	ASSERT3(p_nDstMixIdx>=0&&p_nDstMixIdx<m_nNewMixNum,"Error call SplitMix() : p_nDstMixIdx=%d!",p_nDstMixIdx);


	// ��ֵ�������
	CloneMixPDF(p_pGmmModel->pGauss[p_nDstMixIdx],p_pGmmModel->pGauss[p_nSrcMixIdx]); 

	// weight���¸�ֵ
	p_pGmmModel->pfWeight[p_nSrcMixIdx] /= 2.f;
	p_pGmmModel->pfWeight[p_nDstMixIdx] = p_pGmmModel->pfWeight[p_nSrcMixIdx];

	// ���Ѽ�������
	m_pnSplitCount[p_nSrcMixIdx]++;
	m_pnSplitCount[p_nDstMixIdx] = m_pnSplitCount[p_nSrcMixIdx];

	// ���Ѻ��ģ�;�ֵ ��ԭ�о�ֵ����������ƫ��һ����
	float fBias;
	for (int k=0;k<m_nVecSize;k++) 
	{    
		fBias = sqrt(1.f/p_pGmmModel->pGauss[p_nSrcMixIdx].pfDiagCov[k])*m_fSplitDepth;

		p_pGmmModel->pGauss[p_nSrcMixIdx].pfMean[k] += fBias;
		p_pGmmModel->pGauss[p_nDstMixIdx].pfMean[k] -= fBias;
	}
}

void GMMMixup::GConstStats(GaussMixModel *p_pGmmModle)
{
	float fSum,fSumSQ;
	fSum=0.f; fSumSQ=0.f;
	for (int m=0;m<p_pGmmModle->nMixNum;m++)
	{
		fSum += p_pGmmModle->pGauss[m].dMat;
		fSumSQ += p_pGmmModle->pGauss[m].dMat*p_pGmmModle->pGauss[m].dMat;
	}
	
	m_fMeanGC = fSum / (float)p_pGmmModle->nMixNum;
	m_fStdGC = sqrt(fSumSQ/(float)p_pGmmModle->nMixNum - m_fMeanGC*m_fMeanGC);
}


int GMMMixup::HeaviestMix(GaussMixModel *p_pGmmModel,int p_nMixNum)
{
	float fgThresh;
	fgThresh = m_fMeanGC - 4.f*m_fStdGC;

	int m,nMaxIdx;
	GaussPDF *mp;
	float fMax,fWeight;
	
	nMaxIdx = 0;  
	mp = &p_pGmmModel->pGauss[0];
    
	fMax = p_pGmmModel->pfWeight[0] - 0.5*m_pnSplitCount[0]/(float)m_nNewMixNum;

	if ((int)m_pnSplitCount[0] < MAXSPLITNUM && mp->dMat < fgThresh) 
	{
		fMax -= MAXSPLITNUM;
		m_pnSplitCount[0] = MAXSPLITNUM;
		WARNING2("HeaviestMix: mix 0 has v.small gConst [%f]",mp->dMat);
	}
	for (m=1; m<p_nMixNum; m++) 
	{   
		mp = &p_pGmmModel->pGauss[m];

		fWeight = p_pGmmModel->pfWeight[m] - 0.5*m_pnSplitCount[m]/(float)m_nNewMixNum;

		if (m_pnSplitCount[m] < MAXSPLITNUM && mp->dMat < fgThresh) 
		{
			fWeight -= MAXSPLITNUM;
			m_pnSplitCount[m] = MAXSPLITNUM;
			WARNING3("HeaviestMix: mix %d has v.small gConst [%f]",
				m,mp->dMat);
		}
		if (fWeight>fMax) 
		{
			fMax = fWeight; 
			nMaxIdx = m;
		}
	}

	ASSERT2(p_pGmmModel->pfWeight[nMaxIdx]>MINMIX,"HeaviestMix:  heaviest mix is defunct!");

	return nMaxIdx;	
}
