/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: exampleController.h
 *
 * Code generated for Simulink model 'exampleController'.
 *
 * Model version                  : 9.1
 * Simulink Coder version         : 9.5 (R2021a) 14-Nov-2020
 * C/C++ source code generated on : Sat May  8 10:51:56 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_exampleController_h_
#define RTW_HEADER_exampleController_h_
#ifndef exampleController_COMMON_INCLUDES_
#define exampleController_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* exampleController_COMMON_INCLUDES_ */

#include "exampleController_types.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm)         ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val)    ((rtm)->errorStatus = (val))
#endif

/* External inputs (root inport signals with default storage) */
typedef struct {
  real_T Input;                        /* '<Root>/Input' */
} ExtU_exampleController_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  real_T Output;                       /* '<Root>/Output' */
} ExtY_exampleController_T;

/* Real-time Model Data Structure */
struct tag_RTM_exampleController_T {
  const char_T * volatile errorStatus;
};

/* External inputs (root inport signals with default storage) */
extern ExtU_exampleController_T exampleController_U;

/* External outputs (root outports fed by signals with default storage) */
extern ExtY_exampleController_T exampleController_Y;

/* Model entry point functions */
extern void exampleController_initialize(void);
extern void exampleController_step(void);
extern void exampleController_terminate(void);

/* Real-time Model object */
extern RT_MODEL_exampleController_T *const exampleController_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'exampleController'
 */
#endif                                 /* RTW_HEADER_exampleController_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
