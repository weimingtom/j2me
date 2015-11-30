#ifndef __F_PLATFORM_H__
#define __F_PLATFORM_H__

#define FVT_SDK(FVT_FEATURE) (defined FVT_SDK_##FVT_FEATURE && FVT_SDK_##FVT_FEATURE)
#define FTM_SOC(FTM_FEATURE) (defined FTM_SOC_##FTM_FEATURE && FTM_SOC_##FTM_FEATURE)
#define FTM_IMAGE_IPTVS(FTM_FEATURE) (defined FTM_IMAGE_IPTVS_##FTM_FEATURE && FTM_IMAGE_IPTVS_##FTM_FEATURE)
#define FTM_PROD(FTM_FEATURE) (defined FTM_PROD_##FTM_FEATURE && FTM_PROD_##FTM_FEATURE)
#define FTM_COMPILER(FTM_FEATURE) (defined FTM_##FTM_FEATURE && FTM_##FTM_FEATURE)
#define FTM_CPU(FTM_FEATURE) (defined FTM_CPU_##FTM_FEATURE && FTM_CPU_##FTM_FEATURE)
#define FTM_HAVE(FTM_FEATURE) (defined FTM_HAVE_##FTM_FEATURE && FTM_HAVE_##FTM_FEATURE)
#define FTM_OS(FTM_FEATURE) (defined FTM_OS_##FTM_FEATURE && FTM_OS_##FTM_FEATURE)
#define FTM_USE(FTM_FEATURE) (defined FTM_USE_##FTM_FEATURE && FTM_USE_##FTM_FEATURE)
#define FTM_ENABLE(FTM_FEATURE) (defined FTM_ENABLE_##FTM_FEATURE && FTM_ENABLE_##FTM_FEATURE)

#endif
