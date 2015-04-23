/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : GMMParam.h
*D
*D ��Ŀ����            : 
*D
*D �汾��              : 1.1.0004
*D
*D �ļ�����            :
*D
*D
*D �ļ��޸ļ�¼
*D ------------------------------------------------------------------------------ 
*D �汾��       �޸�����       �޸���     �Ķ�����
*D ------------------------------------------------------------------------------ 
*D 1.1.0001     2007.02.13     plu        �����ļ�
*D 1.1.0002     2007.03.20     plu        ���� m_fpLog��SetLogFile(char *p_pcLogFile)
*D 1.1.0003     2007.09.11     plu        ����MFCC�������飬������������д��GMM param�ļ�
*D 1.1.0004     2007.09.17     plu        �ı��ڴ���䷽ʽ�����еľ�ֵ����һ�飬���еķ������һ��
*D 1.1.0004     2007.09.17     plu        �޸��˽ṹ��GaussMixModel
*D*******************************************************************************/
#ifndef _GMMPARAM_20070213_H_
#define _GMMPARAM_20070213_H_

#include "memory_srlr.h"
#include "comm_srlr.h"
#include "Feat_comm.h"

#define LZERO (-1.0E10)			// 2007.03.20 plu : add

struct GaussPDF					// ��˹�����ܶ�
{	
	float	*pfMean;			// ��ֵʸ�����������ڴ棬ֻ�Ǳ���ÿ����˹������ֵʸ�����׵�ַ
	float	*pfDiagCov;			// �Խ���б������ʸ�������ڴ�ͬ��
//	double	dMat;				// = log(weight) - 0.5*Dim*log(2pi) + 0.5*log|��б������|
    float dMat;
	int		nDim;				// ��Чά������һ����4�ı�����

	GaussPDF()
	{
		pfMean=pfDiagCov=NULL;
		dMat = 0.0;
		nDim = 0;
	}
};

struct GaussMixModel			// ��ϸ�˹ģ��
{
    float		*pfWeight;		// Ȩ��ʸ�� 
	float		*pfMeanBuf;		// 2007.09.17 plu : ���еľ�ֵ
	float		*pfDiagCovBuf;	// 2007.09.17 plu : ���еķ���
    GaussPDF	*pGauss;		// Gaussian component
	int			nMixNum;		// ��ϸ�˹����Ŀ

	GaussMixModel()
	{
		pfWeight= NULL;
		pGauss  = NULL;
		nMixNum = 0;

		pfMeanBuf = pfDiagCovBuf = NULL;	// 2007.09.17 plu : 
	}
};

struct GMMFileHeader			// ��ϸ�˹ģ���ļ����ļ�ͷ
{
	int nModelNum;				// ��ϸ�˹ģ�͵���Ŀ
	int nMixNum;				// ��ϸ�˹����Ŀ
	int nDim;					// ά��
	int nMfccKind;				// 2007.09.11 plu : ��������

	GMMFileHeader()
	{
		nModelNum = nMixNum = nDim = nMfccKind = -1 ; // ��ʾ��Ч
	}
};

/*********************************CLASS*COMMENT***********************************
*C
*C ������              : GMMParam
*C
*C ������              : 
*C
*C
*C ���޸ļ�¼
*C ------------------------------------------------------------------------------ 
*C �޸�����       �޸���     �Ķ�����
*C ------------------------------------------------------------------------------ 
*C 2007.02.13     plu        ������
*C*******************************************************************************/
class GMMParam 
{
protected:
	
	int		m_nModelNum;		// ��ϸ�˹ģ�͵���Ŀ
	int		m_nMixNum;			// ��ϸ�˹��������Ŀ
	int		m_nVecSize;			// ����ά��
	int		m_nVecSize4;		// ����ά������չ��4�ı�����

	short   m_nMfccKind;			// 2007.09.11 plu : ���������MFCC����

	GaussMixModel *m_pGmmModel;		// ģ�Ͳ���

	FILE    *m_fpLog;				// 2007.02.15 plu : Log file pointer 

protected:
	void FreeGaussMixModel(GaussMixModel *p_gmmParam,int p_nModelNum );	
	void AllocGaussMixModel(GaussMixModel *p_pModel,int p_nMixNum,int p_nVecSize);	
    
public:

	GMMParam(void);
	~GMMParam();

    void report();
    
	virtual void LoadModel(char *p_pcModelFile);		// �����ϸ�˹ģ��
	virtual bool WriteModel(char *p_pcModelFile);		// ��ģ�Ͳ���д���ļ�

	int	GetModelNum(void)   { return m_nModelNum; };
	int GetMixtureNum(void) { return m_nMixNum; };
	int GetVecSize(void)	{ return m_nVecSize; };
    GaussMixModel * GetRawMixModel() { return m_pGmmModel; }

	void SetLogFile(char *p_pcLogFile);
    
    void LoadModel(const GMMParam &gmmParam);
    void WriteModel(const GMMParam &gmmParam);

    void AllocAll(int p_nMixNum, int p_nVecSize);
    void FreeAll();
    void InitModel(struct Features &features, int vecSize, int vecSize4);
};


#endif // _GMMPARAM_H_
