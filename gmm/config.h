/******************************DOCUMENT*COMMENT***********************************
*D
*D �ļ�����            : config.h
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
*D 1.1.0001                               �����ļ�
*D 1.1.0002     2007.08.29     plu        �޸Ĺ��캯��,����SetCfgFile����
*D*******************************************************************************/

#ifndef _CONFIGURATION_HCCL_
#define _CONFIGURATION_HCCL_

#include "comm_srlr.h"		

struct aConfigEnv {
   aConfigEnv *next;
   char256 env;
   char256 def;
};

class Config {
   protected:

      aConfigEnv *envs;
      FILE *fenv;
      char *value;
      bool exam;

      char *GetEnv(const char *env);
      
   public:
      // =%d 
      bool ReadConfig(const char *line,int& num);
      // =%d %d
      bool ReadConfig(const char *line,int& num1,int& num2);
      // =true/false
      bool ReadConfig(const char *line,bool& bln);
      // =%g
      bool ReadConfig(const char *line,float& num);
      // =%g %g
      bool ReadConfig(const char *line,float& num1,float& num2);
      // =%g %g %g
      bool ReadConfig(const char *line,float& num1,float& num2,float& num3);
      // =%g,%g %g,%g
      bool ReadConfig(const char *line,float& n1,float& n2,float& n3,float& n4);
      // =%s
      bool ReadConfig(const char *line,char *str);
      // =%s %s
      bool ReadConfig(const char *line,char *str1,char *str2);
      
      /*// plu 2007.08.29_12:40:15
      Config(int argc,char *argv[],char *head=NULL);
      */// plu 2007.08.29_12:40:15
	  Config();
      ~Config();

	  // 2007.08.29 plu : 
	  bool SetConfigFile(char *cfgFile);
};

#endif
