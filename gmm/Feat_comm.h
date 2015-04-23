/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : Feat_comm.h
*D
*D ��Ŀ����            : 
*D
*D �汾��              : 1.1.0002
*D
*D �ļ�����            :
*D
*D
*D �ļ��޸ļ�¼
*D ------------------------------------------------------------------------------ 
*D �汾��       �޸�����       �޸���     �Ķ�����
*D ------------------------------------------------------------------------------ 
*D 1.1.0001     2007.12.24     plu        �����ļ�
*D 1.1.0002     2007.12.25     plu        ֧��SDC
*D*******************************************************************************/

#ifndef _FEAT_COMM_20071224_H_
#define _FEAT_COMM_20071224_H_

enum _BaseFeatureKind			// �����������ͣ�ע�ⲻ�ܳ���7
{
	FYT_MFCC=1,
	FYT_MFCCPLP,
	FYT_RASTAPLP
};

#define KINDMASK    (0x3f)

#define DYN_RANGE   50

// ���붨�壬8���ƣ�ת����2����ʱ��ֻ��һλ��1
#define HASENERGY    010       // _E log energy included , 
#define HASZEROC     020       // _0 0'th Cepstra included , ע�⣺_E _0 ����ͬʱ����
#define HASDELTA     040       // _D delta coef appended ��1�ײ��
#define HASACCS     0100       // _A acceleration coefs appended ��2�ײ��
#define HASTHIRD    0200       // _T has Delta-Delta-Delta index attached ��3�ײ��

#define HASSDC	  010000	   // _S SDC ���

// ������������
#define DOCMN		0400		
#define DOCVN	   01000       
#define DOWARP	   02000
#define DORASTA    04000

// ע�� ��Ϊm_MFCCKind��������short�ͣ��������õ����� 
//   010000						// 2007.12.25 plu : sdc ռ����
//   020000
//   040000
//  0100000
				   
#define BASEMASK  07         // �����ظ���m_MFCCKind����������ͣ�FYT_MFCC��FYT_MFCCPLP��FYT_RASTAPLP

struct FEATURE_MFCCBASEINFO  // ����������Ļ�����Ϣ
{
    char  targetKind[16];
    
    int   smpPeriod;				// �������ڣ�֡��
    int   framePeriod;
    
    // MfccBased
    bool  zeroGlobalMean;
	int   chlNum;					// �˲�����Ŀ��ͨ��������������
	int	  cepNum;

	int	  cepLifter;				// ��������������
	int	  winSize;
		    
	float lowPass;					// ��/�߽���Ƶ��
	float highPass;
	
	// EnergyBased
	bool  normEnergy;
	float energyScale;
	float silFloor;
		
	// robust Processing
	bool  doCMN;
	bool  doCVN;
	bool  doRASTA;
	float RASTACoff;
	bool  doFeatWarp;

	// SDC��ز���
	int	  nSdc_N;		// 7
	int   nSdc_D;		// 1
	int	  nSdc_P;		// 3
	int   nSdc_K;		// 7
	

	FEATURE_MFCCBASEINFO()
	{
		targetKind[0]='\0';
    
		smpPeriod=625;					// �������ڣ�֡��
		framePeriod=100000;
    
		// MfccBased
		zeroGlobalMean=false;
		chlNum=24;						// �˲�����Ŀ��ͨ��������������
		cepNum=12;

		cepLifter=22;					// ��������������
		winSize=250000;
		
		lowPass=-1.f;					// ��/�߽���Ƶ��
		highPass=-1.f;
			
		// EnergyBased
		normEnergy=false;
		energyScale=1.f;
		silFloor=50.f;

		// robust processing
		doCMN = false;
		doCVN = false;
		doRASTA = false;
		doFeatWarp = false;
		RASTACoff = 0.98f;	// or 0.94

		// SDC
		nSdc_N=7;		// 7
		nSdc_D=1;		// 1
		nSdc_P=3;		// 3
		nSdc_K=7;		// 7
	};
};


struct Feature_BaseInfo		// �����ļ�ͷ��ı������Ϣ
{
	char cFeatType[16];		// ���磺MFCC_A
	int  nFrameNum;			// ֡��
	int  nVecSize;			// ����ά��
	int  nVecSizeStore;		// ʵ�ʴ洢������ά��
	int  nFeatKind;			// ��������
	int  nWinSize;			// ��ʱ���Ĵ���
	int  nFrameRate;		// ֡��

	int  nTotalParamSize;	// �����ܵĲ���������λ���ֽ�

	Feature_BaseInfo()
	{
		cFeatType[0]='\0';
		nFrameNum = nVecSizeStore = nVecSize = nFeatKind = -1;
		nFrameRate = nWinSize = -1;
		nTotalParamSize = 0;
	}
};

struct Features {
    float ** features;
    int nFeatures;
    int * featureSize;
    int featureDim;
};

void ReadFeatures(char *p_pcFlistFile, struct Features &features, int &maxFrameNum, int VecDim);
void ReadFeature(char *p_pcFeatFile, float **feature, int *featureSize);
void FreeFeatures(struct Features &features);

bool ReadFeatFile(char *p_pcFeatFile,float *&p_pfFeatBuf,Feature_BaseInfo &p_sFeatInfo);
bool WriteFeatFile(char *p_pcFeatFile,float *p_pfFeatBuf,Feature_BaseInfo &p_sFeatInfo);
bool IsSameFeat(Feature_BaseInfo &p_sFeatInfo1,Feature_BaseInfo &p_sFeatInfo2);
void CopyFeatBaseInfo(Feature_BaseInfo &p_sDstInfo,Feature_BaseInfo &p_sSrcInfo);
bool ReadLimitFeatFile(char *p_pcFeatFile,float *&p_pfFeatBuf,Feature_BaseInfo &p_sFeatInfo, int nLimitLeng);
#endif // _FEAT_COMM_H_
