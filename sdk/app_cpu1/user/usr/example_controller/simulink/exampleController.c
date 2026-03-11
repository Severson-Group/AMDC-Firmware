/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: exampleController.c
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

#include "exampleController.h"
#include "exampleController_private.h"

/* External inputs (root inport signals with default storage) */
ExtU_exampleController_T exampleController_U;

/* External outputs (root outports fed by signals with default storage) */
ExtY_exampleController_T exampleController_Y;

/* Real-time model */
static RT_MODEL_exampleController_T exampleController_M_;
RT_MODEL_exampleController_T *const exampleController_M = &exampleController_M_;

/* Model step function */
void exampleController_step(void)
{
  /* Gain: '<Root>/Gain' incorporates:
   *  Inport: '<Root>/Input'
   */
  exampleController_Y.Output = 0.1 * exampleController_U.Input;

  /* Saturate: '<Root>/Sat' */
  if (exampleController_Y.Output > 0.9) {
    /* Gain: '<Root>/Gain' incorporates:
     *  Outport: '<Root>/Output'
     */
    exampleController_Y.Output = 0.9;
  } else if (exampleController_Y.Output < 0.0) {
    /* Gain: '<Root>/Gain' incorporates:
     *  Outport: '<Root>/Output'
     */
    exampleController_Y.Output = 0.0;
  }

  /* End of Saturate: '<Root>/Sat' */
}

/* Model initialize function */
void exampleController_initialize(void)
{
  /* (no initialization code required) */
}

/* Model terminate function */
void exampleController_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
