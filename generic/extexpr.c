
#include "extexpr.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>

const char *TclGetUnqualifiedName(const char *qualifiedName) {
    const char *p = qualifiedName;
    const char *last = qualifiedName;

    while ((p = strstr(p, "::")) != NULL) {
        p += 2;
        last = p;
    }
    return last;
}

extern DLLEXPORT int Extexpr_Init(Tcl_Interp *interp) {
    if (Tcl_InitStubs(interp, "8.6-10.0", 0) == NULL) {
        return TCL_ERROR;
    }
    /* check the existence of the namespace */
    if (Tcl_Eval(interp, "namespace eval ::tcl::mathfunc {}") != TCL_OK) {
        return TCL_ERROR;
    }
    /* Provide the current package */
    if (Tcl_PkgProvideEx(interp, PACKAGE_NAME, PACKAGE_VERSION, NULL) != TCL_OK) {
        return TCL_ERROR;
    }
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::mul", (Tcl_ObjCmdProc2 *)MulCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::sum", (Tcl_ObjCmdProc2 *)SumCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::sub", (Tcl_ObjCmdProc2 *)SubCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::div", (Tcl_ObjCmdProc2 *)DivCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::pow", (Tcl_ObjCmdProc2 *)PowCmdProc2, NULL, NULL);
    Tcl_CreateObjCommand2(interp, "::tcl::mathfunc::dot", (Tcl_ObjCmdProc2 *)DotCmdProc2, NULL, NULL);
    return TCL_OK;
}
enum Operations { SUM, SUB, MUL, DIV, POW, DOT };
enum Mode { SL, LS, LL, SS };

static int MulCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(MUL, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SumCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(SUM, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int SubCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(SUB, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int DivCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(DIV, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int PowCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(POW, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int DotCmdProc2(void *clientData, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    if (OperationProc(DOT, interp, objc, objv) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}
static int OperationProc(int operation, Tcl_Interp *interp, Tcl_Size objc, Tcl_Obj *const objv[]) {
    Tcl_Obj *resultList = Tcl_NewListObj(0, NULL); /* result list */
    double elementValue1, elementValue2;           /* first and second input list elements values */
    double resultValue;                            /* result list element value */
    Tcl_Size list1Length, list2Length;             /* lengths of the first and second input lists */
    Tcl_Obj **elements1, **elements2;              /* elements of the first and second input lists */
    double intpart;
    int mode = SS;

    if (objc != 3) {
        const char *fullName = Tcl_GetString(objv[0]);
        const char *simpleName = TclGetUnqualifiedName(fullName);
        Tcl_AppendResult(interp, "wrong # args: should be one of the following:\n", 
                         "  ", Tcl_GetString(objv[0]), " list list\n", 
                         "  ", Tcl_GetString(objv[0]), " list value\n",
                         "  ", Tcl_GetString(objv[0]), " value list\n", 
                         "  ", Tcl_GetString(objv[0]), " value value\n", 
                         "  ", "or use [expr] syntax: ", simpleName,
                         "(arg1,arg2) with any of the argument combinations above", (char *)NULL);
        return TCL_ERROR;
    }
    /* get list elements values from the command arguments */
    if (Tcl_ListObjGetElements(interp, objv[1], &list1Length, &elements1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tcl_ListObjGetElements(interp, objv[2], &list2Length, &elements2) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((list1Length == 1) && (list2Length > 1)) {
        mode = SL;
    } else if ((list1Length > 1) && (list2Length == 1)) {
        mode = LS;
    } else if ((list1Length == 1) && (list2Length == 1)) {
        mode = SS;
    } else if ((list1Length > 1) && (list2Length > 1)) {
        if (list1Length != list2Length) {
            int len =
                snprintf(NULL, 0, "Lengths of the input lists '%ld' and '%ld' are not equal", list1Length, list2Length);
            char *buffer = (char *)Tcl_Alloc(len + 1);
            snprintf(buffer, len + 1, "Lengths of the input lists '%ld' and '%ld' are not equal", list1Length,
                     list2Length);
            Tcl_SetObjResult(interp, Tcl_NewStringObj(buffer, -1));
            Tcl_Free(buffer);
            return TCL_ERROR;
        }
        mode = LL;
    } else if ((list1Length == 0) || (list2Length == 0)) {
        Tcl_SetObjResult(interp, Tcl_NewStringObj("Empty list is not allowed as scalar input", -1));
        return TCL_ERROR;
    }
    if (operation == DOT) {
        resultValue = 0.0;
    }
    if (mode == LL) {
        for (Tcl_Size i = 0; i < list1Length; ++i) {
            if (Tcl_GetDoubleFromObj(interp, elements1[i], &elementValue1) != TCL_OK) {
                return TCL_ERROR;
            }
            if (Tcl_GetDoubleFromObj(interp, elements2[i], &elementValue2) != TCL_OK) {
                return TCL_ERROR;
            }
            switch ((enum Operations)operation) {
            case SUM:
                resultValue = elementValue1 + elementValue2;
                break;
            case MUL:
                resultValue = elementValue1 * elementValue2;
                break;
            case SUB:
                resultValue = elementValue1 - elementValue2;
                break;
            case DIV:
                if (elementValue2 == 0.0) {
                    Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                    return TCL_ERROR;
                }
                resultValue = elementValue1 / elementValue2;
                break;
            case POW:
                if ((modf(elementValue2, &intpart) != 0.0) && (elementValue1 < 0.0)) {
                    Tcl_SetObjResult(interp,
                                     Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                    return TCL_ERROR;
                }
                resultValue = pow(elementValue1, elementValue2);
                break;
            case DOT:
                resultValue += elementValue1 * elementValue2;
                break;
            };
            /* append elements to the result list */
            if (operation != DOT) {
                Tcl_ListObjAppendElement(interp, resultList, Tcl_NewDoubleObj(resultValue));
            }
        }
        if (operation == DOT) {
            Tcl_SetObjResult(interp, Tcl_NewDoubleObj(resultValue));
            return TCL_OK;
        }
    } else if ((mode == SL) || (mode == LS)) {
        if (mode == SL) {
            if (Tcl_GetDoubleFromObj(interp, elements1[0], &elementValue1) != TCL_OK) {
                return TCL_ERROR;
            }
            double scalarValue = elementValue1;
            for (Tcl_Size i = 0; i < list2Length; ++i) {
                if (Tcl_GetDoubleFromObj(interp, elements2[i], &elementValue2) != TCL_OK) {
                    return TCL_ERROR;
                }
                switch ((enum Operations)operation) {
                case SUM:
                    resultValue = scalarValue + elementValue2;
                    break;
                case MUL:
                    resultValue = scalarValue * elementValue2;
                    break;
                case SUB:
                    resultValue = scalarValue - elementValue2;
                    break;
                case DIV:
                    if (elementValue2 == 0.0) {
                        Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                        return TCL_ERROR;
                    }
                    resultValue = scalarValue / elementValue2;
                    break;
                case POW:
                    if ((modf(elementValue2, &intpart) != 0.0) && (scalarValue < 0.0)) {
                        Tcl_SetObjResult(interp,
                                         Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                        return TCL_ERROR;
                    }
                    resultValue = pow(scalarValue, elementValue2);
                    break;
                case DOT:
                    Tcl_SetObjResult(interp, Tcl_NewStringObj("Dot product elements must be the same length", -1));
                    return TCL_ERROR;
                };
                /* append elements to the result list */
                Tcl_ListObjAppendElement(interp, resultList, Tcl_NewDoubleObj(resultValue));
            }
        } else if (mode == LS) {
            if (Tcl_GetDoubleFromObj(interp, elements2[0], &elementValue2) != TCL_OK) {
                return TCL_ERROR;
            }
            double scalarValue = elementValue2;
            for (Tcl_Size i = 0; i < list1Length; ++i) {
                if (Tcl_GetDoubleFromObj(interp, elements1[i], &elementValue1) != TCL_OK) {
                    return TCL_ERROR;
                }
                switch ((enum Operations)operation) {
                case SUM:
                    resultValue = elementValue1 + scalarValue;
                    break;
                case MUL:
                    resultValue = elementValue1 * scalarValue;
                    break;
                case SUB:
                    resultValue = elementValue1 - scalarValue;
                    break;
                case DIV:
                    if (scalarValue == 0.0) {
                        Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                        return TCL_ERROR;
                    }
                    resultValue = elementValue1 / scalarValue;
                    break;
                case POW:
                    if ((modf(scalarValue, &intpart) != 0.0) && (elementValue1 < 0.0)) {
                        Tcl_SetObjResult(interp,
                                         Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                        return TCL_ERROR;
                    }
                    resultValue = pow(elementValue1, scalarValue);
                    break;
                case DOT:
                    Tcl_SetObjResult(interp, Tcl_NewStringObj("Dot product elements must be the same length", -1));
                    return TCL_ERROR;
                };
                /* append elements to the result list */
                Tcl_ListObjAppendElement(interp, resultList, Tcl_NewDoubleObj(resultValue));
            }
        }
    } else {
        if (Tcl_GetDoubleFromObj(interp, elements1[0], &elementValue1) != TCL_OK) {
            return TCL_ERROR;
        }
        if (Tcl_GetDoubleFromObj(interp, elements2[0], &elementValue2) != TCL_OK) {
            return TCL_ERROR;
        }
        switch ((enum Operations)operation) {
        case SUM:
            resultValue = elementValue1 + elementValue2;
            break;
        case MUL:
            resultValue = elementValue1 * elementValue2;
            break;
        case SUB:
            resultValue = elementValue1 - elementValue2;
            break;
        case DIV:
            if (elementValue2 == 0.0) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Division by zero", -1));
                return TCL_ERROR;
            }
            resultValue = elementValue1 / elementValue2;
            break;
        case POW:
            if ((modf(elementValue2, &intpart) != 0.0) && (elementValue1 < 0.0)) {
                Tcl_SetObjResult(interp, Tcl_NewStringObj("Negative base cannot be raised to a non-integer power", -1));
                return TCL_ERROR;
            }
            resultValue = pow(elementValue1, elementValue2);
            break;
        case DOT:
            resultValue = elementValue1 * elementValue2;
            break;
        };
        Tcl_SetObjResult(interp, Tcl_NewDoubleObj(resultValue));
        return TCL_OK;
    }
    /* set result of the command to result list we calculated */
    Tcl_SetObjResult(interp, resultList);
    return TCL_OK;
}
