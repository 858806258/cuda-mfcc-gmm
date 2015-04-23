/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : GMMParam.cpp
*D
*D ��Ŀ����            : 
*D
*D �汾��              : 1.1.0003
*D
*D �ļ�����            :
*D
*D
*D �ļ��޸ļ�¼
*D ------------------------------------------------------------------------------ 
*D �汾��       �޸�����       �޸���     �Ķ�����
*D ------------------------------------------------------------------------------ 
*D 1.1.0001     2007.01.20     plu        �����ļ�
*D 1.1.0002     2007.09.11     plu        ����MFCC�������飬������������д��GMM param�ļ�
*D 1.1.0003     2007.09.17     plu        �ı�GMMģ���ڴ���䷽ʽ�����еľ�ֵ����һ�飬���еķ������һ��
*D*******************************************************************************/
#include "GMMParam.h"

#include "comm_srlr.h"

GMMParam::GMMParam()
{
	m_nVecSize4 = m_nVecSize = -1;
	m_nModelNum = m_nMixNum = -1;
		
	m_pGmmModel = NULL;
	m_fpLog = NULL;   // 2007.03.20 plu : 

	m_nMfccKind = -1;			// 2007.09.11 plu : 
}

GMMParam::~GMMParam()
{   
	if (NULL!=m_fpLog) fclose(m_fpLog);   // 2007.03.20 plu : 
   //Y if(m_pGmmModel) {
   //Y     FreeGaussMixModel(m_pGmmModel,m_nModelNum);
   //Y }
}

// �ͷ�ģ��
void GMMParam::FreeGaussMixModel(GaussMixModel *p_gmmParam,int p_nModelNum)
{
	if (p_gmmParam)
	{
		ASSERT2(p_nModelNum,"Error call FreeGaussMixModel() : p_nModelNum<0!");
		for (int i=0;i<p_nModelNum;i++)
		{
			if (p_gmmParam[i].pGauss)		
			{
				/*// plu 2007.09.17_16:08:11
				for (int m=0;m<p_gmmParam[i].nMixNum;m++)
				{
					Free(p_gmmParam[i].pGauss[m].pfMean);
					Free(p_gmmParam[i].pGauss[m].pfDiagCov);
				}
				*/// plu 2007.09.17_16:08:11

				// 2007.09.17 plu : 
				Free(p_gmmParam[i].pfMeanBuf);
				Free(p_gmmParam[i].pfDiagCovBuf);

				Free(p_gmmParam[i].pGauss);

				p_gmmParam[i].pfMeanBuf = NULL;
				p_gmmParam[i].pfDiagCovBuf = NULL;
				p_gmmParam[i].pGauss = NULL;
			}
			Free(p_gmmParam[i].pfWeight);
			p_gmmParam[i].pfWeight = NULL;
		}
		Free(p_gmmParam);
		p_gmmParam = NULL;
	}
}

// ����һ��GMMģ�͵Ŀռ�
void GMMParam::AllocGaussMixModel(GaussMixModel *p_pModel,int p_nMixNum,int p_nVecSize)
{
	ASSERT2(p_nMixNum,"Error call AllocGaussMixModel() : p_nMixNum<0!");
	ASSERT2(p_nVecSize,"Error call AllocGaussMixModel() : p_nVecSize<0!");
	
	// ����
	p_pModel->pfWeight = (float *)malloc(p_nMixNum*sizeof(float));
	p_pModel->pGauss = (GaussPDF*)malloc (p_nMixNum*sizeof(GaussPDF));

	/*// plu 2007.09.17_16:04:42
	for(int m=0;m<p_nMixNum;m++)
	{
		p_pModel->pGauss[m].pfMean=(float *)Malloc (p_nVecSize*sizeof(float),true);
		p_pModel->pGauss[m].pfDiagCov=(float *)Malloc (p_nVecSize*sizeof(float),true);
	}
	*/// plu 2007.09.17_16:04:42

	// 2007.09.17 plu : ���������ֵ��������ڴ�ռ�
	p_pModel->pfMeanBuf = (float *)Malloc(p_nMixNum*sizeof(float)*p_nVecSize,true);
	p_pModel->pfDiagCovBuf = (float *)Malloc(p_nMixNum*sizeof(float)*p_nVecSize,true);
	for(int m=0;m<p_nMixNum;m++)
	{
		// ����ÿ����˹�����ľ�ֵ�ͷ�����ڴ���׵�ַ
		p_pModel->pGauss[m].pfMean = p_pModel->pfMeanBuf + m*p_nVecSize;
		p_pModel->pGauss[m].pfDiagCov = p_pModel->pfDiagCovBuf + m*p_nVecSize;
	}

	// ��ʼ��
	p_pModel->pfWeight[0] = 1.f;
}


// ����GMMģ��
// ע��: Ŀǰ�İ汾�ǽ�m_nVecSize4ά��ʸ��д��model�ļ�������
void GMMParam::LoadModel(char *p_pcModelFile)
{
	ASSERT2(p_pcModelFile,"Error call LoadModel() : p_pcModelFile=NULL!");

	// ��ģ���ļ�
	FILE *fpModel;
	ReadOpen(fpModel,p_pcModelFile);

	// ����������ļ�ͷ
	GMMFileHeader  ModelHeader;					// Header *header; ģ���ļ����ļ�ͷ
	fread(&ModelHeader,sizeof(GMMFileHeader),1,fpModel);
	ASSERT3(ModelHeader.nDim>0, "Error in model file %s : nDim<0!",p_pcModelFile);
	ASSERT3(ModelHeader.nMixNum>0, "Error in model file %s : nMixNum<0!",p_pcModelFile);
	ASSERT3(ModelHeader.nModelNum>0, "Error in model file %s : nModelNum<0!",p_pcModelFile);

	// �趨 ʸ��ά������ϸ�˹��Ŀ��ģ����Ŀ
	m_nVecSize = ModelHeader.nDim;
	m_nVecSize4 = ALIGN_4F(m_nVecSize);
	m_nMixNum  = ModelHeader.nMixNum;		// �ļ�p_pcModelFile�е�ÿ��ģ�͵�mix��Ŀ���ڶ���һ���ģ�����
	m_nModelNum  = ModelHeader.nModelNum;

	// �����ڴ�
//Y	if (NULL != m_pGmmModel)
//Y		FreeGaussMixModel(m_pGmmModel,m_nModelNum);
	m_pGmmModel = (GaussMixModel *) Malloc(m_nModelNum,sizeof(GaussMixModel),false);

	for(int i=0;i<m_nModelNum;i++)
	{
		// ���� GMM ģ��
		AllocGaussMixModel(&m_pGmmModel[i],m_nMixNum,m_nVecSize4);

		m_pGmmModel[i].nMixNum = m_nMixNum;

		// ����weight
		fread(m_pGmmModel[i].pfWeight,sizeof(float),m_nMixNum,fpModel);

		// ������ֵ��б���mat
		for(int m=0;m<m_nMixNum;m++)
		{
			m_pGmmModel[i].pGauss[m].nDim = m_nVecSize;

			fread(m_pGmmModel[i].pGauss[m].pfMean,sizeof(float),m_nVecSize4,fpModel);
			fread(m_pGmmModel[i].pGauss[m].pfDiagCov,sizeof(float),m_nVecSize4,fpModel);
			fread(&m_pGmmModel[i].pGauss[m].dMat,sizeof(double),1,fpModel);
		
			// ���matֵ�����㣬����ģ�������⣬����
			if (m_pGmmModel[i].pGauss[m].dMat>=0.0)
				printf("Warning : m_pGmmModel[%d].pGauss[%d].dMat=%.3f!\n",i,m,m_pGmmModel[i].pGauss[m].dMat);
		}
	}
	fclose(fpModel);
}


// ע��: Ŀǰ�İ汾�ǽ�m_nVecSize4ά��ʸ��д��model�ļ�������
bool GMMParam::WriteModel(char *p_pcModelFile)
{
	if (m_nVecSize<=0)
	{
		printf("Error call WriteModel() : m_nVexSize<=0!");
		return false;
	}
	if (m_nMixNum<=0)
	{
		printf("Error call WriteModel() : m_nMixNum<=0!");
		return false;
	}
	if (m_nModelNum<=0)
	{
		printf("Error call WriteModel() : m_nModelNum<=0!");
		return false;
	}
	if (NULL==m_pGmmModel)
	{
		printf("Error call WriteModel() : m_pGmmModel=NULL!");
		return false;
	}

	FILE *fpModel;

	// ���ļ�
	WriteOpen(fpModel,p_pcModelFile);

    m_nModelNum = 1;
	// д�ļ�ͷ
	fwrite(&m_nModelNum,1,sizeof(int),fpModel);
	fwrite(&m_nMixNum,1,sizeof(int),fpModel);
	fwrite(&m_nVecSize,1,sizeof(int),fpModel);
	
	int nOrthogonal;
	/*// plu 2007.09.11_17:07:44
	fwrite(&nOrthogonal,1,sizeof(int),fpModel);		// Ϊ�˺���ǰ��ģ��һ�£����������������
	*/// plu 2007.09.11_17:07:44
	nOrthogonal = (int)m_nMfccKind;
	fwrite(&nOrthogonal,1,sizeof(int),fpModel);		// ����������д���ļ�

	for(int i=0;i<m_nModelNum;i++)
	{
		// дweight
		fwrite(m_pGmmModel[i].pfWeight,sizeof(float),m_nMixNum,fpModel);
		
		for(int m=0;m<m_nMixNum;m++)
		{
			fwrite(m_pGmmModel[i].pGauss[m].pfMean,sizeof(float),m_nVecSize4,fpModel);
			fwrite(m_pGmmModel[i].pGauss[m].pfDiagCov,sizeof(float),m_nVecSize4,fpModel);
			fwrite(&(m_pGmmModel[i].pGauss[m].dMat),sizeof(double),1,fpModel);
		}
	}
	
	fclose(fpModel);

	return true; 
} 

// 2007.03.20 plu : add
void GMMParam::SetLogFile(char *p_pcLogFile)
{
	if (NULL!=m_fpLog)	fclose(m_fpLog);

	// log�ļ�
	m_fpLog=fopen(p_pcLogFile,"wt");
	ASSERT3(m_fpLog,"Error open %s for write!",p_pcLogFile)
}

void GMMParam::AllocAll(int p_nMixNum, int p_nVecSize) {
    if(m_pGmmModel) {
        FreeGaussMixModel( m_pGmmModel, 1 );
    }
    
    m_pGmmModel = (GaussMixModel *) malloc(sizeof(GaussMixModel));
    
    AllocGaussMixModel(m_pGmmModel, p_nMixNum, p_nVecSize);
}

void GMMParam::FreeAll() {
    if(m_pGmmModel) {
        FreeGaussMixModel( m_pGmmModel, 1 );
    }
}

void GMMParam::InitModel(struct Features &features, int vecSize, int vecSize4) {
    m_nModelNum = 1; //modelNum;
    m_nMixNum   = 1; //mixNum;
    m_nVecSize  = vecSize;
    m_nVecSize4    = vecSize4;


    m_pGmmModel[0].pfWeight[0] = 1.0f; 
    

    m_pGmmModel[0].nMixNum = 1;
    float *mean = m_pGmmModel[0].pfMeanBuf;
    float *diagCov = m_pGmmModel[0].pfDiagCovBuf;
    
    float * feature;
    float val;
    int featureSize;
    int featDim = features.featureDim;
    int wholeCnt = 0;
    
    for(int fileIdx = 0; fileIdx < features.nFeatures; fileIdx ++) {
        feature = features.features[fileIdx];
        
        featureSize = features.featureSize[fileIdx];
        
        wholeCnt += featureSize;
        
        for(int featIdx = 0; featIdx < featureSize; featIdx ++) {
            for(int featDimIdx = 0; featDimIdx < featDim; featDimIdx ++) {
                val = feature[featIdx * featDim + featDimIdx]; 
               mean[featDimIdx] += val;
               diagCov[featDimIdx] += val * val;
            }
        }
    }
    for(int featDimIdx = 0; featDimIdx < featDim; featDimIdx ++) {
        mean[featDimIdx] /= wholeCnt;
        diagCov[featDimIdx] /= wholeCnt;

        diagCov[featDimIdx] -= mean[featDimIdx] * mean[featDimIdx];
        diagCov[featDimIdx] = 1.f / diagCov[featDimIdx];
    }
    
    float &dMat = m_pGmmModel[0].pGauss[0].dMat;
    dMat = 0;
    
    for (int i=0;i<featDim;i++)
        dMat += (diagCov[i]<=0.f)?LZERO:log(diagCov[i]);
    dMat -= featDim*log(TPI);
    dMat *= 0.5;
}

void GMMParam::LoadModel(const GMMParam &gmmParam) {
    m_nModelNum = gmmParam.m_nModelNum;
    m_nMixNum = gmmParam.m_nMixNum;
    m_nVecSize = gmmParam.m_nVecSize;
    m_nVecSize4 = gmmParam.m_nVecSize4;
    
    m_pGmmModel = gmmParam.m_pGmmModel;
}
void GMMParam::WriteModel(const GMMParam &gmmParam) {
//    gmmParam.LoadModel(*this);
}

void GMMParam::report() {
    printf("Mixture Number: %d\n", m_nMixNum);
    printf("Feature Width: %d\n", m_nVecSize);
    
    for(int i = 0; i < m_nMixNum; i++) {
        printf("%dth Gaussian Model\n", i);

        printf("MEAN: \n");
        for(int j = 0;j < m_nVecSize; j++) {
            printf("%f ", m_pGmmModel[0].pGauss[i].pfMean[j]);
        }
        puts("");
        
        printf("Diag Var: \n");
        for(int j = 0;j < m_nVecSize; j++) {
            printf("%f ", m_pGmmModel[0].pGauss[i].pfDiagCov[j]);
        }
        puts("");
    }
}
